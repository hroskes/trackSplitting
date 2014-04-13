#ifndef def
#define def

/***********************************
Table Of Contents
0. Track Split Plot
1. Make Plots
2. Axis Label
3. Axis Limits
***********************************/

#include "trackSplitPlot.h"
//#include "tdrstyle.C"
//#include "placeLegend.C"
#include "misalignmentDependence.C"

//=================================
//0. Track Split Plot
//=================================

TCanvas *trackSplitPlot(Int_t nFiles,TString *files,TString *names,TString xvar,TString yvar,
                        Bool_t relative,Bool_t resolution,Bool_t pull,
                        TString saveas)
{
    stufftodelete->SetOwner(true);
    cout << xvar << " " << yvar << endl;
    if (xvar == "" && yvar == "")
        return 0;

    PlotType type;
    if (xvar == "")      type = Histogram;
    else if (yvar == "") type = OrgHistogram;
    else if (resolution) type = Resolution;
    else if (nFiles < 1) type = ScatterPlot;
    else                 type = Profile;
    if (nFiles < 1) nFiles = 1;

    const Int_t n = nFiles;
    
    setTDRStyle();
    gStyle->SetOptStat(0);        //for histograms, the mean and rms are included in the legend if nFiles >= 2
                                  //if nFiles == 1, there is no legend, so they're in the statbox
    if ((type == Histogram || type == OrgHistogram) && nFiles == 1)
        gStyle->SetOptStat(1110);
    //for a scatterplot, this is needed to show the z axis scale
    //for non-pull histograms or when run number is on the x axis, this is needed so that 10^-? on the right is not cut off
    if (type == ScatterPlot || (type == Histogram && !pull) || xvar == "runNumber")
    {
        gStyle->SetCanvasDefW(678);
        gStyle->SetPadRightMargin(0.115);
    }
    else
    {
        gStyle->SetCanvasDefW(600);
        gStyle->SetPadRightMargin(0.04);
    }

    Bool_t nHits = (xvar[0] == 'n' && xvar[1] == 'H' && xvar[2] == 'i'    //This includes nHits, nHitsTIB, etc.
                                   && xvar[3] == 't' && xvar[4] == 's');

    Int_t nBinsScatterPlotx = binsScatterPlotx;
    Int_t nBinsScatterPloty = binsScatterPloty;
    Int_t nBinsHistogram = binsHistogram;
    Int_t nBinsProfileResolution = binsProfileResolution;
    if (xvar == "runNumber")
    {
        nBinsProfileResolution = runNumberBins;
        nBinsHistogram = runNumberBins;
    }
    if (nHits)
    {
        nBinsHistogram = (int)(findMax(nFiles,files,xvar,'x') - findMin(nFiles,files,xvar,'x') + 1.1);     //in case it's .99999
        nBinsScatterPlotx = nBinsHistogram;
        nBinsProfileResolution = nBinsHistogram;
    }

    vector<TH1*> p;
    Int_t lengths[n];

    stringstream sx,sy,srel,ssigma1,ssigma2,ssigmaorg;

    sx << xvar << "_org";
    TString xvariable = sx.str();
    TString xvariable2 = "";
    if (xvar == "runNumber") xvariable = "runNumber";
    if (nHits)
    {
        xvariable  = xvar;
        xvariable2 = xvar;
        xvariable.Append("1_spl");
        xvariable2.Append("2_spl");
    }

    sy << "Delta_" << yvar;
    TString yvariable = sy.str();

    TString relvariable = "1";
    if (relative)
    {
        srel << yvar << "_org";
        relvariable = srel.str();
    }

    TString sigma1variable = "",sigma2variable = "";
    if (pull)
    {
        ssigma1 << yvar << "1Err_spl";
        ssigma2 << yvar << "2Err_spl";
    }
    sigma1variable = ssigma1.str();
    sigma2variable = ssigma2.str();

    TString sigmaorgvariable = "";
    if (pull && relative)
        ssigmaorg << yvar << "Err_org";
    sigmaorgvariable = ssigmaorg.str();


    Double_t xmin = -1,xmax = 1,ymin = -1,ymax = 1;
    if (type == Profile || type == ScatterPlot || type == OrgHistogram || type == Resolution)
        axislimits(nFiles,files,xvar,'x',relative,pull,xmin,xmax);
    if (type == Profile || type == ScatterPlot || type == Histogram || type == Resolution)
        axislimits(nFiles,files,yvar,'y',relative,pull,ymin,ymax);

    TString meansrmss[n];
    Bool_t  used[n];        //a file is not "used" if it's MC data and the x variable is run number

    for (Int_t i = 0; i < n; i++)
    {
        stringstream sid;
        sid << "p" << i;
        TString id = sid.str();

        //for a profile or resolution, it fills a histogram, q[j], for each bin, then gets the mean and width from there.
        vector<TH1F*> q;

        if (type == ScatterPlot)
            p.push_back(new TH2F(id,"",nBinsScatterPlotx,xmin,xmax,nBinsScatterPloty,ymin,ymax));
        if (type == Histogram)
            p.push_back(new TH1F(id,"",nBinsHistogram,ymin,ymax));
        if (type == OrgHistogram)
            p.push_back(new TH1F(id,"",nBinsHistogram,xmin,xmax));
        if (type == Resolution || type == Profile)
        {
            p.push_back(new TH1F(id,"",nBinsProfileResolution,xmin,xmax));
            for (Int_t j = 0; j < nBinsProfileResolution; j++)
            {

                stringstream sid2;
                sid2 << "q" << i << j;
                TString id2 = sid2.str();
                q.push_back(new TH1F(id2,"",nBinsHistogram,ymin,ymax));

            }
        }
        stufftodelete->Add(p[i]);
        p[i]->SetBit(kCanDelete,true);

        used[i] = true;
        if ((files[i].Contains("MC") && xvar == "runNumber") || files[i] == "")  //if it's MC data, the run number is meaningless
        {
            used[i] = false;
            p[i]->SetLineColor(kWhite);
            p[i]->SetMarkerColor(kWhite);
            for (unsigned int j = 0; j < q.size(); j++)
                delete q[j];
            continue;
        }

        TFile *f = TFile::Open(files[i]);
        TTree *tree = (TTree*)f->Get("cosmicValidation/splitterTree");
        if (tree == 0)
            tree = (TTree*)f->Get("splitterTree");

        lengths[i] = tree->GetEntries();

        Double_t x = 0, y = 0, rel = 1, sigma1 = 1, sigma2 = 1,           //if !pull, we want to divide by sqrt(2) because we want the error from 1 track
                                                  sigmaorg = 0;
        Int_t xint = 0, xint2 = 0;
        Int_t runNumber = 0;

        if (!relative && !pull && (yvar == "dz" || yvar == "dxy"))
            rel = 1e-4;                                     //it's in cm but we want it in um, so divide by 1e-4

        tree->SetBranchAddress("runNumber",&runNumber);
        if (type == Profile || type == ScatterPlot || type == Resolution || type == OrgHistogram)
        {
            if (xvar == "runNumber")
                tree->SetBranchAddress(xvariable,&xint);
            else if (nHits)
            {
                tree->SetBranchAddress(xvariable,&xint);
                tree->SetBranchAddress(xvariable2,&xint2);
            }
            else
                tree->SetBranchAddress(xvariable,&x);
        }
        if (type == Profile || type == ScatterPlot || type == Resolution || type == Histogram)
        {
            int branchexists = tree->SetBranchAddress(yvariable,&y);
            if (branchexists == -5)   //i.e. it doesn't exist
            {
                yvariable.ReplaceAll("Delta_","d");
                yvariable.Append("_spl");
                tree->SetBranchAddress(yvariable,&y);
            }
        }
        if (relative && xvar != yvar)                       //if xvar == yvar, setting the branch here will undo setting it to x 2 lines earlier
            tree->SetBranchAddress(relvariable,&rel);       //setting the value of rel is then taken care of later: rel = x
        if (pull)
        {
            tree->SetBranchAddress(sigma1variable,&sigma1);
            tree->SetBranchAddress(sigma2variable,&sigma2);
        }
        if (relative && pull)
            tree->SetBranchAddress(sigmaorgvariable,&sigmaorg);
   
        Int_t notincluded = 0;                              //this counts the number that aren't in the right run range.
                                                            //it's subtracted from lengths[i] in order to normalize the histograms

        for (Int_t j = 0; j<lengths[i]; j++)
        {
            tree->GetEntry(j);
            if (xvar == "runNumber" || nHits)
                x = xint;
            if (xvar == "runNumber")
                runNumber = x;
            if (runNumber < minrun || (runNumber > maxrun && maxrun > 0))  //minrun and maxrun are global variables.  
            {                                                              //they're defined in axislimits.C because they're used there too
                notincluded++;
                continue;
            }
            if (relative && xvar == yvar)
                rel = x;
            Double_t error = 0;
            if (relative && pull)
                error = sqrt((sigma1/rel)*(sigma1/rel) + (sigma2/rel)*(sigma2/rel) + (sigmaorg*y/(rel*rel))*(sigmaorg*x/(rel*rel)));
            else
                error = sqrt(sigma1 * sigma1 + sigma2 * sigma2);   // = sqrt(2) if !pull; this divides by sqrt(2) to get the error in 1 track
            y /= (rel * error);

            if (ymin <= y && y < ymax && xmin <= x && x < xmax)
            {
                if (type == Histogram)
                    p[i]->Fill(y);
                if (type == ScatterPlot)
                    p[i]->Fill(x,y);
                if (type == Resolution || type == Profile)
                {
                    int which = (p[i]->Fill(x,0)) - 1;
                    //get which q[j] by filling p[i] with nothing.  (TH1F::Fill returns the bin number)
                    //p[i]'s actual contents are set later.
                    if (which >= 0 && (unsigned)which < q.size()) q[which]->Fill(y);
                }
                if (type == OrgHistogram)
                    p[i]->Fill(x);
            }

            if (nHits)
            {
                x = xint2;
                if (ymin <= y && y < ymax && xmin <= x && x < xmax)
                {
                    if (type == Histogram)
                        p[i]->Fill(y);
                    if (type == ScatterPlot)
                        p[i]->Fill(x,y);
                    if (type == Resolution || type == Profile)
                    {
                        int which = (p[i]->Fill(x,0)) - 1;
                        if (which >= 0) q[which]->Fill(y);         //get which q[j] by filling p[i] (with nothing), which returns the bin number
                    }
                    if (type == OrgHistogram)
                        p[i]->Fill(x);
                }
            }

            if (lengths[i] < 10 ? true : 
                (((j+1)/(int)(pow(10,(int)(log10(lengths[i]))-1)))*(int)(pow(10,(int)(log10(lengths[i]))-1)) == j + 1 || j + 1 == lengths[i]))
            //print when j+1 is a multiple of 10^x, where 10^x has 1 less digit than lengths[i]
            // and when it's finished
            //For example, if lengths[i] = 123456, it will print this when j+1 = 10000, 20000, ..., 120000, 123456
            //So it will print between 10 and 100 times: 10 when lengths[i] = 10^x and 100 when lengths[i] = 10^x - 1
            {
                cout << j + 1 << "/" << lengths[i] << ": "; 
                if (type == Profile || type == ScatterPlot || type == Resolution)
                    cout << x << ", " << y << endl;
                if (type == OrgHistogram)
                    cout << x << endl;
                if (type == Histogram)
                    cout << y << endl;
            }
        }
        lengths[i] -= notincluded;

        meansrmss[i] = "";
        if (type == Histogram || type == OrgHistogram)
        {
            cout << "Average = " << p[i]->GetMean() << endl;
            cout << "RMS     = " << p[i]->GetRMS()  << endl;
            stringstream meanrms;
            meanrms.precision(3);
            meanrms << "#mu=" << p[i]->GetMean() << ", #sigma=" << p[i]->GetRMS();
            meansrmss[i] = meanrms.str();
        }

        if (type == Resolution)
        {
            for (Int_t j = 0; j < nBinsProfileResolution; j++)
            {
                p[i]->SetBinContent(j+1,q[j]->GetRMS());
                p[i]->SetBinError  (j+1,q[j]->GetRMSError());
                delete q[j];
            }
        }

        if (type == Profile)
        {
            for (Int_t j = 0; j < nBinsProfileResolution; j++)
            {
                p[i]->SetBinContent(j+1,q[j]->GetMean());
                p[i]->SetBinError  (j+1,q[j]->GetMeanError());
                delete q[j];
            }
        }

        setAxisLabels(p[i],type,xvar,yvar,relative,pull);

        p[i]->SetLineColor(colors[i]);
        if (type == Resolution || type == Profile)
        {
            p[i]->SetMarkerColor(colors[i]);
            p[i]->SetMarkerStyle(20+i);
        }
        else
        {
            p[i]->SetMarkerColor(kWhite);
            p[i]->SetMarkerStyle(1);
        }
    }

    TH1 *firstp = 0;
    for (int i = 0; i < n; i++)
    {
        if (used[i])
        {
            firstp = p[i];
            break;
        }
    }
    if (firstp == 0)
    {
        stufftodelete->Clear();
        return 0;
    }

    TCanvas *c1 = TCanvas::MakeDefCanvas();

    TH1 *maxp = firstp;
    if (type == ScatterPlot)
        firstp->Draw("COLZ");
    else if (type == Resolution || type == Profile)
    {
        vector<TGraphErrors*> g;
        TMultiGraph *list = new TMultiGraph();
        for (Int_t i = 0, ii = 0; i < n; i++, ii++)
        {
            if (!used[i])
            {
                ii--;
                continue;
            }
            g.push_back(new TGraphErrors(p[i]));
            for (Int_t j = 0; j < g[ii]->GetN(); j++)
            {
                if (g[ii]->GetY()[j] == 0 && g[ii]->GetEY()[j] == 0)
                {
                    g[ii]->RemovePoint(j);
                    j--;
                }
            }
            list->Add(g[ii]);
        }
        list->Draw("AP");
        Double_t yaxismax = list->GetYaxis()->GetXmax();
        Double_t yaxismin = list->GetYaxis()->GetXmin();
        delete list;       //automatically deletes g[i]
        if (yaxismin > 0)
        {
            yaxismax += yaxismin;
            yaxismin = 0;
        }
        firstp->GetYaxis()->SetRangeUser(yaxismin,yaxismax);
    }
    else if (type == Histogram || type == OrgHistogram)
    {
        Bool_t allthesame = true;
        for (Int_t i = 1; i < n && allthesame; i++)
        {
            if (lengths[i] != lengths[0])
                allthesame = false;
        }
        if (!allthesame && xvar != "runNumber")
            for (Int_t i = 0; i < n; i++)
            {
                p[i]->Scale(1.0/lengths[i]);     //This does NOT include events that are out of the run number range (minrun and maxrun).
                                                 //It DOES include events that are out of the histogram range.
            }
        maxp = (TH1F*)firstp->Clone("maxp");
        stufftodelete->Add(maxp);
        maxp->SetBit(kCanDelete,true);
        maxp->SetLineColor(kWhite);
        for (Int_t i = 1; i <= maxp->GetNbinsX(); i++)
        {
            for (Int_t j = 0; j < n; j++)
            {
                if (!used[j])
                    continue;
                maxp->SetBinContent(i,TMath::Max(maxp->GetBinContent(i),p[j]->GetBinContent(i)));
            }
        }
        maxp->Draw();
    }

    TLegend *legend = new TLegend(.6,.7,.9,.9,"","br");
    stufftodelete->Add(legend);
    legend->SetBit(kCanDelete,true);
    if (n == 1 && !used[0])
    {
        deleteCanvas(c1);
        stufftodelete->Clear();
        return 0;
    }
    for (Int_t i = 0; i < n; i++)
    {
        if (!used[i])
            continue;
        if (type == Resolution || type == Profile)
        {
            if (p[i] == firstp)
                p[i]->Draw("P");
            else
                p[i]->Draw("same P");
            legend->AddEntry(p[i],names[i],"pl");
        }
        else if (type == Histogram || type == OrgHistogram)
        {
            p[i]->Draw("same");
            legend->AddEntry(p[i],names[i],"l");
            legend->AddEntry((TObject*)0,meansrmss[i],"");
        }
    }
    if (n>=2)
    {
        if (legend->GetListOfPrimitives()->At(0) == 0)
        {
            stufftodelete->Clear();
            deleteCanvas(c1);
            return 0;
        }

        
        c1->Update();
        Double_t x1min  = .98*gPad->GetUxmin() + .02*gPad->GetUxmax();
        Double_t x2max  = .02*gPad->GetUxmin() + .98*gPad->GetUxmax();
        Double_t y1min  = .98*gPad->GetUymin() + .02*gPad->GetUymax();
        Double_t y2max  = .02*gPad->GetUymin() + .98*gPad->GetUymax();
        Double_t width  = .4*(x2max-x1min);
        Double_t height = (1./20)*legend->GetListOfPrimitives()->GetEntries()*(y2max-y1min);
        if (type == Histogram || type == OrgHistogram)
        {
            width *= 2;
            height /= 2;
            legend->SetNColumns(2);
        }
        Double_t newy2max = placeLegend(legend,width,height,x1min,y1min,x2max,y2max);
        maxp->GetYaxis()->SetRangeUser(gPad->GetUymin(),(newy2max-.02*gPad->GetUymin())/.98);
                
        legend->SetFillStyle(0);
        legend->Draw();
    }

    if (saveas != "")
        saveplot(c1,saveas);

    return c1;
}


