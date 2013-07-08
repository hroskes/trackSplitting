#include <sstream>
#include "TString.h"
#include "TFile.h"
#include "TTree.h"
#include "TMath.h"

enum Statistic {Minimum, Maximum, Average, RMS};

Int_t minrun = -1;
Int_t maxrun = -1;

using namespace std;

Double_t findStatistic(Statistic what,Int_t nFiles,TString *files,TString var,Char_t axis,Bool_t relative = kFALSE,Bool_t pull = kFALSE)
{
    Double_t x = 0, rel = 1, sigma1 = 1, sigma2 = 1;           //if !pull, we want to divide by sqrt(2) because we want the error from 1 track
    Int_t xint = 0, xint2 = 0;                                 //xint is for integer variables like runNumber and nHits.  xint2 is for nHits.
     Int_t runNumber = 0;

    if (axis == 'x')
    {
        sigma1 = 1/sqrt(2);
        sigma2 = 1/sqrt(2);
    }

    Double_t totallength = 0;
    Double_t result = 0;
    if (what == Minimum) result = 1e100;
    if (what == Maximum) result = -1e100;

    Double_t average = 0;
    if (what == RMS)
        average = findStatistic(Average,nFiles,files,var,axis,relative,pull);

    Bool_t nHits = (var[0] == 'n' && var[1] == 'H' && var[2] == 'i'
                                  && var[3] == 't' && var[4] == 's');

    stringstream sx,srel,ssigma1,ssigma2;

    if (axis == 'y')
        sx << "Delta_";
    sx << var;
    if (axis == 'x' && var != "runNumber" && !nHits)
        sx << "_org";
    if (axis == 'x' && nHits)
        sx << "1_spl";
    TString variable = sx.str(),
            variable2 = variable;
    variable2.ReplaceAll("1_spl","2_spl");

    TString relvariable = "1";
    if (relative)
    {
        srel << var << "_org";
        relvariable = srel.str();
    }

    if (pull)
    {
        ssigma1 << var << "1Err_spl";
        ssigma2 << var << "2Err_spl";
    }
    TString sigma1variable = ssigma1.str();
    TString sigma2variable = ssigma2.str();

    if (!relative && !pull && (variable == "Delta_dxy" || variable == "Delta_dz"))
        rel = 1e-4;                                           //it's in cm but we want um

    for (Int_t j = 0; j < nFiles; j++)
    {
        if (files[j].Contains("MC") && var == "runNumber")
            continue;
        TFile *f = TFile::Open(files[j]);
        TTree *tree = (TTree*)f->Get("splitterTree");
        Int_t length = tree->GetEntries();

        tree->SetBranchAddress("runNumber",&runNumber);
        if (var == "runNumber")
            tree->SetBranchAddress(variable,&xint);
        else if (nHits)
        {
            tree->SetBranchAddress(variable,&xint);
            tree->SetBranchAddress(variable2,&xint2);
        }
        else
            tree->SetBranchAddress(variable,&x);

        if (relative)
            tree->SetBranchAddress(relvariable,&rel);
        if (pull)
        {
            tree->SetBranchAddress(sigma1variable,&sigma1);
            tree->SetBranchAddress(sigma2variable,&sigma2);
        }

        for (Int_t i = 0; i<length; i++)
        {
            tree->GetEntry(i);
            if (var == "runNumber" || nHits)
                x = xint;
            if (var == "runNumber")
                runNumber = x;
            if (runNumber < minrun || (runNumber > maxrun && maxrun > 0)) continue;
            
            totallength++;
            x /= (rel * sqrt(sigma1 * sigma1 + sigma2 * sigma2));
            if (what == Minimum && x < result)
                result = x;
            if (what == Maximum && x > result)
                result = x;
            if (what == Average)
                result += x;
            if (what == RMS)
                result += (x - average) * (x - average);
            if (nHits)
            {
                x = xint2;
                x /= (rel * sqrt(sigma1 * sigma1 + sigma2 * sigma2));
                if (what == Minimum && x < result)
                    result = x;
                if (what == Maximum && x > result)
                    result = x;
                if (what == Average)
                    result += x;
                if (what == RMS)
                    result += (x - average) * (x - average);
            }
        }
        f->Close();
        delete f;
    }
    if (nHits) totallength *= 2;
    if (what == Average) result /= totallength;
    if (what == RMS)  result = sqrt(result / (totallength - 1));
    return result;
}

Double_t findAverage(Int_t nFiles,TString *files,TString var,Char_t axis,Bool_t relative = kFALSE,Bool_t pull = kFALSE)
{
    return findStatistic(Average,nFiles,files,var,axis,relative,pull);
}

