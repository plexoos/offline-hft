const bool isSimFavor = false;

int elecIdToGeomId(int chipElecId) {
    int ladderGeomId = -1, chipGeomId = -1;
    int ladderElecId = chipElecId/36; //0, 1, ..., 23
    if(ladderElecId<22) ladderGeomId = 22 - ladderElecId; //y2014 map
    else                ladderGeomId = 46 - ladderElecId;
    int apvGeomIdxOnLadder = 36 - chipElecId%36; //1, 2, ..., 36
    chipGeomId = (ladderGeomId-1)*36 + apvGeomIdxOnLadder; //geometry ID: 1, 2, ..., 864
    int chipGeomIdTemp = chipGeomId;

    if(chipGeomIdTemp>=433 && chipGeomIdTemp<=444)  chipGeomId += 12;  //section B&C swapping on ladder 13
    if(chipGeomIdTemp>=445 && chipGeomIdTemp<=456)  chipGeomId -= 12;

    return chipGeomId;
}

void write_ist_chipStatus() {
    	gSystem->Setenv("DB_ACCESS_MODE", "write");
    	gROOT->Macro("LoadLogger.C");
    	gSystem->Load("St_base.so");
    	gSystem->Load("libStDb_Tables.so");
    	gSystem->Load("StDbLib.so");

	int maxRunNumber = 3151; //number of runs in 2014 with IST included
        int deadChipsInRun14 = 31; //initialize chip status
        UChar_t chipStatus[864];

	//counting number of runs in this entry
	if(isSimFavor!)
	    std::ifstream runList("/star/u/ypwang/disk01/offline_hft/DB/runListPerWeek4ChipStatusDB/run_2014_ist_full.lis");
	else {
	    std::ifstream runList("/star/u/ypwang/disk01/offline_hft/DB/runListPerWeek4ChipStatusDB/run_2014_ist_sim.lis");
	    maxRunNumber = 1;
	}

	runNumber=0;
        int runCounter = 0;
        while(!runList.eof() && runCounter<maxRunNumber) {
            runList >> runNumber;

	    //retrieve time stamp information
	    char cmd[256];
            sprintf(cmd, "/usr/bin/mysql -h dbbak.starp.bnl.gov --port=3413 -e \"SELECT FROM_UNIXTIME(startRunTime) as beginTime_human FROM RunLog.runDescriptor WHERE runNumber = %d\" >timeStamp_tmp", runNumber);
            system(cmd);

	    TString mTimeStamp;
	    FILE *inR = fopen("timeStamp_tmp","r");
            while(!feof(inR)) {
                char buff[64];
                if(fgets(buff,sizeof(buff),inR) == 0) continue ;
                switch(buff[0]) {
                    case 'b' : //beginTime_human
                        continue ;
                }
                mTimeStamp = Form(buff);
            }
            fclose(inR);

	    sprintf(cmd, "/bin/rm timeStamp_tmp");
            system(cmd);
	
	    int found = 19;
            mTimeStamp.Replace(found, mTimeStamp.Length(), "");
	    if(isSimFavor)  mTimeStamp = "2013-12-15 00:00:00";
	    cout << runCounter << ": \t" << runNumber << "\t" << mTimeStamp << endl;

	    StDbManager* mgr = StDbManager::Instance();
            StDbConfigNode* node = mgr->initConfig("Calibrations_ist");
            StDbTable* dbtable = node->addDbTable("istChipConfig");
            mgr->setStoreTime(mTimeStamp.Data());

	    istChipConfig_st table;

            for(int i=0; i<864; i++)
                chipStatus[i] = 1; //1: active and good

            //mask out dead chips
	    if(runNumber<15174075)
	        std::ifstream deadChipList("/star/u/ypwang/disk01/offline_hft/DB/runListPerWeek4ChipStatusDB/ist_apv_dead.txt");
	    else {
		std::ifstream deadChipList("/star/u/ypwang/disk01/offline_hft/DB/runListPerWeek4ChipStatusDB/ist_apv_dead_AuHe3.txt");
		deadChipsInRun14 = 29; //since run 15174075 (June 23, 2014), two chips (566/567) were taken out of online mask-out chip list
	    }

            int rdoT, armT, apvT, deadChipId;
            int deadCounter = 0;
            while(!deadChipList.eof() && deadCounter<deadChipsInRun14) {
                deadChipList >> rdoT >> armT >> apvT;
                deadChipId = (rdoT-1)*6*24 + armT*24 + apvT; //electronics ID
                if(deadChipId<0 || deadChipId>=864) continue;
                //translate to geometry ID
                int deadChipGeomIdx = elecIdToGeomId(deadChipId);
                //cout << "Dead chip geometry index: " << deadChipGeomIdx << endl;

		chipStatus[deadChipGeomIdx-1] = 0; //0: dead
                deadCounter++;
            }
            deadChipList.close();

	    //mask out bad chips (eg. mis-configured)
	    std::ifstream badChipList("/star/u/ypwang/disk01/offline_hft/DB/runListPerWeek4ChipStatusDB/ist_apv_bad.txt");
            if (!badChipList.is_open())
                exit(0);

            Int_t runID, rdo, arm, group, chip, code;
            Int_t badChipId;
            while(!badChipList.eof()) {
                badChipList >> runID >> rdo >> arm >> group >> chip >> code;
                if(runID != runNumber) continue;

                badChipId = (rdo-1)*6*24 + arm*24 + group*12 + chip; //electronics ID: 0, 1, ..., 863
                if(badChipId<0 || badChipId>=864) continue;
                //translate to geometry ID
                int badChipGeomIdx = elecIdToGeomId(badChipId);
                //cout << "Bad chip geometry index: " << badChipGeomIdx << endl; 

		chipStatus[badChipGeomIdx-1] = 10; //10: mis-configured
            }
            badChipList.close();

	    table.run = runNumber;
	    //cout << "table.run = " << runNumber << ";" << endl;

	    for(int iChip=0; iChip<864; iChip++) {
		table.s[iChip] = (unsigned)chipStatus[iChip];
		//cout << "table.s["<<iChip<<"]="<<(int)chipStatus[iChip]<<"; ";
	    }

	    // Store data to the StDbTable
	    dbtable->SetTable((char*)&table, 1);

	    // Store table to database
	    mgr->storeDbTable(dbtable);

	    runCounter++;
        }
        cout << "Number of run recorded: " << runCounter << endl;
	runList.close();
};
