#include "addDifferences.C"
#include "misalignmentDependence.C"
#include "TString.h"

#include "TStopwatch.h"

const Int_t xsize = 10;
const Int_t ysize = 9;

TString xvariables[xsize]      = {"pt", "eta", "phi", "dz", "dxy","theta","qoverpt","runNumber","nHits",""};
TString yvariables[ysize]      = {"pt",   "pt",   "eta",  "phi",  "dz",   "dxy",  "theta", "qoverpt", ""};
Bool_t relative[ysize]         = {kTRUE,  kFALSE, kFALSE, kFALSE, kFALSE, kFALSE, kFALSE,  kFALSE,    kFALSE};

void placeholders(TString directory);

/*
        y var   pt rel  pt      eta     phi     dz      dxy     theta   q/pt    (hist)
x var     +---------------------------------------------------------------------------
pt        |     1       2       3       4       5       6       7       8       9
eta       |     10      11      12      13      14      15      16      17      18
phi       |     19      20      21      22      23      24      25      26      27
dz        |     28      29      30      31      32      33      34      35      36
dxy       |     37      38      39      40      41      42      43      44      45
theta     |     46      47      48      49      50      51      52      53      54
q/pt      |     55      56      57      58      59      60      61      62      63
run #     |     64      65      66      67      68      69      70      71      72
nHits     |     73      74      75      76      77      78      79      80      81
(hist)    |     82      83      84      85      86      87      88      89      [90]

min and max refer to these numbers.  For example, min = 17 and max = 19 means that it will
make eta_org Delta_qoverpt, a histogram of eta_org, and phi_org Delta_pt relative.
*/


void makePlots(Int_t nFiles,TString *files,TString *names,TString misalignment,Double_t *values,TString directory = "plots",
               Int_t min = 1, Int_t max = xsize*ysize)
{
    stufftodelete->SetOwner(true);

    for (Int_t i = 0; i < nFiles; i++)
    {
        TString test = files[i];
        test.ReplaceAll("_wDiffs.root","");
        if (test != files[i])   //then it's already been processed by addDifferences
            continue;

        test.ReplaceAll(".root","_wDiffs.root");
        TFile *f = TFile::Open(test);
        if (f != 0)                  //this has to be with 2 ifs and not with &&,
            if (f->IsOpen())         //because otherwise when f==0 it will give an error when trying to call
            {                        //IsOpen()
                delete f;
                files[i] = test;
                continue;
            }
        addDifferences(files[i]);
        files[i].ReplaceAll(".root","_wDiffs.root");
    }

    TString directorytomake = directory;
    gSystem->mkdir(directorytomake);
    if (misalignment != "")
    {
        directorytomake.Append("/fits");
        gSystem->mkdir(directorytomake);
    }

    for (Int_t x = 0; x < xsize; x++)
    {
        for (Int_t y = 0; y < ysize; y++)
        {
            TStopwatch stopwatch;
            for (Int_t pull = 0; pull == 0 || (pull == 1 && yvariables[y] != ""); pull++)
            {
                if (false) continue;        //this line is to make it easier to do e.g. all plots involving Delta eta
                                            //(replace false with yvariables[y] != "eta")

                if (y + ysize*x + 1 < min || y + ysize*x + 1 > max) continue;

                if (x == 9 && y == 8)
                {
                    if (!pull) placeholders(directory);
                    continue;
                }

                Int_t nPlots = nFiles+4;                     //scatterplot for each (if you uncomment it), profile, resolution, and fits for each.
                vector<TString> s;

                TString slashstring = "";
                if (directory.Last('/') != directory.Length() - 1) slashstring = "/";

                vector<TString> plotnames;
                for (Int_t i = 0; i < nFiles; i++)
                {
                    plotnames.push_back(names[i]);   //this is plotnames[i]
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

                //if (xvariables[x] == "" && yvariables[y] == "")
                //{
                //    xvarstring = "pt_org";
                //    relativestring = ".relative";
                //}

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
                    //TCanvas *c1 = trackSplitPlot(files[i],xvariables[x],yvariables[y],kFALSE,relative[y],kFALSE,(bool)pull,s[i]); //deletes the canvas
                    //stufftodelete->Clear();
                    //delete c1;
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
                    }
                    for (int z = 0; z < stufftodelete->GetEntries(); z++)
                        cout << stufftodelete->At(z)->ClassName() << " " << stufftodelete->At(z)->GetName() << endl;
                    stufftodelete->Clear();
                    for ( ; gROOT->GetListOfCanvases()->GetEntries() > 0; )
                        deleteCanvas( gROOT->GetListOfCanvases()->Last());

                    TCanvas *c2 = trackSplitPlot(nFiles,files,names,xvariables[x],yvariables[y],relative[y],kTRUE ,(bool)pull,s[i+1]);
                    if (misalignmentDependence(c2,nFiles,names,misalignment,values,xvariables[x],yvariables[y],
                                               true,relative[y],true,(bool)pull,s[i+3]))
                    {
                        s[i+2].ReplaceAll("/fits/profile.","/fits/parameter.profile");
                        misalignmentDependence(c2,nFiles,names,misalignment,values,xvariables[x],yvariables[y],
                                                   false,relative[y],true,(bool)pull,s[i+3]);
                    }
                    for (int z = 0; z < stufftodelete->GetEntries(); z++)
                        cout << stufftodelete->At(z)->ClassName() << " " << stufftodelete->At(z)->GetName() << endl;
                    stufftodelete->Clear();
                    for ( ; gROOT->GetListOfCanvases()->GetEntries() > 0; )
                        deleteCanvas( gROOT->GetListOfCanvases()->Last());
                }
                else
                {
                    TCanvas *c1 = trackSplitPlot(nFiles,files,names,xvariables[x],yvariables[y],relative[y],kFALSE,(bool)pull,s[i]);
                    if (misalignmentDependence(c1,nFiles,names,misalignment,values,xvariables[x],yvariables[y],
                                               true,relative[y],false,(bool)pull,s[i+2]))
                    {
                        misalignmentDependence(c1,nFiles,names,misalignment,values,xvariables[x],yvariables[y],
                                               true,relative[y],true,(bool)pull,s[i+3]);
                    }
                    for (int z = 0; z < stufftodelete->GetEntries(); z++)
                        cout << stufftodelete->At(z)->ClassName() << " " << stufftodelete->At(z)->GetName() << endl;
                    stufftodelete->Clear();
                    for ( ; gROOT->GetListOfCanvases()->GetEntries() > 0; )
                        deleteCanvas( gROOT->GetListOfCanvases()->Last());
                }
            }
            cout << y + ysize * x + 1 << "/" << xsize*ysize << endl;
            cerr << y + ysize * x + 1 << " " << xvariables[x] << " " << yvariables[y] << " " << stopwatch.RealTime() << endl;
            for (int i = 0; i < gDirectory->GetList()->GetEntries(); i++)
                cerr << gDirectory->GetList()->At(i)->ClassName() << " " << ((TNamed*)gDirectory->GetList()->At(i))->GetName() << endl;
        }
    }
}

void makePlots(Int_t nFiles,TString *files,TString *names,TString directory = "plots",
               Int_t min = 1, Int_t max = xsize*ysize)
{
    makePlots(nFiles,files,names,"",(Double_t*)0,directory,
              min,max);
}

void makePlots(TString file,TString directory = "plots", 
               Int_t min = 1, Int_t max = xsize*ysize)
{
    TString *files = &file;
    TString name = "scatterplot";     //With 1 file there's no legend, so this is only used in the filename of the scatterplots
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
