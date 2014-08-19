const int port = 3413; //Run 14: 3413	Run13: 3412	Run12: 3411 ....
const int miniRunNumber = 15000000;
const int istDetId = 23;

void generateRunList() {
	char cmd[1024];
        sprintf(cmd, "/usr/bin/mysql -h dbbak.starp.bnl.gov --port=%d RunLog -e \"SELECT runDescriptor.runNumber AS run_Id, runDescriptor.glbSetupName AS global_setupName, runDescriptor.daqSetupName AS daq_setupName, detectorSet.detectorID AS ist_Id, runStatus.rtsStatus AS rts_Status, runStatus.shiftLeaderStatus AS shiftLeader_Status FROM runDescriptor LEFT JOIN detectorSet ON runDescriptor.runNumber=detectorSet.runNumber LEFT JOIN runStatus ON detectorSet.runNumber=runStatus.runNumber WHERE runDescriptor.runNumber>%d AND detectorSet.detectorID = %d\" >fullRunListTable_ist_2014.txt", port, miniRunNumber, istDetId);
        system(cmd);

	std::ofstream fRunList_physics("runList2014_ist_physics.txt", std::ios_base::out & std::ios_base::trunc);
        std::ofstream fRunList_pedestal("runList2014_ist_pedestal.txt", std::ios_base::out & std::ios_base::trunc);
        std::ofstream fRunList_pedAsPhys("runList2014_ist_pedAsPhys.txt", std::ios_base::out & std::ios_base::trunc);

        Int_t runNumber = 0, detId = -1, rtsStatusCode = -2, shiftStatusCode = -2;
	char glbSetup[128];
	char daqSetup[128];
	TString glbSetupS, daqSetupS;
        FILE *inR = fopen("fullRunListTable_ist_2014.txt", "r");
        while(!feof(inR)) {
            char buff[256];
            if(fgets(buff,sizeof(buff),inR) == 0) continue ;
                switch(buff[0]) {
                    case 'r' : //run_Id
                        continue ;
                }
                int ret = sscanf(buff,"%d %s %s %d %d %d", &runNumber, glbSetup, daqSetup, &detId, &rtsStatusCode, &shiftStatusCode);
		if(ret != 6) continue ;
		cout << runNumber << "\t" << detId << "\t" << rtsStatusCode << "\t" << shiftStatusCode << "\t" << glbSetup << "\t" << daqSetup << endl;

		glbSetupS = Form(glbSetup);
		int found = glbSetupS.First(' ');
                if(found >= 0)
                        glbSetupS.Replace(found, glbSetupS.Length(), "");

		daqSetupS = Form(daqSetup);
		found = daqSetupS.First(' ');
                if(found >= 0)
                        daqSetupS.Replace(found, daqSetupS.Length(), "");

		if( glbSetupS.Contains("pedestal_tcd_only") && daqSetupS.Contains("pedestal") )
			fRunList_pedestal << runNumber << endl;

		if( glbSetupS.Contains("pedAsPhys") && daqSetupS.Contains("physics") )
			fRunList_pedAsPhys << runNumber << endl;

		if( (glbSetupS.Contains("CosmicLocalClock") && daqSetupS.Contains("CosmicLocalClock")) || 
		    (glbSetupS.Contains("CosmicLocalClock") && daqSetupS.Contains("physics")) ||
		    (glbSetupS.Contains("production_15GeV_2014") && daqSetupS.Contains("physics")) || 
		    (glbSetupS.Contains("AuAu_200") && daqSetupS.Contains("physics")) || 
		    (glbSetupS.Contains("AuHe3") && daqSetupS.Contains("physics"))  )
                        fRunList_physics << runNumber << endl;
            }
        fclose(inR);
	fRunList_physics.close();
	fRunList_pedAsPhys.close();
	fRunList_pedestal.close();
}
