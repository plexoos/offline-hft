{
  gROOT->Reset();
#include "Riostream.h"
#include "TFile.h"
#include "TH1.h"
#include "TH2.h"
  //#include "StChain.h"
  //#include "St_DataSetIter.h"
  //#include "StMessMgr.h"
#include "TNtuple.h"
  Float_t col1[3000]; 
  Float_t col2[3000];   
  Float_t col3[3000]; 
  Float_t col4[3000]; 
  Float_t col5[3000]; 
  Float_t col6[3000]; 
  Float_t col7[3000]; 
  const Int_t max_files = 3000;
  ifstream in;
  // we assume a file basic.dat in the current directory
  // this file has 3 columns of float data
  //  in.open("mytextfile.txt");
  in.open("Ball_test_8.dat");

  Float_t val1=0,val2=0,val3=0,val4=0,val5=0,val6=0,val7=0;
  Int_t i;
  Int_t nlines    = 0;
  Int_t cpt       = 0;
  int tempo;
  while (1) {
    in >> val1 >> val2 >> val3 >> val4 >> val5 >> val6 >> val7 ;
    if (!in.good()) break;
    if (nlines<max_files)  {
      col1[nlines] = val1;
      col2[nlines] = val2;
      col3[nlines] = val3;
      col4[nlines] = val4;
      col5[nlines] = val5;
      col6[nlines] = val6;
      col7[nlines] = val7;
      col1[nlines] = val1 + val7*val4;
      col2[nlines] = val2 + val7*val5;
      col3[nlines] = val3 + val7*val6;
      nlines++;
    }  
  }
  printf(" found %d points\n",nlines);
  in.close();

  Float_t tuple[8];
  mFile           = new TFile("Ball_test.root","RECREATE");
  TNtuple *mTuple = new TNtuple("SurveyTuple","survey variables","id:val1:val2:val3:val4:val5:val6:val7");
  for(int ii =0 ; ii<8;ii++) tuple[ii] =0.0;
  // test

  for (int ii = 0;ii<nlines ;ii++)
    {
      cout << " col1 = " << col1[ii] <<" col2 = " << col2[ii] << " col3 = " << col3[ii] << endl;
      tuple[0] = ii ;
      tuple[1] = col1[ii] ;
      tuple[2] = col2[ii] ;
      tuple[3] = col3[ii] ;
      tuple[4] = col4[ii] ;
      tuple[5] = col5[ii] ;
      tuple[6] = col6[ii] ;
      tuple[7] = col7[ii] ;
      mTuple->Fill(tuple);
    } 
  mFile->Write();
  mFile->Close();
}
