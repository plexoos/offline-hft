#include <fstream>
#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <TROOT.h>
#include <TSystem.h>
#include <iterator>
#include "CalUtil.h"

std::vector<int>  pedRunNumVec;
bool debug = 1;


void pedNoiseCorrection()
{
	std::ifstream fPDlist("/star/u/ypwang/disk01/offline_hft/DB/pedNoiseTable4DB/pedNoiseFiles_Tonko/pedRunlist.txt");
        if(!fPDlist.is_open())
        {
                std::cout << " There is no pedRunList file! " << endl;
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
	if(debug) {
		cout << nPedRuns << " good pedestal runs recorded! " << endl;
		for(int i=0; i<nPedRuns; i++)
			cout << i << "th pedestal run: " << pedRunNumVec[i] << endl;
	}

	Float_t refPedestal[110592][9];
	Float_t refNoise[110592][9];
	Int_t   isBadchip[864];

	//loop pedestal runs
	for(int iRun=0; iRun<nPedRuns; iRun++)
	{
		int runNumber = pedRunNumVec[iRun];
		int TbinPD = PDTBin(runNumber);
		if(debug) {
			cout << "Correcting pedestal run: " << runNumber << endl;
                	cout << "Number of Time bins for " << runNumber << ": " << TbinPD << endl;
		}

		//////////////////////////////////////////////////////////////////////////
		Float_t pedestalT = 0., rmsT = 0.;
        	Int_t   timebin = 0;
        	Int_t   rdoIndex = -1, armIndex = -1, apvIndex = -1, chanIndex = -1;
        	Int_t   channelId = -1;
		for(int i=0; i<110592; i++) {
            	    for(int j=0; j<9; j++) {
                	refPedestal[i][j] = 0.;
                	refNoise[i][j] = 0.;
            	    }
        	}

		if(runNumber>=15065020) {
			FILE *inR = fopen(Form("/star/u/ypwang/disk01/offline_hft/DB/pedNoiseTable4DB/pedNoiseFiles_Corr/IST_pedestals_%d.txt", pedRunNumVec[iRun-1]), "r");
        		while(!feof(inR))
        		{
                		pedestalT = 0., rmsT = 0.;
                		timebin = 0;
                		rdoIndex = -1, armIndex = -1, apvIndex = -1, chanIndex = -1;
                		char buff[256] ;
                		if(fgets(buff,sizeof(buff),inR) == 0) continue ;
                		switch(buff[0]) {
                        		case '#' :
                        		case '!' :
                        		case '*' :
                        		case '/' :
                        		case '.' :
                                		continue ;
                		}
                		int ret = sscanf(buff,"%d %d %d %d %d %f %f",&rdoIndex,&armIndex,&apvIndex,&chanIndex,&timebin,&pedestalT,&rmsT) ;
                		if(ret != 7) continue ;

                		channelId = (rdoIndex - 1) * 6 * 24 * 128 + armIndex * 24 * 128 + apvIndex * 128 + chanIndex;
                		if(channelId<0 || channelId>=110592) continue;

                		refPedestal[channelId][timebin] = pedestalT;
                		refNoise[channelId][timebin] = rmsT;
        		}
        		fclose(inR);

			if(runNumber==15065020) { //4 mis-configured chips in run 15065020: 2/2/9, 3/0/8, 4/1/17 and 3/5/7. The first 3 chips work in good status in run 15061042, and the last one works in good status in run 15062046
				FILE *inR2 = fopen(Form("/star/u/ypwang/disk01/offline_hft/DB/pedNoiseTable4DB/pedNoiseFiles_Corr/IST_pedestals_%d.txt", pedRunNumVec[iRun-2]), "r");
				while(!feof(inR2))
                        	{
                                	pedestalT = 0., rmsT = 0.;
                                	timebin = 0;
                                	rdoIndex = -1, armIndex = -1, apvIndex = -1, chanIndex = -1;
                                	char buff[256] ;
                                	if(fgets(buff,sizeof(buff),inR2) == 0) continue ;
                                	switch(buff[0]) {
                                        	case '#' :
                                        	case '!' :
                                        	case '*' :
                                        	case '/' :
                                        	case '.' :
                                                	continue ;
                                	}
                                	int ret = sscanf(buff,"%d %d %d %d %d %f %f",&rdoIndex,&armIndex,&apvIndex,&chanIndex,&timebin,&pedestalT,&rmsT) ;
                                	if(ret != 7) continue ;

					if( rdoIndex==3 && armIndex==5 && apvIndex==7 ) continue;
                                	channelId = (rdoIndex - 1) * 6 * 24 * 128 + armIndex * 24 * 128 + apvIndex * 128 + chanIndex;
                                	if(channelId<0 || channelId>=110592) continue;

                                	refPedestal[channelId][timebin] = pedestalT;
                                	refNoise[channelId][timebin] = rmsT;
                        	}
                        	fclose(inR2);
			}
		}

		//////////////////////////////////////////////////////////////////////////
		if(debug)
			cout << "Looknig for mis-configured chips for the run:" << runNumber << endl;
		TString fmisconfig = "/star/u/ypwang/disk01/offline_hft/DB/pedNoiseTable4DB/ist_apv_bad.txt";

		std::ifstream misConfigChiplist(fmisconfig);
		if(!misConfigChiplist.is_open())
		{
			cout << "ist_apv_bad file was NOT found " << endl;
			exit(0);
		}

		int tem_Run, tem_Rdo, tem_Arm, tem_Group, tem_Apv, tem_Status, ChipEID;
		for(int i=0; i<864; i++) {
                    isBadchip[i] = 0;
                }

		int badChipCounter = 0;
		while(!misConfigChiplist.eof() && runNumber>=15065020) //mis-configured chips recorded since 15065020
		{
			misConfigChiplist >> tem_Run >> tem_Rdo >> tem_Arm >> tem_Group >> tem_Apv >> tem_Status;
			if(tem_Run == runNumber)
			{
				ChipEID = (tem_Rdo - 1) * 6 * 24 + tem_Arm * 24 + tem_Group * 12 + tem_Apv;
				if(debug)
					cout << badChipCounter << "th mis-configured chip electronics ID: " << ChipEID << endl;

				isBadchip[ChipEID] = 1;
				badChipCounter++;
			}
		}
		misConfigChiplist.close();
		if(debug)
			cout << "Number of mis-configured chips in run " << runNumber << ": " << badChipCounter << endl;

		/////////////////////////////////////////////////////////////////////////
		TString pedNoiseFile_corr;
                pedNoiseFile_corr = Form("pedNoiseFiles_Corr/IST_pedestals_%d.txt", runNumber);
                std::ofstream fout(pedNoiseFile_corr.Data(), std::ios_base::out & std::ios_base::trunc);
		if(debug)
                	cout << " Creating corrected pedestal/noise file: " << pedNoiseFile_corr << endl;

		FILE *fPedestal = fopen(Form("/star/u/ypwang/disk01/offline_hft/DB/pedNoiseTable4DB/pedNoiseFiles_Tonko/IST_pedestals_%d.txt", runNumber), "r");
		int badchip = 0;
		while(!feof(fPedestal))
		{
			pedestalT = 0., rmsT = 0.;
        		timebin = 0;
        		rdoIndex = -1, armIndex = -1, apvIndex = -1, chanIndex = -1;
			char buff[256] ;
                	if(fgets(buff,sizeof(buff),fPedestal) == 0) continue ;
                	switch(buff[0]) {
                        	case '#' :
                        	case '!' :
                        	case '*' :
                        	case '/' :
                        	case '.' :
                                	continue ;
                	}
                	int ret = sscanf(buff,"%d %d %d %d %d %f %f",&rdoIndex,&armIndex,&apvIndex,&chanIndex,&timebin,&pedestalT,&rmsT) ;
                	if(ret != 7) continue ;

			ChipEID = (rdoIndex - 1) * 6 * 24 + armIndex * 24 + apvIndex;
			channelId = (rdoIndex - 1) * 6 * 24 * 128 + armIndex * 24 * 128 + apvIndex * 128 + chanIndex;
			if(ChipEID>=0&&ChipEID<864&&channelId>=0&&channelId<110592&&timebin<TbinPD)
			{
				if((isBadchip[ChipEID] == 0))
				{	/* good chip */
					refPedestal[channelId][timebin] = pedestalT;
					refNoise[channelId][timebin] = rmsT;
				}
				if(runNumber>=15098043&&runNumber<=15098046&&timebin==4) continue;//these 4 runs with 5 time bins output, but last time bin empty, removed here
				fout << rdoIndex << " " << armIndex << " " << apvIndex << " " << chanIndex << " " << timebin << " " << refPedestal[channelId][timebin] << " " << refNoise[channelId][timebin] << endl;
			}
		}
		fclose(fPedestal);
		fout.close();

		cout << " Finish pedestal/noise correction file: " << pedNoiseFile_corr << endl;
	}
}
