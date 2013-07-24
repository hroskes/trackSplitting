#include "misalignmentDependence.C"
#include "TString.h"
#include "TROOT.h"

const Int_t xsize = 10;
const Int_t ysize = 9;

TString xvariables[xsize]      = {"pt", "eta", "phi", "dz", "dxy","theta","qoverpt","runNumber","nHits",""};
TString yvariables[ysize]      = {"pt",   "pt",   "eta",  "phi",  "dz",   "dxy",  "theta", "qoverpt", ""};
Bool_t relative[ysize]         = {kTRUE,  kFALSE, kFALSE, kFALSE, kFALSE, kFALSE, kFALSE,  kFALSE,    kFALSE};

void placeholders(TString directory);

void makePlots(Int_t nFiles,TString *files,TString *names,TString misalignment,Double_t *values,TString directory = "plots",
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

                Int_t nPlots = nFiles+4;                     //scatterplot for each (if you uncomment it), profile, resolution, and fits for each.
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
                if (plotnames[nFiles] == "profile")
                {
                    plotnames[nFiles+2] = ".profile";
                    plotnames[nFiles+2].Prepend(misalignment);
                    plotnames[nFiles+3] = ".resolution";
                    plotnames[nFiles+3].Prepend(misalignment);
                    plotnames[nFiles+2].Prepend("fits/");
                    plotnames[nFiles+3].Prepend("fits/");
                }
                else
                {
                    plotnames[nFiles+2] = "profile.";
                    plotnames[nFiles+2].Append(misalignment);
                    plotnames[nFiles+3] = "resolution.";
                    plotnames[nFiles+3].Append(misalignment);
                }

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
                    if (misalignment != "")
                    {
                        TString wrongway = misalignment;
                        TString rightway = misalignment;
                        wrongway.Append (".pull");
                        rightway.Prepend("pull.");
                        s[i].ReplaceAll(wrongway,rightway);
                    }
                }

                Int_t i;
                for (i = 0; i < nFiles; i++)
                {
                    if (xvariables[x] == "" || yvariables[y] == "") continue;
                    //delete trackSplitPlot(files[i],xvariables[x],yvariables[y],kFALSE,relative[y],kFALSE,(bool)pull,s[i]); //deletes the canvas
                }

                if (xvariables[x] != "" && yvariables[y] != "")
                {
                    TCanvas *c1 = trackSplitPlot(nFiles,files,names,xvariables[x],yvariables[y],relative[y],kFALSE,(bool)pull,s[i]);
                    if (misalignmentDependence(c1,nFiles,names,misalignment,values,xvariables[x],yvariables[y],
                                               true,relative[y],false,(bool)pull,s[i+2]))
                    {
                        s[i+2].ReplaceAll(".png",".parameter.png");
                        misalignmentDependence(c1,nFiles,names,misalignment,values,xvariables[x],yvariables[y],
                                                   false,relative[y],false,(bool)pull,s[i+2]);
                        delete gROOT->GetListOfCanvases()->Last();
                        delete gROOT->GetListOfCanvases()->Last();
                    }
                    delete c1;

                    TCanvas *c2 = trackSplitPlot(nFiles,files,names,xvariables[x],yvariables[y],relative[y],kTRUE ,(bool)pull,s[i+1]);
                    if (misalignmentDependence(c2,nFiles,names,misalignment,values,xvariables[x],yvariables[y],
                                               true,relative[y],true,(bool)pull,s[i+3]))
                    {
                        s[i+2].ReplaceAll("/fits/profile.","/fits/parameter.profile");
                        misalignmentDependence(c2,nFiles,names,misalignment,values,xvariables[x],yvariables[y],
                                                   false,relative[y],true,(bool)pull,s[i+3]);
                        delete gROOT->GetListOfCanvases()->Last();
                        delete gROOT->GetListOfCanvases()->Last();
                    }
                    delete c2;
                }
                else
                {
                    TCanvas *c1 = trackSplitPlot(nFiles,files,names,xvariables[x],yvariables[y],relative[y],kFALSE,(bool)pull,s[i]);
                    if (misalignmentDependence(c1,nFiles,names,misalignment,values,xvariables[x],yvariables[y],
                                               true,relative[y],false,(bool)pull,s[i+2]))
                    {
                        delete gROOT->GetListOfCanvases()->Last();
                        misalignmentDependence(c1,nFiles,names,misalignment,values,xvariables[x],yvariables[y],
                                               true,relative[y],true,(bool)pull,s[i+3]);
                        delete gROOT->GetListOfCanvases()->Last();
                    }
                    delete c1;
                }
            }
            cout << y + ysize * x + 1 << "/" << xsize*ysize << endl;
        }
    }
}

void makePlots(Int_t nFiles,TString *files,TString *names,TString directory = "plots",
               Int_t min = 0, Int_t max = xsize*ysize + 100)
{
    makePlots(nFiles,files,names,"",(Double_t*)0,directory,
              min,max);
}

void makePlots(TString file,TString directory = "plots", 
               Int_t min = 0, Int_t max = xsize*ysize + 100)
{
    TString *files = &file;
    TString name = "scatterplot";
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
