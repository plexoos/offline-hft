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
  const Int_t max_files = 3000;
  ifstream in;
  // we assume a file basic.dat in the current directory
  // this file has 3 columns of float data
  //  in.open("mytextfile.txt");
  in.open("Ball_test_8.dat");

  Float_t val1=0,val2=0,val3=0;
  Int_t i;
  Int_t nlines    = 0;
  Int_t cpt       = 0;
  int tempo;
  while (1) {
    in >> i >> val1 >> val2 >> val3;
    if (!in.good()) break;
    if (nlines<max_files)  {
      col1[nlines] = val1;
      col2[nlines] = val2;
      col3[nlines] = val3;
      nlines++;
    }  
  }
  printf(" found %d points\n",nlines);
  in.close();

  Float_t tuple[4];
  mFile           = new TFile("Ball_test.root","RECREATE");
  TNtuple *mTuple = new TNtuple("SurveyTuple","survey variables","id:val1:val2:val3");
  for(int ii =0 ; ii<4;ii++) tuple[ii] =0.0;
  // test

  for (int ii = 0;ii<nlines ;ii++)
    {
      cout << " col1 = " << col1[ii] <<" col2 = " << col2[ii] << " col3 = " << col3[ii] << endl;
      tuple[0] = ii ;
      tuple[1] = col1[ii] ;
      tuple[2] = col2[ii] ;
      tuple[3] = col3[ii] ;
      mTuple->Fill(tuple);
    } 
  mFile->Write();
  mFile->Close();
}
