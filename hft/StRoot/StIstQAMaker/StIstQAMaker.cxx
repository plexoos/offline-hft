/***************************************************************************
*
* $Id$
*
* Author: Yaping Wang, October 2013
****************************************************************************
* Description: 
* See header file.
****************************************************************************
*
* $Log$
* Revision 1.13  2014/03/18 02:19:37  ypwang
* add two 2D histogram sets for max ADC time bin vs APV/Group electronics ID
*
* Revision 1.12  2014/03/11 17:51:32  ypwang
* Histogram type changed from TH2F to TH2S, and reduce several histogram's bin number
*
* Revision 1.11  2014/03/05 16:54:14  ypwang
* minor update on istRawHit branch to include all time bins' charge and chargeErr informations
*
* Revision 1.10  2014/03/01 00:31:42  ypwang
* StIstDigiHit object called
*
* Revision 1.9  2014/02/26 01:58:05  ypwang
* adding meanRow/meanColumn/ApvId transforms from local position
*
* Revision 1.8  2014/02/25 17:12:26  ypwang
* minor update due to mClusteringType moved to StIstHitCollection
*
* Revision 1.4  2014/02/14 14:47:20  ypwang
* update due to removal of getNumLadders() member function from StIstCollection
*
* Revision 1.3  2014/02/06 12:28:35  ypwang
* updating scripts
*
*
****************************************************************************
* StIstQAMaker.cxx,v 1.0
* Revision 1.0 2013/11/04 16:20:30 Yaping
* Initial version
****************************************************************************/

#include "StIstQAMaker.h"
#include "StIstHitCollection.h"
#include "StIstHit.h"
#include "StRoot/StIstUtil/StIstDigiHit.h"
#include "StRoot/StIstUtil/StIstCollection.h"
#include "StRoot/StIstUtil/StIstRawHitCollection.h"
#include "StRoot/StIstUtil/StIstRawHit.h"
#include "StRoot/StIstUtil/StIstConsts.h"
#include "StIOMaker/StIOMaker.h"
#include "StEvent.h"
#include "StTrack.h"

#include "StEventTypes.h"
#include "StMessMgr.h"
#include "StDcaGeometry.h"
#if ROOT_VERSION_CODE < 334081
#include "TArrayL.h"
#else
#include "TArrayL64.h"
#endif
#include "TClassTable.h"
#include "TH2F.h"
#include "TProfile.h"
#include "TNtuple.h"
#include "TFile.h"
#include "TString.h"
#include "StThreeVectorF.hh"
#include "StDetectorName.h"

#include <string>

// constructor
StIstQAMaker::StIstQAMaker( const char* name ) :
   StMaker(name), mEventCounter(0) {
   for(int iSensor=0; iSensor<kIstNumSensors; iSensor++) {
	rawHitMap[iSensor] = NULL;
	hitMap[iSensor] = NULL;
	numOfRawHits_EventId[iSensor] = NULL;
   }
   for(unsigned char iTimeBin=0; iTimeBin<kIstNumTimeBins; iTimeBin++)
    	rawHitCharge_TimeBin[iTimeBin] = NULL;

   rawHitMaxTimeBin_APV = NULL;
   rawHitMaxTimeBin_Section = NULL;
   hitMapOfIST = NULL;
   hitMapOfAPV = NULL;
   hitGlobalXY = NULL;
   hitGlobalPhiZ = NULL;
   rawHitChargeErr = NULL;
   hitCharge_SensorId = NULL;
   hitChargeErr_SensorId = NULL;
   maxTimeBin_SensorId = NULL;
   numOfRawHits_SensorId = NULL;
   numOfHits_SensorId = NULL;
   clusterSize_SensorId = NULL;
   clusterSizeZ_SensorId = NULL;
   clusterSizeRPhi_SensorId = NULL;
}

