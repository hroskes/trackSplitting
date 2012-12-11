#include "etaDependence.C"

const Int_t xsize = 5;
const Int_t ysize = 6;
const Char_t *xvariables[xsize] = {"pt",     "eta","phi","dz","dxy"};
const Char_t *yvariables[ysize] = {"pt","pt","eta","phi","dz","dxy"};
const Bool_t relative[ysize] = {kTRUE,kFALSE,kFALSE,kFALSE,kFALSE,kFALSE};
const Double_t xcutdef[xsize] = {10,10,10,10,10};
const Double_t yplotcutdef[ysize] = {3,1.5,3,3,3,3};
const Double_t yprofilecutdef[ysize] = {10,10,10,10,10,10};

void makePlots(TString directory = "plots",Double_t *xcut = xcutdef,Double_t *yplotcut = yplotcutdef,Double_t *yprofilecut = yprofilecutdef)
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
            etaDependence(xvariables[x],yvariables[y],kFALSE,relative[y],xcut[x],yplotcut[y],   s1.Data());
            cout << y + ysize * x + 1 << "/" << xsize*ysize << endl;
            etaDependence(xvariables[x],yvariables[y],kTRUE, relative[y],xcut[x],yprofilecut[y],s2.Data());
            cout << y + ysize * x + 2 << "/" << xsize*ysize << endl;
        }
    }
}
