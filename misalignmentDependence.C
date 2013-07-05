#include "trackSplitPlot.C"

using namespace std;

//relative = false:  xvar_org is on the x axis, Delta_yvar is on the y axis
//relative = true:   xvar_org is on the x axis, Delta_yvar / yvar_org is on the y axis

void misalignmentDependence(Int_t nFiles,TString *files,TString *names,Double_t *values,TString misalignment,TString xvar,TString yvar,
                            TF1 *function,Int_t parameter,TString parametername = "",TString functionname = "",
                            Bool_t relative = kFALSE,Bool_t logscale = kFALSE,Bool_t resolution = kFALSE,Bool_t pull = kFALSE,
                            TString saveas = "")
{
    Bool_t drawfits = (parameter < 0);
    if (parameter < 0)
        parameter = -parameter - 1;   //-1 --> 0, -2 --> 1, -3 --> 2, ...
    TString yaxislabel = nPart(1,parametername);
    TString parameterunits = nPart(2,parametername);
    if (parameterunits != "")
        yaxislabel.Append(" (").Append(parameterunits).Append(")");
    TCanvas *c1old = trackSplitPlot(nFiles,files,names,xvar,yvar,relative,logscale,resolution,pull,"");
    TList *list = c1old->GetListOfPrimitives();
    int n = list->GetEntries() - 2;

    TStyle *tdrStyle = setTDRStyle();
    tdrStyle->SetOptStat(0);
    tdrStyle->SetOptFit(0);
    if (!drawfits)
    {
        tdrStyle->SetCanvasDefW(678);
        tdrStyle->SetPadRightMargin(0.115);
    }

    TH1 **p = new TH1*[n];
    TF1 **f = new TF1*[n];
    f[0] = function;
    for (Int_t i = 0; i < n; i++)
    {
        p[i] = (TH1*)list->At(i+1);
        p[i]->SetDirectory(0);
        if (i == 0)
            continue;
        stringstream s;
        s << function->GetName() << i;
        TString newname = s.str();
        f[i] = (TF1*)function->Clone(newname);
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
    c1->SetLogy((Bool_t)(logscale));

    if (drawfits && xvar != "" && yvar != "")
    {
        TString legendtitle = "[";
        legendtitle.Append(functionname);
        legendtitle.Append("]");
        TLegend *legend = new TLegend(.7,.7,.9,.9,legendtitle,"br");
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
        TGraphErrors *g = new TGraphErrors(nFiles,values,result,(Double_t*)0,error);

        g->GetXaxis()->SetTitle(misalignment);
        yaxislabel.Append("   [");
        yaxislabel.Append(functionname);
        yaxislabel.Append("]");
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
            //delete g[i];
        }
        //delete list;
        //delete maxp;
        //delete legend;
    }
}


void misalignmentDependence(Int_t nFiles,TString *files,TString *names,Double_t *values,TString misalignment,TString xvar,TString yvar,
                            TString function,Int_t parameter,TString parametername = "",TString functionname = "",
                            Bool_t relative = kFALSE,Bool_t logscale = kFALSE,Bool_t resolution = kFALSE,Bool_t pull = kFALSE,
                            TString saveas = "")
{
    TF1 *f = new TF1("func",function);
    misalignmentDependence(nFiles,files,names,values,misalignment,xvar,yvar,f,parameter,parametername,functionname,relative,logscale,resolution,pull,saveas);
}


