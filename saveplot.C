void saveplot(TCanvas *c1,Char_t *saveas)
{
    if (saveas == "")
        return;
    TString saveas2 = saveas,
            saveas3 = saveas;
    saveas2.ReplaceAll(".pngeps","");
    saveas3.Remove(saveas3.Length()-7);
    cout << saveas2 << endl << saveas3 << endl;
    if (saveas2 == saveas3)
    {
        stringstream s1,s2;
        s1 << saveas2 << ".png";
        s2 << saveas2 << ".eps";
        saveas2 = s1.str();
        saveas3 = s2.str();
    cout << saveas2 << endl << saveas3 << endl;
        c1->SaveAs(saveas2);
        c1->SaveAs(saveas3);
        return;
    }
    else
    {
        c1->SaveAs(saveas);
    }
}
