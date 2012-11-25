//profile = true:  profile
//profile = false: scatter plot
TH1 *etaDependence(Char_t *variable = "Delta_pt",Bool_t profile = kTRUE,Char_t *saveas = "")
{
    Char_t *file = "TrackSplitting_Split_Alignment_wDiffs.root";
    TFile *f = new TFile(file);
    TTree *tree = (TTree*)f->Get("splitterTree");

    stringstream ss;
    ss << "Profile of eta and " << variable;
    TString name = ss.str();

    if (profile)
        TProfile *p = new TProfile("p",name,100,-2.5,2.5);
    else
        TH2F *p = new TH2F("p",name,100,-2.5,2.5,100,tree->GetMinimum(variable),tree->GetMaximum(variable));
    Double_t var,eta;
    tree->SetBranchAddress(variable,&var);
    tree->SetBranchAddress("eta_org",&eta);
    int length = tree->GetEntries();

    for (Int_t i = 0; i<length; i++)
    {
        tree->GetEntry(i);
        p->Fill(eta,var);
        if ((i/1000)*1000 == i)
        {
            cout << i << "/" << length << endl;
        }
    }

    p->SetXTitle("eta_org");
    p->SetYTitle(variable);
    TCanvas *c1 = TCanvas::MakeDefCanvas();
    p->Draw();
    
    if (saveas != "")
    {
        c1->SaveAs(saveas);
    }
    
    return p;
}