void misalignmentDependence(Int_t nFiles,TString *files,TString *names,Double_t *values,TString misalignment,TString xvar,TString yvar,
                            Bool_t drawfits = true,
                            Bool_t relative = kFALSE,Bool_t logscale = kFALSE,Bool_t resolution = kFALSE,Bool_t pull = kFALSE,
                            TString saveas = "")
{
    bool works = false;
    if (xvar == "")
        return misalignmentDependence(nFiles,files,names,values,misalignment,xvar,yvar,(TF1*)0,0,"","",relative,logscale,resolution,pull,saveas);
    if (misalignment == "sagitta")
    {
        if (xvar == "phi" && yvar == "phi" && !resolution && !pull)
        {
            TF1 *f = new TF1("sine","[0]*sin([1]*x+[2])");
            f->FixParameter(1,1);
            f->FixParameter(2,-TMath::Pi()/2);
            TString parametername = "A";
            TString functionname = "#Delta#phi=-Acos(#phi_{org})";
            Int_t parameter = 0;
            works = true;
        }
        if (xvar == "phi" && yvar == "phi" && !resolution && pull)
        {
            TF1 *f = new TF1("sine","[0]*sin([1]*x+[2])");
            f->FixParameter(1,-2);
            f->FixParameter(2,0);
            //f->SetParameters(1,1,0);
            TString parametername = "A";
            TString functionname = "#Delta#phi/#delta(#Delta#phi)=-Asin(2#phi_{org})";
            Int_t parameter = 0;
            works = true;
        }
        if (xvar == "theta" && yvar == "theta" && !resolution && pull)
        {
            TF1 *f = new TF1("line","-[0]*(x+[1])");
            f->FixParameter(1,-TMath::Pi()/2);
            TString parametername = "A";
            TString functionname = "#Delta#theta/#delta(#Delta#theta)=-A(#theta_{org}-#pi/2)";
            Int_t parameter = 0;
            works = true;
        }
        if (xvar == "theta" && yvar == "theta" && !resolution && !pull)
        {
            TF1 *f = new TF1("sine","[0]*sin([1]*x+[2])");
            f->FixParameter(1,2);
            f->FixParameter(2,0);
            TString parametername = "A";
            TString functionname = "#Delta#theta=-Asin(2#theta_{org})";
            Int_t parameter = 0;
            works = true;
        }
    }
    if (misalignment == "elliptical")
    {
        if (xvar == "phi" && yvar == "dxy" && !resolution && !pull)
        {
            TF1 *f = new TF1("sine","[0]*sin([1]*x+[2])");
            f->FixParameter(1,-2);
            f->FixParameter(2,0);
            TString parametername = "A;#mum";
            TString functionname = "#Deltadxy=-Asin(2#phi_{org})";
            Int_t parameter = 0;
            works = true;
        }
        if (xvar == "phi" && yvar == "dxy" && !resolution && pull)
        {
            TF1 *f = new TF1("sine","[0]*sin([1]*x+[2])");
            f->FixParameter(1,-2);
            f->FixParameter(2,0);
            TString parametername = "A";
            TString functionname = "#Deltadxy/#delta(#Deltadxy)=-Asin(2#phi_{org})";
            Int_t parameter = 0;
            works = true;
        }
        if (xvar == "theta" && yvar == "dz" && !resolution && !pull)
        {
            TF1 *f = new TF1("line","-[0]*(x-[1])");
            f->FixParameter(1,TMath::Pi()/2);
            TString parametername = "A;#mum";
            TString functionname = "#Deltadz=-A(#theta_{org}-#pi/2)";
            Int_t parameter = 0;
            works = true;
        }
        if (xvar == "theta" && yvar == "dz" && !resolution && pull)
        {
            TF1 *f = new TF1("line","-[0]*(x-[1])");
            f->FixParameter(1,TMath::Pi()/2);
            TString parametername = "A";
            TString functionname = "#Deltadz/#delta(#Deltadz)=-A(#theta_{org}-#pi/2)";
            Int_t parameter = 0;
            works = true;
        }
        if (xvar == "dxy" && yvar == "phi" && !resolution && !pull)
        {
            TF1 *f = new TF1("line","-[0]*(x-[1])");
            f->FixParameter(1,0);
            TString parametername = "A;cm^{-1}";
            TString functionname = "#Delta#phi=-Adxy_{org}";
            Int_t parameter = 0;
            works = true;
        }
        if (xvar == "dxy" && yvar == "phi" && !resolution && pull)
        {
            TF1 *f = new TF1("line","-[0]*(x-[1])");
            f->FixParameter(1,0);
            TString parametername = "A;cm^{-1}";
            TString functionname = "#Delta#phi/#delta(#Delta#phi)=-Adxy_{org}";
            Int_t parameter = 0;
            works = true;
        }
    }
    if (!works)
    {
        cout << "There is no default function to fit for this misalignment and these variables." << endl;
        return;
    }
    if (drawfits)
        parameter = -parameter-1;
    misalignmentDependence(nFiles,files,names,values,misalignment,xvar,yvar,
                           f,parameter,parametername,functionname,relative,logscale,resolution,pull,saveas);
    
}

/*
void plotwithfit(Int_t nFiles,TString *files,Double_t *values,TString misalignment,TString xvar,TString yvar,
                 Bool_t relative = kFALSE,Bool_t logscale = kFALSE,Bool_t resolution = kFALSE,Bool_t pull = kFALSE,
                 TString saveas = "")
{
    if (xvar == "")
        
}
*/
