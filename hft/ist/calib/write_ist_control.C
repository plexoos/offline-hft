{
  gSystem->Setenv("DB_ACCESS_MODE", "write");
  gROOT->Macro("LoadLogger.C");
  gSystem->Load("St_base.so");
  gSystem->Load("libStDb_Tables.so");
  gSystem->Load("StDbLib.so");

  TString storeTime = "2014-04-08 14:44:00";
  cout <<"The RTS Start date and time is " << storeTime<<endl;  

  StDbManager* mgr = StDbManager::Instance();
  StDbConfigNode* node = mgr->initConfig("Calibrations_ist");
  StDbTable* dbtable = node->addDbTable("istControl");
  mgr->setStoreTime(storeTime.Data());
 
  istControl_st table;

  table.kIstChanMaxRmsNoiseLevel    = 35.0;
  table.kIstChanMinRmsNoiseLevel    = 12.0;
  table.kIstApvMaxCmNoiseLevel      = 15.0;
  table.kIstPedCutDefault           = 3.0;
  table.kIstHitCutDefault           = 5.0;
  table.kIstCMNCutDefault           = 3.0;
  table.kIstMinNumOfRawHits         = 1;
  table.kIstMaxNumOfRawHits         = 460;
  table.kIstAlldata                 = 0;
  table.kIstADCdata                 = 1;
  table.kIstZSdata                  = 2;
  table.kIstDefaultTimeBin          = 2;
  table.kIstCurrentTimeBinNum	    = 4;

  // Store data to the StDbTable
  dbtable->SetTable((char*)&table, 1);
  
  // Store table to database
  mgr->storeDbTable(dbtable);
};

