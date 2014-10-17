//
//  StPxlMaskMaker.c
//  
//
//  Created by Michael Lomnitz on 5/29/13.
//  Updated 6/7/13 output to table structure used by Xin
//  Copyright 2013 Home. All rights reserved.
//
#include <assert.h>
#include <exception>
#include <iostream>
#include<fstream>
#include <sstream>
#include <vector>
#include "TTree.h"
#include "TStyle.h"
#include "TCanvas.h"
#include "TF1.h"
#include "Riostream.h"
#include "TFile.h"
#include "TSystem.h"
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
//#include "tables/St_pxlSensorStatus_Table.h"
//#include "tables/St_pxlRowColumnStatus_Table.h"
//#include "tables/St_pxlSensorPixelMask_Table.h"
#include "StPxlMaskConstants.h"
#include "StPxlSensor.h"
#include "StPxlRun.h"
static const Char_t *path;
static string opt;
class St_pxlSensorStatus;
class St_pxlRowColumnStatus;
class St_pxlSensorPixelMask;

//class StPxlMaskMaker{
class StPxlMaskMaker:public StPxlSensor,public StPxlRun{
 private:
  //Char_t *path;
  ofstream oSummary;
  ofstream warning;
  ofstream sensor_summary;
  Int_t hot_columns_all;
  Int_t hot_rows_all;
  Double_t Events;
  Double_t cut;
  //For testing purposes
  Double_t inner, outer,innerR, outerR;
  StPxlSensor mySensor[400];
  StPxlRun currRun;
  //pxlRowColumnStatus_st temp;
  ofstream writeSensor,writeRowCol,writePxl;
  ofstream oSensor, oRowCol, oPxl;
  TNtuple *senTuple;
  TFile *myoutfile, *myoutTest;
  TFile *myfile;
  TString str;
  Char_t *outfile;
  //  StmyTable temp[400];
  Int_t listSensorStatus[400];
  Int_t *sDim;
  Int_t perlayer[2][7];
 public:
  StPxlMaskMaker(){};
  ~StPxlMaskMaker(){};
  void Init();
  void Make(Int_t run_proc, Char_t *mytimestamp, Char_t *yourPath,string runopt);
  void Finish();
  void getSaverage();
  void summarizeMasking(Int_t run_proc,Char_t *mytimestamp);
  //Added to create Fresh tables
  void beginFreshSensorTable();
  void endFreshSensorTable();
  void beginFreshRCTable();
  void endFreshRCTable();
  void FreshRCTable();
  void writeFreshTables();
  void MakeFreshTables(Char_t *s);
  //Original 
  void beginSensorTable();
  void endSensorTable();
  void beginRCTable();
  void endRCTable();
  void RCTable();
  //2014 --------------------------
  void PxlTable();
  void beginPxlTable();
  void endPxlTable();
  void removeEntries(int remove);
  // ------------------------------
  void runMask();
  void noSector(Int_t i);
  void flagSector(Int_t i);
  void sensorStatus();
  void maskSensor(Int_t id);
  void writeTables();
  ClassDef(StPxlMaskMaker,1);
};
