#ifndef TRACKSPLITPLOT_H
#define TRACKSPLITPLOT_H

#include <iostream>
#include <sstream>
#include "TCanvas.h"
#include "TClass.h"
#include "TColor.h"
#include "TFile.h"
#include "TGaxis.h"
#include "TGraph2DErrors.h"
#include "TGraphErrors.h"
#include "TH1.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TLegend.h"
#include "TLegendEntry.h"
#include "TList.h"
#include "TMath.h"
#include "TMultiGraph.h"
#include "TObject.h"
#include "TProfile.h"
#include "TROOT.h"
#include "TString.h"
#include "TStyle.h"
#include "TSystem.h"
#include "TText.h"
#include "TTree.h"

using namespace std;

enum PlotType {ScatterPlot,Profile,Histogram,OrgHistogram,Resolution};
enum Statistic {Minimum, Maximum, Average, RMS};

const Double_t pi = TMath::Pi();
Int_t minrun = -1;
Int_t maxrun = -1;
const Int_t xsize = 10;
const Int_t ysize = 9;
Int_t legendGrid = 100;
Double_t margin = .1;
Double_t increaseby = .1;
Int_t binsScatterPlotx = 1000;
Int_t binsScatterPloty = 1000;
Int_t binsHistogram = 100;
Int_t runNumberBins = 30;
Int_t binsProfileResolution = 30;    //for everything but runNumber and nHits
                                     //(nHits gets a bin for each integer between the minimum and the maximum)

TString xvariables[xsize]      = {"pt", "eta", "phi", "dz",  "dxy", "theta", "qoverpt", "runNumber","nHits",""};
TString yvariables[ysize]      = {"pt", "pt",  "eta", "phi", "dz",  "dxy",   "theta",   "qoverpt", ""};
Bool_t relativearray[ysize]         = {true, false, false, false, false, false,   false,     false,     false};

TList *stufftodelete = new TList();



TString axislabel(TString variable, Char_t axis, Bool_t relative = false, Bool_t resolution = false, Bool_t pull = false);
void deleteCanvas(TObject *canvas);
TString fancyname(TString variable);
TString nPart(Int_t part,TString string,TString delimit = ";");
void placeholder(TString saveas = "",Bool_t wide = false);
void saveplot(TCanvas *c1,TString saveas);
void setAxisLabels(TH1 *p, PlotType type,TString xvar,TString yvar,Bool_t relative,Bool_t pull);
void setAxisLabels(TMultiGraph *p, PlotType type,TString xvar,TString yvar,Bool_t relative,Bool_t pull);
TString units(TString variable,Char_t axis);




#endif
