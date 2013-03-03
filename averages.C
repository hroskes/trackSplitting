Double_t findMin(Char_t *file,TString variable,Char_t *relative = "1")
{
    TFile *f = new TFile(file);
    TTree *tree = (TTree*)f->Get("splitterTree");
    Int_t length = tree->GetEntries();
    Double_t var,rel = 1,min = 1e100;
    if (variable == "Delta_dxy" || variable == "Delta_dz")
        rel = 1e-4;                                           //it's in cm but we want um
    tree->SetBranchAddress(variable,&var);
    if (relative[0] != '1')
        tree->SetBranchAddress(relative,&rel);
    for (Int_t i = 0; i<length; i++)
    {
        tree->GetEntry(i);
        if (var / rel < min)
            min = var / rel;
    }
    f->Close();
    return min;
}

Double_t findMax(Char_t *file,TString variable,Char_t *relative = "1")
{
    TFile *f = new TFile(file);
    TTree *tree = (TTree*)f->Get("splitterTree");
    Int_t length = tree->GetEntries();
    Double_t var,rel = 1,max = -1e100;
    if (variable == "Delta_dxy" || variable == "Delta_dz")
        rel = 1e-4;                                           //it's in cm but we want um
    tree->SetBranchAddress(variable,&var);
    if (relative[0] != '1')
        tree->SetBranchAddress(relative,&rel);
    for (Int_t i = 0; i<length; i++)
    {
        tree->GetEntry(i);
        if (var / rel > max)
            max = var / rel;
    }
    f->Close();
    return max;
}



Double_t findAverage(Char_t *file,TString variable,Char_t *relative = "1")
{
    TFile *f = new TFile(file);
    TTree *tree = (TTree*)f->Get("splitterTree");
    Int_t length = tree->GetEntries();
    Double_t var,rel = 1,average = 0;
    if (variable == "Delta_dxy" || variable == "Delta_dz")
        rel = 1e-4;                                           //it's in cm but we want um
    tree->SetBranchAddress(variable,&var);
    if (relative[0] != '1')
        tree->SetBranchAddress(relative,&rel);
    for (Int_t i = 0; i<length; i++)
    {
        tree->GetEntry(i);
        average += (var / rel) / length;
    }
    f->Close();
    return average;
}

Double_t findStdDev(Char_t *file,TString variable,Char_t *relative = "1")
{
    cout << variable << endl;
    TFile *f = new TFile(file);
    TTree *tree = (TTree*)f->Get("splitterTree");
    Int_t length = tree->GetEntries();
    Double_t var,
             rel = 1,
             average = findAverage(file,variable,relative),
             stddevsquared = 0;
    if (variable == "Delta_dxy" || variable == "Delta_dz")
        rel = 1e-4;                                           //it's in cm but we want um
    tree->SetBranchAddress(variable,&var);
    if (relative[0] != '1')
        tree->SetBranchAddress(relative,&rel);
    for (Int_t i = 0; i<length; i++)
    {
        tree->GetEntry(i);
        stddevsquared += ((var / rel) - average) * ((var / rel) - average) / length;
    }
    cout << TMath::Power(stddevsquared,.5) << endl;
    f->Close();
    return TMath::Power(stddevsquared,.5);
}




Double_t findMin(Int_t nFiles,Char_t **files,TString variable,Char_t *relative = "1")
{
    Double_t min = findMin(files[0],variable,relative);
    for (Int_t i = 1; i < nFiles; i++)
    {
        min = TMath::Min(min,findMin(files[i],variable,relative));
    }
    return min;
}

Double_t findMax(Int_t nFiles,Char_t **files,TString variable,Char_t *relative = "1")
{
    Double_t max = findMax(files[0],variable,relative);
    for (Int_t i = 1; i < nFiles; i++)
    {
        max = TMath::Max(max,findMax(files[i],variable,relative));
    }
    return max;
}

Double_t findAverage(Int_t nFiles,Char_t **files,TString variable,Char_t *relative = "1")
{
    Double_t var,
             rel = 1,
             sum = 0,
             totallength = 0;
    for (Int_t j = 0; j < nFiles; j++)
    {
        TFile *f = new TFile(files[j]);
        TTree *tree = (TTree*)f->Get("splitterTree");
        Int_t length = tree->GetEntries();
        totallength += length;
        if (variable == "Delta_dxy" || variable == "Delta_dz")
            rel = 1e-4;                                           //it's in cm but we want um
        tree->SetBranchAddress(variable,&var);
        if (relative[0] != '1')
            tree->SetBranchAddress(relative,&rel);
        for (Int_t i = 0; i<length; i++)
        {
            tree->GetEntry(i);
            sum += (var / rel);
        }
        f->Close();
    }
    return sum / totallength;
}

Double_t findStdDev(Int_t nFiles,Char_t **files,TString variable,Char_t *relative = "1")
{
    cout << variable << endl;
    Double_t var,
             rel = 1,
             average = findAverage(file,variable,relative),
             stddevsquared = 0,
             totallength = 0;
    for (Int_t j = 0; j < nFiles; j++)
    {
        TFile *f = new TFile(files[j]);
        TTree *tree = (TTree*)f->Get("splitterTree");
        Int_t length = tree->GetEntries();
        totallength += length;
        if (variable == "Delta_dxy" || variable == "Delta_dz")
            rel = 1e-4;                                           //it's in cm but we want um
        tree->SetBranchAddress(variable,&var);
        if (relative[0] != '1')
            tree->SetBranchAddress(relative,&rel);
        for (Int_t i = 0; i<length; i++)
        {
            tree->GetEntry(i);
            stddevsquared += ((var / rel) - average) ** 2;
        }
    }
    stddevsquared /= totallength;
    cout << sqrt(stddevsquared) << endl;
    f->Close();
    return sqrt(stddevsquared);
}

