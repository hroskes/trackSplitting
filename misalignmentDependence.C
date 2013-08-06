#include "trackSplitPlot.C"

using namespace std;


//This can do three different things:
// (1) if xvar == "", it will plot the mean (if !resolution) or width (if resolution) of Delta_yvar as a function
//     of the misalignment values, as given in values.  misalignment (e.g. sagitta, elliptical) will be used as the
//     x axis label.
// (2) if xvar != "", it will fit the profile/resolution to a function.  If paramter > 0, it will plot the parameter given by parameter as
//     a function of the misalignment.  parametername is used as the y axis label.  You can put a semicolon in parametername
//     to separate the name from the units.  Functionname describes the funciton, and is put in brackets in the y axis label.
//     For example, to fit to Delta_pt = [0]*(eta_org-[1]), you could use functionname = "#Deltap_{T} = A(#eta_{org}-B)",
//     parameter = 0, and parametername = "A;GeV".
// (3) if parameter < 0, it will draw the profile/resolution along with the fitted functions.
//     The parameter of interest is still indicated by parameter, which is transformed to -parameter - 1.
//     For example, -1 --> 0, -2 --> 1, -3 --> 2, ...
//     This parameter's value and error will be in the legend.  You still need to enter parametername, and functionname,
//     because they will be used for labels.

//The LAST function is probably the most practical to use.


// The first function takes a canvas as its argument.  This canvas needs to have been produced with trackSplitPlot using
// the same values of xvar, yvar, relative, resolution, and pull or something strange could happen.

