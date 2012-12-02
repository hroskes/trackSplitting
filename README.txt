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

TH1* etaDependence(Char_t* xvar = "eta", Char_t* yvar = "pt", Bool_t profile = kFALSE, Bool_t relative = kFALSE, Double_t xnumberofsigmas = 10,
Double_t ynumberofsigmas = 10, Char_t* saveas = "")

Creates a profile (profile = kTRUE) or a scatter plot (profile = kFALSE) of Delta_yvar vs. eta_org.
xvar and yvar should be one of this list: pt, eta, phi, dxy, dz
If relative = kTRUE, it instead plots the relative error on the y axis, Delta_yvar / yvar_org.
xnumberofsigmas and ynumberofsigmas determine the minimum and maximum values on the y axis.
To save the plot or profile, put a file name in saveas.

To run:

root -l -n
.L etaDependence.C+
etaDependence("dxy","phi",kTRUE,kFALSE,10,10,"profile.phi_org.Delta_dxy.png")  //(for example)

================
makePlots.C ----
================

void makePlots(TString directory = "plots", Double_t xnumberofsigmas = 10, Double_t ynumberofsigmas = 10)

Creates a scatter plot and a profile for each of the 5 possible choices for the x axis and y axis,
for a total of 25 scatter plots and 25 profiles.  relative = kFALSE for all of them.
The files will be saved, in the directory given, as something like this: plot.phi_org.Delta_dz.png
                                                                  or: profile.pt_org.Delta_eta.png
You can include "/" at the end of directory, but you don't have to.

To run:

root -l -n
.L makePlots.C+
makePlots("directory",10,10)
