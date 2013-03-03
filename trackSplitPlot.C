#include "tdrstyle.C"
#include "axislabel.C"
#include "averages.C"

//profile =  true:   profile
//profile =  false:  scatter plot
//relative = false:  xvar_org is on the x axis, Delta_yvar is on the y axis
//relative = true:   xvar_org is on the x axis, Delta_yvar / yvar_org is on the y axis

void trackSplitPlot(Char_t *file,Char_t *xvar,Char_t *yvar,Bool_t profile = kFALSE,
                    Bool_t relative = kFALSE,Bool_t logscale = kFALSE,Double_t xcut = 10,Double_t ycut = 3,Char_t *saveas = "")
{
    if (xvar == "")
    {
        trackSplitPlot(file,yvar,relative,logscale,ycut,saveas);
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

    TFile *f = new TFile(file);
    TTree *tree = (TTree*)f->Get("splitterTree");

//Set up strings ========================================

    stringstream sx,sy,srel;

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

//=======================================================

    Double_t xaverage = findAverage(file,xvariable),
             xstddev  = findStdDev (file,xvariable),
             xmin     = TMath::Max(xaverage - xcut * xstddev,findMin(file,xvariable)),
             xmax     = TMath::Min(xaverage + xcut * xstddev,findMax(file,xvariable)),
             yaverage = 0 /*findAverage(file,yvariable,relvariable)*/,
             ystddev  = findStdDev (file,yvariable,relvariable),
             ymin     = TMath::Max(TMath::Max(-TMath::Abs(yaverage) - ycut * ystddev,findMin(file,yvariable,relvariable)),-findMax(file,yvariable,relvariable)),
             ymax     = -ymin;
    if (xvar == "pt")
        xmin = findMin(file,xvariable);

    if (profile)
        TProfile *p = new TProfile("p","",25,xmin,xmax);
    else
        TH2F *p = new TH2F("p","",1000,xmin,xmax,1000/*00*/,ymin,ymax);

    Double_t x,y,rel = 1;
    
    if (!relative && (yvar == "dz" || yvar == "dxy"))
        rel = 1e-4;                                     //it's in cm but we want it in um, so divide by 1e-4
    
    tree->SetBranchAddress(xvariable,&x);
    tree->SetBranchAddress(yvariable,&y);
    if (relative && xvar != yvar)                       //if xvar == yvar, setting the branch here will undo setting it to x 2 lines earlier
        tree->SetBranchAddress(relvariable,&rel);
    
    int length = tree->GetEntries();

    for (Int_t i = 0; i<length; i++)
    {
        tree->GetEntry(i);
        if (relative && xvar == yvar)
        {
            rel = x;
        }
        y /= rel;                                          // if !relative, rel = 1 from before
        if (logscale)
            y = fabs(y);
        if (ymin <= y && y <= ymax)
            p->Fill(x,y);
        if (((i+1)/1000)*1000 == i + 1 || i + 1 == length)
        {
            cout << i + 1 << "/" << length << ": " << x << ", " << y << endl;
        }
    }

    p->SetXTitle(axislabel(xvar,'x'));
    p->SetYTitle(axislabel(yvar,'y',relative));
    TCanvas *c1 = TCanvas::MakeDefCanvas();
    c1->SetLogy((Bool_t)(logscale));

    if(profile)
        p->Draw();
    else
        p->Draw("COLZ");

    if (saveas != "")
    {
        c1->SaveAs(saveas);
    }
    
    f->Close();
}

//1D version

void trackSplitPlot(Char_t *file,Char_t *var,
                    Bool_t relative = kFALSE,Bool_t logscale = kFALSE,Double_t cut = 3,Char_t *saveas = "")
{
    setTDRStyle();
    set_plot_style();

    TFile *f = new TFile(file);
    TTree *tree = (TTree*)f->Get("splitterTree");

//Set up strings ========================================

    stringstream s,srel;

    s << "Delta_" << var;
    TString variable = s.str();

    TString relvariable = "1";
    if (relative)
    {
        srel << var << "_org";
        relvariable = srel.str();
    }

//=======================================================

    Double_t average = 0 /*findAverage(file,variable,relvariable)*/,
             stddev  = findStdDev (file,variable,relvariable),
             min     = TMath::Max(TMath::Max(-TMath::Abs(average) - cut * stddev,findMin(file,variable,relvariable)),-findMax(file,variable,relvariable)),
             max     = -min;

    TH1F *p = new TH1F("p","",100,min,max);

    Double_t x,rel = 1;

    if (!relative && (var == "dz" || var == "dxy"))
        rel = 1e-4;                                     //it's in cm but we want it in um, so divide by 1e-4
    
    tree->SetBranchAddress(variable,&x);
    if (relative)
        tree->SetBranchAddress(relvariable,&rel);
    
    int length = tree->GetEntries();

    for (Int_t i = 0; i<length; i++)
    {
        tree->GetEntry(i);
        x /= rel;                                          // if !relative, rel = 1 from before
        if (logscale)
            x = fabs(x);
        p->Fill(x);
        if (((i+1)/1000)*1000 == i + 1 || i + 1 == length)
        {
            cout << i + 1 << "/" << length << ": " << x << endl;
        }
    }

    p->SetXTitle(axislabel(var,'y',relative));
    TCanvas *c1 = TCanvas::MakeDefCanvas();
    c1->SetLogx((Bool_t)(logscale));

    gStyle->SetOptStat(1110);
    gStyle->SetStatX(.4);

    p->Draw();

    if (saveas != "")
    {
        c1->SaveAs(saveas);
    }
    
    f->Close();
}