void misalignmentDependence(TCanvas *c1old,
                            Int_t nFiles,TString *names,TString misalignment,Double_t *values,TString xvar,TString yvar,
                            TF1 *function,Int_t parameter,TString parametername = "",TString functionname = "",
                            Bool_t relative = kFALSE,Bool_t resolution = kFALSE,Bool_t pull = kFALSE,
                            TString saveas = "")
{
    if (c1old == 0) return;
    c1old = (TCanvas*)c1old->Clone("c1old");
    if (misalignment == "" || yvar == "") return;
    Bool_t drawfits = (parameter < 0);
    if (parameter < 0)
        parameter = -parameter - 1;   //-1 --> 0, -2 --> 1, -3 --> 2, ...
    TString yaxislabel = nPart(1,parametername);
    TString parameterunits = nPart(2,parametername);
    if (parameterunits != "")
        yaxislabel.Append(" (").Append(parameterunits).Append(")");
    TList *list = c1old->GetListOfPrimitives();
    int n = list->GetEntries() - 2 - (xvar == "");

    setTDRStyle();
    gStyle->SetOptStat(0);
    gStyle->SetOptFit(0);
    if (!drawfits)
    {
        gStyle->SetCanvasDefW(678);
        gStyle->SetPadRightMargin(0.115);
    }

    TGraphErrors *g = 0;
    TH1 **p = new TH1*[n];
    TF1 **f = new TF1*[n];
    for (Int_t i = 0; i < n; i++)
    {
        p[i] = (TH1*)list->At(i+1+(xvar == ""));
        p[i]->SetDirectory(0);
        if (xvar == "")
            continue;
        stringstream s;
        s << function->GetName() << i;
        TString newname = s.str();
        f[i] = (TF1*)function->Clone(newname);
        stufftodelete->Add(f[i]);
    }

    Double_t *result = new Double_t[nFiles];
    Double_t *error  = new Double_t[nFiles];
    if (xvar == "")
    {
        yaxislabel = axislabel(yvar,'y',relative,resolution,pull);
        for (Int_t i = 0; i < nFiles; i++)
        {
            if (!resolution)
            {
                result[i] = p[i]->GetMean();
                error[i]  = p[i]->GetMeanError();
            }
            else
            {
                result[i] = p[i]->GetRMS();
                error[i]  = p[i]->GetRMSError();
            }
            cout << result[i] << " +/- " << error[i] << endl;
        }
    }
    else
    {
        for (int i = 0; i < nFiles; i++)
        {
            f[i]->SetLineColor(colors[i]);
            p[i]->Fit(f[i]);
            result[i] = f[i]->GetParameter(parameter);
            error[i]  = f[i]->GetParError (parameter);
        }
    }

    TCanvas *c1 = TCanvas::MakeDefCanvas();

    if (drawfits && xvar != "" && yvar != "")
    {
        TString legendtitle = "[";
        legendtitle.Append(functionname);
        legendtitle.Append("]");
        TLegend *legend = new TLegend(.7,.7,.9,.9,legendtitle,"br");
        stufftodelete->Add(legend);
        TString drawoption = "";
        for (int i = 0; i < nFiles; i++)
        {
            p[i]->Draw(drawoption);
            f[i]->Draw("same");
            drawoption = "same";

            stringstream s;
            s.precision(3);
            s << nPart(1,parametername) << " = " <<  result[i] << " #pm " << error[i];
            if (parameterunits != "") s << " " << parameterunits;
            TString str = s.str();
            legend->AddEntry(p[i],names[i],"pl");
            legend->AddEntry(f[i],str,"l");
        }
        c1->Update();
        Double_t x1min  = .98*gPad->GetUxmin() + .02*gPad->GetUxmax();
        Double_t x2max  = .02*gPad->GetUxmin() + .98*gPad->GetUxmax();
        Double_t y1min  = .98*gPad->GetUymin() + .02*gPad->GetUymax();
        Double_t y2max  = .02*gPad->GetUymin() + .98*gPad->GetUymax();
        Double_t width  = .4*(x2max-x1min);
        Double_t height = (1./20)*legend->GetListOfPrimitives()->GetEntries()*(y2max-y1min);
        width *= 2;
        height /= 2;
        legend->SetNColumns(2);

        Double_t newy2max = placeLegend(legend,width,height,x1min,y1min,x2max,y2max);
        p[0]->GetYaxis()->SetRangeUser(gPad->GetUymin(),(newy2max-.02*gPad->GetUymin())/.98);

        legend->SetFillStyle(0);
        legend->Draw();
    }
    else
    {
        if (values == 0) return;

        g = new TGraphErrors(nFiles,values,result,(Double_t*)0,error);
        stufftodelete->Add(g);

        g->GetXaxis()->SetTitle(misalignment);
        if (xvar != "")
        {
            yaxislabel.Append("   [");
            yaxislabel.Append(functionname);
            yaxislabel.Append("]");
        }
        g->GetYaxis()->SetTitle(yaxislabel);

        g->SetMarkerColor(colors[0]);
        g->SetMarkerStyle(20);

        g->Draw("AP");
        Double_t yaxismax = g->GetYaxis()->GetXmax();
        Double_t yaxismin = g->GetYaxis()->GetXmin();
        if (yaxismin > 0)
        {
            yaxismax += yaxismin;
            yaxismin = 0;
        }
        g->GetYaxis()->SetRangeUser(yaxismin,yaxismax);
        g->Draw("AP");
    }

    if (saveas != "")
    {
        saveplot(c1,saveas);
        for (int i = 0; i < nFiles; i++)
        {
            //delete p[i];
            //if (xvar != "")
                //delete f[i];
        }
        delete[] p;
        delete[] f;
        delete[] result;
        delete[] error;
    }
}


//This version does not take a canvas as its argument.  It simply runs trackSplitPlot to produce the canvas.

void misalignmentDependence(Int_t nFiles,TString *files,TString *names,TString misalignment,Double_t *values,TString xvar,TString yvar,
                            TF1 *function,Int_t parameter,TString parametername = "",TString functionname = "",
                            Bool_t relative = kFALSE,Bool_t resolution = kFALSE,Bool_t pull = kFALSE,
                            TString saveas = "")
{
    misalignmentDependence(trackSplitPlot(nFiles,files,names,xvar,yvar,relative,resolution,pull,""),
                           nFiles,names,misalignment,values,xvar,yvar,
                           function,parameter,parametername,functionname,
                           relative,resolution,pull,saveas);
}


