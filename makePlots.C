#include "etaDependence.C"

void makePlots(TString directory = "plots",Double_t xnumberofsigmas = 10,Double_t ynumberofsigmas = 10)
{
    Char_t *variables[5] = {"pt","eta","phi","dz","dxy"};
    Bool_t relative[5] = {kFALSE,kFALSE,kFALSE,kFALSE,kFALSE};
    for (Int_t x = 0; x < 5; x++)
    {
        for (Int_t y = 0; y < 5; y++)
        {
            stringstream ss1,ss2;
            ss1 << directory;
            ss2 << directory;
            if (directory.Last('/') != directory.Length() - 1)
            {
                ss1 << "/";
                ss2 << "/";
            }
            ss1 << "plot."    << variables[x] << "_org.Delta_" << variables[y] << ".png";
            ss2 << "profile." << variables[x] << "_org.Delta_" << variables[y] << ".png";
            TString s1 = ss1.str();
            TString s2 = ss2.str();
            etaDependence(variables[x],variables[y],kFALSE,relative[y],xnumberofsigmas,ynumberofsigmas,s1.Data());
            etaDependence(variables[x],variables[y],kTRUE, relative[y],xnumberofsigmas,ynumberofsigmas,s2.Data());
        }
    }
}
