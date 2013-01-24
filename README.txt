=====================
addDifferences.C ----
=====================

script to add variables to outputted root trees from track 
splitting validations.  To run:

root -l -n
.L addDifference.C+
addDifference("myFile.root")

The output will be saved to myFile_wDiffs.root

==================
unbinnedFit.C ----
==================

script to plot and fit gaussian to different variables from 
the output of the above script.

void unbinnedFits(TString varName="Delta_pt",
                  double range_low=-10., double range_high=10.,
                  TString cutString="pt_org>50."){

varName: name of branch to plot/fit
range_low (high): bounds for plotting/fitting
cutString: cuts to apply to data before fitting 
(var names should be the same as the branches in tree)

To run:

root -l -n "unbinnedFits.C(\"Delta_pt\",-2.,2.,\"pt_org>20.&&pt_org<30.\")"

=====================
trackSplitPlot.C ----
=====================

TH1 *trackSplitPlot(Char_t *file = "TrackSplitting_Split_Alignment_wDiffs.root",Char_t *xvar = "eta",Char_t *yvar = "pt",
                    Bool_t profile = kFALSE,Bool_t relative = kFALSE,Bool_t logscale = kFALSE,
                    Double_t xcut = 10,Double_t ycut = 3,Char_t *saveas = "")

Creates a profile (profile = true) or a scatter plot (profile = false) of Delta_yvar vs. xvar_org.
xvar and yvar should be one of this list: pt, eta, phi, dxy, dz
If relative = true, it instead plots the relative difference on the y axis, Delta_yvar / yvar_org.
If logscale = true, the y axis is on a log scale

xcut and ycut are numbers of standard deviations away from the average.
xcut determines the minimum and maximum values on the x axis.
In a plot, ycut does the same on the y axis.  In a profile, ycut determines which events are included,
  but the y axis is set automatically.

To save the plot or profile, put a file name in saveas.

To run:

root -l -n
.L trackSplitPlot.C+
etaDependence("dxy","phi",kTRUE,kFALSE,kFALSE,3,10,"profile.phi_org.Delta_dxy.png")  //(for example)

================
nProfiles.C ----
================

TProfile **nProfiles(Int_t nFiles,Char_t **files,Char_t **names,Char_t *xvar,Char_t *yvar,
                     Bool_t relative = kFALSE,Bool_t logscale = kFALSE,
                     Double_t xcut = 10,Double_t ycut = 3,Char_t *saveas = "")

Creates multiple profiles on the same canvas.
The profiles come from different files, but all use the same variables.
files and names should be arrays of strings, and their lengths should be nFiles.
files contains filenames, and names are labels that go in the legend.
The rest of the parameters are explained in trackSplitPlot.C
Technically it would work with nFiles up to 13, but
 long before that it would be impossible to see anything.

root -l -n
.L nProfiles.C+
Char_t *files[3] = {"TrackSplitting_Alignment_Split1_wDiffs.root",
                    "TrackSplitting_Alignment_Split2_wDiffs.root",
                    "TrackSplitting_Alignment_Split3_wDiffs.root"}
Char_t *names[3] = {"split1","split2","split3"}
nProfiles(3,files,names,"phi","pt",kFALSE,kFALSE,3,10,"profile.phi_org.Delta_pt.png")  //(for example)


================
makePlots.C ----
================

void makePlots(Char_t *file,TString directory = "plots",Double_t *xcut = xcutdef,
               Double_t *yplotcut = yplotcutdef,Double_t *yprofilecut = yprofilecutdef)

Creates a scatter plot and a profile for each of the 5 possible choices for the x axis and y axis
 using trackSplitPlot().
The plots and profiles for yvar = dxy, dz, eta, and phi are created with relative = kFALSE.
For yvar = pt, 2 plots and 2 profiles are created for each possible xvar, one with relative = kTRUE
   and one with relative = kFALSE.
So the total is 30 plots and 30 profiles.

xcut is an array with 5 values, and yplot- and profilecut are arrays with 6 values.
The easiest way to change them is probably just to change the default values at the top of makePlots.C
You will probably have to change them depending on the dataset.
There are also other values there that you can change.

The files will be saved, in the directory given, as something like this: plot.phi_org.Delta_dz.png
                                                                  or: profile.pt_org.Delta_eta.png
You can include "/" at the end of directory, but you don't have to.

To run:

root -l -n
.L makePlots.C+
makePlots("TrackSplitting_Alignment_Split_wDiffs.root","directory")


void makePlots(Int_t nFiles,Char_t **files,Char_t **names,TString directory = "plots",
               Double_t *xcut = xcutdef,Double_t *yplotcut = yplotcutdef,Double_t *yprofilecut = yprofilecutdef)

Like the previous function but uses multiple files.
Each file will have separate scatter plots, but the profiles will be made from all files
 using nProfiles().
So there will be 30*nFiles plots, but just 30 profiles.
The parameters are all explained either in nProfiles.C or in the previous makePlots() function.
However, names is also used in the filenames for the plots, so none of them can have spaces.

To run:

root -l -n
.L nProfiles.C+
Char_t *files[3] = {"TrackSplitting_Alignment_Split1_wDiffs.root",
                    "TrackSplitting_Alignment_Split2_wDiffs.root",
                    "TrackSplitting_Alignment_Split3_wDiffs.root"}
Char_t *names[3] = {"split1","split2","split3"}
nProfiles(3,files,names,"plots");
