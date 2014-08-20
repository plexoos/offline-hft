#include <fstream>
#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <TROOT.h>
#include <TSystem.h>
#include <iterator>
#include "CalUtil.h"

std::vector<int>  cosmicRunNumVec;
std::vector<int>  pedRunNumVec;
const bool debug = 0;

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

void write_ist_pednoise() {
	gSystem->Setenv("DB_ACCESS_MODE", "write");
  	gROOT->Macro("LoadLogger.C");
  	gSystem->Load("St_base.so");
  	gSystem->Load("libStDb_Tables.so");
  	gSystem->Load("StDbLib.so");

	std::ifstream fPDlist("/star/u/ypwang/disk01/offline_hft/DB/pedNoiseTable4DB/pedNoiseFiles_Corr/pedRunList_Corr.txt");
        if(!fPDlist.is_open())
        {
                std::cout << " There is no corrected pedRunList file! " << endl;
                exit(0);
        }

        while(!fPDlist.eof()) {
                TString  sPDfile;
                fPDlist >> sPDfile;

                int found = sPDfile.Last('_');
                if(found >= 0)
                        sPDfile.Replace(0, found + 1, "");

                found = sPDfile.Last('.');
                if(found >= 0)
                        sPDfile.Replace(found, sPDfile.Length(), "");

                int runnumb = sPDfile.Atoi();
                if(runnumb)
                        pedRunNumVec.push_back(runnumb);
        }
        fPDlist.close();
	const int nPedRuns = pedRunNumVec.size();
	cout << nPedRuns << " good pedestal runs recorded! " << endl;
        if(debug) {
                for(int i=0; i<nPedRuns; i++)
                        cout << i << "th pedestal run: " << pedRunNumVec[i] << endl;
        }


  	std::ifstream fCosmicRunList("/star/u/ypwang/disk01/offline_hft/DB/pedNoiseTable4DB/cmNoise_cosmic/cosmicRunlist.txt");
        if(!fCosmicRunList.is_open())
        {
                std::cout << " There is no cosmicRunList file! " << endl;
                exit(0);
        }

        while(!fCosmicRunList.eof()) {
                TString  sCMNfile;
                fCosmicRunList >> sCMNfile;

                int found = sCMNfile.First('.');
                if(found >= 0)
                        sCMNfile.Replace(found, sCMNfile.Length(), "");

                int runnumb = sCMNfile.Atoi();
                if(runnumb)
                        cosmicRunNumVec.push_back(runnumb);
        }
        fCosmicRunList.close();
	const int nCosmicRuns = cosmicRunNumVec.size();
	cout << nCosmicRuns << " good cosmic runs recorded! " << endl;
        if(debug) {
                for(int i=0; i<nCosmicRuns; i++)
                        cout << i << "th cosmic run: " << cosmicRunNumVec[i] << endl;
        }

	Int_t   isBadChannel[110592];
        Int_t   isBadChip[864];
	//loop pedestal runs
	for(int iRun=0; iRun<nPedRuns; iRun++)
        {
                int runNumber = pedRunNumVec[iRun];
		//below only for testing
		//if(runNumber<15065020) continue;

                int TbinPD = PDTBin(runNumber);
		cout << "Reading pedestal run " << runNumber << " with " << TbinPD << " time bins! " << endl;
		
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
            	cout << iRun << ": \t" << runNumber << "\t" << mTimeStamp << endl;
	
		//find the closest cosmic run to match, and using its common-mode noise
		int diffRun = 15000000, miniDiffRun = 15000000;
		int matchedCosmicRunNumber = 15032029;
		for(int iCosmic=0; iCosmic<nCosmicRuns; iCosmic++) {
			diffRun = cosmicRunNumVec[iCosmic] - runNumber;
			if(diffRun < miniDiffRun && diffRun > 0) { //corresponding cosmic run should come after the pedestal run
				miniDiffRun = diffRun;
				matchedCosmicRunNumber = cosmicRunNumVec[iCosmic];
			}
		}
		if(runNumber>=15158006) matchedCosmicRunNumber = 15157002; //if no later cosmic runs found, then use the last one
		int TbinCMN = CMTBin(matchedCosmicRunNumber);
		cout << "Matched cosmic run " << matchedCosmicRunNumber << " with " << TbinCMN << " time bins! " << endl;
		
		StDbManager* mgr = StDbManager::Instance();
  		StDbConfigNode* node = mgr->initConfig("Calibrations_ist");
  		StDbTable* dbtable = node->addDbTable("istPedNoise");
  		mgr->setStoreTime(mTimeStamp.Data());
		
		//open matched CMN DB file
		std::ifstream inCMN(Form("/star/u/ypwang/disk01/offline_hft/DB/pedNoiseTable4DB/cmNoise_cosmic/%d.istCmNoise.dat", matchedCosmicRunNumber));
    		if (!inCMN.is_open()) {
    			std::cout << " There is no CMN DB file  " << end ;
    			exit(0);
    		}

		//open corrected pedestal/RMS DB file
		std::ifstream inPD(Form("/star/u/ypwang/disk01/offline_hft/DB/pedNoiseTable4DB/pedNoiseFiles_Corr/IST_pedestals_%d.txt", runNumber));
    		if (!inPD.is_open()) {
    			std::cout << " There is no PED/RMS DB file  " << end ;
    			exit(0);
    		}
	
		//retreive online masking chip/channel and offline channel masking
		for(int i=0; i<110592; i++) {
                    isBadChannel[i] = 0;
                }

		for(int i=0; i<864; i++) {
                    isBadChip[i] = 0;
                }
		
		//online masking-out chips
		int tem_Run, tem_Rdo, tem_Arm, tem_Group, tem_Apv, tem_Status, ChipEID;
		int deadChipsInRun14 = 31, deadChipCounter=0;
                if(runNumber<15181030) {
                    std::ifstream deadChipList("/star/u/ypwang/disk01/offline_hft/DB/pedNoiseTable4DB/ist_apv_dead.txt");
                }
                else {
                    std::ifstream deadChipList("/star/u/ypwang/disk01/offline_hft/DB/pedNoiseTable4DB/ist_apv_dead_AuHe3.txt");
                    deadChipsInRun14 = 29; //since run 15174075 (June 23, 2014), two chips (566/567) were taken out of online mask-out chip list
                }
		if(!deadChipList.is_open()) 
		{
                        cout << "ist_apv_dead file was NOT found " << endl;
                        exit(0);
                }

		int deadChipId = -1;
                while(!deadChipList.eof() && deadChipCounter<deadChipsInRun14) {
                    deadChipList >> tem_Rdo >> tem_Arm >> tem_Apv;
                    deadChipId = (tem_Rdo-1)*6*24 + tem_Arm*24 + tem_Apv; //electronics ID
                    if(deadChipId<0 || deadChipId>=864) continue;

		    int deadChipGeomIdx = elecIdToGeomId(deadChipId);
                    isBadChip[deadChipGeomIdx-1] = 1; //dead chip
                    deadChipCounter++;
                }
                deadChipList.close();
		cout << "Number of online masking-out chips: " << deadChipCounter << endl;

		//mis-configured chips
                std::ifstream misConfigChiplist("/star/u/ypwang/disk01/offline_hft/DB/pedNoiseTable4DB/ist_apv_bad.txt");
                if(!misConfigChiplist.is_open())
                {
                        cout << "ist_apv_bad file was NOT found " << endl;
                        exit(0);
                }
		
		int badChipCounter = 0;
		while(!misConfigChiplist.eof()) //mis-configured chips recorded since 15065020
                {
                        misConfigChiplist >> tem_Run >> tem_Rdo >> tem_Arm >> tem_Group >> tem_Apv >> tem_Status;
                        if(tem_Run == matchedCosmicRunNumber)
                        {
                                ChipEID = (tem_Rdo - 1) * 6 * 24 + tem_Arm * 24 + tem_Group * 12 + tem_Apv;
                                if(ChipEID<0 || ChipEID>=864) continue;

				int badChipGeomIdx = elecIdToGeomId(ChipEID);
                                isBadChip[badChipGeomIdx-1] = 2; //mis-configured chip
                                badChipCounter++;
                        }
                }
                misConfigChiplist.close();
		cout << "Number of offine masking-out chips: " << badChipCounter << endl;

		//online masking-out channel list
		Int_t   rdoIndex = -1, armIndex = -1, apvIndex = -1, chanIndex = -1;
                Int_t   channelId = -1;

		std::ifstream badChannelListOnline("/star/u/ypwang/disk01/offline_hft/DB/pedNoiseTable4DB/channelMaskingList_ist_online.txt"); //179 bad channels masked out online
		if(!badChannelListOnline.is_open()) 
		{
                        cout << "channelMaskingList_ist_online file was NOT found " << endl;
                        exit(0);
                }

		int deadChannelCounter = 0;
                while(!badChannelListOnline.eof()) {
                        badChannelListOnline >> rdoIndex >> armIndex >> apvIndex >> chanIndex;
                        channelId = (rdoIndex-1)*6*24 + armIndex*24 + apvIndex*128 + chanIndex;
                        isBadChannel[channelId] = 1;
			deadChannelCounter++;
                }
                badChannelListOnline.close();
		cout << "Number of online masking-out channels: " << deadChannelCounter << endl;

		//offline masking-out channel list
                std::ifstream badChannelListOfflineB("/star/u/ypwang/disk01/offline_hft/DB/pedNoiseTable4DB/ChannelMaskList.20140601.txt"); //336 bad channels masked out offline
		if(!badChannelListOfflineB.is_open())
		{
			cout << "ChannelMaskList.20140601 file was NOT found " << endl;
                        exit(0);
		}

		int badChannelCounter = 0;
                while(!badChannelListOfflineB.eof()) {
                        badChannelListOfflineB >> channelId;
                        isBadChannel[channelId] = 2;
			badChannelCounter++;
                }
                badChannelListOfflineB.close();
		cout << "Number of offline masking-out channels: " << badChannelCounter << endl;

		istPedNoise_st table;
		for (int i = 0; i < 110592; i++) {
    			table.pedestal[i] = 0;
    			table.rmsNoise[i] = 0;
  		}

  		for (int i = 0; i < 864; i++) {
    			table.cmNoise[i] = 0;
  		}

		//Pedestal and RMS Noise
		Float_t pedestalT, rmsT;
		Float_t averagePed = 0., averageRms = 0.;
		Int_t timebin = -1;
		channelId = -1;
		for (int counter=0; counter<110592*TbinPD; counter++){
			inPD >> rdoIndex >> armIndex >> apvIndex >> chanIndex >> timebin >> pedestalT >> rmsT;
			channelId = (rdoIndex - 1) * 6 * 24 * 128 + armIndex * 24 * 128 + apvIndex * 128 + chanIndex;
			if(channelId<0 || channelId>=110592) continue;
    			averagePed += pedestalT/TbinPD;
    			averageRms += rmsT/TbinPD;

    			if(((timebin+1)/TbinPD) && ((counter+1)/TbinPD)) {
        			//cout << counter/TbinPD << ": channelId = " << channelId<<"\t pedestal = "<<averagePed<<"\t rms = "<<averageRms<<endl;

        			table.pedestal[channelId] = (int)averagePed;
        			table.rmsNoise[channelId] = (int)(averageRms*100);
				//masking-out online/offline bad channels
				if(isBadChannel[channelId]) 
					table.rmsNoise[channelId] = 10000;
				if(debug)
					cout << counter/TbinPD << ": channelId = " << channelId << "\t pedestal = " << table.pedestal[channelId] << "\t rms = " << table.rmsNoise[channelId] <<endl;
        			averagePed = 0.;
        			averageRms = 0.;
    			}
  		}
  		inPD.close();

		//Common Mode Noise
		Float_t cmNoiseT;
		Float_t averageCmNoise = 0.;
		Int_t chipId;
		Int_t numTimeBins = 9; //IST CMN DB files output 9 time bins format in default
		for(int counter=0; counter<864*numTimeBins; counter++){
    			inCMN >> chipId >> timebin >> cmNoiseT;
    			averageCmNoise += cmNoiseT/TbinCMN;

    			if(((timebin+1)/numTimeBins) && ((counter+1)/numTimeBins)) {
        			//cout << counter/numTimeBins << ": chipId = " << chipId << " cmNoiseT = " << averageCmNoise << endl;
        			table.cmNoise[chipId] = (int)(averageCmNoise*100);
				//masking-out online/offline bad chips
				if(isBadChip[chipId])
					table.cmNoise[chipId] = 10000;
				if(debug)
					cout << counter/numTimeBins << ": chipId = " << chipId << " cmNoiseT = " << table.cmNoise[chipId] << endl;
        			averageCmNoise = 0.;
    			}
  		}
  		inCMN.close();

		// Please comment out below lines if you do testing, or it will write the DB!!!!
		// Store data to the StDbTable
		dbtable->SetTable((char*)&table, 1);

		// Store table to database
		mgr->storeDbTable(dbtable);

		//exit(0); //only for testing
	}
};