Double_t findMin(Int_t nFiles,TString *files,TString var,Char_t axis,Bool_t relative = kFALSE,Bool_t pull = kFALSE)
{
    return findStatistic(Minimum,nFiles,files,var,axis,relative,pull);
}

Double_t findMax(Int_t nFiles,TString *files,TString var,Char_t axis,Bool_t relative = kFALSE,Bool_t pull = kFALSE)
{
    return findStatistic(Maximum,nFiles,files,var,axis,relative,pull);
}

Double_t findRMS(Int_t nFiles,TString *files,TString var,Char_t axis,Bool_t relative = kFALSE,Bool_t pull = kFALSE)
{
    return findStatistic(RMS,nFiles,files,var,axis,relative,pull);
}



Double_t findStatistic(Statistic what,TString file,TString var,Char_t axis,Bool_t relative = kFALSE,Bool_t pull = kFALSE)
{
    return findStatistic(what,1,&file,var,axis,relative,pull);
}

Double_t findAverage(TString file,TString var,Char_t axis,Bool_t relative = kFALSE,Bool_t pull = kFALSE)
{
    return findStatistic(Average,file,var,axis,relative,pull);
}

Double_t findMin(TString file,TString var,Char_t axis,Bool_t relative = kFALSE,Bool_t pull = kFALSE)
{
    return findStatistic(Minimum,file,var,axis,relative,pull);
}

Double_t findMax(TString file,TString var,Char_t axis,Bool_t relative = kFALSE,Bool_t pull = kFALSE)
{
    return findStatistic(Maximum,file,var,axis,relative,pull);
}

Double_t findRMS(TString file,TString var,Char_t axis,Bool_t relative = kFALSE,Bool_t pull = kFALSE)
{
    return findStatistic(RMS,file,var,axis,relative,pull);
}





void axislimits(Int_t nFiles,TString *files,TString var,Char_t axis,Bool_t relative,Bool_t pull,Double_t &min,Double_t &max)
{
    if (axis == 'x')
    {
        Bool_t nHits = (var[0] == 'n' && var[1] == 'H' && var[2] == 'i'
                                      && var[3] == 't' && var[4] == 's');
        if (var == "pt")
        {
            min = 5;
            max = 100;
        }
        else if (var == "qoverpt")
        {
            min = -.35;
            max = .35;
        }
        else if (var == "dxy")
        {
            min = -10;
            max = 10;
        }
        else if (var == "dz")
        {
            min = -25;
            max = 25;
        }
        else if (var == "theta")
        {
            min = .5;
            max = 2.5;
        }
        else if (var == "eta")
        {
            min = -1.2;
            max = 1.2;
        }
        else if (var == "phi")
        {
            min = -3;
            max = 0;
        }
        else if (var == "runNumber" || nHits)
        {
            min = findMin(nFiles,files,var,'x') - .5;
            max = findMax(nFiles,files,var,'x') + .5;
        }
        else
        {
            cout << "No x axis limits for " << var << ".  Using average +/- 5*rms" << endl;
            Double_t average = findAverage(nFiles,files,var,'x');
            Double_t rms = findRMS (nFiles,files,var,'x');
            max = TMath::Min(average + 5 * rms,findMax(nFiles,files,var,'x'));
            min = TMath::Max(average - 5 * rms,findMin(nFiles,files,var,'x'));
        }
    }
    if (axis == 'y')
    {
        if (pull)
        {
            min = -5;
            max = 5;
        }
        else if (var == "pt" && relative)
        {
            min = -.06;
            max = .06;
        }
        else if (var == "pt" && !relative)
        {
            min = -.8;
            max = .8;
        }
        else if (var == "qoverpt")
        {
            min = -.0025;
            max = .0025;
        }
        else if (var == "dxy")
        {
            min = -125;
            max = 125;
        }
        else if (var == "dz")
        {
            min = -200;
            max = 200;
        }
        else if (var == "theta")
        {
            min = -.005;
            max = .005;
        }
        else if (var == "eta")
        {
            min = -.003;
            max = .003;
        }
        else if (var == "phi")
        {
            min = -.002;
            max = .002;
        }
        else
        {
            cout << "No y axis limits for " << var << ".  Using average +/- 5 * rms." << endl;
            Double_t average = 0 /*findAverage(nFiles,files,var,'y',relative,pull)*/;
            Double_t rms = findRMS (nFiles,files,var,'y',relative,pull);
            min = TMath::Max(TMath::Max(-TMath::Abs(average) - 5*rms,
                             findMin(nFiles,files,var,'y',relative,pull)),
                             -findMax(nFiles,files,var,'y',relative,pull));
            max = -min;
        }
    }
}
