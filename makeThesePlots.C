#include "-----directory-----/makePlots.C"

const int nFiles = -----number of files-----;
TString files[nFiles] = {
    "root://eoscms//eos/cms/store/caf/user/$USER/AlignmentValidation/test/-----filename1-----.root",
    "root://eoscms//eos/cms/store/caf/user/$USER/AlignmentValidation/test/-----filename2-----.root",
    ...
};
TString names[nFiles] = {
    "-----name1-----",
    "-----name2-----",         //these are used in the legend
    ...
};

void makeThesePlots(int min = 1, int max = xsize*ysize)
{
    makePlots(nFiles,files,names,"",0,"-----directory to save plots-----",min,max);
}

/*
        y var   pt rel  pt      eta     phi     dz      dxy     theta   q/pt    (hist)
x var
pt              1       2       3       4       5       6       7       8       9
eta             10      11      12      13      14      15      16      17      18
phi             19      20      21      22      23      24      25      26      27
dz              28      29      30      31      32      33      34      35      36
dxy             37      38      39      40      41      42      43      44      45
theta           46      47      48      49      50      51      52      53      54
q/pt            55      56      57      58      59      60      61      62      63
run #           64      65      66      67      68      69      70      71      72
nHits           73      74      75      76      77      78      79      80      81
(hist)          82      83      84      85      86      87      88      89      [90]

min and max refer to these numbers.  For example, min = 17 and max = 19 means that it will
make eta_org Delta_qoverpt, a histogram of eta_org, and phi_org Delta_pt relative.
*/