//make a 1D histogram of Delta_yvar

TCanvas *trackSplitPlot(Int_t nFiles,TString *files,TString *names,TString var,
                        Bool_t relative,Bool_t pull,TString saveas)
{
    return trackSplitPlot(nFiles,files,names,"",var,relative,false,pull,saveas);
}



//For 1 file

TCanvas *trackSplitPlot(TString file,TString xvar,TString yvar,Bool_t profile,
                        Bool_t relative,Bool_t resolution,Bool_t pull,
                        TString saveas)
{
    Int_t nFiles = 0;
    if (profile)                       //it interprets nFiles < 1 as 1 file, make a scatterplot
        nFiles = 1;
    TString *files = &file;
    TString name = "";
    TString *names = &name;
    return trackSplitPlot(nFiles,files,names,xvar,yvar,relative,resolution,pull,saveas);
}

//make a 1D histogram of Delta_yvar

TCanvas *trackSplitPlot(TString file,TString var,
                        Bool_t relative,Bool_t pull,
                        TString saveas)
{
    Int_t nFiles = 1;
    TString *files = &file;
    TString name = "";
    TString *names = &name;
    return trackSplitPlot(nFiles,files,names,var,relative,pull,saveas);
}

void placeholder(TString saveas,Bool_t wide)
{
    setTDRStyle();
    if (wide)
        gStyle->SetCanvasDefW(678);
    else
        gStyle->SetCanvasDefW(600);
    TText line1(.5,.6,"This is a placeholder so that when there are");
    TText line2(.5,.4,"4 plots per line it lines up nicely");
    line1.SetTextAlign(22);
    line2.SetTextAlign(22);
    TCanvas *c1 = TCanvas::MakeDefCanvas();
    line1.Draw();
    line2.Draw();
    if (saveas != "")
        saveplot(c1,saveas);
    deleteCanvas(c1);
}