// initialize
Int_t StIstQAMaker::Init()
{
    Int_t ierr = kStOk;

    istRawHitTree = new TTree("istRawHits", "istRawHits_QA");
    istRawHitTree->Branch("rawHits", &istRawHit, "channelId/I:geoId:ladder:sensor:column:row:maxTimeBin:rdo:arm:apv:channel:idTruth:EventId:charge[9]/F:chargeErr[9]/F");

    istHitTree = new TTree("istHits", "istHits_QA");
    istHitTree->Branch("hits", &istHit, "hitId/I:ladder:sensor:apv:idTruth:EventId:maxTimeBin:clusteringType:nRawHits:nRawHitsZ:nRawHitsRPhi:meanColumn/F:meanRow:localX:localY:localZ:x:y:z:charge:chargeErr");

    numOfRawHits_SensorId = new TH2S("numOfRawHits_SensorId", "The number of RawHits vs. sensor ID", 144, 1, 145, 128, 0, 128);
    numOfRawHits_SensorId->GetXaxis()->SetTitle("Sensor ID");
    numOfRawHits_SensorId->GetYaxis()->SetTitle("Number of Raw Hits");

    rawHitMaxTimeBin_Section = new TH2S("rawHitMaxTimeBin_Section", "Max time bin of raw hits vs group ID", 72, 0, 72, kIstNumTimeBins, 0, kIstNumTimeBins);
    rawHitMaxTimeBin_Section->GetXaxis()->SetTitle("Group ID [12*(RDO-1)+2*ARM+GROUP]");
    rawHitMaxTimeBin_Section->GetYaxis()->SetTitle("Max Time Bin Index");

    rawHitMaxTimeBin_APV = new TH2S("rawHitMaxTimeBin_APV", "Max time bin of raw hits vs APV ID", 864, 0, 864, kIstNumTimeBins, 0, kIstNumTimeBins);
    rawHitMaxTimeBin_APV->GetXaxis()->SetTitle("APV ID [144*(RDO-1)+24*ARM+APV]");
    rawHitMaxTimeBin_APV->GetYaxis()->SetTitle("Max Time Bin Index");

    char buffer[100];
    for(int iTimeBin=0; iTimeBin<kIstNumTimeBins; iTimeBin++) {
	sprintf(buffer, "rawHitCharge_TimeBin%d", iTimeBin);
	rawHitCharge_TimeBin[iTimeBin] = new TH2S(buffer, Form("ADC of raw hits at time bin %d vs. channel geometry ID",iTimeBin), 864, 1, 110593, 512, 0, kIstMaxAdc);
	rawHitCharge_TimeBin[iTimeBin]->GetXaxis()->SetTitle("Channel ID");
	rawHitCharge_TimeBin[iTimeBin]->GetYaxis()->SetTitle("ADC of Raw Hits");
    }

    rawHitChargeErr = new TH2S("rawHitChargeErr", "RMS noise of raw hits vs. channel geometry ID", 864, 1, 110593, 128, 0, 64);
    rawHitChargeErr->GetXaxis()->SetTitle("Channel ID");
    rawHitChargeErr->GetYaxis()->SetTitle("RMS noise of Raw Hits");

    numOfHits_SensorId = new TH2S("numOfHits_SensorId", "The number of hits vs. sensor ID", 144, 1, 145, 128, 0, 128);
    numOfHits_SensorId->GetXaxis()->SetTitle("Sensor ID");
    numOfHits_SensorId->GetYaxis()->SetTitle("Number of Hits");

    hitCharge_SensorId = new TH2S("hitCharge_SensorId", "ADC of hits vs. sensor ID", 144, 1, 145, 512, 0, kIstMaxAdc);
    hitCharge_SensorId->GetXaxis()->SetTitle("Sensor ID");
    hitCharge_SensorId->GetYaxis()->SetTitle("ADC of Hits");

    hitChargeErr_SensorId = new TH2S("hitChargeErr_SensorId", "RMS noise of hits vs. sensor ID", 144, 1, 145, 128, 0, 64);
    hitChargeErr_SensorId->GetXaxis()->SetTitle("Sensor ID");
    hitChargeErr_SensorId->GetYaxis()->SetTitle("RMS noise of Hits");

    maxTimeBin_SensorId = new TH2S("maxTimeBin_SensorId", "Max time bin of hits vs. sensor ID", 144, 1, 145, kIstNumTimeBins, 0, kIstNumTimeBins);
    maxTimeBin_SensorId->GetXaxis()->SetTitle("Sensor ID");
    maxTimeBin_SensorId->GetYaxis()->SetTitle("Max Time Bin Index");

    clusterSize_SensorId = new TH2S("clusterSize_SensorId", "Cluster size of hits vs. sensor ID", 144, 1, 145, 20, 0, 20);
    clusterSize_SensorId->GetXaxis()->SetTitle("Sensor ID");
    clusterSize_SensorId->GetYaxis()->SetTitle("Cluster Size of Hits");

    clusterSizeZ_SensorId = new TH2S("clusterSizeZ_SensorId", "Cluster size in Z of hits vs. sensor ID", 144, 1, 145, 20, 0, 20);
    clusterSizeZ_SensorId->GetXaxis()->SetTitle("Sensor ID");
    clusterSizeZ_SensorId->GetYaxis()->SetTitle("Cluster Size in Z of Hits");

    clusterSizeRPhi_SensorId = new TH2S("clusterSizeRPhi_SensorId", "Cluster size in r-#phi of hits vs. sensor ID", 144, 1, 145, 20, 0, 20);
    clusterSizeRPhi_SensorId->GetXaxis()->SetTitle("Sensor ID");
    clusterSizeRPhi_SensorId->GetYaxis()->SetTitle("Cluster Size in r-#phi of hits");

    hitMapOfIST = new TH2S("hitMapOfIST", "IST hit map in r-phi vs. Z", 1536, 1, 1537, 72, 1, 73);
    hitMapOfIST->GetXaxis()->SetTitle("Row in r-#phi");
    hitMapOfIST->GetYaxis()->SetTitle("Column in Z");

    hitMapOfAPV = new TH2S("hitMapOfAPV", "IST hit map in APV geometry Id vs. ladder", 36, 1, 37, 24, 1, 25);
    hitMapOfAPV->GetXaxis()->SetTitle("APV geometry ID");
    hitMapOfAPV->GetYaxis()->SetTitle("Ladder ID");

    hitGlobalXY = new TH2F("hitGlobalXY", "Global X vs. Global Y", 150, -15, 15, 150, -15, 15);
    hitGlobalXY->GetXaxis()->SetTitle("Global X [cm]");
    hitGlobalXY->GetYaxis()->SetTitle("Global Y [cm]");

    hitGlobalPhiZ = new TH2F("hitGlobalPhiZ", "Global #phi vs. Global Z", 100, -3.14159, 3.14159, 300, -25, 25);
    hitGlobalPhiZ->GetXaxis()->SetTitle("Global #phi [rad.]");
    hitGlobalPhiZ->GetYaxis()->SetTitle("Global Z [cm]");

    char histtitle[128];
    for(int iLadder=0; iLadder<kIstNumLadders; iLadder++) {
	for(int iSensor=0; iSensor<kIstNumSensorsPerLadder; iSensor++) {
	    sprintf(histtitle, "Raw Hit column vs. row: Ladder %d Sensor %d", iLadder+1, iSensor+1);
	    sprintf(buffer,"rawHitMap_Sensor%d", iLadder*6+iSensor+1);
	    rawHitMap[iLadder*6+iSensor] = new TH2S(buffer, histtitle, 12, 1, 13, 64, 1, 65);
	    rawHitMap[iLadder*6+iSensor]->GetXaxis()->SetTitle("Column");
	    rawHitMap[iLadder*6+iSensor]->GetYaxis()->SetTitle("Row");

	    sprintf(histtitle, "Number of raw hits vs. EventID: Ladder %d Sensor %d", iLadder+1, iSensor+1);
            sprintf(buffer,"numOfRawHitsVsEventId_Sensor%d", iLadder*6+iSensor+1);
	    numOfRawHits_EventId[iLadder*6+iSensor] = new TProfile(buffer, histtitle, 10000, 0, 10000);
            numOfRawHits_EventId[iLadder*6+iSensor]->GetXaxis()->SetTitle("EventID (Time)");
            numOfRawHits_EventId[iLadder*6+iSensor]->GetYaxis()->SetTitle("<rawhits>");

	    sprintf(histtitle, "Hit mean column vs. mean row: Ladder %d Sensor %d", iLadder+1, iSensor+1);
            sprintf(buffer,"hitMap_Sensor%d", iLadder*6+iSensor+1);
            hitMap[iLadder*6+iSensor] = new TH2S(buffer, histtitle, 12, 1, 13, 64, 1, 65);
            hitMap[iLadder*6+iSensor]->GetXaxis()->SetTitle("Mean Column");
            hitMap[iLadder*6+iSensor]->GetYaxis()->SetTitle("Mean Row");
	}		
    }
    return ierr;
}

