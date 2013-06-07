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

void placeholder(TString saveas = "");
void saveplot(TCanvas *c1,TString saveas);

//profile =  true:   profile
//profile =  false:  scatter plot
//relative = false:  xvar_org is on the x axis, Delta_yvar is on the y axis
//relative = true:   xvar_org is on the x axis, Delta_yvar / yvar_org is on the y axis

const Int_t nColors = 13;
const Color_t colors[nColors] = {kRed,kBlue,kGreen,kMagenta,kCyan,kYellow,kBlack,
                                 kOrange,kPink-2,kTeal+9,kAzure-8,kViolet-6,kSpring-1};

const Int_t maxBinsScatterPlotx = 1000;
const Int_t maxBinsScatterPloty = 1000;
const Int_t maxBinsHistogram = 100;
const Int_t maxBinsProfileResolution = 25;
      Int_t    binsProfileResolution = 8;     //for everything but runNumber and nHits

void trackSplitPlot(Int_t nFiles,TString *files,TString *names,TString xvar,TString yvar,
                    Bool_t relative = kFALSE,Bool_t logscale = kFALSE,Bool_t resolution = kFALSE,Bool_t pull = kFALSE,
                    Double_t xcut = 10,Double_t ycut = 3,TString saveas = "")
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

    TStyle *tdrStyle = setTDRStyle();
    tdrStyle->SetOptStat(0);
    if ((type == Histogram || type == OrgHistogram) && nFiles == 1)
        tdrStyle->SetOptStat(1110);
    if (type == ScatterPlot)
    {
        tdrStyle->SetCanvasDefW(678);
        tdrStyle->SetPadRightMargin(0.115);
    }

    Bool_t nHits = (xvar[0] == 'n' && xvar[1] == 'H' && xvar[2] == 'i'
                                   && xvar[3] == 't' && xvar[4] == 's');

    Int_t nBinsScatterPlotx = maxBinsScatterPlotx;
    Int_t nBinsScatterPloty = maxBinsScatterPloty;
    Int_t nBinsHistogram = maxBinsHistogram;
    Int_t nBinsProfileResolution = binsProfileResolution;
    if (xvar == "runNumber")
    {
        nBinsProfileResolution = maxBinsProfileResolution;
        nBinsHistogram = maxBinsProfileResolution;
    }
    if (nHits)
    {
        nBinsHistogram = (int)(findMax(nFiles,files,xvar,'x') - findMin(nFiles,files,xvar,'x') + 1.1);     //in case it's .99999
        nBinsScatterPlotx = nBinsHistogram;
        nBinsProfileResolution = nBinsHistogram;
    }

    TH1 *p[n];

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
        else if (xvar == "runNumber" || nHits)
        {
            xmin = findMin(nFiles,files,xvar,'x') - .5;
            xmax = findMax(nFiles,files,xvar,'x') + .5;
        }
        else
        {
            xaverage = findAverage(nFiles,files,xvar,'x');
            xrms     = findRMS    (nFiles,files,xvar,'x');
            xmax     = TMath::Min(xaverage + xcut * xrms,findMax(nFiles,files,xvar,'x'));
            xmin     = TMath::Max(xaverage - xcut * xrms,findMin(nFiles,files,xvar,'x'));
        }
    }
    if (type == Profile || type == ScatterPlot || type == Histogram || type == Resolution)
    {
        yaverage = 0 /*findAverage(nFiles,files,yvar,'y',relative,pull)*/;
        yrms     = findRMS (nFiles,files,yvar,'y',relative,pull);
        ymin     = TMath::Max(TMath::Max(-TMath::Abs(yaverage) - ycut*yrms,
                              findMin(nFiles,files,yvar,'y',relative,pull)),
                              -findMax(nFiles,files,yvar,'y',relative,pull));
        ymax     = -ymin;
    }

    for (Int_t i = 0; i < n; i++)
    {
        TFile *f = TFile::Open(files[i]);
        TTree *tree = (TTree*)f->Get("splitterTree");

        stringstream sid;
        sid << "p" << i;
        TString id = sid.str();

        TH1F *q[maxBinsProfileResolution];
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

        Double_t x = 0, y = 0, rel = 1, sigma1 = 1, sigma2 = 1;           //if !pull, we want to divide by sqrt(2) because we want the error from 1 track
        Int_t xint = 0, xint2 = 0;

        if (!relative && !pull && (yvar == "dz" || yvar == "dxy"))
            rel = 1e-4;                                     //it's in cm but we want it in um, so divide by 1e-4

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
        
        int length = tree->GetEntries();
        
        for (Int_t j = 0; j<length; j++)
        {
            tree->GetEntry(j);
            if (xvar == "runNumber" || nHits)
                x = xint;
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

            if (((j+1)/(int)(pow(10,(int)(log10(length))-1)))*(int)(pow(10,(int)(log10(length))-1)) == j + 1 || j + 1 == length)
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
        maxp = (TH1F*)p[0]->Clone("maxp");
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
    TLegend *legend = new TLegend(.6,.7,.9,.9,"","br");
    if (n>=2)
    {
        if (type == Resolution || type == Profile)
            legend->AddEntry(p[0],names[0],"pl");
        else
            legend->AddEntry(p[0],names[0],"l");
        for (Int_t i = 1; i < n; i++)
        {
            if (type == Resolution || type == Profile)
            {
                p[i]->Draw("same P");
                legend->AddEntry(p[i],names[i],"pl");
            }
            else
            {
                p[i]->Draw("same");
                legend->AddEntry(p[i],names[i],"l");
            }
        }
        
        c1->Update();
        Double_t x1min  = .9*gPad->GetUxmin() + .1*gPad->GetUxmax();
        Double_t x2max  = .1*gPad->GetUxmin() + .9*gPad->GetUxmax();
        Double_t y1min  = .9*gPad->GetUymin() + .1*gPad->GetUymax();
        Double_t y2max  = .1*gPad->GetUymin() + .9*gPad->GetUymax();
        Double_t width  = .4*(x2max-x1min);
        Double_t height = .3*(y2max-y1min);
        Double_t newy2max = placeLegend(legend,width,height,x1min,y1min,x2max,y2max);
        maxp->GetYaxis()->SetRangeUser(gPad->GetUymin(),(newy2max-.1*gPad->GetUymin())/.9);
                
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
                    Bool_t relative = kFALSE,Bool_t logscale = kFALSE,Bool_t pull = kFALSE,Double_t cut = 3,TString saveas = "")
{
    return trackSplitPlot(nFiles,files,names,"",var,relative,logscale,false,pull,0,cut,saveas);
}



//For 1 file

void trackSplitPlot(TString file,TString xvar,TString yvar,Bool_t profile = kFALSE,
                    Bool_t relative = kFALSE,Bool_t logscale = kFALSE,Bool_t resolution = kFALSE,Bool_t pull = kFALSE,
                    Double_t xcut = 10,Double_t ycut = 3,TString saveas = "")
{
    Int_t nFiles = 0;
    if (profile)                       //it interprets nFiles < 1 as 1 file, make a scatterplot
        nFiles = 1;
    TString *files = &file;
    TString name = "";
    TString *names = &name;
    trackSplitPlot(nFiles,files,names,xvar,yvar,relative,logscale,resolution,pull,xcut,ycut,saveas);
}

//1D version

void trackSplitPlot(TString file,TString var,
                    Bool_t relative = kFALSE,Bool_t logscale = kFALSE,Bool_t pull = kFALSE,
                    Double_t cut = 3,TString saveas = "")
{
    Int_t nFiles = 1;
    TString *files = &file;
    TString name = "";
    TString *names = &name;
    trackSplitPlot(nFiles,files,names,var,relative,logscale,pull,cut,saveas);
}

void placeholder(TString saveas)
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
