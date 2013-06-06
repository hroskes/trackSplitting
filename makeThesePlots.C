#include "---------directory-------------/trackSplitting/makePlots.C"

/*
void makeThesePlots()
{
    const Int_t nFiles = 2
    TString files[nFiles] = {"TrackSplitting_Split1_Alignment1_wDiffs.root","TrackSplitting_Split2_Alignment2_wDiffs.root"};
    TString names[nFiles] = {"split1","split2"};

  //TString xvariables[xsize]      = {"pt", "eta", "phi", "dz", "dxy","theta","qoverpt","runNumber","nHits",""};
    Double_t xcut[xsize]           = {-.03, 2.3,   2.5,   1.8,  10,   3,      3,        0,          0,      0};

  //TString yvariables[ysize]      = {"pt",   "pt",   "eta",  "phi",  "dz",   "dxy",  "theta", "qoverpt", ""};
  //Bool_t relative[ysize]         = {kTRUE,  kFALSE, kFALSE, kFALSE, kFALSE, kFALSE, kFALSE,  kFALSE,    kFALSE};
    Double_t yplotcut[ysize]       = {.015,   .001,   3,      3,      .25,    3,      3,       3,         0};
    Double_t yprofilecut[ysize]    = {3,      3,      3,      3,      3,      3,      3,       3,         0};
    Double_t histcut[ysize]        = {3,      .05,    3,      3,      1,      3,      3,       3,         0};

    makePlots(nFiles,files,names,"plots",xcut,yplotcut,yprofilecut,histcut);
}
*/

/*
void makeThesePlots()
{
    TString file = "TrackSplitting_Split_Alignment_wDiffs.root";

  //TString xvariables[xsize]      = {"pt", "eta", "phi", "dz", "dxy","theta","qoverpt","runNumber","nHits",""};
    Double_t xcut[xsize]           = {-.03, 2.3,   2.5,   1.8,  10,   3,      3,        0,          0,      0};

  //TString yvariables[ysize]      = {"pt",   "pt",   "eta",  "phi",  "dz",   "dxy",  "theta", "qoverpt", ""};
  //Bool_t relative[ysize]         = {kTRUE,  kFALSE, kFALSE, kFALSE, kFALSE, kFALSE, kFALSE,  kFALSE,    kFALSE};
    Double_t yplotcut[ysize]       = {.015,   .001,   3,      3,      .25,    3,      3,       3,         0};
    Double_t yprofilecut[ysize]    = {3,      3,      3,      3,      3,      3,      3,       3,         0};
    Double_t histcut[ysize]        = {3,      .05,    3,      3,      1,      3,      3,       3,         0};

    makePlots(file,"plots",xcut,yplotcut,yprofilecut,histcut);
}
*/
