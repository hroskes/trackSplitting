#include "misalignmentDependence.C"
#include "TString.h"
#include "TROOT.h"

const Int_t xsize = 10;
const Int_t ysize = 9;

bool misalign = false;

TString xvariables[xsize]      = {"pt", "eta", "phi", "dz", "dxy","theta","qoverpt","runNumber","nHits",""};
TString yvariables[ysize]      = {"pt",   "pt",   "eta",  "phi",  "dz",   "dxy",  "theta", "qoverpt", ""};
Bool_t relative[ysize]         = {kTRUE,  kFALSE, kFALSE, kFALSE, kFALSE, kFALSE, kFALSE,  kFALSE,    kFALSE};
Bool_t logscale[ysize]         = {kFALSE, kFALSE, kFALSE, kFALSE, kFALSE, kFALSE, kFALSE,  kFALSE,    kFALSE};

void placeholders(TString directory);

void makePlots(Int_t nFiles,TString *files,TString *names,TString directory = "plots",
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
                    //trackSplitPlot(files[i],xvariables[x],yvariables[y],kFALSE,relative[y],logscale[y],kFALSE,(bool)pull,s[i]);
                    //delete gROOT->GetListOfCanvases()->Last();
                }

                if (xvariables[x] != "" && yvariables[y] != "")
                {
                    trackSplitPlot(nFiles,files,names,xvariables[x],yvariables[y],relative[y],logscale[y],kFALSE,(bool)pull,s[i]);
                    delete gROOT->GetListOfCanvases()->Last();
                    trackSplitPlot(nFiles,files,names,xvariables[x],yvariables[y],relative[y],logscale[y],kTRUE ,(bool)pull,s[i+1]);
                    delete gROOT->GetListOfCanvases()->Last();
                }
                else
                {
                    trackSplitPlot(nFiles,files,names,xvariables[x],yvariables[y],relative[y],logscale[y],kFALSE,(bool)pull,s[i]);
                    delete gROOT->GetListOfCanvases()->Last();
                }
            }
            if (!misalign)
                cout << y + ysize * x + 1 << "/" << xsize*ysize << endl;
            else
                cout << y + ysize * x + 1 << "/" << (xsize+1)*ysize << endl;
        }
    }
}

void makePlots(Int_t nFiles,TString *files,TString *names,TString misalignment,Double_t *values,TString directory = "plots",
               Int_t min = 0, Int_t max = xsize*ysize + 100)
{
    const int n = nFiles;
    if (names == 0)
    {
        names = new TString[n];
        for (int i = 0; i < n; i++)
        {
            stringstream s;
            s << values[i];
            names[i] = s.str();
        }
    }
    misalign = true;
    makePlots(nFiles,files,names,directory,min,max);
    misalign = false;
    int x = xsize;

    TString slashstring = "";
    if (directory.Last('/') != directory.Length() - 1) slashstring = "/";

    for (int y = 0; y < ysize; y++)
    {
        if (y + ysize * x + 1 < min || y + ysize * x + 1 > max) continue;
        for (int resolution = 0; resolution < 2; resolution++)
        {
            for (int pull = 0; pull < 2; pull++)
            {
                TString plotname;
                if (!resolution)
                    plotname = "profile";
                else
                    plotname = "resolution";

                TString pullstring = "";
                if (pull) pullstring = "pull.";

                TString xvarstring = misalignment;
                xvarstring.Append(".");

                TString yvarstring = yvariables[y];
                yvarstring.Prepend("Delta_");

                TString relativestring = "";
                if (relative[y]) relativestring = ".relative";

                stringstream s;
                s << directory << slashstring << plotname << "." << pullstring
                  << xvarstring << yvarstring << relativestring << ".pngepsroot";

                misalignmentDependence(nFiles,files,values,misalignment,yvariables[y],relative[y],logscale[y],resolution,pull,s.str());
                delete gROOT->GetListOfCanvases()->Last();
            }
        }
        cout << y + ysize * x + 1 << "/" << (xsize+1)*ysize << endl;
    }
}

void makePlots(TString file,TString directory = "plots", 
               Int_t min = 0, Int_t max = xsize*ysize + 100)
{
    TString *files = &file;
    TString name = "plot";
    TString *names = &name;
    makePlots(1,files,names,directory,min,max);
}

void placeholders(TString directory)
{
    directory = directory; //to keep it from giving a warning unused parameter
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
