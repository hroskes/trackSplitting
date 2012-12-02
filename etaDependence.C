#include "tdrstyle.C"

//profile =  true:   profile
//profile =  false:  scatter plot
//relative = false:  xvar_org is on the x axis, Delta_yvar is on the y axis
//relative = true:   xvar_org is on the x axis, Delta_yvar / yvar_org is on the y axis

Double_t findAverage(Char_t *variable,Char_t *relative = "1")
{
    Char_t *file = "TrackSplitting_Split_Alignment_wDiffs.root";
    TFile *f = new TFile(file);
    TTree *tree = (TTree*)f->Get("splitterTree");
    Int_t length = tree->GetEntries();
    Double_t var,rel = 1,average = 0;
    tree->SetBranchAddress(variable,&var);
    if (relative[0] != '1')
        tree->SetBranchAddress(relative,&rel);
    for (Int_t i = 0; i<length; i++)
    {
        tree->GetEntry(i);
        average += (var / rel) / length;
    }
    return average;
}

Double_t findStdDev(Char_t *variable,Char_t *relative = "1")
{
    Char_t *file = "TrackSplitting_Split_Alignment_wDiffs.root";
    TFile *f = new TFile(file);
    TTree *tree = (TTree*)f->Get("splitterTree");
    Int_t length = tree->GetEntries();
    Double_t var,
             rel = 1,
             average = findAverage(variable,relative),
             stddevsquared = 0;
    tree->SetBranchAddress(variable,&var);
    if (relative[0] != '1')
        tree->SetBranchAddress(relative,&rel);
    for (Int_t i = 0; i<length; i++)
    {
        tree->GetEntry(i);
        stddevsquared += ((var / rel) - average) * ((var / rel) - average) / length;
    }
    cout << TMath::Power(stddevsquared,.5) << endl;
    return TMath::Power(stddevsquared,.5);
}

TH1 *etaDependence(Char_t *xvar = "eta",Char_t *yvar = "pt",Bool_t profile = kFALSE,Bool_t relative = kFALSE,Double_t xnumberofsigmas = 10,Double_t ynumberofsigmas = 10,Char_t *saveas = "")
{
    setTDRStyle();

    Char_t *file = "TrackSplitting_Split_Alignment_wDiffs.root";
    TFile *f = new TFile(file);
    TTree *tree = (TTree*)f->Get("splitterTree");

//Set up strings ========================================

    stringstream stitle,sx,sy,srel,syaxislabel;

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

    syaxislabel << yvariable;
    if (relative)
        syaxislabel << " / " << relvariable;
    TString yaxislabel = syaxislabel.str();

    if (profile)
        stitle << "Profile";
    else
        stitle << "Plot";
    stitle << " of " << xvariable << " and " << yaxislabel;
    TString name = stitle.str();

//=======================================================

    Double_t xaverage = findAverage(xvariable),
             xstddev  = findStdDev (xvariable),
             xmin     = TMath::Max(xaverage - xnumberofsigmas * xstddev,tree->GetMinimum(xvariable)),
             xmax     = TMath::Min(xaverage + xnumberofsigmas * xstddev,tree->GetMaximum(xvariable)),
             yaverage = findAverage(yvariable,relvariable),
             ystddev  = findStdDev (yvariable,relvariable),
             ymin     = TMath::Max(TMath::Max(-TMath::Abs(yaverage) - ynumberofsigmas * ystddev,tree->GetMinimum(yvariable)),-tree->GetMaximum(yvariable)),
             ymax     = -ymin;

    if (profile)
        TProfile *p = new TProfile("p",name,25,xmin,xmax);
    else
        TH2F *p = new TH2F("p",name,1000,xmin,xmax,100000,ymin,ymax);
    Double_t x,y,rel = 1;
    tree->SetBranchAddress(xvariable,&x);
    tree->SetBranchAddress(yvariable,&y);
    if (relative)
        tree->SetBranchAddress(relvariable,&rel);
    
    int length = tree->GetEntries();

    for (Int_t i = 0; i<length; i++)
    {
        tree->GetEntry(i);
        y /= rel;                                          // if !relative, rel = 1 from before
        if (ymin <= y && y <= ymax)
            p->Fill(x,y);
        if (((i+1)/1000)*1000 == i + 1 || i + 1 == length)
        {
            cout << i + 1 << "/" << length << ": " << x << ", " << y << endl;
        }
    }

    p->SetXTitle(xvariable);
    p->SetYTitle(yaxislabel);
    TCanvas *c1 = TCanvas::MakeDefCanvas();

    if(profile)
        p->Draw();
    else
        p->Draw("COLZ");

    if (saveas != "")
    {
        c1->SaveAs(saveas);
    }
    
    return p;
}
