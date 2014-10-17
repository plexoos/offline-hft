#include <assert.h>
#include <iostream>
#include<fstream>
#include <sstream>
#include<vector>
#include "TTree.h"
#include "TStyle.h"
#include "TCanvas.h"
#include "TF1.h"
#include "Riostream.h"
#include "TFile.h"
#include "TSystem.h"
#include "TROOT.h"
#include "TMath.h"
#include "TH1.h"
#include "TH2.h"
#include "TH3.h"
#include "TStyle.h"
#include "TF1.h"
#include "TProfile.h"
#include "TTree.h"
#include "TChain.h"
#include "TFile.h"
#include "TNtuple.h"
#include "TCanvas.h"
#include "TClassTable.h"
#include "TFileSet.h"
#include "TDataSetIter.h"
#include "TDataSet.h"
#include "TDataSetIter.h"
#include "TClassTable.h"
//#include "StMaskMaker.h"
void MaskFiles(Int_t runToMask,Char_t *runStamp, Char_t *yourPath,string runopt){  

  gSystem->Load("St_base.so");
  gSystem->Load("libStDb_Tables.so");
  gSystem->Load("StDbLib.so");
  gSystem->Load("StPxlMaskMaker");
  //cout<<runToMask<<"\t"<<<runStamp<<endl;
  //Int_t  tot_run = sizeof(runToMask)/sizeof(runToMask[0]);
  Int_t tot_run=1;
  StPxlMaskMaker fMask;
  fMask.Make(runToMask,runStamp,yourPath,runopt);
}
