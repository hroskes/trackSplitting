#include "---------directory-------------/trackSplitting/makePlots.C"

/*
void makeThesePlots()
{
    const Int_t nFiles = 2
    Char_t *files[nFiles] = {"TrackSplitting_Split1_Alignment1_wDiffs.root","TrackSplitting_Split2_Alignment2_wDiffs.root"};
    Char_t *names[nFiles] = {"split1","split2"};

    const Int_t xsize = 8;
    const Int_t ysize = 8;

  //const Char_t *xvariables[xsize]      = {"pt", "eta", "phi", "dz", "dxy", "theta", "qoverpt",""};
    const Double_t xcut[xsize]           = {-.03, 2.3,   2.5,   1.8,  10,    3,       3,         0};             //last one makes no difference

  //const Char_t *yvariables[ysize]      = {"pt",   "pt",   "eta",  "phi",  "dz",   "dxy",  "theta", "qoverpt"};
  //const Bool_t relative[ysize]         = {kTRUE,  kFALSE, kFALSE, kFALSE, kFALSE, kFALSE, kFALSE,  kFALSE};
  //const Bool_t logscale[ysize]         = {kFALSE, kFALSE, kFALSE, kFALSE, kFALSE, kFALSE, kFALSE,  kFALSE};
    const Double_t yplotcut[ysize]       = {.015,   .001,   3,      3,      .25,    3,      3,       3};
    const Double_t yprofilecut[ysize]    = {3,      3,      3,      3,      3,      3,      3,       3};
    const Double_t histcut[ysize]        = {3,      .05,    3,      3,      1,      3,      3,       3};

    makePlots(nFiles,files,names,"plots",xcut,yplotcut,yprofilecut,histcut);
}
*/

/*
void makeThesePlots()
{
    Char_t *file = "TrackSplitting_Split_Alignment_wDiffs.root";

    const Int_t xsize = 8;
    const Int_t ysize = 8;

  //const Char_t *xvariables[xsize]      = {"pt", "eta", "phi", "dz", "dxy", "theta", "qoverpt",""};
    const Double_t xcut[xsize]           = {-.03, 2.3,   2.5,   1.8,  10,    3,       3,         0};             //last one makes no difference

  //const Char_t *yvariables[ysize]      = {"pt",   "pt",   "eta",  "phi",  "dz",   "dxy",  "theta", "qoverpt"};
  //const Bool_t relative[ysize]         = {kTRUE,  kFALSE, kFALSE, kFALSE, kFALSE, kFALSE, kFALSE,  kFALSE};
  //const Bool_t logscale[ysize]         = {kFALSE, kFALSE, kFALSE, kFALSE, kFALSE, kFALSE, kFALSE,  kFALSE};
    const Double_t yplotcut[ysize]       = {.015,   .001,   3,      3,      .25,    3,      3,       3};
    const Double_t yprofilecut[ysize]    = {3,      3,      3,      3,      3,      3,      3,       3};
    const Double_t histcut[ysize]        = {3,      .05,    3,      3,      1,      3,      3,       3};

    makePlots(file,"plots",xcut,yplotcut,yprofilecut,histcut);
}
*/
