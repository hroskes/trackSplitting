#include "TFile.h"
#include "TTree.h"
#include "TString.h"
#include <iostream>

using namespace std;

// = = = = = = = = = = 
// global variables
// = = = = = = = = = = 

bool debug_=false;


// = = = = = = = = = = 
// main function 
// = = = = = = = = = = 

void addDifferences(TString inputFile="TrackSplitting_Split_Alignment.root"){

  TFile* fin = new TFile(inputFile);
  TString outFileName = inputFile;
  outFileName.ReplaceAll(".root","_wDiffs.root");

  TFile *newfile = new TFile(outFileName,"recreate");

  TTree* ch=(TTree*)fin->Get("cosmicValidation/splitterTree"); 

  if (ch==0x0){
    cout << "ERROR: no tree found" << endl;
    return; 
  }

  TTree* evt_tree=(TTree*) ch->CloneTree(0, "fast");
  

  double pt1,pt2,dxy1,dxy2,dz1,dz2;

  ch->SetBranchAddress("pt1_spl",&pt1);
  ch->SetBranchAddress("pt2_spl",&pt2);

  ch->SetBranchAddress("dxy1_spl",&dxy1);
  ch->SetBranchAddress("dxy2_spl",&dxy2);

  ch->SetBranchAddress("dz1_spl",&dz1);
  ch->SetBranchAddress("dz2_spl",&dz2);

  double delta_pt, delta_dxy, delta_dz;

  evt_tree->Branch("Delta_pt",&delta_pt,"Delta_pt/D");
  evt_tree->Branch("Delta_dz",&delta_dz,"Delta_dz/D");
  evt_tree->Branch("Delta_dxy",&delta_dxy,"Delta_dxy/D");

  for(int i=0; i<ch->GetEntries(); i++){

    ch->GetEntry(i);

    if(debug_){
      cout << "pt1: " << pt1 << " pt2: " << pt2 << endl;
      cout << "delta pt: " << pt1-pt2 << endl;
      cout << "dz1: " << dz1 << " dz2: " << dz2 << endl;
      cout << "delta dz: " << dz1-dz2 << endl;
      cout << "dxy1: " << dxy1 << " dxy2: " << dxy2 << endl;
      cout << "delta dxy: " << dxy1-dxy2 << endl;
    }

    delta_pt = pt1-pt2;
    delta_dz = dz1-dz2;
    delta_dxy= dxy1-dxy2;

    evt_tree->Fill();

  }

  fin->Close();
  evt_tree->Write("splitterTree");
  newfile->Close();

}
