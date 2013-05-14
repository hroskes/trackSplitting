#include "tdrstyle.C"
#include "axislabel.C"
#include "averages.C"

//profile =  true:   profile
//profile =  false:  scatter plot
//relative = false:  xvar_org is on the x axis, Delta_yvar is on the y axis
//relative = true:   xvar_org is on the x axis, Delta_yvar / yvar_org is on the y axis

const Int_t nColors = 13;
const Color_t colors[nColors] = {kRed,kBlue,kGreen,kMagenta,kCyan,kYellow,kBlack,
                                 kOrange,kPink-2,kTeal+9,kAzure-8,kViolet-6,kSpring-1};

const Double_t maxBinsScatterPlotx = 1000;
const Double_t maxBinsScatterPloty = 1000;
const Double_t maxBinsHistogram = 100;
const Double_t maxBinsProfileResolution = 25;
      Double_t    binsProfileResolution = 8;     //for everything but runNumber

void trackSplitPlot(Int_t nFiles,Char_t **files,Char_t **names,Char_t *xvar,Char_t *yvar,
                    Bool_t relative = kFALSE,Bool_t logscale = kFALSE,Bool_t resolution = kFALSE,Bool_t pull = kFALSE,
                    Double_t xcut = 10,Double_t ycut = 3,Char_t *saveas = "")
{
    cout << xvar << " " << yvar << endl;
    if (relative && pull)
    {
        placeholder(saveas);
        return;
    }
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
    
    if (n > nColors)
    {
        cout << "Doesn't work for more than " << nColors << " files" << endl
             << "If you want to use more files add more colors to Color_t colors in trackSplitPlot.C" <<endl
             << "But you probably won't be able to make sense of the plots anyway with so many" << endl;
        return;
    }

    setTDRStyle();
    set_plot_style();
    gStyle->SetOptStat(0);
    if ((type == Histogram || type == OrgHistogram) && nFiles == 1)
        gStyle->SetOptStat(1110);
    if (type == ScatterPlot)
    {
        gStyle->SetCanvasDefW(678);
        gStyle->SetPadRightMargin(0.115);
    }

    Double_t nBinsScatterPlotx = maxBinsScatterPlotx;
    Double_t nBinsScatterPloty = maxBinsScatterPloty;
    Double_t nBinsHistogram = maxBinsHistogram;
    Double_t nBinsProfileResolution = binsProfileResolution;
    if (xvar == "runNumber")
    {
        nBinsProfileResolution = maxBinsProfileResolution;
        nBinsHistogram = maxBinsProfileResolution;
    }

    TH1 *p[n];

    stringstream sx,sy,srel,ssigma1,ssigma2;

    sx << xvar << "_org";
    TString xvariable = sx.str();
    if (xvar == "runNumber") xvariable = "runNumber";

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


    Double_t xaverage,xstddev,xmin = -1,xmax = 1,yaverage,ystddev,ymin = -1,ymax = 1;
    if (type == Profile || type == ScatterPlot || type == OrgHistogram || type == Resolution)
    {
        xaverage = findAverage(nFiles,files,xvar,'x');
        xstddev  = findStdDev (nFiles,files,xvar,'x');
        xmax     = TMath::Min(xaverage + xcut * xstddev,findMax(nFiles,files,xvar,'x'));

        if (xvar == "pt")
        {
            xmin = /*findMin(nFiles,files,xvar,'x')*/ 5;
            xmax = 100;
        }
        else if (xvar == "runNumber")
        {
            xmin = findMin(nFiles,files,xvar,'x');
            xmax = findMax(nFiles,files,xvar,'x');
        }
        else
            xmin = TMath::Max(xaverage - xcut * xstddev,findMin(nFiles,files,xvar,'x'));
    }
    if (type == Profile || type == ScatterPlot || type == Histogram || type == Resolution)
    {
        yaverage = 0 /*findAverage(nFiles,files,yvar,'y',relative,pull)*/;
        ystddev  = findStdDev (nFiles,files,yvar,'y',relative,pull);
        ymin     = TMath::Max(TMath::Max(-TMath::Abs(yaverage) - ycut*ystddev,
                              findMin(nFiles,files,yvar,'y',relative,pull)),
                              -findMax(nFiles,files,yvar,'y',relative,pull));
        ymax     = -ymin;
    }

    for (Int_t i = 0; i < n; i++)
    {
        TFile *f = new TFile(files[i]);
        TTree *tree = (TTree*)f->Get("splitterTree");

        stringstream sid;
        sid << "p" << i;
        TString id = sid.str();

        if (type == ScatterPlot)
            p[i] = new TH2F(id,"",nBinsScatterPlotx,xmin,xmax,nBinsScatterPlotx,ymin,ymax);
        if (type == Histogram)
            p[i] = new TH1F(id,"",nBinsHistogram,ymin,ymax);
        if (type == OrgHistogram)
            p[i] = new TH1F(id,"",nBinsHistogram,xmin,xmax);
        if (type == Resolution || type == Profile)
        {
            p[i] = new TH1F(id,"",nBinsProfileResolution,xmin,xmax);
            TH1F *q[maxBinsProfileResolution];
            for (Int_t j = 0; j < nBinsProfileResolution; j++)
            {
                stringstream sid2;
                sid2 << "q" << j;
                TString id2 = sid2.str();
                q[j] = new TH1F(id2,"",nBinsHistogram,ymin,ymax);
            }
        }

        Double_t x = 0, y = 0, rel = 1, sigma1 = 1, sigma2 = 1;           //if !pull, we want to divide by sqrt(2) because we want the error from 1 track
        Int_t xint = 0;

        if (!relative && !pull && (yvar == "dz" || yvar == "dxy"))
            rel = 1e-4;                                     //it's in cm but we want it in um, so divide by 1e-4

        if (type == Profile || type == ScatterPlot || type == Resolution || type == OrgHistogram)
        {
            if (xvar == "runNumber")
                tree->SetBranchAddress(xvariable,&xint);
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
        
        int length = tree->GetEntries();
        
        for (Int_t j = 0; j<length; j++)
        {
            tree->GetEntry(j);
            if (xvar == "runNumber") x = xint;
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
            if (((j+1)/1000)*1000 == j + 1 || j + 1 == length)
            {
                cout << j + 1 << "/" << length << ": "; 
                if (type == Profile || type == ScatterPlot || type == Resolution)
                    cout << x << ", " << y << endl;
                if (type == OrgHistogram)
                    cout << x << endl;
                if (type == Histogram)
                    cout << y << endl;
            }
        }

        if (type == Histogram || type == OrgHistogram)
        {
            cout << "Average = " << p[i]->GetMean() << endl;
            cout << "RMS     = " << p[i]->GetRMS()  << endl;
        }

        if (type == Resolution)
        {
            for (Int_t j = 0; j < nBinsProfileResolution; j++)
            {
                p[i]->SetBinContent(j+1,q[j]->GetRMS());
                p[i]->SetBinError  (j+1,q[j]->GetRMSError());
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

        if (n>=2)
        {
            p[i]->SetLineColor(colors[i]);
            if (type == Resolution || type == Profile)
            {
                p[i]->SetMarkerColor(colors[i]);
                p[i]->SetMarkerStyle(20+i);
            }
        }
    }

    TCanvas *c1 = TCanvas::MakeDefCanvas();
    if (type == ScatterPlot || type == Profile || type == Resolution)
        c1->SetLogy((Bool_t)(logscale));
    if (type == Histogram)
        c1->SetLogx((Bool_t)(logscale));

    if (type == ScatterPlot)
        p[0]->Draw("COLZ");
    else if (type == Resolution || type == Profile)
    {
        TGraphErrors *g[n];
        TMultiGraph *list = new TMultiGraph();
        for (Int_t i = 0; i < n; i++)
        {
            g[i] = new TGraphErrors(p[i]);
            for (Int_t j = 0; j < g[i]->GetN(); j++)
            {
                if (g[i]->GetY()[j] == 0)
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
        TH1F *maxp = (TH1F*)p[0]->Clone("maxp");
        maxp->SetLineColor(kWhite);
        for (Int_t i = 1; i <= maxp->GetNbinsX(); i++)
        {
            for (Int_t j = 0; j < n; j++)
            {
                maxp->SetBinContent(i,TMath::Max(maxp->GetBinContent(i),p[j]->GetBinContent(i)));
            }
        }
        maxp->Draw();
        p[0]->Draw("same");
    }
    if (n>=2)
    {
        TLegend *legend = new TLegend(.6,.7,.9,.9);
        if (type == Resolution)
            legend->AddEntry(p[0],names[0],"p");
        else
            legend->AddEntry(p[0],names[0],"l");
        for (Int_t i = 1; i < n; i++)
        {
            if (type == Resolution)
            {
                p[i]->Draw("same P");
                legend->AddEntry(p[i],names[i],"p");
            }
            else
            {
                p[i]->Draw("same");
                legend->AddEntry(p[i],names[i],"l");
            }
        }
        legend->SetFillStyle(0);
        legend->Draw();
    }
    if (saveas != "")
    {
        saveplot(c1,saveas);
    }
}

void trackSplitPlot(Int_t nFiles,Char_t **files,Char_t **names,Char_t *var,
                    Bool_t relative = kFALSE,Bool_t logscale = kFALSE,Bool_t pull = kFALSE,Double_t cut = 3,Char_t *saveas = "")
{
    trackSplitPlot(nFiles,files,names,"",var,relative,logscale,false,pull,0,cut,saveas);
}



//For 1 file

void trackSplitPlot(Char_t *file,Char_t *xvar,Char_t *yvar,Bool_t profile = kFALSE,
                    Bool_t relative = kFALSE,Bool_t logscale = kFALSE,Bool_t resolution = kFALSE,Bool_t pull = kFALSE,
                    Double_t xcut = 10,Double_t ycut = 3,Char_t *saveas = "")
{
    if (profile)
        Int_t nFiles = 1;
    else
        Int_t nFiles = 0;                       //it interprets nFiles < 1 as 1 file, make a scatterplot
    Char_t **files = &file;
    Char_t *name = "";
    Char_t **names = &name;
    trackSplitPlot(nFiles,files,names,xvar,yvar,relative,logscale,resolution,pull,xcut,ycut,saveas);
}

//1D version

void trackSplitPlot(Char_t *file,Char_t *var,
                    Bool_t relative = kFALSE,Bool_t logscale = kFALSE,Bool_t pull = kFALSE,
                    Double_t cut = 3,Char_t *saveas = "")
{
    Int_t nFiles = 1;
    Char_t **files = &file;
    Char_t *name = "";
    Char_t **names = &name;
    trackSplitPlot(nFiles,files,names,var,relative,logscale,pull,cut,saveas);
}

void placeholder(Char_t *saveas = "")
{
    setTDRStyle();
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

void saveplot(TCanvas *c1,Char_t *saveas)
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

