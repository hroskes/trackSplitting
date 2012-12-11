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

====================
etaDependence.C ----
====================

TH1 *etaDependence(Char_t *xvar = "eta",Char_t *yvar = "pt",Bool_t profile = kFALSE,Bool_t relative = kFALSE,
                   Double_t xcut = 10,Double_t ycut = 3,Char_t *saveas = "")

Creates a profile (profile = kTRUE) or a scatter plot (profile = kFALSE) of Delta_yvar vs. eta_org.
xvar and yvar should be one of this list: pt, eta, phi, dxy, dz
If relative = kTRUE, it instead plots the relative error on the y axis, Delta_yvar / yvar_org.

xcut and ycut are numbers of standard deviations away from the average.
xcut determines the minimum and maximum values on the x axis.
In a plot, ycut does the same on the y axis.  In a profile, ycut determines which events are included,
  but the y axis is set automatically.

To save the plot or profile, put a file name in saveas.

To run:

root -l -n
.L etaDependence.C+
etaDependence("dxy","phi",kTRUE,kFALSE,10,10,"profile.phi_org.Delta_dxy.png")  //(for example)

================
makePlots.C ----
================

void makePlots(TString directory = "plots",Double_t *xcut = xcutdef,Double_t *yplotcut = yplotcutdef,Double_t *yprofilecut = yprofilecutdef)

Creates a scatter plot and a profile for each of the 5 possible choices for the x axis and y axis.
The plots and profiles for yvar = dxy, dz, eta, and phi are created with relative = kFALSE.
For yvar = pt, 2 plots and 2 profiles are created for each possible xvar, one with relative = kTRUE
   and one with relative = kFALSE.

xcut is an array with 5 values, and yplot- and profilecut are arrays with 6 values.
The easiest way to change them is probably just to change the default values at the top of makePlots.C

The files will be saved, in the directory given, as something like this: plot.phi_org.Delta_dz.png
                                                                  or: profile.pt_org.Delta_eta.png
You can include "/" at the end of directory, but you don't have to.

To run:

root -l -n
.L makePlots.C+
makePlots("directory")
