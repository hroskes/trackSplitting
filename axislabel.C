#include "TString.h"
#include "TH1.h"
#include "TMultiGraph.h"
#include <sstream>
using namespace std;

enum PlotType {ScatterPlot,Profile,Histogram,OrgHistogram,Resolution};
//ScatterPlot:  make a scatterplot of Delta_yvar vs. xvar_org
//Profile:      make a profile of Delta_yvar vs. xvar_org
//Histogram:    make a histogram of Delta_yvar
//OrgHistogram: make a histogram of xvar_org
//Resolution:   make a plot of (width of Delta_yvar) vs xvar_org

//This just puts the variable name into a fancy format, with greek letters and subscripts
TString fancyname(TString variable)
{
    if (variable == "pt")
        return "p_{T}";
    else if (variable == "phi")
        return "#phi";
    else if (variable == "eta")
        return "#eta";
    else if (variable == "theta")
        return "#theta";
    else if (variable == "qoverpt")
        return "(q/p_{T})";
    else if (variable == "runNumber")
        return "run number";
    else
        return variable;
}

//this gives the units, to be put in the axis label
TString units(TString variable,Char_t axis)
{
    if (variable == "pt")
        return "GeV";
    if (variable == "dxy" || variable == "dz")
    {
        if (axis == 'y')
            return "#mum";      //in the tree, it's listed in centimeters, but in trackSplitPlot the value is divided by 1e4
        if (axis == 'x')
            return "cm";
    }
    if (variable == "qoverpt")
        return "e/GeV";
    return "";
}


//this gives the full axis label, including units.  It can handle any combination of relative, resolution, and pull.
TString axislabel(TString variable, Char_t axis, Bool_t relative = false, Bool_t resolution = false, Bool_t pull = false)
{
    stringstream s;
    if (resolution && axis == 'y')
        s << "#sigma(";
    if (axis == 'y')
        s << "#Delta";
    s << fancyname(variable);
    if (relative && axis == 'y')
        s << " / " << fancyname(variable);
    Bool_t nHits = (variable[0] == 'n' && variable[1] == 'H' && variable[2] == 'i'
                                       && variable[3] == 't' && variable[4] == 's');
    if (relative || (axis == 'x' && variable != "runNumber" && !nHits))
        s << "_{org}";
    if (pull && axis == 'y')
    {
        s << " / #delta(#Delta" << fancyname(variable);
        if (relative)
            s << " / " << fancyname(variable) << "_{org}";
        s << ")";
    }
    if (resolution && axis == 'y')
        s << ")";
    if (((!relative && !pull) || axis == 'x') && units(variable,axis) != "")
        s << " (" << units(variable,axis) << ")";
    return s.str();
}

void setAxisLabels(TH1 *p, PlotType type,TString xvar,TString yvar,Bool_t relative,Bool_t pull)
{
    if (type == Histogram)
        p->SetXTitle(axislabel(yvar,'y',relative,false,pull));
    if (type == ScatterPlot || type == Profile || type == Resolution || type == OrgHistogram)
        p->SetXTitle(axislabel(xvar,'x'));

    if (type == ScatterPlot || type == Profile)
        p->SetYTitle(axislabel(yvar,'y',relative,false,pull));
    if (type == Resolution)
        p->SetYTitle(axislabel(yvar,'y',relative,true,pull));
}

void setAxisLabels(TMultiGraph *p, PlotType type,TString xvar,TString yvar,Bool_t relative,Bool_t pull)
{
    if (type == Histogram)
        p->GetXaxis()->SetTitle(axislabel(yvar,'y',relative,false,pull));
    if (type == ScatterPlot || type == Profile || type == Resolution || type == OrgHistogram)
        p->GetXaxis()->SetTitle(axislabel(xvar,'x'));

    if (type == ScatterPlot || type == Profile)
        p->GetYaxis()->SetTitle(axislabel(yvar,'y',relative,false,pull));
    if (type == Resolution)
        p->GetYaxis()->SetTitle(axislabel(yvar,'y',relative,true,pull));
}


//This divides a string at semicolons
//e.g. nPart(1,"a;b;c;d;e") = a
//if part <= 0 or part > (number of semicolons + 1), it returns ""
//It's used in misalignmentDependence.C
TString nPart(Int_t part,TString string)
{
    if (part <= 0) return "";
    for (int i = 1; i < part; i++)    //part-1 times
    {
        if (string.Index(";") < 0) return "";
        string.Replace(0,string.Index(";")+1,"",0);
    }
    if (string.Index(";") >= 0)
        string.Remove(string.Index(";"));
    return string;
}