Int_t StIstQAMaker::Make(){
   Int_t ierr = kStOk;

   //hit data input
   StEvent* eventPtr = 0;
   eventPtr = (StEvent*)GetInputDS("StEvent");

   if( !eventPtr ) {
      LOG_WARN << "StIstQAMaker::Make : Error getting pointer to StEvent from '" << ClassName() << "'" << endm;
      ierr = kStWarn;
   }

   StIstHitCollection* istHitCollection = 0;
   if( eventPtr ) {
      istHitCollection = eventPtr->istHitCollection();
   }

   if( !istHitCollection) {
      LOG_WARN << "StIstQAMaker::Make : Error getting pointer to StIstHitCollection from '" << ClassName() << "'" << endm;
      ierr = kStWarn;
   }

   //raw hit data input
    TObjectSet* istDataSet = (TObjectSet*)GetDataSet("istRawHitAndCluster");
    if (!istDataSet) {
        LOG_WARN << "StIstQAMaker::Make() - there is no istDataSet (raw hit) " << endm;
        ierr = kStWarn;
    }        
          
    StIstCollection* istCollectionPtr = (StIstCollection*)istDataSet->GetObject();
    if(!istCollectionPtr) {
        LOG_WARN << "StIstQAMaker::Make() - no istCollection."<<endm;
        ierr = kStWarn;
    }

   //*******Initialization of the raw hit and hit level variables********** 
   istRawHit.channelId = istRawHit.geoId = istRawHit.ladder = istRawHit.sensor = istRawHit.column = istRawHit.row = istRawHit.maxTimeBin = istRawHit.rdo = istRawHit.arm = istRawHit.apv = istRawHit.channel = istRawHit.idTruth = istRawHit.EventId = -1;
   for(int iTB=0; iTB<kIstNumTimeBins; iTB++) {
	istRawHit.charge[iTB] = 0.;
	istRawHit.chargeErr[iTB] = 0.;
   }
   istHit.hitId = istHit.ladder = istHit.sensor = istHit.apv = istHit.idTruth = istHit.EventId = istHit.maxTimeBin = istHit.clusteringType = istHit.nRawHits = istHit.nRawHitsZ = istHit.nRawHitsRPhi = -1;
   istHit.meanColumn = istHit.meanRow = istHit.localX = istHit.localY = istHit.localZ = istHit.x = istHit.y = istHit.z = istHit.charge = istHit.chargeErr = 0.; 

   //loop
   if( !ierr ){
      StPrimaryVertex *primaryVertex = eventPtr->primaryVertex();
      if(primaryVertex) {
          const StThreeVectorF &primXYZ = primaryVertex->position();
          LOG_DEBUG << "primaryVertex \t" << primXYZ.x() << "\t" << primXYZ.y() << "\t" << primXYZ.z() << endm;
      }
      else{
          LOG_DEBUG << "no primaryVertex found" << endm;
      }
      
      if(mEventCounter%10 == 0)
      	  LOG_DEBUG << "event index: " << mEventCounter << endm; 

      if(istHitCollection->numberOfHits() > 0) {
         for(int ladderIdx=0; ladderIdx<kIstNumLadders; ladderIdx++ )	{
	    StIstLadderHitCollection* ladderHitCollection = istHitCollection->ladder(ladderIdx);
	    unsigned char nClusteringType = istHitCollection->getClusteringType();
	    for(int sensorIdx=0; sensorIdx<kIstNumSensorsPerLadder; sensorIdx++)	{
	       StIstSensorHitCollection* sensorHitCollection = ladderHitCollection->sensor(sensorIdx);
	       int sensorIdxTemp = 0;
               for(int idx=0; idx<(int)sensorHitCollection->hits().size(); idx++ ){
		  StIstHit* &hitT = sensorHitCollection->hits()[idx];
		  StIstDigiHit* hit = new StIstDigiHit(*hitT);

		  if(hit)	{
			const StThreeVectorF &P = hit->position();

			istHit.hitId 		= (int)hit->id();
			istHit.ladder		= (int)hit->getLadder();
			istHit.sensor		= (int)hit->getSensor();
			istHit.maxTimeBin	= (int)hit->getMaxTimeBin();
			istHit.clusteringType	= (int)nClusteringType;
			istHit.nRawHits		= (int)hit->getNRawHits();
			istHit.nRawHitsZ	= (int)hit->getNRawHitsZ();
			istHit.nRawHitsRPhi	= (int)hit->getNRawHitsRPhi();
			istHit.idTruth		= (int)hit->idTruth();
			istHit.EventId		= (int)eventPtr->id();
			istHit.localX          	= (float)hit->localPosition(0);
			istHit.localY		= (float)hit->localPosition(1);
			istHit.localZ		= (float)hit->localPosition(2);
			istHit.x		= (float)P.x();
			istHit.y		= (float)P.y();
			istHit.z		= (float)P.z();
			istHit.charge		= (float)hit->charge();
			istHit.chargeErr	= (float)hit->getChargeErr();
			istHit.apv		= (int)hit->getApv();
			istHit.meanColumn	= (float)hit->getMeanColumn();
			istHit.meanRow		= (float)hit->getMeanRow();

			istHitTree->Fill();

			sensorIdxTemp = ((int)hit->getLadder()-1)*kIstNumSensorsPerLadder + (int)hit->getSensor();
			hitMap[sensorIdxTemp-1]->Fill((int)(hit->getMeanColumn()-0.5)+1, (int)(hit->getMeanRow()-0.5)+1);
			hitMapOfIST->Fill(((int)hit->getLadder()-1)*kIstNumRowsPerSensor+(int)(hit->getMeanRow()-0.5)+1, ((int)hit->getSensor()-1)*kIstNumColumnsPerSensor+(int)(hit->getMeanColumn()-0.5)+1);
			hitMapOfAPV->Fill(((int)hit->getSensor()-1)*kIstApvsPerLadder/kIstNumSensorsPerLadder + (int)hit->getApv(), (int)hit->getLadder());
			hitGlobalXY->Fill((float)P.x(), (float)P.y());
			hitGlobalPhiZ->Fill((float)P.phi(), (float)P.z());

			hitCharge_SensorId->Fill(sensorIdxTemp, (int)hit->charge());
			hitChargeErr_SensorId->Fill(sensorIdxTemp, (int)(hit->getChargeErr()+0.5));
			maxTimeBin_SensorId->Fill(sensorIdxTemp, (int)hit->getMaxTimeBin());
			clusterSize_SensorId->Fill(sensorIdxTemp, (int)hit->getNRawHits());
			clusterSizeZ_SensorId->Fill(sensorIdxTemp, (int)hit->getNRawHitsZ());
			clusterSizeRPhi_SensorId->Fill(sensorIdxTemp, (int)hit->getNRawHitsRPhi());
		 }//hit cut
              }//end loop over hits
	      numOfHits_SensorId->Fill(sensorIdxTemp, sensorHitCollection->hits().size());
	   }//loop over sensors
        }//loop over ladders 
      }//end of hits cut

      if(istCollectionPtr->getNumRawHits() > 0) {
	 int counter[kIstNumSensors]; //raw hit multiplicity per sensor per event
         for(int iS=0; iS<kIstNumSensors; iS++)
             counter[iS] = 0;

         for(int ladderIdx=0; ladderIdx<kIstNumLadders; ++ladderIdx ){
            StIstRawHitCollection *rawHitCollectionPtr = istCollectionPtr->getRawHitCollection( ladderIdx );
            if( rawHitCollectionPtr ){
               std::vector<StIstRawHit*>& rawHitVec = rawHitCollectionPtr->getRawHitVec();
               std::vector< StIstRawHit* >::iterator rawHitIter;

               for( rawHitIter = rawHitVec.begin(); rawHitIter != rawHitVec.end(); ++rawHitIter ){
		   StIstRawHit* rawHit = *rawHitIter;

		   unsigned char ladder = rawHit->getLadder();
		   unsigned char sensor = rawHit->getSensor();
		   unsigned char maxTimeBin = rawHit->getMaxTimeBin();
		   int sensorId = (ladder-1)*kIstNumSensorsPerLadder + sensor;
		   counter[sensorId-1]++;

                   for( unsigned char timeBin = 0; timeBin < kIstNumTimeBins; ++timeBin ) {
		       rawHitCharge_TimeBin[timeBin]->Fill(rawHit->getGeoId(), (int)rawHit->getCharge( timeBin ));
		       istRawHit.charge[timeBin] = rawHit->getCharge(timeBin);
		       istRawHit.chargeErr[timeBin]  = rawHit->getChargeErr(timeBin);
		   }
		   rawHitChargeErr->Fill(rawHit->getGeoId(), (int)(rawHit->getChargeErr( maxTimeBin )+0.5));
		   rawHitMap[sensorId-1]->Fill((int)rawHit->getColumn(), (int)rawHit->getRow());
		   rawHitMaxTimeBin_Section->Fill(((int)rawHit->getRdo()-1)*12+(int)rawHit->getArm()*2+(int)rawHit->getApv()/12, (int)maxTimeBin);
		   rawHitMaxTimeBin_APV->Fill(((int)rawHit->getRdo()-1)*144+(int)rawHit->getArm()*24+(int)rawHit->getApv(), (int)maxTimeBin);

		   istRawHit.channelId 	= (int)rawHit->getChannelId();
		   istRawHit.geoId      = (int)rawHit->getGeoId();
		   istRawHit.ladder	= (int)rawHit->getLadder();
		   istRawHit.sensor	= (int)rawHit->getSensor();
		   istRawHit.column	= (int)rawHit->getColumn();
		   istRawHit.row	= (int)rawHit->getRow();
		   istRawHit.maxTimeBin = (int)maxTimeBin;
		   istRawHit.rdo	= (int)rawHit->getRdo();
		   istRawHit.arm        = (int)rawHit->getArm();
		   istRawHit.apv        = (int)rawHit->getApv();
		   istRawHit.channel    = (int)rawHit->getChannel();
		   istRawHit.idTruth	= (int)rawHit->getIdTruth();
		   istRawHit.EventId	= (int)eventPtr->id();

		   istRawHitTree->Fill();
                }//loop over raw hits
             }//end raw hit collection
          }//loop over ladders

	  for(int iS=0; iS<kIstNumSensors; iS++) {
              numOfRawHits_SensorId->Fill(iS+1, counter[iS]);
              numOfRawHits_EventId[iS]->Fill((int)eventPtr->id()/100+1, counter[iS]);
          }
       }//end number of raw hits cut
   }
  
   mEventCounter++;

   return ierr;
}