void saveplot(TCanvas *c1,TString saveas)
{
    if (saveas == "")
        return;
    TString saveas2 = saveas,
            saveas3 = saveas,
            saveas4;
    saveas2.ReplaceAll(".pngepsroot","");
    saveas3.Remove(saveas3.Length()-11);
    if (saveas2 == saveas3)
    {
        stringstream s1,s2,s3;
        s1 << saveas2 << ".png";
        s2 << saveas2 << ".eps";
        s3 << saveas2 << ".root";
        saveas2 = s1.str();
        saveas3 = s2.str();
        saveas4 = s3.str();
        c1->SaveAs(saveas2);
        c1->SaveAs(saveas3);
        c1->SaveAs(saveas4);
        return;
    }
    else
    {
        c1->SaveAs(saveas);
    }
}

void deleteCanvas(TObject *canvas)
{
    if (canvas == 0) return;
    if (!canvas->InheritsFrom("TCanvas"))
    {
        delete canvas;
        return;
    }
    TCanvas *c1 = (TCanvas*)canvas;
    TList *list = c1->GetListOfPrimitives();
    list->SetOwner(true);
    list->Clear();
    delete c1;
}

//=============
//1. Make Plots
//=============

void makePlots(Int_t nFiles,TString *files,TString *names,TString misalignment,Double_t *values,Double_t *phases,TString directory,
               Bool_t matrix[xsize][ysize])
{
    stufftodelete->SetOwner(true);

    for (Int_t i = 0, totaltime = 0; i < nFiles; i++)
    {
        TFile *f = 0;
        bool exists = false;
        if (files[i] == "") exists = true;

        for (int j = 1; j <= 60*24 && !exists; j++, totaltime++)  //wait up to 1 day for the validation to be finished
        {
            f = TFile::Open(files[i]);
            if (f != 0)
                exists = f->IsOpen();
            delete f;
            if (exists) continue;
            gSystem->Sleep(60000);
            cout << "It's been ";
            if (j >= 60)
                cout << j/60 << " hour";
            if (j >= 120)
                cout << "s";
            if (j % 60 != 0 && j >= 60)
                cout << " and ";
            if (j % 60 != 0)
                cout << j%60 << " minute";
            if (j % 60 >= 2)
                cout << "s";
            cout << endl;
        }
        if (!exists) return;
        if (i == nFiles - 1 && totaltime > nFiles)
            gSystem->Sleep(60000);
    }

    TString directorytomake = directory;
    gSystem->mkdir(directorytomake,true);
    if (misalignment != "")
    {
        directorytomake.Append("/fits");
        gSystem->mkdir(directorytomake);
    }

    for (Int_t x = 0; x < xsize; x++)
    {
        for (Int_t y = 0; y < ysize; y++)
        {
            for (Int_t pull = 0; pull == 0 || (pull == 1 && yvariables[y] != ""); pull++)
            {
                if (false) continue;        //this line is to make it easier to do e.g. all plots involving Delta eta
                                            //(replace false with yvariables[y] != "eta")

                if (!matrix[x][y]) continue;

                if (xvariables[x] == "" && yvariables[y] == "") continue;

                Int_t nPlots = nFiles+4;                     //scatterplot for each (if you uncomment it), profile, resolution, and fits for each.
                vector<TString> s;

                TString slashstring = "";
                if (directory.Last('/') != directory.Length() - 1) slashstring = "/";

                vector<TString> plotnames;
                for (Int_t i = 0; i < nFiles; i++)
                {
                    plotnames.push_back(names[i]);   //this is plotnames[i]
                    plotnames[i].ReplaceAll(" ","");
                }

                plotnames.push_back("");             //this is plotnames[nFiles], but gets changed
                if (yvariables[y] == "")
                    plotnames[nFiles] = "orghist";
                else if (xvariables[x] == "")
                    plotnames[nFiles] = "hist";
                else
                    plotnames[nFiles] = "profile";

                plotnames.push_back("resolution");   //this is plotnames[nFiles+1]

                plotnames.push_back("");             //this is plotnames[nFiles+2]
                plotnames.push_back("");             //this is plotnames[nFiles+3]
                if (plotnames[nFiles] == "profile")
                {
                    plotnames[nFiles+2] = ".profile";
                    plotnames[nFiles+2].Prepend(misalignment);
                    plotnames[nFiles+3] = ".resolution";
                    plotnames[nFiles+3].Prepend(misalignment);
                    plotnames[nFiles+2].Prepend("fits/");
                    plotnames[nFiles+3].Prepend("fits/");
                }
                else
                {
                    plotnames[nFiles+2] = "profile.";
                    plotnames[nFiles+2].Append(misalignment);
                    plotnames[nFiles+3] = "resolution.";
                    plotnames[nFiles+3].Append(misalignment);
                }

                TString pullstring = "";
                if (pull) pullstring = "pull.";

                TString xvarstring = xvariables[x];
                if (xvariables[x] != "runNumber" && xvariables[x] != "nHits" && xvariables[x] != "") xvarstring.Append("_org");
                if (xvariables[x] != "" && yvariables[y] != "") xvarstring.Append(".");

                TString yvarstring = yvariables[y];
                if (yvariables[y] != "") yvarstring.Prepend("Delta_");

                TString relativestring = "";
                if (relativearray[y]) relativestring = ".relative";

                for (Int_t i = 0; i < nPlots; i++)
                {
                    stringstream ss;
                    ss << directory << slashstring << plotnames[i] << "." << pullstring 
                       << xvarstring << yvarstring << relativestring << ".pngepsroot";
                    s.push_back(ss.str());
                    if (misalignment != "")
                    {
                        TString wrongway = misalignment;
                        TString rightway = misalignment;
                        wrongway.Append (".pull");
                        rightway.Prepend("pull.");
                        s[i].ReplaceAll(wrongway,rightway);
                    }
                }

                Int_t i;
                for (i = 0; i < nFiles; i++)
                {
                    if (xvariables[x] == "" || yvariables[y] == "") continue;
                    //uncomment this section to make scatterplots
                    /*
                    trackSplitPlot(files[i],xvariables[x],yvariables[y],false,relativearray[y],false,(bool)pull,s[i]);
                    stufftodelete->Clear();
                    for ( ; gROOT->GetListOfCanvases()->GetEntries() > 0; )
                        deleteCanvas( gROOT->GetListOfCanvases()->Last());
                    for ( ; gROOT->GetListOfFiles()->GetEntries() > 0; )
                        delete (TFile*)gROOT->GetListOfFiles()->Last();
                    */
                }

                if (xvariables[x] != "" && yvariables[y] != "")
                {
                    //make profile
                    TCanvas *c1 = trackSplitPlot(nFiles,files,names,xvariables[x],yvariables[y],relativearray[y],false,(bool)pull,s[i]);
                    if (misalignmentDependence(c1,nFiles,names,misalignment,values,phases,xvariables[x],yvariables[y],
                                               true,relativearray[y],false,(bool)pull,s[i+2]))
                    {
                        s[i+2].ReplaceAll(".png",".parameter.png");
                        misalignmentDependence(c1,nFiles,names,misalignment,values,phases,xvariables[x],yvariables[y],
                                                   false,relativearray[y],false,(bool)pull,s[i+2]);
                    }
                    stufftodelete->Clear();
                    for ( ; gROOT->GetListOfCanvases()->GetEntries() > 0; )
                        deleteCanvas( gROOT->GetListOfCanvases()->Last());
                    for ( ; gROOT->GetListOfFiles()->GetEntries() > 0; )
                        delete (TFile*)gROOT->GetListOfFiles()->Last();

                    //make resolution plot
                    TCanvas *c2 = trackSplitPlot(nFiles,files,names,xvariables[x],yvariables[y],relativearray[y],true ,(bool)pull,s[i+1]);
                    if (misalignmentDependence(c2,nFiles,names,misalignment,values,phases,xvariables[x],yvariables[y],
                                               true,relativearray[y],true,(bool)pull,s[i+3]))
                    {
                        s[i+3].ReplaceAll(".png",".parameter.png");
                        misalignmentDependence(c2,nFiles,names,misalignment,values,phases,xvariables[x],yvariables[y],
                                                   false,relativearray[y],true,(bool)pull,s[i+3]);
                    }
                    stufftodelete->Clear();
                    for ( ; gROOT->GetListOfCanvases()->GetEntries() > 0; )
                        deleteCanvas( gROOT->GetListOfCanvases()->Last());
                    for ( ; gROOT->GetListOfFiles()->GetEntries() > 0; )
                        delete (TFile*)gROOT->GetListOfFiles()->Last();
                }
                else
                {
                    //make histogram
                    TCanvas *c1 = trackSplitPlot(nFiles,files,names,xvariables[x],yvariables[y],relativearray[y],false,(bool)pull,s[i]);
                    if (misalignmentDependence(c1,nFiles,names,misalignment,values,phases,xvariables[x],yvariables[y],
                                               true,relativearray[y],false,(bool)pull,s[i+2]))
                    {
                        misalignmentDependence(c1,nFiles,names,misalignment,values,phases,xvariables[x],yvariables[y],
                                               true,relativearray[y],true,(bool)pull,s[i+3]);
                    }
                    stufftodelete->Clear();
                    for ( ; gROOT->GetListOfCanvases()->GetEntries() > 0; )
                        deleteCanvas( gROOT->GetListOfCanvases()->Last());
                    for ( ; gROOT->GetListOfFiles()->GetEntries() > 0; )
                        delete (TFile*)gROOT->GetListOfFiles()->Last();
                }
            }
            cout << y + ysize * x + 1 << "/" << xsize*ysize << endl;
        }
    }
}

