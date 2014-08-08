{
#include <iomanip>
#include "istGain.h"
 
  gSystem->Setenv("DB_ACCESS_MODE", "write");
  gROOT->Macro("LoadLogger.C");
  gSystem->Load("St_base.so");
  gSystem->Load("libStDb_Tables.so");
  gSystem->Load("StDbLib.so");

  TString storeTime = "2013-06-03 00:00:01";
  cout <<"The RTS Start date and time is " << storeTime<<endl;  

  StDbManager* mgr = StDbManager::Instance();
  StDbConfigNode* node = mgr->initConfig("Calibrations_ist");
  StDbTable* dbtable = node->addDbTable("istGain");
  mgr->setStoreTime(storeTime.Data());
 
  //open up file to read in pedesstals
  TString infile = "pedestalNoise_ist.dat";
  cout<<" Opening File "<<infile<<endl;

  std::ifstream in(infile);
  if (!in.is_open()) {
    cout << "Can't find file!\n"; 
    exit(0); 
  } 
  //in.precision(10);
  //std::cout << setprecision(10);

  Float_t pedestalT, rmsT, gainT;
  Int_t channelId, timebin;
  istGain_st table;
  
 
  for (int i = 0; i < 110592; i++) {
    table.gain[i] = 1.0; //50.0e- per ADC count
  }
  
  Int_t counter = 0;
  while (!in.eof()){
    in >> channelId >> timebin >> pedestalT >> rmsT >> gainT;
    if(timebin == 4) {
    	cout<<counter++<<" channelId="<<channelId<<" gain="<<gainT<<endl;
    	table.gain[channelId]=gainT;
    }
  }
  
  in.close();
 
   
  // Store data to the StDbTable
  dbtable->SetTable((char*)&table, 1);
  
  // Store table to database
  mgr->storeDbTable(dbtable);
  

};

