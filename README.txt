===========================
CosmicSplitterValidation.cc
===========================

To put in CMSSW_.../src/Alignment/OfflineValidation/plugins
Adds more variables to the tree than the version there.

=====================
addDifferences.C ----
=====================

script to add variables to outputted root trees from track 
splitting validations.  To run:

root -l -n
.L addDifference.C+
addDifference("myFile.root")

The output will be saved to myFile_wDiffs.root
Use this file for all of the scripts below

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

void trackSplitPlot(Char_t *file,Char_t *xvar,Char_t *yvar,Bool_t profile = false,
                    Bool_t relative = false,Bool_t logscale = false,Double_t xcut = 10,Double_t ycut = 3,Char_t *saveas = "")

Creates a profile (profile = true) or a scatter plot (profile = false) of Delta_yvar vs. xvar_org.
xvar and yvar should be one of this list: pt, eta, phi, dxy, dz, theta, qoverpt
If relative = true, it instead plots the relative difference on the y axis, Delta_yvar / yvar_org.
If logscale = true, the y axis is on a log scale

xcut and ycut are numbers of standard deviations away from the average.
xcut determines the minimum and maximum values on the x axis.
In a plot, ycut does the same on the y axis.  In a profile, ycut determines which events are included,
  but the y axis is set automatically.

If xvar = "", it instead creates a 1D histogram of Delta_yvar (or Delta_yvar/yvar_org, if relative is true),
 using ycut as the cut (xcut is ignored).

To save the plot, put a file name in saveas.

To run:

root -l -n
.L trackSplitPlot.C+
trackSplitPlot("myFile_wDiffs.root","dxy","phi",true,false,false,3,3,"profile.phi_org.Delta_dxy.png")  //(for example)



void trackSplitPlot(Char_t *file,Char_t *var,
                    Bool_t relative = false,Bool_t logscale = false,Bool_t normalize = true,
                    Double_t cut = 3,Char_t *saveas = "")

Creates a 1D histogram for Delta_var.  If relative = true, the histogram is for Delta_var / var_org.
If logscale = true, the x axis is on a log scale.
If normalize = true, the histogram is normalized
Cut is in terms of standard deviations.
To save the histogram, put a filename in saveas.

To run:

root -l -n
.L trackSplitPlot.C+
trackSplitPlot("myFile_wDiffs.root","dz",false,false,false,3,3,"hist.Delta_dz.png")  //(for example)


void trackSplitPlot(Int_t nFiles,Char_t **files,Char_t **names,Char_t *xvar,Char_t *yvar,
                    Bool_t relative = false,Bool_t logscale = false,
                    Double_t xcut = 10,Double_t ycut = 3,Char_t *saveas = "")

Creates multiple profiles or histograms on the same canvas.
The profiles come from different files, but all use the same variables.
files and names should be arrays of strings, and their lengths should be nFiles.
files contains filenames, and names are labels that go in the legend.
The rest of the parameters are explained the first trackSplitPlot()
There are enough colors in the list for it to work with nFiles up to 13, but
 long before that it would be impossible to see anything.
If xvar = "", it runs trackSplitPlot(nFiles,files,names,yvar,relative,logscale,true,ycut,saveas)

To run:
root -l -n
.L trackSplitPlot.C+
Char_t *files[3] = {"TrackSplitting_Alignment_Split1_wDiffs.root",
                    "TrackSplitting_Alignment_Split2_wDiffs.root",
                    "TrackSplitting_Alignment_Split3_wDiffs.root"}
Char_t *names[3] = {"split1","split2","split3"}
trackSplitPlot(3,files,names,"phi","pt",false,false,3,10,"profile.phi_org.Delta_pt.png")  //(for example)



void trackSplitPlot(Int_t nFiles,Char_t **files,Char_t **names,Char_t *var,
                    Bool_t relative = false,Bool_t logscale = false,Bool_t normalize = true,
                    Double_t cut = 3,Char_t *saveas = "")

Creates multiple histograms, like in the second trackSplitPlot(), from multiple files on the same canvas.
The parameters are all explained in the previous functions.

To run:
root -l -n
.L trackSplitPlot.C+
Char_t *files[3] = {"TrackSplitting_Alignment_Split1_wDiffs.root",
                    "TrackSplitting_Alignment_Split2_wDiffs.root",
                    "TrackSplitting_Alignment_Split3_wDiffs.root"}
Char_t *names[3] = {"split1","split2","split3"}
trackSplitPlot(3,files,names,"phi",false,false,true,3,"hist.Delta_phi.png")  //(for example)


================
makePlots.C ----
================

void makePlots(Char_t *file,TString directory = "plots",Double_t *xcut = xcutdef,Double_t *yplotcut = yplotcutdef,Double_t *yprofilecut = yprofilecutdef,
               Double_t *histcut = histcutdef)

Creates a profile for each of the possible choices for the x axis and y axis,
  and a histogram for each of the yvar choices, using trackSplitPlot().
The profiles for yvar = dxy, dz, eta, theta, q/pt, and phi are created with relative = false.
For yvar = pt, 2 profiles are created for each possible xvar, one with relative = true
   and one with relative = false.
For xvar, the different plots are dxy, dz, eta, theta, q/pt, phi, pt, and "" (for the histogram)
So the total is 56 profiles and 8 histograms.
makePlots can also make scatterplots, but that line is currently commented.

xcut, yplotcut, and yprofilecut are arrays with 8 values.  However, the 8th value of xcut is meaningless,
 since it corresponds to xvar = "".  yplotcut also currently does nothing.
You will probably have to change these cuts depending on the dataset.
There are also other values there that you can change, such as which variables are relative (currently pt)
 or on a logscale (currently none).

The files will be saved, in the directory given, as something like this: hist.Delta_phi.png
                                                           or: profile.pt_org.Delta_eta.png
You can include "/" at the end of directory, but you don't have to.

To run:

root -l -b
.L makePlots.C+
makePlots("TrackSplitting_Alignment_Split_wDiffs.root","directory")


void makePlots(Int_t nFiles,Char_t **files,Char_t **names,TString directory = "plots",
               Double_t *xcut = xcutdef,Double_t *yplotcut = yplotcutdef,Double_t *yprofilecut = yprofilecutdef,
               Double_t *histcut = histcutdef)

Like the previous function, but uses multiple files.
The profiles and histograms are all made together on one canvas.
If you uncomment the section in the middle, it will also make scatterplots, separately for each file.
So there will be 56 profiles, 8 histograms, and if you make them, 56*nFiles scatterplots.
The parameters are all explained either in trackSplitPlot() or in the previous makePlots() function.
If you make scatterplots, names[i] are also used in their filenames, so in that case none of them can have spaces.

To run:

root -l -b
.L makePlots.C+
Char_t *files[3] = {"TrackSplitting_Alignment_Split1_wDiffs.root",
                    "TrackSplitting_Alignment_Split2_wDiffs.root",
                    "TrackSplitting_Alignment_Split3_wDiffs.root"}
Char_t *names[3] = {"split1","split2","split3"}
makePlots(3,files,names,"plots");



====================
makeThesePlots.C----
====================

This makes it easier to use makePlots.C, particularly if you have multiple files.
It also makes it possible to easily change the cuts without changing the default values at
 the top of makePlots.C
Copy it into a directory with data files, uncomment one of the functions,
 change ------directory------ (in the "include" line at the top), the filename(s),
 and (optionally) the cuts.
Then run:
root -l -b
.x makeThesePlots.C
