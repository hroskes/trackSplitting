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

/*******************************
To make ALL plots (313 in total)
*******************************/

void makeThesePlots()
{
    makePlots(nFiles,files,names,"",0,plotdirectory);
}

/**************************************************************************
to make a single plot, or all plots involving a single x or y variable

   xvar = "nHits", yvar = "ptrel" - makes plots of nHits vs Delta_pt/pt_org
   xvar = "all",   yvar = "pt"    - makes all plots involving Delta_pt
                                    (not Delta_pt/pt_org)
   xvar = "",      yvar = "all"   - makes all histograms of Delta_???
                                    (including Delta_pt/pt_org)
**************************************************************************/

void makeThesePlots(TString xvar,TString yvar)
{
    makePlots(nFiles,files,names,"",0,plotdirectory,xvar,yvar);
}

/*************************************************************************************
To make a custom selection of plots
The matrix should look like this - use true to make that plot, and false not to make it.
        y var   pt rel  pt      eta     phi     dz      dxy     theta   q/pt    (hist)
x var     +---------------------------------------------------------------------------
pt        |
eta       |
phi       |
dz        |
dxy       |
theta     |
q/pt      |
run #     |
nHits     |
(hist)    |

*************************************************************************************/


void makeThesePlots(Bool_t matrix[xsize][ysize])
{
    makePlots(nFiles,files,names,"",0,plotdirectory,matrix);
}