void makePlots(Int_t nFiles,TString *files,TString *names,TString directory, Bool_t matrix[xsize][ysize])
{
    makePlots(nFiles,files,names,"",(Double_t*)0,(Double_t*)0,directory,
              matrix);
}

void makePlots(TString file,TString directory,Bool_t matrix[xsize][ysize])
{
    TString *files = &file;
    TString name = "scatterplot";     //With 1 file there's no legend, so this is only used in the filename of the scatterplots, if made
    TString *names = &name;
    makePlots(1,files,names,directory,matrix);
}

//***************************************************************************
//functions to make plots for 1 row, column, or cell of the matrix
//examples:
//   xvar = "nHits", yvar = "ptrel" - makes plots of nHits vs Delta_pt/pt_org
//   xvar = "all",   yvar = "pt"    - makes all plots involving Delta_pt
//                                    (not Delta_pt/pt_org)
//   xvar = "",      yvar = "all"   - makes all histograms of Delta_???
//                                    (including Delta_pt/pt_org)
//***************************************************************************

void makePlots(Int_t nFiles,TString *files,TString *names,TString misalignment,Double_t *values,Double_t *phases,TString directory,
               TString xvar,TString yvar)
{
    Bool_t matrix[xsize][ysize];
    for (int x = 0; x < xsize; x++)
        for (int y = 0; y < ysize; y++)
        {
            bool xmatch = (xvar == "all" || xvar == xvariables[x]);
            bool ymatch = (yvar == "all" || yvar == yvariables[y]);
            if (yvar == "pt" && yvariables[y] == "pt" && relativearray[y] == true)
                ymatch = false;
            if (yvar == "ptrel" && yvariables[y] == "pt" && relativearray[y] == true)
                ymatch = true;
            matrix[x][y] = (xmatch && ymatch);
        }
    makePlots(nFiles,files,names,misalignment,values,phases,directory,matrix);
}

