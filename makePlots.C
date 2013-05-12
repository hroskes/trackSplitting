#include "trackSplitPlot.C"

const Int_t xsize = 8;
const Int_t ysize = 9;

const Char_t *xvariables[xsize]      = {"pt", "eta", "phi", "dz", "dxy","theta","qoverpt",""};
const Double_t xcutdef[xsize]        = {-.03,    2.3,   2.5, 1.8, 10,   3,      3,        0};

const Char_t *yvariables[ysize]      = {"pt",   "pt",   "eta",  "phi",  "dz",   "dxy",  "theta", "qoverpt", ""};
const Bool_t relative[ysize]         = {kTRUE,  kFALSE, kFALSE, kFALSE, kFALSE, kFALSE, kFALSE,  kFALSE,    kFALSE};
const Bool_t logscale[ysize]         = {kFALSE, kFALSE, kFALSE, kFALSE, kFALSE, kFALSE, kFALSE,  kFALSE,    kFALSE};
const Double_t yplotcutdef[ysize]    = {.015,   .001,   3,      3,      .25,    3,      3,       3,         0};
const Double_t yprofilecutdef[ysize] = {3,      3,      3,      3,      3,      3,      3,       3,         0};
const Double_t histcutdef[ysize]     = {3,      .05,    3,      3,      1,      3,      3,       3,         0};

void makePlots(Char_t *file,TString directory = "plots",Double_t *xcut = xcutdef,Double_t *yplotcut = yplotcutdef,Double_t *yprofilecut = yprofilecutdef, 
               Double_t *histcut = histcutdef)
{
    for (Int_t x = 0; x < xsize; x++)
    {
        for (Int_t y = 0; y < ysize; y++)
        {
            for (Int_t pull = 0; pull == 0 || (pull == 1 && yvariables[y] != ""); pull++)
            {
                if (false) continue;

                if (pull)
                {
                    Double_t plottemp    = yplotcut[y];
                    Double_t profiletemp = yprofilecut[y];
                    Double_t histtemp    = histcut[y];
                    yplotcut[y]    = 3;
                    yprofilecut[y] = 3;
                    histcut[y]    = 3;
                }
                Char_t *pullstring = "";
                if (pull) pullstring = "pull.";
                stringstream ss1,ss2,ss3;
                ss1 << directory;
                ss2 << directory;
                ss3 << directory;
                if (directory.Last('/') != directory.Length() - 1)
                {
                    ss1 << "/";
                    ss2 << "/";
                    ss3 << "/";
                }
                if (xvariables[x] == "" && yvariables[y] == "")
                    ss1 << "orghist." << pullstring << "ptrel_placeholder";
                else if (xvariables[x] == "")
                {
                    ss1 << "hist." << pullstring << "Delta_" << yvariables[y];
                }
                else if (yvariables[y] == "")
                {
                    ss1 << "orghist." << pullstring << xvariables[x] << "_org";
                }
                else
                {
                    ss1 << "plot."       << pullstring << xvariables[x] << "_org.Delta_" << yvariables[y];
                    ss2 << "profile."    << pullstring << xvariables[x] << "_org.Delta_" << yvariables[y];
                    ss3 << "resolution." << pullstring << xvariables[x] << "_org.Delta_" << yvariables[y];
                }
                if (relative[y])
                {
                    ss1 << ".relative";
                    ss2 << ".relative";
                    ss3 << ".relative";
                }
                ss1 << ".pngeps";
                ss2 << ".pngeps";
                ss3 << ".pngeps";
                TString s1 = ss1.str();
                TString s2 = ss2.str();
                TString s3 = ss3.str();
                if (xvariables[x] != "" && yvariables[y] != "")
                {
//                  trackSplitPlot(file,xvariables[x],yvariables[y],kFALSE,relative[y],logscale[y],kFALSE,(bool)pull,xcut[x],yplotcut[y],   s1.Data());
//                  delete gROOT->GetListOfCanvases()->Last();
                    trackSplitPlot(file,xvariables[x],yvariables[y],kTRUE, relative[y],logscale[y],kFALSE,(bool)pull,xcut[x],yprofilecut[y],s2.Data());
                    delete gROOT->GetListOfCanvases()->Last();
                    trackSplitPlot(file,xvariables[x],yvariables[y],kFALSE,relative[y],logscale[y],kTRUE, (bool)pull,xcut[x],yprofilecut[y],s3.Data());
                    delete gROOT->GetListOfCanvases()->Last();
                }
                else
                {
                    trackSplitPlot(file,xvariables[x],yvariables[y],kFALSE,relative[y],logscale[y],kFALSE,(bool)pull,xcut[x],3,         s1.Data());
                    delete gROOT->GetListOfCanvases()->Last();
                }
                if (pull)
                {
                    yplotcut[y]    = plottemp;
                    yprofilecut[y] = profiletemp;
                    histcut[y]    = histtemp;
                }
            }
            cout << y + ysize * x + 1 << "/" << xsize*ysize << endl;
        }
    }
}


