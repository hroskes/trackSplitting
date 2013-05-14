#include "trackSplitPlot.C"

const Int_t xsize = 9;
const Int_t ysize = 9;

const Char_t *xvariables[xsize]      = {"pt", "eta", "phi", "dz", "dxy","theta","qoverpt","runNumber",""};
const Double_t xcutdef[xsize]        = {-.03, 2.3,   2.5,   1.8,  10,   3,      3,        0,          0};

const Char_t *yvariables[ysize]      = {"pt",   "pt",   "eta",  "phi",  "dz",   "dxy",  "theta", "qoverpt", ""};
const Bool_t relative[ysize]         = {kTRUE,  kFALSE, kFALSE, kFALSE, kFALSE, kFALSE, kFALSE,  kFALSE,    kFALSE};
const Bool_t logscale[ysize]         = {kFALSE, kFALSE, kFALSE, kFALSE, kFALSE, kFALSE, kFALSE,  kFALSE,    kFALSE};
const Double_t yplotcutdef[ysize]    = {.015,   .001,   3,      3,      .25,    3,      3,       3,         0};
const Double_t yprofilecutdef[ysize] = {3,      3,      3,      3,      3,      3,      3,       3,         0};
const Double_t histcutdef[ysize]     = {3,      .05,    3,      3,      1,      3,      3,       3,         0};

void makePlots(Char_t *file,TString directory = "plots",Double_t *xcut = xcutdef,Double_t *yplotcut = yplotcutdef,Double_t *yprofilecut = yprofilecutdef, 
               Double_t *histcut = histcutdef)
{
    Char_t **files = &file;
    Char_t *name = "plot";
    Char_t **names = &name;
    makePlots(1,files,names,directory,xcut,yplotcut,yprofilecut,histcut);
/*
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
                ss1 << ".pngepsroot";
                ss2 << ".pngepsroot";
                ss3 << ".pngepsroot";
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
*/
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
                if (x != 7) continue;
                if (pull)
                {
                    Double_t plottemp    = yplotcut[y];
                    Double_t profiletemp = yprofilecut[y];
                    Double_t histtemp    = histcut[y];
                    yplotcut[y]    = 3;
                    yprofilecut[y] = 3;
                    histcut[y]    = 3;
                }

                const Int_t nPlots = nFiles+2;
                stringstream *ss = new stringstream[nPlots];
                TString *s = new TString[nPlots];

                Char_t *slashstring = "";
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

                Char_t *pullstring = "";
                if (pull) pullstring = "pull.";

                TString xvarstring = xvariables[x];
                if (xvariables[x] != "runNumber" && xvariables[x] != "") xvarstring.Append("_org");
                if (xvariables[x] != "" && yvariables[y] != "") xvarstring.Append(".");

                TString yvarstring = yvariables[y];
                if (yvariables[y] != "") yvarstring.Prepend("Delta_");

                Char_t *relativestring = "";
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
                    //trackSplitPlot(files[i],xvariables[x],yvariables[y],kFALSE,relative[y],logscale[y],kFALSE,(bool)pull,xcut[x],yplotcut[y],s[i].Data());
                    //delete gROOT->GetListOfCanvases()->Last();
                }

                if (xvariables[x] != "" && yvariables[y] != "")
                {
                    trackSplitPlot(nFiles,files,names,xvariables[x],yvariables[y],relative[y],logscale[y],kFALSE,(bool)pull,xcut[x],yprofilecut[y],s[i].Data());
                    delete gROOT->GetListOfCanvases()->Last();
                    trackSplitPlot(nFiles,files,names,xvariables[x],yvariables[y],relative[y],logscale[y],kTRUE ,(bool)pull,xcut[x],yprofilecut[y],s[i+1].Data());
                    delete gROOT->GetListOfCanvases()->Last();
                }
                else
                {
                    trackSplitPlot(nFiles,files,names,xvariables[x],yvariables[y],relative[y],logscale[y],kFALSE,(bool)pull,xcut[x],histcut[y]    ,s[i].Data());
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
