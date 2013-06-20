#include "trackSplitPlot.C"
#include "TString.h"
#include "TROOT.h"

const Int_t xsize = 10;
const Int_t ysize = 9;

TString xvariables[xsize]      = {"pt", "eta", "phi", "dz", "dxy","theta","qoverpt","runNumber","nHits",""};
Double_t xcutdef[xsize]        = {-.03, 2.3,   2.5,   1.8,  10,   3,      3,        0,          0,      0};

TString yvariables[ysize]      = {"pt",   "pt",   "eta",  "phi",  "dz",   "dxy",  "theta", "qoverpt", ""};
Bool_t relative[ysize]         = {kTRUE,  kFALSE, kFALSE, kFALSE, kFALSE, kFALSE, kFALSE,  kFALSE,    kFALSE};
Bool_t logscale[ysize]         = {kFALSE, kFALSE, kFALSE, kFALSE, kFALSE, kFALSE, kFALSE,  kFALSE,    kFALSE};
Double_t yplotcutdef[ysize]    = {.015,   .001,   3,      3,      .25,    3,      3,       3,         0};
Double_t yprofilecutdef[ysize] = {3,      3,      3,      3,      3,      3,      3,       3,         0};
Double_t histcutdef[ysize]     = {3,      .05,    3,      3,      1,      3,      3,       3,         0};

void placeholders(TString directory);

void makePlots(Int_t nFiles,TString *files,TString *names,TString directory = "plots",
               Double_t *xcut = xcutdef,Double_t *yplotcut = yplotcutdef,Double_t *yprofilecut = yprofilecutdef, Double_t *histcut = histcutdef,
               Int_t min = 0, Int_t max = xsize*ysize + 100)
{
    for (Int_t x = 0; x < xsize; x++)
    {
        for (Int_t y = 0; y < ysize; y++)
        {
            for (Int_t pull = 0; pull == 0 || (pull == 1 && yvariables[y] != ""); pull++)
            {
                if (false) continue;
                if (y + ysize*x + 1 < min || y + ysize*x + 1 > max) continue;

                if (x == 9 && y == 8)
                {
                    if (!pull) placeholders(directory);
                    continue;
                }
                Double_t plottemp    = yplotcut[y];
                Double_t profiletemp = yprofilecut[y];
                Double_t histtemp    = histcut[y];
                if (pull)
                {
                    yplotcut[y]    = 3;
                    yprofilecut[y] = 3;
                    histcut[y]    = 3;
                }

                const Int_t nPlots = nFiles+2;
                stringstream *ss = new stringstream[nPlots];
                TString *s = new TString[nPlots];

                TString slashstring = "";
                if (directory.Last('/') != directory.Length() - 1) slashstring = "/";

                TString *plotnames = new TString[nPlots];
                for (Int_t i = 0; i < nFiles; i++)
                {
                    plotnames[i] = names[i];
                }
                if (yvariables[y] == "")
                    plotnames[nFiles] = "orghist";
                else if (xvariables[x] == "")
                    plotnames[nFiles] = "hist";
                else
                    plotnames[nFiles] = "profile";
                plotnames[nFiles+1] = "resolution";

                TString pullstring = "";
                if (pull) pullstring = "pull.";

                TString xvarstring = xvariables[x];
                if (xvariables[x] != "runNumber" && xvariables[x] != "nHits" && xvariables[x] != "") xvarstring.Append("_org");
                if (xvariables[x] != "" && yvariables[y] != "") xvarstring.Append(".");

                TString yvarstring = yvariables[y];
                if (yvariables[y] != "") yvarstring.Prepend("Delta_");

                TString relativestring = "";
                if (relative[y]) relativestring = ".relative";

                //if (xvariables[x] == "" && yvariables[y] == "")
                //{
                //    xvarstring = "pt_org";
                //    relativestring = ".relative";
                //}

                for (Int_t i = 0; i < nPlots; i++)
                {
                    ss[i] << directory << slashstring << plotnames[i] << "." << pullstring 
                          << xvarstring << yvarstring << relativestring << ".pngepsroot";
                    s[i] = ss[i].str();
                }

                Int_t i;
                for (i = 0; i < nFiles; i++)
                {
                    if (xvariables[x] == "" || yvariables[y] == "") continue;
                    //trackSplitPlot(files[i],xvariables[x],yvariables[y],kFALSE,relative[y],logscale[y],kFALSE,(bool)pull,xcut[x],yplotcut[y],s[i]);
                    //delete gROOT->GetListOfCanvases()->Last();
                }

                if (xvariables[x] != "" && yvariables[y] != "")
                {
                    trackSplitPlot(nFiles,files,names,xvariables[x],yvariables[y],relative[y],logscale[y],kFALSE,(bool)pull,xcut[x],yprofilecut[y],s[i]);
                    delete gROOT->GetListOfCanvases()->Last();
                    trackSplitPlot(nFiles,files,names,xvariables[x],yvariables[y],relative[y],logscale[y],kTRUE ,(bool)pull,xcut[x],yprofilecut[y],s[i+1]);
                    delete gROOT->GetListOfCanvases()->Last();
                }
                else
                {
                    trackSplitPlot(nFiles,files,names,xvariables[x],yvariables[y],relative[y],logscale[y],kFALSE,(bool)pull,xcut[x],histcut[y]    ,s[i]);
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

void makePlots(TString file,TString directory = "plots",Double_t *xcut = xcutdef,Double_t *yplotcut = yplotcutdef,Double_t *yprofilecut = yprofilecutdef, 
               Double_t *histcut = histcutdef, Int_t min = 0, Int_t max = xsize*ysize + 100)
{
    TString *files = &file;
    TString name = "plot";
    TString *names = &name;
    makePlots(1,files,names,directory,xcut,yplotcut,yprofilecut,histcut,min,max);
}

void makePlots(Int_t nFiles,TString *files,TString *names,TString directory = "plots",
               Double_t *xcut = xcutdef,Double_t *yplotcut = yplotcutdef,Double_t *yprofilecut = yprofilecutdef, Double_t *histcut = histcutdef)
{
    makePlots(nFiles,files,names,directory,xcut,yplotcut,yprofilecut,histcut,0,ysize*xsize+100);
}

void makePlots(TString file,TString directory = "plots",Double_t *xcut = xcutdef,Double_t *yplotcut = yplotcutdef,Double_t *yprofilecut = yprofilecutdef, 
               Double_t *histcut = histcutdef)
{
    makePlots(file,directory,xcut,yplotcut,yprofilecut,histcut,0,ysize*xsize+100);
}

void placeholders(TString directory)
{
    /*
    TString filename = "orghist.z_placeholder1.pngepsroot";
    TString slashstring = "";
    if (directory.Last('/') != directory.Length() - 1) slashstring = "/";
    filename.Prepend(slashstring);
    filename.Prepend(directory);
    placeholder(filename);
    placeholder(filename.ReplaceAll("placeholder1","placeholder2"));
    placeholder(filename.ReplaceAll("placeholder2","placeholder3"));
    */
    //keeping this space in case any more placeholders are needed in the future
}
