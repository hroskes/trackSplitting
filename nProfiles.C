#include "trackSplitPlot.C"

//profile =  true:   profile
//profile =  false:  scatter plot
//relative = false:  xvar_org is on the x axis, Delta_yvar is on the y axis
//relative = true:   xvar_org is on the x axis, Delta_yvar / yvar_org is on the y axis

const Int_t nColors = 13;
const Color_t colors[nColors] = {kRed,kBlue,kGreen,kMagenta,kCyan,kYellow,kBlack,
                                 kOrange,kPink-2,kTeal+9,kAzure-8,kViolet-6,kSpring-1};

TProfile **nProfiles(Int_t nFiles,Char_t **files,Char_t **names,Char_t *xvar,Char_t *yvar,
                     Bool_t relative = kFALSE,Bool_t logscale = kFALSE,Double_t xcut = 10,Double_t ycut = 3,Char_t *saveas = "")
{
    const Int_t n = nFiles;
    if (n > nColors)
    {
        cout << "Doesn't work for more than 13 files." << endl
             << "If you want to use more files add more colors to Color_t colors in nProfiles.C" << endl;
        return;
    }

    setTDRStyle();

    TProfile *p[n];

    for (Int_t i = 0; i < n; i++)
    {
        TFile *f = new TFile(files[i]);
        TTree *tree = (TTree*)f->Get("splitterTree");

//Set up strings ========================================

        stringstream stitle,sx,sy,srel,syaxislabel,sid;

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

        stitle << "Profile of " << xvariable << " and " << yaxislabel;
        TString name = stitle.str();
    
        sid << "p" << i;
        TString id = sid.str();


//=======================================================

        Double_t xaverage,xstddev,xmin,xmax,yaverage,ystddev,ymin,ymax;
        xaverage = findAverage(files[i],xvariable);
        xstddev  = findStdDev (files[i],xvariable);
        xmin     = TMath::Max(xaverage - xcut * xstddev,tree->GetMinimum(xvariable));
        xmax     = TMath::Min(xaverage + xcut * xstddev,tree->GetMaximum(xvariable));
        yaverage = findAverage(files[i],yvariable,relvariable);
        ystddev  = findStdDev (files[i],yvariable,relvariable);
        ymin     = TMath::Max(TMath::Max(-TMath::Abs(yaverage) - ycut * ystddev,tree->GetMinimum(yvariable)),-tree->GetMaximum(yvariable));
        ymax     = -ymin;
        
        p[i] = new TProfile(id,name,25,xmin,xmax);

        Double_t x,y,rel = 1;
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

        p[i]->SetXTitle(xvariable);
        p[i]->SetYTitle(yaxislabel);
        p[i]->SetLineColor(colors[i]);
        p[i]->SetFillColor(colors[i]);
        p[i]->SetLineWidth(nFiles - i);                        //biggest for the first one, which goes on the bottom, down to 1 for the last (top) one
    }
    
    TCanvas *c1 = TCanvas::MakeDefCanvas();
    c1->SetLogy((Bool_t)(logscale));
    TLegend *legend = new TLegend(.6,.7,.9,.9);

    p[0]->Draw();
    legend->AddEntry(p[0],names[0],"l");
    for (Int_t i = 1; i < n; i++)
    {
        p[i]->Draw("same");
        legend->AddEntry(p[i],names[i],"l");
    }
    legend->SetFillStyle(0);
    legend->Draw();

    if (saveas != "")
    {
        c1->SaveAs(saveas);
    }
    return p;
}
