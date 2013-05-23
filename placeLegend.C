Int_t legendGrid = 100;
Double_t margin = .1;
Double_t increaseby = .1;

Double_t placeLegend(TLegend *l, Double_t width, Double_t height, Double_t x1min, Double_t y1min, Double_t x2max, Double_t y2max)
{
    for (int i = legendGrid; i >= 0; i--)
    {
        for (int j = legendGrid; j >= 0; j--)
        {
            Double_t x1 = x1min * (1-(double)i/legendGrid) + (x2max - width)  * (double)i/legendGrid - margin*width;
            Double_t y1 = y1min * (1-(double)j/legendGrid) + (y2max - height) * (double)j/legendGrid - margin*height;
            Double_t x2 = x1 + (1+2*margin) * width;
            Double_t y2 = y1 + (1+2*margin) * height;
            if (fitsHere(l,x1,y1,x2,y2))
            {
                x1 += margin*width;
                y1 += margin*height;
                x2 -= margin*width;
                y2 -= margin*height;
                l->SetX1(x1);
                l->SetY1(y1);
                l->SetX2(x2);
                l->SetY2(y2);
                return y2max;
            }
        }
    }
    Double_t newy2max = y2max + increaseby * (y2max-y1min);
    Double_t newheight = height * (newy2max - y1min) / (y2max - y1min);
    return placeLegend(l,width,newheight,x1min,y1min,x2max,newy2max);
}

Bool_t fitsHere(TLegend *l,Double_t x1, Double_t y1, Double_t x2, Double_t y2)
{
    Bool_t fits = true;
    TList *list = l->GetListOfPrimitives();
    for (Int_t k = 0; list->At(k) != 0 && fits; k++)
    {
        TObject *obj = ((TLegendEntry*)(list->At(k)))->GetObject();
        TClass *class = obj->IsA();

        //Histogram, drawn as a histogram
        if (class->InheritsFrom("TH1") && !class->InheritsFrom("TH2") && !class->InheritsFrom("TH3")
         && class != TProfile::Class() && ((TH1*)obj)->GetMarkerStyle() == 1)
        {
            Int_t where = 0;
            TH1 *h = (TH1*)obj;
            for (Int_t i = 1; i <= h->GetNbinsX() && fits; i++)
            {
                if (h->GetBinLowEdge(i) + h->GetBinWidth(i) < x1) continue;   //to the left of the legend
                if (h->GetBinLowEdge(i)                     > x2) continue;   //to the right of the legend
                if (h->GetBinContent(i) > y1 && h->GetBinContent(i) < y2) fits = false;   //inside the legend
                if (h->GetBinContent(i) < y1)
                {
                    if (where == 0) where = -1;             //below the legend
                    if (where == 1) fits = false;           //a previous bin was above it so there's a vertical line through it
                }
                if (h->GetBinContent(i) > y2)
                {
                    if (where == 0) where = 1;              //above the legend
                    if (where == -1) fits = false;          //a previous bin was below it so there's a vertical line through it
                }
            }
            continue;
        }
        //Histogram, drawn with Draw("P")
        else if (class->InheritsFrom("TH1") && !class->InheritsFrom("TH2") && !class->InheritsFrom("TH3")
              && class != TProfile::Class())
        {
            TH1 *h = (TH1*)obj;
            for (Int_t i = 1; i <= h->GetNbinsX() && fits; i++)
            {
                if (h->GetBinLowEdge(i) + h->GetBinWidth(i)/2 < x1) continue;
                if (h->GetBinLowEdge(i)                       > x2) continue;
                if (h->GetBinContent(i) > y1 && h->GetBinContent(i) < y2) fits = false;
                if (h->GetBinContent(i) + h->GetBinError(i) > y2 && h->GetBinContent(i) - h->GetBinError(i) < y2) fits = false;
                if (h->GetBinContent(i) + h->GetBinError(i) > y1 && h->GetBinContent(i) - h->GetBinError(i) < y1) fits = false;
            }
            
        }
    }
    return fits;
}
