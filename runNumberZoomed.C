#include "trackSplitPlot.C"

//This makes a plot, of Delta_yvar vs. runNumber, zoomed in to between firstrun and lastrun.
//Each bin contains 1 run.
//Before interpreting the results, make sure to look at the histogram of run number (using yvar = "")
//There might be bins with very few events => big error bars,
//or just 1 event => no error bar

void runNumberZoomed(Int_t nFiles,TString *files,TString *names,TString yvar,
                     Bool_t relative = false,Bool_t resolution = false,Bool_t pull = false,
                     Int_t firstRun = -1,Int_t lastRun = -1,TString saveas = "")
{
    Int_t tempminrun = minrun;
    Int_t tempmaxrun = maxrun;
    Int_t tempbins = runNumberBins;
    minrun = firstRun;
    maxrun = lastRun;
    runNumberBins = (int)(findMax(nFiles,files,"runNumber",'x')
                        - findMin(nFiles,files,"runNumber",'x') + 1.001);
    trackSplitPlot(nFiles,files,names,"runNumber",yvar,relative,resolution,pull,saveas);
    minrun = tempminrun;
    maxrun = tempmaxrun;
    runNumberBins = tempbins;
}
