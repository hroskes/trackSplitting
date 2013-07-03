#include "trackSplitPlot.C"

void runNumberZoomed(Int_t nFiles,TString *files,TString *names,TString yvar,
                     Bool_t relative = kFALSE,Bool_t logscale = kFALSE,Bool_t resolution = kFALSE,Bool_t pull = kFALSE,
                     Int_t firstRun = -1,Int_t lastRun = -1,TString saveas = "")
{
    Int_t tempminrun = minrun;
    Int_t tempmaxrun = maxrun;
    Int_t tempbins = runNumberBins;
    minrun = firstRun;
    maxrun = lastRun;
    runNumberBins = (int)(findMax(nFiles,files,"runNumber",'x')
                        - findMin(nFiles,files,"runNumber",'x') + 1.001);
    trackSplitPlot(nFiles,files,names,"runNumber",yvar,relative,logscale,resolution,pull,saveas);
    minrun = tempminrun;
    maxrun = tempmaxrun;
    runNumberBins = tempbins;
}
