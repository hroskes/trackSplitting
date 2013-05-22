#include <sstream>
#include "TString.h"
#include "TFile.h"
#include "TTree.h"

enum Statistic {Minimum, Maximum, Average, StdDev};

using namespace std;

Double_t findStatistic(Statistic what,Int_t nFiles,Char_t **files,Char_t *var,Char_t axis,Bool_t relative = kFALSE,Bool_t pull = kFALSE)
{
    Double_t x = 0, rel = 1, sigma1 = 1, sigma2 = 1;           //if !pull, we want to divide by sqrt(2) because we want the error from 1 track
    Int_t xint = 0, xint2 = 0;                                 //xint is for integer variables like runNumber and nHits.  xint2 is for nHits.

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
    if (what == StdDev)
        average = findStatistic(Average,nFiles,files,var,axis,relative,pull);

    stringstream sx,srel,ssigma1,ssigma2;

    if (axis == 'y')
        sx << "Delta_";
    sx << var;
    if (axis == 'x' && var != "runNumber" && var != "nHits")
        sx << "_org";
    if (axis == 'x' && var == "nHits")
        sx << "1_spl";
    TString variable = sx.str(),
            variable2 = variable;
    variable2.ReplaceAll("1","2");

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
        TFile *f = new TFile(files[j]);
        TTree *tree = (TTree*)f->Get("splitterTree");
        Int_t length = tree->GetEntries();
        totallength += length;

        if (var == "runNumber")
            tree->SetBranchAddress(variable,&xint);
        else if (var == "nHits")
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
            if (var == "runNumber" || var == "nHits")
                x = xint;
            x /= (rel * sqrt(sigma1 * sigma1 + sigma2 * sigma2));
            if (what == Minimum && x < result)
                result = x;
            if (what == Maximum && x > result)
                result = x;
            if (what == Average)
                result += x;
            if (what == StdDev)
                result += (x - average) * (x - average);
            if (var == "nHits")
            {
                x = xint2;
                x /= (rel * sqrt(sigma1 * sigma1 + sigma2 * sigma2));
                if (what == Minimum && x < result)
                    result = x;
                if (what == Maximum && x > result)
                    result = x;
                if (what == Average)
                    result += x;
                if (what == StdDev)
                    result += (x - average) * (x - average);
            }
        }
        f->Close();
    }
    if (var == "nHits") totallength *= 2;
    if (what == Average) result /= totallength;
    if (what == StdDev)  result = sqrt(result / (totallength - 1));
    return result;
}

Double_t findAverage(Int_t nFiles,Char_t **files,Char_t *var,Char_t axis,Bool_t relative = kFALSE,Bool_t pull = kFALSE)
{
    return findStatistic(Average,nFiles,files,var,axis,relative,pull);
}

Double_t findMin(Int_t nFiles,Char_t **files,Char_t *var,Char_t axis,Bool_t relative = kFALSE,Bool_t pull = kFALSE)
{
    return findStatistic(Minimum,nFiles,files,var,axis,relative,pull);
}

Double_t findMax(Int_t nFiles,Char_t **files,Char_t *var,Char_t axis,Bool_t relative = kFALSE,Bool_t pull = kFALSE)
{
    return findStatistic(Maximum,nFiles,files,var,axis,relative,pull);
}

Double_t findStdDev(Int_t nFiles,Char_t **files,Char_t *var,Char_t axis,Bool_t relative = kFALSE,Bool_t pull = kFALSE)
{
    return findStatistic(StdDev,nFiles,files,var,axis,relative,pull);
}



Double_t findStatistic(Statistic what,Char_t *file,Char_t *var,Char_t axis,Bool_t relative = kFALSE,Bool_t pull = kFALSE)
{
    return findStatistic(what,1,&file,var,axis,relative,pull);
}

Double_t findAverage(Char_t *file,Char_t *var,Char_t axis,Bool_t relative = kFALSE,Bool_t pull = kFALSE)
{
    return findStatistic(Average,file,var,axis,relative,pull);
}

Double_t findMin(Char_t *file,Char_t *var,Char_t axis,Bool_t relative = kFALSE,Bool_t pull = kFALSE)
{
    return findStatistic(Minimum,file,var,axis,relative,pull);
}

Double_t findMax(Char_t *file,Char_t *var,Char_t axis,Bool_t relative = kFALSE,Bool_t pull = kFALSE)
{
    return findStatistic(Maximum,file,var,axis,relative,pull);
}

Double_t findStdDev(Char_t *file,Char_t *var,Char_t axis,Bool_t relative = kFALSE,Bool_t pull = kFALSE)
{
    return findStatistic(StdDev,file,var,axis,relative,pull);
}
