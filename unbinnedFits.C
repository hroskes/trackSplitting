#include "RooGlobalFunc.h"
#include "RooRealVar.h"
using namespace RooFit;

void unbinnedFits(TString varName="Delta_pt",
		  double range_low=-10., double range_high=10.,
		  TString cutString="pt_org>50."){

//  gROOT->ProcessLine(".L ~whitbeck/tdrstyle.C");
//  setTDRStyle();

  TString fancyVarName = varName;
  if (varName == "Delta_pt")
    fancyVarName = "#Delta p_{T}";
  if (varName == "Delta_dz")
    fancyVarName = "#Delta d_{z}";
  if (varName == "Delta_dxy")
    fancyVarName = "#Delta d_{xy}";
  if (varName == "Delta_phi")
    fancyVarName = "#Delta #phi";
  if (varName == "Delta_eta")
    fancyVarName = "#Delta #eta";

  RooRealVar x(varName,fancyVarName,range_low,range_high);
  RooRealVar phi("phi_org","#phi",-3.2,0);
  RooRealVar eta("eta_org","#eta",-2.,2.);
  RooRealVar pt("pt_org","p_{T}",0.0,1000.0);

  TFile* f =new TFile("TrackSplitting_Split_Alignment_wDiffs.root");
  TTree* t = (TTree*) f->Get("splitterTree");

  cout << "entries: " << t->GetEntries() << endl;

  RooDataSet data("data","data",t,RooArgSet(x,phi,eta,pt),cutString);

  // initial function and fit

  RooRealVar mean("mean","mean",0.0,-100.0,100.0);
  RooRealVar width("width","width",1.0,0.,100.0);

  RooGaussian gauss("gauss","gauss",x,mean,width);

  gauss.fitTo(data);

  // plot data and fit

  RooPlot* myPlot = x.frame();
  data.plotOn(myPlot);
  gauss.plotOn(myPlot);

  myPlot->Draw();

}
