#include "misalignmentDependence.C"
#include "TString.h"

const Int_t xsize = 10;
const Int_t ysize = 9;

TString xvariables[xsize]      = {"pt", "eta", "phi", "dz",  "dxy", "theta", "qoverpt", "runNumber","nHits",""};
TString yvariables[ysize]      = {"pt", "pt",  "eta", "phi", "dz",  "dxy",   "theta",   "qoverpt", ""};
Bool_t relative[ysize]         = {true, false, false, false, false, false,   false,     false,     false};

//********************************************************************
//general functions
//it makes plots for each pair of variables if matrix[x][y] is true
//the order of the variables in the matrix is the same as shown above,
// in xvariables and yvariables.
//********************************************************************

void makePlots(Int_t nFiles,TString *files,TString *names,TString misalignment,Double_t *values,TString directory,
               Bool_t matrix[xsize][ysize])
{
    stufftodelete->SetOwner(true);

    for (Int_t i = 0; i < nFiles; i++)
    {
        TFile *f = 0;
        bool exists = false;

        for (int j = 1; j <= 60*24 && !exists; j++)  //wait up to 1 day for the validation to be finished
        {
            f = TFile::Open(files[i]);
            if (f != 0)
                exists = f->IsOpen();
            delete f;
            if (exists) continue;
            gSystem->Sleep(60000);
            cout << "It's been ";
            if (j >= 60)
                cout << j/60 << " hour";
            if (j >= 120)
                cout << "s";
            if (j % 60 != 0 && j >= 60)
                cout << " and ";
            if (j % 60 != 0)
                cout << j%60 << " minute";
            if (j % 60 >= 2)
                cout << "s";
            cout << endl;
          }
          if (!exists) return;
    }

    TString directorytomake = directory;
    gSystem->mkdir(directorytomake,true);
    if (misalignment != "")
    {
        directorytomake.Append("/fits");
        gSystem->mkdir(directorytomake);
    }

    for (Int_t x = 0; x < xsize; x++)
    {
        for (Int_t y = 0; y < ysize; y++)
        {
            for (Int_t pull = 0; pull == 0 || (pull == 1 && yvariables[y] != ""); pull++)
            {
                if (false) continue;        //this line is to make it easier to do e.g. all plots involving Delta eta
                                            //(replace false with yvariables[y] != "eta")

                if (!matrix[x][y]) continue;

                if (x == 9 && y == 8) continue;

                Int_t nPlots = nFiles+4;                     //scatterplot for each (if you uncomment it), profile, resolution, and fits for each.
                vector<TString> s;

                TString slashstring = "";
                if (directory.Last('/') != directory.Length() - 1) slashstring = "/";

                vector<TString> plotnames;
                for (Int_t i = 0; i < nFiles; i++)
                {
                    plotnames.push_back(names[i]);   //this is plotnames[i]
                    plotnames[i].ReplaceAll(" ","");
                }

                plotnames.push_back("");             //this is plotnames[nFiles], but gets changed
                if (yvariables[y] == "")
                    plotnames[nFiles] = "orghist";
                else if (xvariables[x] == "")
                    plotnames[nFiles] = "hist";
                else
                    plotnames[nFiles] = "profile";

                plotnames.push_back("resolution");   //this is plotnames[nFiles+1]

                plotnames.push_back("");             //this is plotnames[nFiles+2]
                plotnames.push_back("");             //this is plotnames[nFiles+3]
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

                for (Int_t i = 0; i < nPlots; i++)
                {
                    stringstream ss;
                    ss << directory << slashstring << plotnames[i] << "." << pullstring 
                       << xvarstring << yvarstring << relativestring << ".pngepsroot";
                    s.push_back(ss.str());
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
                    //uncomment this section to make scatterplots
                    /*
                    trackSplitPlot(files[i],xvariables[x],yvariables[y],false,relative[y],false,(bool)pull,s[i]);
                    stufftodelete->Clear();
                    for ( ; gROOT->GetListOfCanvases()->GetEntries() > 0; )
                        deleteCanvas( gROOT->GetListOfCanvases()->Last());
                    for ( ; gROOT->GetListOfFiles()->GetEntries() > 0; )
                        delete (TFile*)gROOT->GetListOfFiles()->Last();
                    */
                }

                if (xvariables[x] != "" && yvariables[y] != "")
                {
                    //make profile
                    TCanvas *c1 = trackSplitPlot(nFiles,files,names,xvariables[x],yvariables[y],relative[y],false,(bool)pull,s[i]);
                    if (misalignmentDependence(c1,nFiles,names,misalignment,values,xvariables[x],yvariables[y],
                                               true,relative[y],false,(bool)pull,s[i+2]))
                    {
                        s[i+2].ReplaceAll(".png",".parameter.png");
                        misalignmentDependence(c1,nFiles,names,misalignment,values,xvariables[x],yvariables[y],
                                                   false,relative[y],false,(bool)pull,s[i+2]);
                    }
                    stufftodelete->Clear();
                    for ( ; gROOT->GetListOfCanvases()->GetEntries() > 0; )
                        deleteCanvas( gROOT->GetListOfCanvases()->Last());
                    for ( ; gROOT->GetListOfFiles()->GetEntries() > 0; )
                        delete (TFile*)gROOT->GetListOfFiles()->Last();

                    //make resolution plot
                    TCanvas *c2 = trackSplitPlot(nFiles,files,names,xvariables[x],yvariables[y],relative[y],true ,(bool)pull,s[i+1]);
                    if (misalignmentDependence(c2,nFiles,names,misalignment,values,xvariables[x],yvariables[y],
                                               true,relative[y],true,(bool)pull,s[i+3]))
                    {
                        s[i+2].ReplaceAll("/fits/profile.","/fits/parameter.profile");
                        misalignmentDependence(c2,nFiles,names,misalignment,values,xvariables[x],yvariables[y],
                                                   false,relative[y],true,(bool)pull,s[i+3]);
                    }
                    stufftodelete->Clear();
                    for ( ; gROOT->GetListOfCanvases()->GetEntries() > 0; )
                        deleteCanvas( gROOT->GetListOfCanvases()->Last());
                    for ( ; gROOT->GetListOfFiles()->GetEntries() > 0; )
                        delete (TFile*)gROOT->GetListOfFiles()->Last();
                }
                else
                {
                    //make histogram
                    TCanvas *c1 = trackSplitPlot(nFiles,files,names,xvariables[x],yvariables[y],relative[y],false,(bool)pull,s[i]);
                    if (misalignmentDependence(c1,nFiles,names,misalignment,values,xvariables[x],yvariables[y],
                                               true,relative[y],false,(bool)pull,s[i+2]))
                    {
                        misalignmentDependence(c1,nFiles,names,misalignment,values,xvariables[x],yvariables[y],
                                               true,relative[y],true,(bool)pull,s[i+3]);
                    }
                    stufftodelete->Clear();
                    for ( ; gROOT->GetListOfCanvases()->GetEntries() > 0; )
                        deleteCanvas( gROOT->GetListOfCanvases()->Last());
                    for ( ; gROOT->GetListOfFiles()->GetEntries() > 0; )
                        delete (TFile*)gROOT->GetListOfFiles()->Last();
                }
            }
            cout << y + ysize * x + 1 << "/" << xsize*ysize << endl;
        }
    }
}

