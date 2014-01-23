{
#include <iomanip>
#include "istControl.h"
 
  gSystem->Setenv("DB_ACCESS_MODE", "write");
  gROOT->Macro("LoadLogger.C");
  gSystem->Load("St_base.so");
  gSystem->Load("libStDb_Tables.so");
  gSystem->Load("StDbLib.so");

  TString storeTime = "2013-06-03 00:00:01";
  cout <<"The RTS Start date and time is " << storeTime<<endl;  

  StDbManager* mgr = StDbManager::Instance();
  StDbConfigNode* node = mgr->initConfig("Calibrations_ist");
  StDbTable* dbtable = node->addDbTable("istControl");
  mgr->setStoreTime(storeTime.Data());
 
  istGain_st table;

  table.kIstChanMaxRmsNoiseLevel    = (float)25.0;
  table.kIstChanMinRmsNoiseLevel    = (float)12.0;
  table.kIstApvMaxCmNoiseLevel      = (float)15.0;
  table.kIstPedCutDefault           = (float)3.0;
  table.kIstHitCutDefault           = (float)5.0;
  table.kIstCMNCutDefault           = (float)3.0;
  table.kIstMinNumOfRawHits         = (unsigned short)1;
  table.kIstMaxNumOfRawHits         = (unsigned short)460;
  table.kIstAlldata                 = (unsigned char)0;
  table.kIstADCdata                 = (unsigned char)1;
  table.kIstZSdata                  = (unsigned char)2;
  table.kIstDefaultTimeBin          = (unsigned char)3;
 
  // Store data to the StDbTable
  dbtable->SetTable((char*)&table, 1);
  
  // Store table to database
  mgr->storeDbTable(dbtable);
};