void makePlots(Int_t nFiles,TString *files,TString *names,TString directory,
               TString xvar,TString yvar)
{
    makePlots(nFiles,files,names,"",(Double_t*)0,(Double_t*)0,directory,
              xvar,yvar);
}

void makePlots(TString file,TString directory,
               TString xvar,TString yvar)
{
    TString *files = &file;
    TString name = "scatterplot";     //With 1 file there's no legend, so this is only used in the filename of the scatterplots, if made
    TString *names = &name;
    makePlots(1,files,names,directory,
              xvar,yvar);
}

//***************************
//functions to make all plots
//***************************

void makePlots(Int_t nFiles,TString *files,TString *names,TString misalignment,Double_t *values,Double_t *phases,TString directory)
{
    makePlots(nFiles,files,names,misalignment,values,phases,directory,"all","all");
}

void makePlots(Int_t nFiles,TString *files,TString *names,TString directory)
{
    makePlots(nFiles,files,names,"",(Double_t*)0,(Double_t*)0,directory);
}

void makePlots(TString file,TString directory)
{
    TString *files = &file;
    TString name = "scatterplot";     //With 1 file there's no legend, so this is only used in the filename of the scatterplots, if made
    TString *names = &name;
    makePlots(1,files,names,directory);
}

//=============
//2. Axis Label
//=============

