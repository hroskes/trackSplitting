Char_t *fancyname(Char_t *variable)
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
    else
        return variable;
}

Char_t *units(Char_t *variable,Char_t axis)
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

TString axislabel(Char_t *variable, Char_t axis, Bool_t relative = kFALSE)
{
    stringstream s;
    if (axis == 'y')
        s << "#Delta";
    s << fancyname(variable);
    if (relative && axis == 'y')
        s << " / " << fancyname(variable);
    if (relative || axis == 'x')
        s << "_{org}";
    if ((!relative || axis == 'x') && units(variable,axis) != "")
        s << " (" << units(variable,axis) << ")";
    return s.str();
}
