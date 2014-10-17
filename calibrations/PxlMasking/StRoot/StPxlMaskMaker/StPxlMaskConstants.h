//Masking constants header
#include <assert.h>
#include <exception>
#include <iostream>
#include<fstream>
#include <sstream>
//Maximum number of cols and rows: sensor constants
static const Int_t colMax=959;
static const Int_t rowMax=927;
//Cut used for hot pixels
static const Double_t hPix=0.1;
static const Double_t cosmichPix=0.05;
//Maximum hot pix in a column
//original was 200
static const Int_t maxPix=200;
//Cut used for hot rows and columns
static const Int_t hRow=10.0;
static const Int_t hCol=10.0;
//sAverage used for definition (3) hot columns 
//Test using online plots for multi has inner vs outer 3.4/2
static const Double_t cosmicAverage=1.0/96.0; 
static const Double_t cosmicNoRow=1000.0; 
//TEsts
//Used as follows: if(px->GetBinContent(binX+1)<colAvg/mCol)
static const Double_t mCol=10.0; 
static const Int_t block=50;
static const Char_t *fileloc="/star/u/sss/lbl_prod/mask_table/rootfiles/cosmics_153_runs/%i.pxlMon.root";