TString fancyname(TString variable)
{
    if (variable == "pt")
        return "p_{T}";
    else if (variable == "phi")
        return "#phi";
    else if (variable == "eta")
        return "#eta";
    else if (variable == "theta")
        return "#theta";
    else if (variable == "qoverpt")
        return "(q/p_{T})";
    else if (variable == "runNumber")
        return "run number";
    else
        return variable;
}

//this gives the units, to be put in the axis label
TString units(TString variable,Char_t axis)
{
    if (variable == "pt")
        return "GeV";
    if (variable == "dxy" || variable == "dz")
    {
        if (axis == 'y')
            return "#mum";      //in the tree, it's listed in centimeters, but in trackSplitPlot the value is divided by 1e4
        if (axis == 'x')
            return "cm";
    }
    if (variable == "qoverpt")
        return "e/GeV";
    return "";
}


//this gives the full axis label, including units.  It can handle any combination of relative, resolution, and pull.
TString axislabel(TString variable, Char_t axis, Bool_t relative, Bool_t resolution, Bool_t pull)
{
    stringstream s;
    if (resolution && axis == 'y')
        s << "#sigma(";
    if (axis == 'y')
        s << "#Delta";
    s << fancyname(variable);
    if (relative && axis == 'y')
        s << " / " << fancyname(variable);
    Bool_t nHits = (variable[0] == 'n' && variable[1] == 'H' && variable[2] == 'i'
                                       && variable[3] == 't' && variable[4] == 's');
    if (relative || (axis == 'x' && variable != "runNumber" && !nHits))
        s << "_{org}";
    if (pull && axis == 'y')
    {
        s << " / #delta(#Delta" << fancyname(variable);
        if (relative)
            s << " / " << fancyname(variable) << "_{org}";
        s << ")";
    }
    if (resolution && axis == 'y')
        s << ")";
    if (((!relative && !pull) || axis == 'x') && units(variable,axis) != "")
        s << " (" << units(variable,axis) << ")";
    return s.str();
}

void setAxisLabels(TH1 *p, PlotType type,TString xvar,TString yvar,Bool_t relative,Bool_t pull)
{
    if (type == Histogram)
        p->SetXTitle(axislabel(yvar,'y',relative,false,pull));
    if (type == ScatterPlot || type == Profile || type == Resolution || type == OrgHistogram)
        p->SetXTitle(axislabel(xvar,'x'));

    if (type == ScatterPlot || type == Profile)
        p->SetYTitle(axislabel(yvar,'y',relative,false,pull));
    if (type == Resolution)
        p->SetYTitle(axislabel(yvar,'y',relative,true,pull));
}

