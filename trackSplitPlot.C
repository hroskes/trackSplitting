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

void trackSplitPlot(Int_t nFiles,Char_t **files,Char_t **names,Char_t *xvar,Char_t *yvar,
                    Bool_t relative = kFALSE,Bool_t logscale = kFALSE,Double_t xcut = 10,Double_t ycut = 3,Char_t *saveas = "")
{
    Bool_t profile = (nFiles >= 1);
    if (nFiles < 1) nFiles = 1;
    
    if (xvar == "")
    {
        Bool_t normalize = kTRUE;
        trackSplitPlot(nFiles,files,names,yvar,relative,logscale,normalize,ycut,saveas);
        return;
    }
        
    const Int_t n = nFiles;
    
    if (n > nColors)
    {
        cout << "Doesn't work for more than " << nColors << " files" << endl
             << "If you want to use more files add more colors to Color_t colors in trackSplitPlot.C" <<endl
             << "But you probably won't be able to make sense of the profiles anyway with so many" << endl;
        return;
    }

    setTDRStyle();
    set_plot_style();
    gStyle->SetOptStat(0);
    if (!profile)
    {
        gStyle->SetCanvasDefW(678);
        gStyle->SetPadRightMargin(0.115);
    }

    TH1 *p[n];

    for (Int_t i = 0; i < n; i++)
    {
        TFile *f = new TFile(files[i]);
        TTree *tree = (TTree*)f->Get("splitterTree");

//Set up strings ========================================

        stringstream sx,sy,srel,sid;

        sx << xvar << "_org";
        TString xvariable = sx.str();
        
        sy << "Delta_" << yvar;
        TString yvariable = sy.str();
        
        TString relvariable = "1";
        if (relative)
        {
            srel << yvar << "_org";
            relvariable = srel.str();
        }

        sid << "p" << i;
        TString id = sid.str();


//=======================================================

        Double_t xaverage,xstddev,xmin,xmax,yaverage,ystddev,ymin,ymax;
        xaverage = findAverage(files[i],xvariable);
        xstddev  = findStdDev (files[i],xvariable);
        xmin     = TMath::Max(xaverage - xcut * xstddev,findMin(files[i],xvariable));
        xmax     = TMath::Min(xaverage + xcut * xstddev,findMax(files[i],xvariable));
        yaverage = 0 /*findAverage(files[i],yvariable,relvariable)*/;
        ystddev  = findStdDev (files[i],yvariable,relvariable);
        ymin     = TMath::Max(TMath::Max(-TMath::Abs(yaverage) - ycut*ystddev,findMin(files[i],yvariable,relvariable)),-findMax(files[i],yvariable,relvariable));
        ymax     = -ymin;
        if (xvar == "pt")
            xmin = findMin(files[i],xvariable);
        
        if (profile)
            p[i] = new TProfile(id,"",25,xmin,xmax);
        else
            p[i] = new TH2F(id,"",1000,xmin,xmax,1000/*00*/,ymin,ymax);

        Double_t x,y,rel = 1;

        if (!relative && (yvar == "dz" || yvar == "dxy"))
            rel = 1e-4;                                     //it's in cm but we want it in um, so divide by 1e-4

        tree->SetBranchAddress(xvariable,&x);
        tree->SetBranchAddress(yvariable,&y);
        if (relative && xvar != yvar)                       //if xvar == yvar, setting the branch here will undo setting it to x 2 lines earlier
            tree->SetBranchAddress(relvariable,&rel);
        
        int length = tree->GetEntries();
        
        for (Int_t j = 0; j<length; j++)
        {
            tree->GetEntry(j);
            if (relative && xvar == yvar)
            {
                rel = x;
            }
            y /= rel;                                          // if !relative, rel = 1 from before
            if (logscale)
                y = fabs(y);
            if (ymin <= y && y <= ymax)
                p[i]->Fill(x,y);
            if (((j+1)/1000)*1000 == j + 1 || j + 1 == length)
            {
                cout << j + 1 << "/" << length << ": " << x << ", " << y << endl;
            }
        }

        p[i]->SetXTitle(axislabel(xvar,'x'));
        p[i]->SetYTitle(axislabel(yvar,'y',relative));
        if (n>=2)
        {
            p[i]->SetLineColor(colors[i]);
            p[i]->SetFillColor(colors[i]);
            p[i]->SetLineWidth(nFiles - i);                        //biggest for the first one, which goes on the bottom, down to 1 for the last (top) one
        }
    }
    
    TCanvas *c1 = TCanvas::MakeDefCanvas();
    c1->SetLogy((Bool_t)(logscale));
    p[0]->Draw("COLZ");
    if (n>=2)
    {
        TLegend *legend = new TLegend(.6,.7,.9,.9);
        legend->AddEntry(p[0],names[0],"l");
        for (Int_t i = 1; i < n; i++)
        {
            p[i]->Draw("same");
            legend->AddEntry(p[i],names[i],"l");
        }
        legend->SetFillStyle(0);
        legend->Draw();
    }

    if (saveas != "")
    {
        c1->SaveAs(saveas);
    }
}