// This version allows you to use a string for the function.  It creates a TF1 using this string and uses this TF1

void misalignmentDependence(TCanvas *c1old,
                            Int_t nFiles,TString *names,TString misalignment,Double_t *values,TString xvar,TString yvar,
                            TString function,Int_t parameter,TString parametername = "",TString functionname = "",
                            Bool_t relative = kFALSE,Bool_t resolution = kFALSE,Bool_t pull = kFALSE,
                            TString saveas = "")
{
    TF1 *f = new TF1("func",function);
    misalignmentDependence(c1old,nFiles,names,misalignment,values,xvar,yvar,f,parameter,parametername,functionname,relative,resolution,pull,saveas);
    delete f;
}

void misalignmentDependence(Int_t nFiles,TString *files,TString *names,TString misalignment,Double_t *values,TString xvar,TString yvar,
                            TString function,Int_t parameter,TString parametername = "",TString functionname = "",
                            Bool_t relative = kFALSE,Bool_t resolution = kFALSE,Bool_t pull = kFALSE,
                            TString saveas = "")
{
    TF1 *f = new TF1("func",function);
    misalignmentDependence(nFiles,files,names,misalignment,values,xvar,yvar,f,parameter,parametername,functionname,relative,resolution,pull,saveas);
    delete f;
}



//This version does not take a function as its argument.  It automatically determines what function, parameter,
//functionname, and parametername to use based on misalignment, xvar, yvar, relative, resolution, and pull.
//However, you have to manually put in which plots to fit to what shapes.
//I have put in several fits using elliptical and sagitta misalignments.
//The 2012A data, using the prompt geometry, is a nice example if you want to see an elliptical misalignment.
//If drawfits is true, it draws the fits; otherwise it plots the parameter as a function of misalignment as given by values.

//If the combination of misalignment, xvar, yvar, relative, resolution, pull has a default function to use, it returns true,
// otherwise it returns false.