void setAxisLabels(TMultiGraph *p, PlotType type,TString xvar,TString yvar,Bool_t relative,Bool_t pull)
{
    if (type == Histogram)
        p->GetXaxis()->SetTitle(axislabel(yvar,'y',relative,false,pull));
    if (type == ScatterPlot || type == Profile || type == Resolution || type == OrgHistogram)
        p->GetXaxis()->SetTitle(axislabel(xvar,'x'));

    if (type == ScatterPlot || type == Profile)
        p->GetYaxis()->SetTitle(axislabel(yvar,'y',relative,false,pull));
    if (type == Resolution)
        p->GetYaxis()->SetTitle(axislabel(yvar,'y',relative,true,pull));
}


TString nPart(Int_t part,TString string,TString delimit)
{
    if (part <= 0) return "";
    for (int i = 1; i < part; i++)    //part-1 times
    {
        if (string.Index(delimit) < 0) return "";
        string.Replace(0,string.Index(delimit)+1,"",0);
    }
    if (string.Index(delimit) >= 0)
        string.Remove(string.Index(delimit));
    return string;
}

//==============
//3. Axis limits
//==============


Double_t findStatistic(Statistic what,Int_t nFiles,TString *files,TString var,Char_t axis,Bool_t relative,Bool_t pull)
{
    Double_t x = 0,              //if axis == 'x', var_org goes in x; if axis == 'y', Delta_var goes in x
             rel = 1,            //if relative, var_org goes in rel.  x is divided by rel, so you get Delta_var/var_org
             sigma1 = 1,         //if pull, the error for split track 1 goes in sigma1 and the error for split track 2 goes in sigma2.
             sigma2 = 1,         //x is divided by sqrt(sigma1^2+sigma2^2).  If !pull && axis == 'y', this divides by sqrt(2)
             sigmaorg = 0;       // because we want the error in one track.  sigmaorg is used when relative && pull
    Int_t xint = 0, xint2 = 0;   //xint is used for run number and nHits.  xint2 is used for nhits because each event has 2 values.

    Int_t runNumber = 0;         //this is used to make sure the run number is between minrun and maxrun

    if (axis == 'x')
    {
        sigma1 = 1/sqrt(2);      //if axis == 'x' don't divide by sqrt(2)
        sigma2 = 1/sqrt(2);
    }

    Double_t totallength = 0;
    Double_t result = 0;
    if (what == Minimum) result = 1e100;
    if (what == Maximum) result = -1e100;

    Double_t average = 0;
    if (what == RMS)
        average = findStatistic(Average,nFiles,files,var,axis,relative,pull);

    Bool_t nHits = (var[0] == 'n' && var[1] == 'H' && var[2] == 'i'    //includes nHits, nHitsTIB, etc.
                                  && var[3] == 't' && var[4] == 's');

    stringstream sx,srel,ssigma1,ssigma2,ssigmaorg;

    if (axis == 'y')
        sx << "Delta_";
    sx << var;
    if (axis == 'x' && var != "runNumber" && !nHits)
        sx << "_org";
    if (axis == 'x' && nHits)
        sx << "1_spl";
    TString variable = sx.str(),
            variable2 = variable;
    variable2.ReplaceAll("1_spl","2_spl");

    TString relvariable = "1";
    if (relative)
    {
        srel << var << "_org";
        relvariable = srel.str();
    }

    if (pull)
    {
        ssigma1 << var << "1Err_spl";
        ssigma2 << var << "2Err_spl";
    }
    TString sigma1variable = ssigma1.str();
    TString sigma2variable = ssigma2.str();

    if (pull && relative)
        ssigmaorg << var << "Err_org";
    TString sigmaorgvariable = ssigmaorg.str();

    if (!relative && !pull && (variable == "Delta_dxy" || variable == "Delta_dz"))
        rel = 1e-4;                                           //it's in cm but we want um

    for (Int_t j = 0; j < nFiles; j++)
    {
        if ((files[j].Contains("MC") && var == "runNumber") || files[j] == "")   //because then run number is meaningless
            continue;
        TFile *f = TFile::Open(files[j]);
        TTree *tree = (TTree*)f->Get("cosmicValidation/splitterTree");
        if (tree == 0)
            tree = (TTree*)f->Get("splitterTree");
        Int_t length = tree->GetEntries();

        tree->SetBranchAddress("runNumber",&runNumber);
        if (var == "runNumber")
            tree->SetBranchAddress(variable,&xint);
        else if (nHits)
        {
            tree->SetBranchAddress(variable,&xint);
            tree->SetBranchAddress(variable2,&xint2);
        }
        else
            tree->SetBranchAddress(variable,&x);

        if (relative)
            tree->SetBranchAddress(relvariable,&rel);
        if (pull)
        {
            tree->SetBranchAddress(sigma1variable,&sigma1);
            tree->SetBranchAddress(sigma2variable,&sigma2);
        }
        if (relative && pull)
            tree->SetBranchAddress(sigmaorgvariable,&sigmaorg);

        for (Int_t i = 0; i<length; i++)
        {
            tree->GetEntry(i);
            if (var == "runNumber" || nHits)
                x = xint;
            if (var == "runNumber")
                runNumber = x;
            if (runNumber < minrun || (runNumber > maxrun && maxrun > 0)) continue;
            
            totallength++;

            Double_t error;
            if (relative && pull)
                error = sqrt((sigma1/rel)*(sigma1/rel) + (sigma2/rel)*(sigma2/rel) + (sigmaorg*x/(rel*rel))*(sigmaorg*x/(rel*rel)));
            else
                error = sqrt(sigma1 * sigma1 + sigma2 * sigma2);   // = 1 if axis == 'x' && !pull
                                                                   // = sqrt(2) if axis == 'y' && !pull, so that you get the error in 1 track
                                                                   //       when you divide by it
            x /= (rel * error);
            if (what == Minimum && x < result)
                result = x;
            if (what == Maximum && x > result)
                result = x;
            if (what == Average)
                result += x;
            if (what == RMS)
                result += (x - average) * (x - average);
            if (nHits)
            {
                x = xint2;
                if (what == Minimum && x < result)
                    result = x;
                if (what == Maximum && x > result)
                    result = x;
                if (what == Average)
                    result += x;
                if (what == RMS)
                    result += (x - average) * (x - average);
            }
        }
        delete f;         //automatically closes the file
    }
    if (nHits) totallength *= 2;
    if (what == Average) result /= totallength;
    if (what == RMS)  result = sqrt(result / (totallength - 1));
    return result;
}

