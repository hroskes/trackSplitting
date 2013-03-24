#include "trackSplitPlot.C"

const Int_t xsize = 8;
const Int_t ysize = 8;

const Char_t *xvariables[xsize]      = {"pt", "eta", "phi", "dz", "dxy","theta","qoverpt",""};
const Double_t xcutdef[xsize]        = {-.03,    2.3,   2.5, 1.8, 10,   3,      3,        0};

const Char_t *yvariables[ysize]      = {"pt",   "pt",   "eta",  "phi",  "dz",   "dxy",  "theta", "qoverpt"};
const Bool_t relative[ysize]         = {kTRUE,  kFALSE, kFALSE, kFALSE, kFALSE, kFALSE, kFALSE,  kFALSE};
const Bool_t logscale[ysize]         = {kFALSE, kFALSE, kFALSE, kFALSE, kFALSE, kFALSE, kFALSE,  kFALSE};
const Double_t yplotcutdef[ysize]    = {.015,   .001,   3,      3,      .25,    3,      3,       3};
const Double_t yprofilecutdef[ysize] = {3,      3,      3,      3,      3,      3,      3,       3};
const Double_t histcutdef[ysize]     = {3,      .05,    3,      3,      1,      3,      3,       3};

void makePlots(Char_t *file,TString directory = "plots",Double_t *xcut = xcutdef,Double_t *yplotcut = yplotcutdef,Double_t *yprofilecut = yprofilecutdef, 
               Double_t *histcut = histcutdef)
{
    for (Int_t x = 0; x < xsize; x++)
    {
        for (Int_t y = 0; y < ysize; y++)
        {
            if (false) continue;
            stringstream ss1,ss2;
            ss1 << directory;
            ss2 << directory;
            if (directory.Last('/') != directory.Length() - 1)
            {
                ss1 << "/";
                ss2 << "/";
            }
            if (xvariables[x] == "")
            {
                ss1 << "hist.Delta_" << yvariables[y];
            }
            else
            {
                ss1 << "plot."    << xvariables[x] << "_org.Delta_" << yvariables[y];
                ss2 << "profile." << xvariables[x] << "_org.Delta_" << yvariables[y];
            }
            if (relative[y])
            {
                ss1 << ".relative";
                ss2 << ".relative";
            }
            ss1 << ".png";
            ss2 << ".png";
            TString s1 = ss1.str();
            TString s2 = ss2.str();
            if (xvariables[x] != "")
            {
//              trackSplitPlot(file,xvariables[x],yvariables[y],kFALSE,relative[y],logscale[y],xcut[x],yplotcut[y],   s1.Data());
                trackSplitPlot(file,xvariables[x],yvariables[y],kTRUE, relative[y],logscale[y],xcut[x],yprofilecut[y],s2.Data());
            }
            else
                trackSplitPlot(file,xvariables[x],yvariables[y],kFALSE,relative[y],logscale[y],xcut[x],histcut[y],    s1.Data());
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
            if (false) continue;
/*
            for (Int_t i = 0; i < nFiles; i++)
            {
                if (xvariables[x] == "") continue;
                stringstream ss1;
                ss1 << directory;
                if (directory.Last('/') != directory.Length() - 1)
                    ss1 << "/";
                ss1 << names[i] << "." << xvariables[x] << "_org.Delta_" << yvariables[y];
                if (relative[y])
                    ss1 << ".relative";
                ss1 << ".png";
                TString s1 = ss1.str();
                trackSplitPlot(files[i],xvariables[x],yvariables[y],kFALSE,relative[y],logscale[y],xcut[x],yplotcut[y],s1.Data());
            }
*/
            stringstream ss2;
            ss2 << directory;
            if (directory.Last('/') != directory.Length() - 1)
                ss2 << "/";
            if (xvariables[x] == "")
                ss2 << "hist.Delta_" << yvariables[y];
            else
                ss2 << "profile." << xvariables[x] << "_org.Delta_" << yvariables[y];
            if (relative[y])
                ss2 << ".relative";
            ss2 << ".png";
            TString s2 = ss2.str();
            if (xvariables[x] != "")
                trackSplitPlot(nFiles,files,names,xvariables[x],yvariables[y],relative[y],logscale[y],xcut[x],yprofilecut[y],s2.Data());
            else
                trackSplitPlot(nFiles,files,names,xvariables[x],yvariables[y],relative[y],logscale[y],xcut[x],histcut[y]    ,s2.Data());
            cout << y + ysize * x + 1 << "/" << xsize*ysize << endl;
        }
    }
}

