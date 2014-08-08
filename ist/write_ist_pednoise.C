{
#include <iomanip>
#include "istPedNoise.h"
 
  gSystem->Setenv("DB_ACCESS_MODE", "write");
  gROOT->Macro("LoadLogger.C");
  gSystem->Load("St_base.so");
  gSystem->Load("libStDb_Tables.so");
  gSystem->Load("StDbLib.so");

  TString storeTime = "2013-06-03 00:00:01";
  cout <<"The RTS Start date and time is " << storeTime<<endl;  

  StDbManager* mgr = StDbManager::Instance();
  StDbConfigNode* node = mgr->initConfig("Calibrations_ist");
  StDbTable* dbtable = node->addDbTable("istPedNoise");
  mgr->setStoreTime(storeTime.Data());
 
  //open up file to read in pedesstals and RMS noise
  TString infile = "pedestalNoise_ist.dat";
  cout<<" Opening File "<<infile<<endl;

  std::ifstream in(infile);
  if (!in.is_open()) {
    cout << "Can't find file!\n"; 
    exit(0); 
  } 

  //open up file to read in CM noise
  TString infile2 = "cmNoise_ist.dat";
  cout<<" Opening File "<<infile2<<endl;

  std::ifstream in2(infile2);
  if (!in2.is_open()) {
    cout << "Can't find file!\n";
    exit(0);
  }

  Float_t pedestalT, rmsT, gainT;
  Int_t channelId, timebin;

  Float_t cmNoiseT;
  Int_t chipId, timebin2;

  Int_t usedTB = 4;

  istPedNoise_st table;
 
  for (int i = 0; i < 110592; i++) {
    table.pedestal[i] = 0; 
    table.rmsNoise[i] = 0;
  }

  for (int i = 0; i < 864; i++) {
    table.cmNoise[i] = 0;  
  }

  Int_t counter = 0;
  while (!in.eof()){
    in >> channelId >> timebin >> pedestalT >> rmsT >> gainT;
    if(timebin == usedTB) {
    	cout<<counter++<<" channelId="<<channelId<<" pedestal="<<pedestalT<<" rms="<<rmsT<<endl;
    	table.pedestal[channelId]=(int)pedestalT;
	table.rmsNoise[channelId]=(int)(rmsT*100);
    }
  }
  in.close();
 
  counter = 0;
  while (!in2.eof()){
    in2 >> chipId >> timebin >> cmNoiseT;
    if(timebin == usedTB) {
        cout<<counter++<<" chipId="<<chipId<<" cmNoiseT="<<cmNoiseT<<endl;
        table.cmNoise[chipId]=(int)(cmNoiseT*100);
    }
  }
  in2.close();
   
  // Store data to the StDbTable
  dbtable->SetTable((char*)&table, 1);
  
  // Store table to database
  mgr->storeDbTable(dbtable);
  

};