Bool_t misalignmentDependence(TCanvas *c1old,
                              Int_t nFiles,TString *names,TString misalignment,Double_t *values,TString xvar,TString yvar,
                              Bool_t drawfits = true,
                              Bool_t relative = kFALSE,Bool_t resolution = kFALSE,Bool_t pull = kFALSE,
                              TString saveas = "")
{
    if (xvar == "")
    {
        if (c1old == 0 || misalignment == "" || values == 0) return false;
        misalignmentDependence(c1old,nFiles,names,misalignment,values,xvar,yvar,(TF1*)0,0,"","",relative,resolution,pull,saveas);
        return true;
    }
    TF1 *f;
    TString parametername = "";
    TString functionname = "";
    Int_t parameter = 9999;
    if (misalignment == "sagitta")
    {
        if (xvar == "phi" && yvar == "phi" && !resolution && !pull)
        {
            f = new TF1("sine","[0]*sin([1]*x+[2])");
            f->FixParameter(1,1);
            f->FixParameter(2,-TMath::Pi()/2);
            parametername = "A";
            functionname = "#Delta#phi=-Acos(#phi_{org})";
            parameter = 0;
        }
        /*
        Neither of these fits work.  It's kind of like sin(2x) but not exactly.
        if (xvar == "phi" && yvar == "phi" && !resolution && pull)
        {
            f = new TF1("sine","[0]*sin([1]*x+[2])");
            f->FixParameter(1,2);
            f->FixParameter(2,0);
            //f->FixParameter(1,1);
            //f->FixParameter(2,-TMath::Pi()/2);
            parametername = "A";
            functionname = "#Delta#phi/#delta(#Delta#phi)=Asin(2#phi_{org})";
            //functionname = "#Delta#phi/#delta(#Delta#phi)=-Acos(#phi_{org})";
            parameter = 0;
        }
        */
        if (xvar == "theta" && yvar == "theta" && !resolution && pull)
        {
            f = new TF1("line","-[0]*(x+[1])");
            f->FixParameter(1,-TMath::Pi()/2);
            parametername = "A";
            functionname = "#Delta#theta/#delta(#Delta#theta)=-A(#theta_{org}-#pi/2)";
            parameter = 0;
        }
        if (xvar == "theta" && yvar == "theta" && !resolution && !pull)
        {
            f = new TF1("sine","[0]*sin([1]*x+[2])");
            f->FixParameter(1,2);
            f->FixParameter(2,0);
            parametername = "A";
            functionname = "#Delta#theta=-Asin(2#theta_{org})";
            parameter = 0;
        }
    }
    if (misalignment == "elliptical")
    {
        if (xvar == "phi" && yvar == "dxy" && !resolution && !pull)
        {
            f = new TF1("sine","[0]*sin([1]*x+[2])");
            f->FixParameter(1,-2);
            f->FixParameter(2,0);
            parametername = "A;#mum";
            functionname = "#Deltadxy=-Asin(2#phi_{org})";
            parameter = 0;
        }
        if (xvar == "phi" && yvar == "dxy" && !resolution && pull)
        {
            f = new TF1("sine","[0]*sin([1]*x+[2])");
            f->FixParameter(1,-2);
            f->FixParameter(2,0);
            parametername = "A";
            functionname = "#Deltadxy/#delta(#Deltadxy)=-Asin(2#phi_{org})";
            parameter = 0;
        }
        
        if (xvar == "theta" && yvar == "dz" && !resolution && !pull)
        {
            f = new TF1("line","-[0]*(x-[1])");
            f->FixParameter(1,TMath::Pi()/2);
            parametername = "A;#mum";
            functionname = "#Deltadz=-A(#theta_{org}-#pi/2)";
            parameter = 0;
        }
        /*
        This fit doesn't work
        if (xvar == "theta" && yvar == "dz" && !resolution && pull)
        {
            f = new TF1("sine","[0]*sin([1]*x+[2])");
            f->FixParameter(2,-TMath::Pi()/2);
            f->FixParameter(1,1);
            parametername = "A";
            functionname = "#Deltadz/#delta(#Deltadz)=Acos(#theta_{org})";
            parameter = 0;
        }
        */
        if (xvar == "dxy" && yvar == "phi" && !resolution && !pull)
        {
            f = new TF1("line","-[0]*(x-[1])");
            f->FixParameter(1,0);
            parametername = "A;cm^{-1}";
            functionname = "#Delta#phi=-Adxy_{org}";
            parameter = 0;
        }
        if (xvar == "dxy" && yvar == "phi" && !resolution && pull)
        {
            f = new TF1("line","-[0]*(x-[1])");
            f->FixParameter(1,0);
            parametername = "A;cm^{-1}";
            functionname = "#Delta#phi/#delta(#Delta#phi)=-Adxy_{org}";
            parameter = 0;
        }
    }
    if (functionname == "") return false;
    if (drawfits)
        parameter = -parameter-1;
    misalignmentDependence(c1old,nFiles,names,misalignment,values,xvar,yvar,
                           f,parameter,parametername,functionname,relative,resolution,pull,saveas);
    delete f;
    return true;
    
}


//This is the most practically useful version.  It does not take a canvas, but produces it automatically and then runs
//the previous function.

Bool_t misalignmentDependence(Int_t nFiles,TString *files,TString *names,TString misalignment,Double_t *values,TString xvar,TString yvar,
                              Bool_t drawfits = true,
                              Bool_t relative = kFALSE,Bool_t resolution = kFALSE,Bool_t pull = kFALSE,
                              TString saveas = "")
{
    return misalignmentDependence(trackSplitPlot(nFiles,files,names,xvar,yvar,relative,resolution,pull,""),
                                  nFiles,names,misalignment,values,xvar,yvar,
                                  drawfits,relative,resolution,pull,saveas);
}
