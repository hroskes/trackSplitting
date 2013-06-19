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

TString units(TString variable,Char_t axis)
{
    if (variable == "pt")
        return "GeV";
    if (variable == "dxy" || variable == "dz")
    {
        if (axis == 'y')
            return "#mum";
        if (axis == 'x')
            return "cm";
    }
    if (variable == "qoverpt")
        return "e/GeV";
    return "";
}

TString axislabel(TString variable, Char_t axis, Bool_t relative = kFALSE, Bool_t resolution = kFALSE, Bool_t pull = kFALSE)
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
        s << " / #delta(#Delta" << fancyname(variable) << ")";
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