void trackSplitPlot(Int_t nFiles,Char_t **files,Char_t **names,Char_t *var,
                    Bool_t relative = kFALSE,Bool_t logscale = kFALSE,Bool_t normalize = kTRUE,Double_t cut = 3,Char_t *saveas = "")
{
    if (nFiles < 1) nFiles = 1;
    
    const Int_t n = nFiles;
    if (n > nColors)
    {
        cout << "Doesn't work for more than " << nColors << " files" << endl
             << "If you want to use more files add more colors to Color_t colors in trackSplitPlot.C" << endl
             << "But you probably won't be able to make sense of the histograms anyway with so many" << endl;
        return;
    }

    setTDRStyle();
    gStyle->SetOptStat(1110 * (nFiles == 1));

    TH1F *p[n];

    stringstream s,srel;

    s << "Delta_" << var;
    TString variable = s.str();

    TString relvariable = "1";
    if (relative)
    {
        srel << var << "_org";
        relvariable = srel.str();
    }

    Double_t average,stddev,min,max;
    average = 0;
    stddev = -1;
    for (int i = 0; i < nFiles; i++)
        stddev  = TMath::Max(stddev,findStdDev(files[i],variable,relvariable));
    min     = TMath::Max(TMath::Max(-TMath::Abs(average) - cut*stddev,findMin(nFiles,files,variable,relvariable)),-findMax(nFiles,files,variable,relvariable));
    max     = -min;
        
    for (Int_t i = 0; i < n; i++)
    {
        TFile *f = new TFile(files[i]);
        TTree *tree = (TTree*)f->Get("splitterTree");

        stringstream sid;

        sid << "p" << i;
        TString id = sid.str();

        p[i] = new TH1F(id,"",100,min,max);

        Double_t x,rel = 1;

        if (!relative && (var == "dz" || var == "dxy"))
            rel = 1e-4;                                     //it's in cm but we want it in um, so divide by 1e-4

        tree->SetBranchAddress(variable,&x);
        if (relative)
            tree->SetBranchAddress(relvariable,&rel);
        
        int length = tree->GetEntries();
        
        for (Int_t j = 0; j<length; j++)
        {
            tree->GetEntry(j);
            x /= rel;                                          // if !relative, rel = 1 from before
            if (logscale)
                x = fabs(y);
            if (min <= x && x <= max)
                p[i]->Fill(x);
            if (((j+1)/1000)*1000 == j + 1 || j + 1 == length)
            {
                cout << j + 1 << "/" << length << ": " << x << endl;
            }
        }

        cout << "Average = " << p[i]->GetMean() << endl;
        cout << "RMS     = " << p[i]->GetRMS()  << endl;
        p[i]->SetXTitle(axislabel(var,'y',relative));          //the label should be like the y axis label for 2d plots (delta not org, etc.)
        if (n >= 2)
            p[i]->SetLineColor(colors[i]);
        if (normalize)
            p[i]->Scale(1/(p[i]->Integral()));
    }
    
    TCanvas *c1 = TCanvas::MakeDefCanvas();
    c1->SetLogx((Bool_t)(logscale));
    if (n >= 2)
    {
        TLegend *legend = new TLegend(.6,.7,.9,.9);
        legend->SetFillStyle(0);
    }

/*
    if (nFiles <= 2)
    {
        gStyle->SetOptStat(1110);
        gStyle->SetStatX(.4);
        gStyle->SetStatColor(colors[0]);
        gStyle->SetStatY(.9);
    }
*/

    Int_t maxbincontent = 0, drawfirst = 0;
    for (Int_t i = 0; i < n; i++)
    {
        if (p[i]->GetBinContent(p[i]->GetMaximumBin()) > maxbincontent)
        maxbincontent = p[i]->GetBinContent(p[i]->GetMaximumBin());
        drawfirst = i;
    }

    p[drawfirst]->Draw();

/*
    gStyle->SetStatY(gStyle->GetStatY() - gStyle->GetStatH());
    gStyle->SetStatColor(colors[1]);
*/

    if (n >= 2)
    {
        for (Int_t i = 0; i < n; i++)
        {
            if (i != drawfirst)
                p[i]->Draw("same");
            legend->AddEntry(p[i],names[i],"l");
        }
        legend->Draw();
    }

    if (saveas != "")
    {
        c1->SaveAs(saveas);
    }
}



//For 1 file

void trackSplitPlot(Char_t *file,Char_t *xvar,Char_t *yvar,Bool_t profile = kFALSE,
                    Bool_t relative = kFALSE,Bool_t logscale = kFALSE,
                    Double_t xcut = 10,Double_t ycut = 3,Char_t *saveas = "")
{
    if (profile)
        Int_t nFiles = 1;
    else
        Int_t nFiles = 0;                       //it interprets nFiles < 1 as 1 file, make a scatterplot
    Char_t **files = &file;
    Char_t *name = "";
    Char_t **names = &name;
    trackSplitPlot(nFiles,files,names,xvar,yvar,relative,logscale,xcut,ycut,saveas);
}

//1D version

void trackSplitPlot(Char_t *file,Char_t *var,
                    Bool_t relative = kFALSE,Bool_t logscale = kFALSE,Bool_t normalize = kFALSE,
                    Double_t cut = 3,Char_t *saveas = "")
{
    Int_t nFiles = 1;
    Char_t **files = &file;
    Char_t *name = "";
    Char_t **names = &name;
    trackSplitPlot(nFiles,files,names,var,relative,logscale,normalize,cut,saveas);

}
