#include "-----directory of track splitting code-----/makePlots.C"

const int nFiles = -----number of files-----;
TString files[nFiles] = {
    "root://eoscms//eos/cms/store/caf/user/$USER/AlignmentValidation/-----validation name-----/TrackSplitting_-----split name-----_-----alignment name-----.root",
    "root://eoscms//eos/cms/store/caf/user/$USER/AlignmentValidation/-----validation name-----/TrackSplitting_-----split name-----_-----alignment name-----.root",
    ...
};
TString names[nFiles] = {
    "-----name1-----",
    "-----name2-----",         //these are used in the legend
    ...
};

TString plotdirectory = "-----directory to save plots-----";

void makeThesePlots()
{
    makePlots(nFiles,files,names,"",0,plotdirectory);
}

//***************************************************************************
//examples:
//   xvar = "nHits", yvar = "ptrel" - makes plots of nHits vs Delta_pt/pt_org
//   xvar = "all",   yvar = "pt"    - makes all plots involving Delta_pt
//                                    (not Delta_pt/pt_org)
//   xvar = "",      yvar = "all"   - makes all histograms of Delta_???
//                                    (including Delta_pt/pt_org)
//***************************************************************************

void makeThesePlots(TString xvar,TString yvar)
{
    makePlots(nFiles,files,names,"",0,plotdirectory,xvar,yvar);
}

void makeThesePlots(Bool_t matrix[xsize][ysize])
{
    makePlots(nFiles,files,names,"",0,plotdirectory,matrix);
}

