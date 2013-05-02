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
                    Bool_t relative = kFALSE,Bool_t logscale = kFALSE,Bool_t resolution = kFALSE,
                    Double_t xcut = 10,Double_t ycut = 3,Char_t *saveas = "")
{
    if (xvar == "" && yvar == "")
    {
        placeholder(saveas);
        return;
    }

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

    TH1 *p[n],
        *q[n];

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

    Double_t xaverage,xstddev,xmin,xmax,yaverage,ystddev,ymin,ymax;
    if (type == Profile || type == ScatterPlot || type == OrgHistogram || type == Resolution)
    {
        xaverage = findAverage(nFiles,files,xvariable);
        xstddev  = findStdDev (nFiles,files,xvariable);
        xmax     = TMath::Min(xaverage + xcut * xstddev,findMax(nFiles,files,xvariable));

        if (xvar == "pt")
            xmin = findMin(nFiles,files,xvariable);
        else
            xmin = TMath::Max(xaverage - xcut * xstddev,findMin(nFiles,files,xvariable));
    }
    if (type == Profile || type == ScatterPlot || type == Histogram || type == Resolution)
    {
        yaverage = 0 /*findAverage(nFiles,files,yvariable,relvariable)*/;
        ystddev  = findStdDev (nFiles,files,yvariable,relvariable);
        ymin     = TMath::Max(TMath::Max(-TMath::Abs(yaverage) - ycut*ystddev,
                              findMin(nFiles,files,yvariable,relvariable)),
                              -findMax(nFiles,files,yvariable,relvariable));
        ymax     = -ymin;
    }

    for (Int_t i = 0; i < n; i++)
    {
        TFile *f = new TFile(files[i]);
        TTree *tree = (TTree*)f->Get("splitterTree");

        stringstream sid;
        sid << "p" << i;
        TString id = sid.str();

        if (type == Profile || type == Resolution)
            p[i] = new TProfile(id,"",25,xmin,xmax);
        if (type == ScatterPlot)
            p[i] = new TH2F(id,"",1000,xmin,xmax,1000/*00*/,ymin,ymax);
        if (type == Histogram)
            p[i] = new TH1F(id,"",100,ymin,ymax);
        if (type == OrgHistogram)
            p[i] = new TH1F(id,"",100,xmin,xmax);

        Double_t x,y,rel = 1;

        if (!relative && (yvar == "dz" || yvar == "dxy"))
            rel = 1e-4;                                     //it's in cm but we want it in um, so divide by 1e-4

        if (type == Profile || type == ScatterPlot || type == Resolution || type == OrgHistogram)
            tree->SetBranchAddress(xvariable,&x);
        if (type == Profile || type == ScatterPlot || type == Resolution || type == Histogram)
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
            {
                if (type == Histogram)
                    p[i]->Fill(y);
                if (type == ScatterPlot || type == Profile || type == Resolution)
                    p[i]->Fill(x,y);
                if (type == OrgHistogram)
                    p[i]->Fill(x);
            }
            if (((j+1)/1000)*1000 == j + 1 || j + 1 == length)
            {
                cout << j + 1 << "/" << length << ": "; 
                if (type == Profile || type == ScatterPlot || type == Resolution || type == OrgHistogram)
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
            stringstream sid2;
            sid << "q" << i;
            TString id2 = sid2.str();

            q[i] = new TH1F(id2,"",25,xmin,xmax);
            for (Int_t j = 1; j <= 25; j++)
            {
                q[i]->SetBinContent(j,p[i]->GetBinError(j));
                cout << q[i]->GetBinContent(j) << endl;
            }

            p[i] = q[i];
        }

        setAxisLabels(p[i],type,xvar,yvar,relative);

        if (n>=2)
        {
            p[i]->SetLineColor(colors[i]);
            if (type == Profile)
            {
                p[i]->SetFillColor(colors[i]);
                p[i]->SetLineWidth(nFiles - i);                        //biggest for the first one, which goes on the bottom, down to 1 for the last (top) one
            }
            if (type == Resolution)
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
    else if (type == Resolution)
        p[0]->Draw("P");
    else
        p[0]->Draw();
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
                    Bool_t relative = kFALSE,Bool_t logscale = kFALSE,Bool_t normalize = kTRUE,Double_t cut = 3,Char_t *saveas = "")
{
    trackSplitPlot(nFiles,files,names,"",var,relative,logscale,false,0,cut,saveas);
}



//For 1 file

void trackSplitPlot(Char_t *file,Char_t *xvar,Char_t *yvar,Bool_t profile = kFALSE,
                    Bool_t relative = kFALSE,Bool_t logscale = kFALSE,Bool_t resolution = kFALSE,
                    Double_t xcut = 10,Double_t ycut = 3,Char_t *saveas = "")
{
    if (profile)
        Int_t nFiles = 1;
    else
        Int_t nFiles = 0;                       //it interprets nFiles < 1 as 1 file, make a scatterplot
    Char_t **files = &file;
    Char_t *name = "";
    Char_t **names = &name;
    trackSplitPlot(nFiles,files,names,xvar,yvar,relative,logscale,resolution,xcut,ycut,saveas);
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
            saveas3 = saveas;
    saveas2.ReplaceAll(".pngeps","");
    saveas3.Remove(saveas3.Length()-7);
    if (saveas2 == saveas3)
    {
        stringstream s1,s2;
        s1 << saveas2 << ".png";
        s2 << saveas2 << ".eps";
        saveas2 = s1.str();
        saveas3 = s2.str();
        c1->SaveAs(saveas2);
        c1->SaveAs(saveas3);
        return;
    }
    else
    {
        c1->SaveAs(saveas);
    }
}

