{
#include <iomanip>
#include "istMapping.h"
 
  gSystem->Setenv("DB_ACCESS_MODE", "write");
  gROOT->Macro("LoadLogger.C");
  gSystem->Load("St_base.so");
  gSystem->Load("libStDb_Tables.so");
  gSystem->Load("StDbLib.so");

  TString storeTime = "2013-06-03 00:00:01";
  cout <<"The RTS Start date and time is " << storeTime<<endl;  

  StDbManager* mgr = StDbManager::Instance();
  StDbConfigNode* node = mgr->initConfig("Calibrations_ist");
  StDbTable* dbtable = node->addDbTable("istMapping");
  mgr->setStoreTime(storeTime.Data());
 
  Int_t channelId=-1, ladder=-1, sensor=-1, column=-1, row=-1;
  istMapping_st table;
 
  for (int i = 0; i < 110592; i++) {
    table.mapping[i] = i+1;
  }
 
  for(int rdo=1; rdo<=6; rdo++) {
        for(int arm=0; arm<6; arm++) {
            for(int apv=0; apv<24; apv++) {
                for(int chan=0; chan<128; chan++) {
                    channelId = (rdo-1)*6*24*128+arm*24*128+apv*128+chan;

		    // the following mapping refered from IST readout channel datasheet
                    if(channelId<101376)
                        ladder = 22 - channelId/4608;
                    else
                        ladder = 46 - channelId/4608;

		    //sections B and C swapped on ladder 13
		    if(elecId>=43008 && elecId<44544) //section C
                        sensor = 4 - (elecId%4608)/768;
                    else if(elecId>=44544 && elecId<46080) //section B
                        sensor = 8 - (elecId%4608)/768;
                    else
                        sensor = 6 - (elecId%4608)/768;

                    Int_t pad = (channelId%4608)%768;
                    column = 12 - pad/64;
                    if(column%2)
                        row = 64 - pad%64;
                    else
                        row = 1 + pad%64;

                    table.mapping[channelId] = (ladder-1)*4608 + (sensor-1)*768 + (column-1)*64 + row;
                }
            }
        }
  } 
   
  // Store data to the StDbTable
  dbtable->SetTable((char*)&table, 1);
  
  // Store table to database
  mgr->storeDbTable(dbtable);
  

};