void makePlots(Int_t nFiles,TString *files,TString *names,TString directory, Bool_t matrix[xsize][ysize])
{
    makePlots(nFiles,files,names,"",(Double_t*)0,directory,
              matrix);
}

void makePlots(TString file,TString directory,Bool_t matrix[xsize][ysize])
{
    TString *files = &file;
    TString name = "scatterplot";     //With 1 file there's no legend, so this is only used in the filename of the scatterplots, if made
    TString *names = &name;
    makePlots(1,files,names,directory,matrix);
}

//***************************************************************************
//functions to make plots for 1 row, column, or cell of the matrix
//examples:
//   xvar = "nHits", yvar = "ptrel" - makes plots of nHits vs Delta_pt/pt_org
//   xvar = "all",   yvar = "pt"    - makes all plots involving Delta_pt
//                                    (not Delta_pt/pt_org)
//   xvar = "",      yvar = "all"   - makes all histograms of Delta_???
//                                    (including Delta_pt/pt_org)
//***************************************************************************

void makePlots(Int_t nFiles,TString *files,TString *names,TString misalignment,Double_t *values,TString directory,
               TString xvar,TString yvar)
{
    Bool_t matrix[xsize][ysize];
    for (int x = 0; x < xsize; x++)
        for (int y = 0; y < ysize; y++)
        {
            bool xmatch = (xvar == "all" || xvar == xvariables[x]);
            bool ymatch = (yvar == "all" || yvar == yvariables[y]);
            if (yvar == "pt" && yvariables[y] == "pt" && relative[y] == true)
                ymatch = false;
            if (yvar == "ptrel" && yvariables[y] == "pt" && relative[y] == false)
                ymatch = true;
            matrix[x][y] = (xmatch && ymatch);
        }
    makePlots(nFiles,files,names,misalignment,values,directory,matrix);
}

void makePlots(Int_t nFiles,TString *files,TString *names,TString directory,
               TString xvar,TString yvar)
{
    makePlots(nFiles,files,names,"",(Double_t*)0,directory,
              xvar,yvar);
}

void makePlots(TString file,TString directory,
               TString xvar,TString yvar)
{
    TString *files = &file;
    TString name = "scatterplot";     //With 1 file there's no legend, so this is only used in the filename of the scatterplots, if made
    TString *names = &name;
    makePlots(1,files,names,directory,
              xvar,yvar);
}

//***************************
//functions to make all plots
//***************************

void makePlots(Int_t nFiles,TString *files,TString *names,TString misalignment,Double_t *values,TString directory)
{
    makePlots(nFiles,files,names,misalignment,values,directory,"all","all");
}

void makePlots(Int_t nFiles,TString *files,TString *names,TString directory)
{
    makePlots(nFiles,files,names,"",(Double_t*)0,directory);
}

void makePlots(TString file,TString directory)
{
    TString *files = &file;
    TString name = "scatterplot";     //With 1 file there's no legend, so this is only used in the filename of the scatterplots, if made
    TString *names = &name;
    makePlots(1,files,names,directory);
}