Int_t StIstQAMaker::Finish(){
   Int_t ierr = kStOk;

   //create output file
    StIOMaker *ioMaker = (StIOMaker * )GetMaker("inputStream");
    if (!ioMaker) {
        gMessMgr->Warning() << "StIstQAMaker::Init(): No StIOMaker" << endm;
    }

    TString mRootFilename = TString(ioMaker->GetFile());
    int found = mRootFilename.Last('/');
    if(found >= 0){
        mRootFilename.Replace(0, found + 1, "");
    }
    found = mRootFilename.First(".");
    if(found == 0) found = mRootFilename.Length();
    mRootFilename.Replace(found, mRootFilename.Length() - found, ".istQa.root");
    LOG_INFO << "IST QA File Name: " << mRootFilename << endm;

    cout << "QA file name: " << mRootFilename << endl;     
    myRootFile = new TFile(mRootFilename.Data(),"RECREATE");
    if( !myRootFile ) {
        LOG_WARN << "Error recreating file '" << mRootFilename << "'" << endl;
        ierr = kStWarn;
    }

    //saving histograms 
    myRootFile->WriteTObject(istRawHitTree);
    myRootFile->WriteTObject(istHitTree);

    myRootFile->WriteTObject(numOfRawHits_SensorId);
    myRootFile->WriteTObject(numOfHits_SensorId);
    for(int iTimeBin=0; iTimeBin<kIstNumTimeBins; iTimeBin++)
   	myRootFile->WriteTObject(rawHitCharge_TimeBin[iTimeBin]);
    myRootFile->WriteTObject(rawHitChargeErr);
    myRootFile->WriteTObject(hitCharge_SensorId);
    myRootFile->WriteTObject(hitChargeErr_SensorId);
    myRootFile->WriteTObject(rawHitMaxTimeBin_Section);
    myRootFile->WriteTObject(rawHitMaxTimeBin_APV);
    myRootFile->WriteTObject(maxTimeBin_SensorId);
    myRootFile->WriteTObject(clusterSize_SensorId);
    myRootFile->WriteTObject(clusterSizeZ_SensorId);
    myRootFile->WriteTObject(clusterSizeRPhi_SensorId);
    myRootFile->WriteTObject(hitMapOfIST);
    myRootFile->WriteTObject(hitMapOfAPV);
    myRootFile->WriteTObject(hitGlobalXY);
    myRootFile->WriteTObject(hitGlobalPhiZ);
    for(int iLadder=0; iLadder<kIstNumLadders; iLadder++) {
        for(int iSensor=0; iSensor<kIstNumSensorsPerLadder; iSensor++) {
    	    myRootFile->WriteTObject(rawHitMap[iLadder*6+iSensor]);
            myRootFile->WriteTObject(hitMap[iLadder*6+iSensor]);
	    myRootFile->WriteTObject(numOfRawHits_EventId[iLadder*6+iSensor]);
   	}
    }

    myRootFile->Close();

    return ierr;
}

ClassImp( StIstQAMaker );
