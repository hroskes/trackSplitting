#include "trackSplitPlot.C"

using namespace std;

//relative = false:  xvar_org is on the x axis, Delta_yvar is on the y axis
//relative = true:   xvar_org is on the x axis, Delta_yvar / yvar_org is on the y axis

void misalignmentDependence(Int_t nFiles,TString *files,Double_t *values,TString misalignment,TString yvar,
                            Bool_t relative = kFALSE,Bool_t logscale = kFALSE,Bool_t resolution = kFALSE,Bool_t pull = kFALSE,
                            TString saveas = "")
{
    cout << misalignment << " " << yvar << endl;
    if (yvar == "")
        return;

    PlotType type;
    if (resolution) type = Resolution;
    else            type = Profile;
    if (nFiles < 1) nFiles = 1;

    const Int_t n = nFiles;
    
    TStyle *tdrStyle = setTDRStyle();
    tdrStyle->SetOptStat(0);
    tdrStyle->SetCanvasDefW(678);
    tdrStyle->SetPadRightMargin(0.115);

    if (relative && pull)
    {
        placeholder(saveas,true);
        return;
    }

    Int_t nBinsHistogram = binsHistogram;

    TH1 *p[n];
    Int_t lengths[n];

    stringstream sy,srel,ssigma1,ssigma2;

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


    Double_t yaverage,yrms,ymin = -1,ymax = 1;
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


    Double_t meanrms[n];
    Double_t meanrmserror[n];

    for (Int_t i = 0; i < n; i++)
    {
        TFile *f = TFile::Open(files[i]);
        TTree *tree = (TTree*)f->Get("splitterTree");

        stringstream sid;
        sid << "p" << i;
        TString id = sid.str();

        p[i] = new TH1F(id,"",nBinsHistogram,ymin,ymax);
        p[i]->SetDirectory(0);

        lengths[i] = tree->GetEntries();

        Double_t y = 0, rel = 1, sigma1 = 1, sigma2 = 1;           //if !pull, we want to divide by sqrt(2) because we want the error from 1 track
        Int_t runNumber = 0;

        if (!relative && !pull && (yvar == "dz" || yvar == "dxy"))
            rel = 1e-4;                                     //it's in cm but we want it in um, so divide by 1e-4

        tree->SetBranchAddress("runNumber",&runNumber);
        tree->SetBranchAddress(yvariable,&y);
        if (relative)
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
            if (runNumber < minrun || (runNumber > maxrun && maxrun > 0)) 
            {
                notincluded++;
                continue;
            }
            y /= (rel * sqrt(sigma1 * sigma1 + sigma2 * sigma2));        //  If !relative, rel == 1 from before
                                                                         //  If !pull, we want to divide by sqrt(2) because we want the error from 1 track
            if (logscale)
                y = fabs(y);
            if (ymin <= y && y < ymax)
                p[i]->Fill(y);

            if (((j+1)/(int)(pow(10,(int)(log10(lengths[i]))-1)))*(int)(pow(10,(int)(log10(lengths[i]))-1)) == j + 1 || j + 1 == lengths[i])
            {
                cout << j + 1 << "/" << lengths[i] << ": "; 
                cout << y << endl;
            }
        }
        lengths[i] -= notincluded;

        if (type == Profile)
        {
            meanrms[i] = p[i]->GetMean();
            meanrmserror[i] = p[i]->GetMeanError();
        }
        else if (type == Resolution)
        {
            meanrms[i] = p[i]->GetRMS();
            meanrmserror[i] = p[i]->GetRMSError();
        }
        delete p[i];
    }

    TCanvas *c1 = TCanvas::MakeDefCanvas();
    c1->SetLogy((Bool_t)(logscale));

    TGraphErrors *g = new TGraphErrors(n,values,meanrms,(Double_t*)0,meanrmserror);

    g->GetXaxis()->SetTitle(misalignment);
    g->GetYaxis()->SetTitle(axislabel(yvar,'y',relative,resolution,pull));

    g->SetMarkerColor(colors[0]);
    g->SetMarkerStyle(20);

    g->Draw("AP");
    Double_t yaxismax = g->GetYaxis()->GetXmax();
    Double_t yaxismin = g->GetYaxis()->GetXmin();
    if (yaxismin > 0)
    {
        yaxismax += yaxismin;
        yaxismin = 0;
    }
    g->GetYaxis()->SetRangeUser(yaxismin,yaxismax);
    g->Draw("AP");

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
