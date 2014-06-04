void write_ist_gain() {

  gSystem->Setenv("DB_ACCESS_MODE", "write");
  gROOT->Macro("LoadLogger.C");
  gSystem->Load("St_base.so");
  gSystem->Load("libStDb_Tables.so");
  gSystem->Load("StDbLib.so");

  TString storeTime = "2014-01-29 00:00:01";
  cout <<"The RTS Start date and time is " << storeTime<<endl;  

  StDbManager* mgr = StDbManager::Instance();
  StDbConfigNode* node = mgr->initConfig("Calibrations_ist");
  StDbTable* dbtable = node->addDbTable("istGain");
  mgr->setStoreTime(storeTime.Data());

  Float_t pedestalT, rmsT, gainT;
  Int_t channelId, timebin;
  istGain_st table;
  
 
  for (int i = 0; i < 110592; i++) {
    table.gain[i] = 1.0; //50.0e- per ADC count
  }
   
  // Store data to the StDbTable
  dbtable->SetTable((char*)&table, 1);
  
  // Store table to database
  mgr->storeDbTable(dbtable);
  

};