void makePlots(Int_t nFiles,Char_t **files,Char_t **names,TString directory = "plots",
               Double_t *xcut = xcutdef,Double_t *yplotcut = yplotcutdef,Double_t *yprofilecut = yprofilecutdef, Double_t *histcut = histcutdef)
{
    for (Int_t x = 0; x < xsize; x++)
    {
        for (Int_t y = 0; y < ysize; y++)
        {
            for (Int_t pull = 0; pull == 0 || (pull == 1 && yvariables[y] != ""); pull++)
            {
                if (false) continue;
                if (pull)
                {
                    Double_t plottemp    = yplotcut[y];
                    Double_t profiletemp = yprofilecut[y];
                    Double_t histtemp    = histcut[y];
                    yplotcut[y]    = 3;
                    yprofilecut[y] = 3;
                    histcut[y]    = 3;
                }
                Char_t *pullstring = "";
                if (pull) pullstring = "pull.";
/*
                for (Int_t i = 0; i < nFiles; i++)
                {
                    if (xvariables[x] == "" || yvariables[y] == "") continue;
                    stringstream ss1;
                    ss1 << directory;
                    if (directory.Last('/') != directory.Length() - 1)
                        ss1 << "/";
                    ss1 << names[i] << "." << pullstring << xvariables[x] << "_org.Delta_" << yvariables[y];
                    if (relative[y])
                        ss1 << ".relative";
                    ss1 << ".pngeps";
                    TString s1 = ss1.str();
                    trackSplitPlot(files[i],xvariables[x],yvariables[y],kFALSE,relative[y],logscale[y],kFALSE,(bool)pull,xcut[x],yplotcut[y],s1.Data());
                    delete gROOT->GetListOfCanvases()->Last();
                }
*/
                stringstream ss2;
                stringstream ss3;
                ss2 << directory;
                ss3 << directory;
                if (directory.Last('/') != directory.Length() - 1)
                {
                    ss2 << "/";
                    ss3 << "/";
                }
                if (xvariables[x] == "" && yvariables[y] == "")
                    ss2 << "orghist." << pullstring << "pt_org.relative";
                else if (xvariables[x] == "")
                    ss2 << "hist." << pullstring << "Delta_" << yvariables[y];
                else if (yvariables[y] == "")
                    ss2 << "orghist." << pullstring << xvariables[x] << "_org";
                else
                {
                    ss2 << "profile." << pullstring << xvariables[x] << "_org.Delta_" << yvariables[y];
                    ss3 << "resolution." << pullstring << xvariables[x] << "_org.Delta_" << yvariables[y];
                }
                if (relative[y])
                {
                    ss2 << ".relative";
                    ss3 << ".relative";
                }
                ss2 << ".pngeps";
                ss3 << ".pngeps";
                TString s2 = ss2.str();
                TString s3 = ss3.str();
                if (xvariables[x] != "" && yvariables[y] != "")
                {
                    trackSplitPlot(nFiles,files,names,xvariables[x],yvariables[y],relative[y],logscale[y],kFALSE,(bool)pull,xcut[x],yprofilecut[y],s2.Data());
                    delete gROOT->GetListOfCanvases()->Last();
                    trackSplitPlot(nFiles,files,names,xvariables[x],yvariables[y],relative[y],logscale[y],kTRUE ,(bool)pull,xcut[x],yprofilecut[y],s3.Data());
                    delete gROOT->GetListOfCanvases()->Last();
                }
                else
                {
                    trackSplitPlot(nFiles,files,names,xvariables[x],yvariables[y],relative[y],logscale[y],kFALSE,(bool)pull,xcut[x],histcut[y]    ,s2.Data());
                    delete gROOT->GetListOfCanvases()->Last();
                }
                if (pull)
                {
                    yplotcut[y]    = plottemp;
                    yprofilecut[y] = profiletemp;
                    histcut[y]     = histtemp;
                }
            }
            cout << y + ysize * x + 1 << "/" << xsize*ysize << endl;
        }
    }
}

