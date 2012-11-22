TH1F *etaDependence(Char_t *variable = "Delta_pt",int multiply = 1)
{
    Char_t *file = "TrackSplitting_Split_Alignment_wDiffs.root";
    TFile *f = new TFile(file);
    TTree *tree = (TTree*)f->Get("splitterTree");

    stringstream ss;
    ss << variable;
    if (multiply != 1)
    {
        ss << " * " << multiply;
    }
    TString name = ss.str();

    TH1F *h = new TH1F("h",name,100,-2.5,2.5);
    TH1F *h1 = new TH1F("h1","eta_org",100,-2.5,2.5);
    Double_t var,eta;
    tree->SetBranchAddress(variable,&var);
    tree->SetBranchAddress("eta_org",&eta);
    int length = tree->GetEntries();

    for (Int_t i = 0; i<length; i++)
    {
        tree->GetEntry(i);
        var *= multiply;
        h->Fill(eta,abs(var));
        h1->Fill(eta);
        if ((i/1000)*1000 == i)
        {
            cout << i << "/" << length << endl;
        }
    }

    for (Int_t i = 1; i <= 100; i++)
    {
        if (h1->GetBinContent(i) != 0)
        {
            h->SetBinContent(i,h->GetBinContent(i)/h1->GetBinContent(i));
        }
    }

    h->SetXTitle("eta_org");
    h->SetYTitle(name);
    gStyle->SetOptStat(11);
    h->Draw();
    return h;
}
