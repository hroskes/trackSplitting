#include "tdrstyle.C"
#include "averages.C"
#include "placeLegend.C"
#include "TCanvas.h"
#include "TStyle.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TGraphErrors.h"
#include "TMultiGraph.h"
#include "TText.h"
#include <iostream>

using namespace std;

void placeholder(TString saveas = "",Bool_t wide = false);
void saveplot(TCanvas *c1,TString saveas);

//profile =  true:   profile
//profile =  false:  scatter plot
//relative = false:  xvar_org is on the x axis, Delta_yvar is on the y axis
//relative = true:   xvar_org is on the x axis, Delta_yvar / yvar_org is on the y axis

const Int_t nColors = 13;
Color_t colors[nColors] = {1,2,4,3,kMagenta,kCyan,kYellow,
                           kOrange,kPink-2,kTeal+9,kAzure-8,kViolet-6,kSpring-1};

Int_t binsScatterPlotx = 1000;
Int_t binsScatterPloty = 1000;
Int_t binsHistogram = 100;
Int_t runNumberBins = 30;
Int_t binsProfileResolution = 8;     //for everything but runNumber and nHits

void trackSplitPlot(Int_t nFiles,TString *files,TString *names,TString xvar,TString yvar,
                    Bool_t relative = kFALSE,Bool_t logscale = kFALSE,Bool_t resolution = kFALSE,Bool_t pull = kFALSE,
                    TString saveas = "")
{
    cout << xvar << " " << yvar << endl;
    if (xvar == "" && yvar == "")
        return;

    PlotType type;
    if (xvar == "")      type = Histogram;
    else if (yvar == "") type = OrgHistogram;
    else if (resolution) type = Resolution;
    else if (nFiles < 1) type = ScatterPlot;
    else                 type = Profile;
    if (nFiles < 1) nFiles = 1;

    const Int_t n = nFiles;
    
    TStyle *tdrStyle = setTDRStyle();
    tdrStyle->SetOptStat(0);
    if ((type == Histogram || type == OrgHistogram) && nFiles == 1)
        tdrStyle->SetOptStat(1110);
    if (type == ScatterPlot || (type == Histogram && !pull) || xvar == "runNumber")
    {
        tdrStyle->SetCanvasDefW(678);
        tdrStyle->SetPadRightMargin(0.115);
    }

    if (relative && pull)
    {
        placeholder(saveas,true);
        return;
    }

    Bool_t nHits = (xvar[0] == 'n' && xvar[1] == 'H' && xvar[2] == 'i'
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

    TH1 *p[n];
    Int_t lengths[n];

    stringstream sx,sy,srel,ssigma1,ssigma2;

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


    Double_t xaverage,xrms,xmin = -1,xmax = 1,yaverage,yrms,ymin = -1,ymax = 1;
    if (type == Profile || type == ScatterPlot || type == OrgHistogram || type == Resolution)
    {

        if (xvar == "pt")
        {
            xmin = 5;
            xmax = 100;
        }
        else if (xvar == "qoverpt")
        {
            xmin = -.35;
            xmax = .35;
        }
        else if (xvar == "dxy")
        {
            xmin = -10;
            xmax = 10;
        }
        else if (xvar == "dz")
        {
            xmin = -25;
            xmax = 25;
        }
        else if (xvar == "theta")
        {
            xmin = 0;
            xmax = 2.5;
        }
        else if (xvar == "eta")
        {
            xmin = -1.2;
            xmax = 1.2;
        }
        else if (xvar == "phi")
        {
            xmin = -3;
            xmax = 0;
        }
        else if (xvar == "runNumber" || nHits)
        {
            xmin = findMin(nFiles,files,xvar,'x') - .5;
            xmax = findMax(nFiles,files,xvar,'x') + .5;
        }
        else
        {
            cout << "No x axis limits for " << xvar << ".  Using average +/- 5*rms" << endl;
            xaverage = findAverage(nFiles,files,xvar,'x');
            xrms     = findRMS    (nFiles,files,xvar,'x');
            xmax     = TMath::Min(xaverage + 5 * xrms,findMax(nFiles,files,xvar,'x'));
            xmin     = TMath::Max(xaverage - 5 * xrms,findMin(nFiles,files,xvar,'x'));
        }
    }
    if (type == Profile || type == ScatterPlot || type == Histogram || type == Resolution)
    {
        if (pull)
        {
            ymin = -5;
            ymax = 5;
        }
        else if (yvar == "pt" && relative)
        {
            ymin = -.06;
            ymax = .06;
        }
        else if (yvar == "pt" && !relative)
        {
            ymin = -.8;
            ymax = .8;
        }
        else if (yvar == "qoverpt")
        {
            ymin = -.0025;
            ymax = .0025;
        }
        else if (yvar == "dxy")
        {
            ymin = -125;
            ymax = 125;
        }
        else if (yvar == "dz")
        {
            ymin = -200;
            ymax = 200;
        }
        else if (yvar == "theta")
        {
            ymin = -.005;
            ymax = .005;
        }
        else if (yvar == "eta")
        {
            ymin = -.003;
            ymax = .003;
        }
        else if (yvar == "phi")
        {
            ymin = -.002;
            ymax = .002;
        }
        else
        {
            cout << "No y axis limits for " << yvar << ".  Using average +/- 5 * rms." << endl;
            yaverage = 0 /*findAverage(nFiles,files,yvar,'y',relative,pull)*/;
            yrms     = findRMS (nFiles,files,yvar,'y',relative,pull);
            ymin     = TMath::Max(TMath::Max(-TMath::Abs(yaverage) - 5*yrms,
                                  findMin(nFiles,files,yvar,'y',relative,pull)),
                                  -findMax(nFiles,files,yvar,'y',relative,pull));
            ymax     = -ymin;
        }
    }

    TString meansrmss[n];

    for (Int_t i = 0; i < n; i++)
    {
        TFile *f = TFile::Open(files[i]);
        TTree *tree = (TTree*)f->Get("splitterTree");

        stringstream sid;
        sid << "p" << i;
        TString id = sid.str();

        TH1F **q = new TH1F*[nBinsProfileResolution];
        if (type == ScatterPlot)
            p[i] = new TH2F(id,"",nBinsScatterPlotx,xmin,xmax,nBinsScatterPloty,ymin,ymax);
        if (type == Histogram)
            p[i] = new TH1F(id,"",nBinsHistogram,ymin,ymax);
        if (type == OrgHistogram)
            p[i] = new TH1F(id,"",nBinsHistogram,xmin,xmax);
        if (type == Resolution || type == Profile)
        {
            p[i] = new TH1F(id,"",nBinsProfileResolution,xmin,xmax);
            for (Int_t j = 0; j < nBinsProfileResolution; j++)
            {
                stringstream sid2;
                sid2 << "q" << j;
                TString id2 = sid2.str();
                q[j] = new TH1F(id2,"",nBinsHistogram,ymin,ymax);
            }
        }
        p[i]->SetDirectory(0);

        lengths[i] = tree->GetEntries();

        if (files[i].Contains("MC") && xvar == "runNumber")
        {
            p[i]->SetLineColor(kWhite);
            p[i]->SetMarkerColor(kWhite);
            f->Close();
            continue;
        }

        Double_t x = 0, y = 0, rel = 1, sigma1 = 1, sigma2 = 1;           //if !pull, we want to divide by sqrt(2) because we want the error from 1 track
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
            tree->SetBranchAddress(yvariable,&y);
        if (relative && xvar != yvar)                       //if xvar == yvar, setting the branch here will undo setting it to x 2 lines earlier
            tree->SetBranchAddress(relvariable,&rel);
        if (pull)
        {
            tree->SetBranchAddress(sigma1variable,&sigma1);
            tree->SetBranchAddress(sigma2variable,&sigma2);
        }
   
        Int_t notincluded = 0;

        for (Int_t j = 0; j<lengths[i]; j++)
        {
            tree->GetEntry(j);
            if (xvar == "runNumber" || nHits)
                x = xint;
            if (xvar == "runNumber")
                runNumber = x;
            if (runNumber < minrun || (runNumber > maxrun && maxrun > 0)) 
            {
                notincluded++;
                continue;
            }
            if (relative && xvar == yvar)
            {
                rel = x;
            }
            y /= (rel * sqrt(sigma1 * sigma1 + sigma2 * sigma2));        //  If !relative, rel == 1 from before
                                                                         //  If !pull, we want to divide by sqrt(2) because we want the error from 1 track
            if (logscale)
                y = fabs(y);
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

            if (((j+1)/(int)(pow(10,(int)(log10(lengths[i]))-1)))*(int)(pow(10,(int)(log10(lengths[i]))-1)) == j + 1 || j + 1 == lengths[i])
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
        delete[] q;

        setAxisLabels(p[i],type,xvar,yvar,relative,pull);

        p[i]->SetLineColor(colors[i]);
        if (type == Resolution || type == Profile)
        {
            p[i]->SetMarkerColor(colors[i]);
            p[i]->SetMarkerStyle(20+i);
        }
        else
        {
            p[i]->SetMarkerStyle(1);
        }
        f->Close();
    }

    TCanvas *c1 = TCanvas::MakeDefCanvas();
    if (type == ScatterPlot || type == Profile || type == Resolution || type == OrgHistogram)
        c1->SetLogy((Bool_t)(logscale));
    if (type == Histogram)
        c1->SetLogx((Bool_t)(logscale));

    TH1 *maxp = p[0];
    TGraphErrors *g[n];
    TMultiGraph *list = new TMultiGraph();
    if (type == ScatterPlot)
        p[0]->Draw("COLZ");
    else if (type == Resolution || type == Profile)
    {
        for (Int_t i = 0; i < n; i++)
        {
            if (files[i].Contains("MC") && xvar == "runNumber")
                continue;
            g[i] = new TGraphErrors(p[i]);
            for (Int_t j = 0; j < g[i]->GetN(); j++)
            {
                if (g[i]->GetY()[j] == 0 && g[i]->GetEY()[j] == 0)
                {
                    g[i]->RemovePoint(j);
                    j--;
                }
            }
            list->Add(g[i]);
        }
        list->Draw("AP");
        Double_t yaxismax = list->GetYaxis()->GetXmax();
        Double_t yaxismin = list->GetYaxis()->GetXmin();
        if (yaxismin > 0)
        {
            yaxismax += yaxismin;
            yaxismin = 0;
        }
        p[0]->GetYaxis()->SetRangeUser(yaxismin,yaxismax);
        p[0]->Draw("P");
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
                p[i]->Scale(1.0/lengths[i]);
            }
        maxp = (TH1F*)p[0]->Clone("maxp");
        maxp->SetLineColor(kWhite);
        for (Int_t i = 1; i <= maxp->GetNbinsX(); i++)
        {
            for (Int_t j = 0; j < n; j++)
            {
                if (files[j].Contains("MC") && xvar == "runNumber")
                    continue;
                maxp->SetBinContent(i,TMath::Max(maxp->GetBinContent(i),p[j]->GetBinContent(i)));
            }
        }
        maxp->Draw();
        p[0]->Draw("same");
    }
    TLegend *legend = new TLegend(.6,.7,.9,.9,"","br");
    if (n == 1 && files[0].Contains("MC") && xvar == "runNumber")
    {
        placeholder(saveas,yvar == "");
        return;
    }
    if (n>=2)
    {
        if (!(files[0].Contains("MC") && xvar == "runNumber") && files[0] != "")
        {
            if (type == Resolution || type == Profile)
                legend->AddEntry(p[0],names[0],"pl");
            else if (type == Histogram || type == OrgHistogram)
            {
                legend->AddEntry(p[0],names[0],"l");
                legend->AddEntry((TObject*)0,meansrmss[0],"");
            }
        }
        for (Int_t i = 1; i < n; i++)
        {
            if (files[i].Contains("MC") && xvar == "runNumber")
                continue;
            if (type == Resolution || type == Profile)
            {
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

        if (legend->GetListOfPrimitives()->At(0) == 0)
        {
            delete legend;
            placeholder(saveas,yvar == "");
            return;
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
    {
        saveplot(c1,saveas);
        for (int i = 0; i < n; i++)
        {
            //delete p[i];
            //delete g[i];
        }
        //delete list;
        //delete maxp;
        //delete legend;
    }
}

void trackSplitPlot(Int_t nFiles,TString *files,TString *names,TString var,
                    Bool_t relative = kFALSE,Bool_t logscale = kFALSE,Bool_t pull = kFALSE,TString saveas = "")
{
    return trackSplitPlot(nFiles,files,names,"",var,relative,logscale,false,pull,saveas);
}



//For 1 file

void trackSplitPlot(TString file,TString xvar,TString yvar,Bool_t profile = kFALSE,
                    Bool_t relative = kFALSE,Bool_t logscale = kFALSE,Bool_t resolution = kFALSE,Bool_t pull = kFALSE,
                    TString saveas = "")
{
    Int_t nFiles = 0;
    if (profile)                       //it interprets nFiles < 1 as 1 file, make a scatterplot
        nFiles = 1;
    TString *files = &file;
    TString name = "";
    TString *names = &name;
    trackSplitPlot(nFiles,files,names,xvar,yvar,relative,logscale,resolution,pull,saveas);
}

//1D version

void trackSplitPlot(TString file,TString var,
                    Bool_t relative = kFALSE,Bool_t logscale = kFALSE,Bool_t pull = kFALSE,
                    TString saveas = "")
{
    Int_t nFiles = 1;
    TString *files = &file;
    TString name = "";
    TString *names = &name;
    trackSplitPlot(nFiles,files,names,var,relative,logscale,pull,saveas);
}

void placeholder(TString saveas,Bool_t wide)
{
    TStyle *tdrStyle = setTDRStyle();
    if (wide)
        tdrStyle->SetCanvasDefW(678);
    TText *line1 = new TText(.5,.6,"This is a placeholder so that when there are");
    TText *line2 = new TText(.5,.4,"4 plots per line it lines up nicely");
    line1->SetTextAlign(22);
    line2->SetTextAlign(22);
    TCanvas *c1 = TCanvas::MakeDefCanvas();
    line1->Draw();
    line2->Draw();
    if (saveas != "")
        saveplot(c1,saveas);
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
