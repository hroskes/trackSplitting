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

TH1F *etaDependence(Char_t *variable = "Delta_pt",Bool_t profile = kTRUE,Char_t saveas = "")

Creates a profile (profile = kTRUE) or a scatter plot (profile = kFALSE) of variable vs. eta.
To save it, enter a file name for saveas.

To run:

root -l -n
.L etaDependence.C+
addDifference("Delta_dxy",kTRUE,"Delta_dxy.png")  //(for example)