Double_t findAverage(Int_t nFiles,TString *files,TString var,Char_t axis,Bool_t relative,Bool_t pull)
{
    return findStatistic(Average,nFiles,files,var,axis,relative,pull);
}

Double_t findMin(Int_t nFiles,TString *files,TString var,Char_t axis,Bool_t relative,Bool_t pull)
{
    return findStatistic(Minimum,nFiles,files,var,axis,relative,pull);
}

Double_t findMax(Int_t nFiles,TString *files,TString var,Char_t axis,Bool_t relative,Bool_t pull)
{
    return findStatistic(Maximum,nFiles,files,var,axis,relative,pull);
}

Double_t findRMS(Int_t nFiles,TString *files,TString var,Char_t axis,Bool_t relative,Bool_t pull)
{
    return findStatistic(RMS,nFiles,files,var,axis,relative,pull);
}


//These functions are for 1 file

Double_t findStatistic(Statistic what,TString file,TString var,Char_t axis,Bool_t relative,Bool_t pull)
{
    return findStatistic(what,1,&file,var,axis,relative,pull);
}

Double_t findAverage(TString file,TString var,Char_t axis,Bool_t relative,Bool_t pull)
{
    return findStatistic(Average,file,var,axis,relative,pull);
}

Double_t findMin(TString file,TString var,Char_t axis,Bool_t relative,Bool_t pull)
{
    return findStatistic(Minimum,file,var,axis,relative,pull);
}

Double_t findMax(TString file,TString var,Char_t axis,Bool_t relative,Bool_t pull)
{
    return findStatistic(Maximum,file,var,axis,relative,pull);
}

Double_t findRMS(TString file,TString var,Char_t axis,Bool_t relative,Bool_t pull)
{
    return findStatistic(RMS,file,var,axis,relative,pull);
}




//This puts the axis limits that should be used for trackSplitPlot in min and max.
//Default axis limits are defined for pt, qoverpt, dxy, dz, theta, eta, and phi.
//For run number and nHits, the minimum and maximum are used.
//For any other variable, average +/- 5*rms are used.
//To use this instead of the default values, just comment out the part that says [else] if (var == "?") {min = ?; max = ?;}

void axislimits(Int_t nFiles,TString *files,TString var,Char_t axis,Bool_t relative,Bool_t pull,Double_t &min,Double_t &max)
{
    if (axis == 'x')
    {
        Bool_t nHits = (var[0] == 'n' && var[1] == 'H' && var[2] == 'i'
                                      && var[3] == 't' && var[4] == 's');
        if (var == "pt")
        {
            min = 5;
            max = 100;
        }
        else if (var == "qoverpt")
        {
            min = -.35;
            max = .35;
        }
        else if (var == "dxy")
        {
            min = -10;
            max = 10;
        }
        else if (var == "dz")
        {
            min = -25;
            max = 25;
        }
        else if (var == "theta")
        {
            min = .5;
            max = 2.5;
        }
        else if (var == "eta")
        {
            min = -1.2;
            max = 1.2;
        }
        else if (var == "phi")
        {
            min = -3;
            max = 0;
        }
        else if (var == "runNumber" || nHits)
        {
            min = findMin(nFiles,files,var,'x') - .5;
            max = findMax(nFiles,files,var,'x') + .5;
        }
        else
        {
            cout << "No x axis limits for " << var << ".  Using average +/- 5*rms" << endl;
            Double_t average = findAverage(nFiles,files,var,'x');
            Double_t rms = findRMS (nFiles,files,var,'x');
            max = TMath::Min(average + 5 * rms,findMax(nFiles,files,var,'x'));
            min = TMath::Max(average - 5 * rms,findMin(nFiles,files,var,'x'));
        }
    }
    if (axis == 'y')
    {
        if (pull)
        {
            min = -5;
            max = 5;
        }
        else if (var == "pt" && relative)
        {
            min = -.06;
            max = .06;
        }
        else if (var == "pt" && !relative)
        {
            min = -.8;
            max = .8;
        }
        else if (var == "qoverpt")
        {
            min = -.0025;
            max = .0025;
        }
        else if (var == "dxy")
        {
            min = -125;
            max = 125;
        }
        else if (var == "dz")
        {
            min = -200;
            max = 200;
        }
        else if (var == "theta")
        {
            min = -.005;
            max = .005;
        }
        else if (var == "eta")
        {
            min = -.003;
            max = .003;
        }
        else if (var == "phi")
        {
            min = -.002;
            max = .002;
        }
        else
        {
            cout << "No y axis limits for " << var << ".  Using average +/- 5 * rms." << endl;
            Double_t average = 0 /*findAverage(nFiles,files,var,'y',relative,pull)*/;
            Double_t rms = findRMS (nFiles,files,var,'y',relative,pull);
            min = TMath::Max(TMath::Max(-TMath::Abs(average) - 5*rms,
                             findMin(nFiles,files,var,'y',relative,pull)),
                             -findMax(nFiles,files,var,'y',relative,pull));
            max = -min;
        }
    }
}

#endif
