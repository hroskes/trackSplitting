#include "nProfiles.C"

const Int_t xsize = 5;
const Int_t ysize = 6;

const Char_t *xvariables[xsize]      = {"pt", "eta", "phi", "dz", "dxy"};
const Double_t xcutdef[xsize]        = {.18,   10,    2.5,   2,    10};

const Char_t *yvariables[ysize]      = {"pt",   "pt",   "eta",  "phi",  "dz",   "dxy"};
const Bool_t relative[ysize]         = {kTRUE,  kFALSE, kFALSE, kFALSE, kFALSE, kFALSE};
const Bool_t logscale[ysize]         = {kFALSE, kFALSE, kFALSE, kFALSE, kFALSE, kFALSE};
const Double_t yplotcutdef[ysize]    = {.02,    .0003,    3,      3,      1,      3};
const Double_t yprofilecutdef[ysize] = {10,     10,     10,     10,     10,     10};

void makePlots(Char_t *file,TString directory = "plots",Double_t *xcut = xcutdef,Double_t *yplotcut = yplotcutdef,Double_t *yprofilecut = yprofilecutdef)
{
    for (Int_t x = 0; x < xsize; x++)
    {
        for (Int_t y = 0; y < ysize; y++)
        {
            stringstream ss1,ss2;
            ss1 << directory;
            ss2 << directory;
            if (directory.Last('/') != directory.Length() - 1)
            {
                ss1 << "/";
                ss2 << "/";
            }
            ss1 << "plot."    << xvariables[x] << "_org.Delta_" << yvariables[y];
            ss2 << "profile." << xvariables[x] << "_org.Delta_" << yvariables[y];
            if (relative[y])
            {
                ss1 << ".relative";
                ss2 << ".relative";
            }
            ss1 << ".png";
            ss2 << ".png";
            TString s1 = ss1.str();
            TString s2 = ss2.str();
            trackSplitPlot(file,xvariables[x],yvariables[y],kFALSE,relative[y],logscale[y],xcut[x],yplotcut[y],   s1.Data());
            trackSplitPlot(file,xvariables[x],yvariables[y],kTRUE, relative[y],logscale[y],xcut[x],yprofilecut[y],s2.Data());
            cout << y + ysize * x + 1 << "/" << xsize*ysize << endl;
        }
    }
}


void makePlots(Int_t nFiles,Char_t **files,Char_t **names,TString directory = "plots",
               Double_t *xcut = xcutdef,Double_t *yplotcut = yplotcutdef,Double_t *yprofilecut = yprofilecutdef)
{
    for (Int_t x = 0; x < xsize; x++)
    {
        for (Int_t y = 0; y < ysize; y++)
        {
            if (y != 1) continue;
            for (Int_t i = 0; i < nFiles; i++)
            {
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
            stringstream ss2;
            ss2 << directory;
            if (directory.Last('/') != directory.Length() - 1)
                ss2 << "/";
            ss2 << "profile." << xvariables[x] << "_org.Delta_" << yvariables[y];
            if (relative[y])
                ss2 << ".relative";
            ss2 << ".png";
            TString s2 = ss2.str();
            nProfiles(nFiles,files,names,xvariables[x],yvariables[y],relative[y],logscale[y],xcut[x],yprofilecut[y],s2.Data());
            cout << y + ysize * x + 1 << "/" << xsize*ysize << endl;
        }
    }
}

