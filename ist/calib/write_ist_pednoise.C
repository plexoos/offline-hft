void write_ist_pednoise(int run=15040023) {
 
  gSystem->Setenv("DB_ACCESS_MODE", "write");
  gROOT->Macro("LoadLogger.C");
  gSystem->Load("St_base.so");
  gSystem->Load("libStDb_Tables.so");
  gSystem->Load("StDbLib.so");

  TString storeTime = "2014-02-02 00:00:01";
  cout <<"The RTS Start date and time is " << storeTime<<endl;  

  StDbManager* mgr = StDbManager::Instance();
  StDbConfigNode* node = mgr->initConfig("Calibrations_ist");
  StDbTable* dbtable = node->addDbTable("istPedNoise");
  mgr->setStoreTime(storeTime.Data());
 
  //open up file to read in common mode noise
  std::ifstream in2(Form("st_cosmic_adc_%d_raw_1000001.istCmNoise.dat", run));
    if (!in2.is_open())
       exit(0);

  //open up file to read in pedestal and rms noise
  std::ifstream in(Form("st_cosmic_adc_%d_raw_1000001.istPedNoise_math.dat", run));
  //std::ifstream in("IST_pedestals_15039169_Tonko.txt");
    if (!in.is_open())
        exit(0);

  //open bad chip/channel list
  std::ifstream badChipList("chipMaskingList4Cosmic_ist_20140209.txt");
  std::ifstream badChannelList("channelMaskingList4Cosmic_ist_20140209.txt");

  Float_t pedestalT, rmsT;
  Int_t channelId, timebin, badChannelId;

  Float_t cmNoiseT;
  Int_t chipId, timebin2, badChipId;

  const int numTimeBins = 9; //depends on real time bin number used

  istPedNoise_st table;
 
  for (int i = 0; i < 110592; i++) {
    table.pedestal[i] = 0; 
    table.rmsNoise[i] = 0;
  }

  for (int i = 0; i < 864; i++) {
    table.cmNoise[i] = 0;  
  }

  //Pedestal and RMS Noise
  Float_t averagePed = 0., averageRms = 0.;
  for (int counter=0; counter<110592*numTimeBins; counter++){
    in >> channelId >> timebin >> pedestalT >> rmsT;
    //in >> rdoIndex >> armIndex >> apvIndex >> chanIndex >> timebin >> pedestalT >> rmsT;
    //channelId = (rdoIndex-1)*6*24*128+armIndex*24*128+apvIndex*128+chanIndex;
    averagePed += pedestalT/numTimeBins;
    averageRms += rmsT/numTimeBins;

    if(((timebin+1)/numTimeBins) && ((counter+1)/numTimeBins)) {
       	cout<<counter<<": channelId="<<channelId<<" pedestal="<<averagePed<<" rms="<<averageRms<<endl;
	table.pedestal[channelId]=(int)averagePed;
	table.rmsNoise[channelId]=(int)(averageRms*100);
	averagePed = 0.;
        averageRms = 0.;
    }
  }
  in.close();
 
  //mask out individual bad channels except for above bad chips's channels
  while(!badChannelList.eof()) { //bad channels to be masked out
        badChannelList >> badChannelId;
        table.rmsNoise[badChannelId] = 10000;
  }
  badChannelList.close();

  //Common Mode Noise
  Float_t averageCmNoise = 0.;
  for(int counter=0; counter<864*numTimeBins; counter++){
    in2 >> chipId >> timebin >> cmNoiseT;
    averageCmNoise += cmNoiseT/numTimeBins;

    if(((timebin+1)/numTimeBins) && ((counter+1)/numTimeBins)) {
        cout<<counter<<" chipId="<<chipId<<" cmNoiseT="<<averageCmNoise<<endl;
        table.cmNoise[chipId]=(int)(averageCmNoise*100);
	averageCmNoise = 0.;
    }
  }
  in2.close();
  
  //mask out bad chips
  while(!badChipList.eof()) { //bad chips to be masked out
        badChipList >> badChipId;
        table.cmNoise[badChipId] = 10000;
  }
  badChipList.close(); 

  // Store data to the StDbTable
  dbtable->SetTable((char*)&table, 1);
  
  // Store table to database
  mgr->storeDbTable(dbtable);

};

