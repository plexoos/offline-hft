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
* Revision 1.1  2014/01/23 20:11:30  ypwang
* adding scripts
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
#include "StRoot/StIstUtil/StIstCollection.h"
#include "StRoot/StIstUtil/StIstRawHitCollection.h"
#include "StRoot/StIstUtil/StIstRawHit.h"
#include "StRoot/StIstUtil/StIstConsts.h"
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
   StMaker(name), mRootFilename("istQa.root"), mEventCounter(0) {
   for(int iSensor=0; iSensor<kIstNumSensors; iSensor++) {
	rawHitMap[iSensor] = NULL;
	hitMap[iSensor] = NULL;
	numOfRawHits_EventId[iSensor] = NULL;
   }
   for(unsigned char iTimeBin=0; iTimeBin<kIstNumTimeBins; iTimeBin++)
    	rawHitCharge_TimeBin[iTimeBin] = NULL;

   hitMapOfIST = NULL;
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

    if( !mRootFilename.empty() ) {
    	myRootFile = new TFile(mRootFilename.data(),"RECREATE");  
        if( !myRootFile ) {
            LOG_WARN << "Error recreating file '" << mRootFilename << "'" << endl;
            ierr = kStWarn;
        }
	
	istQaTree = new TTree("istRawHitsAndHits", "istQA_RawHitsAndHits");
	istQaTree->Branch("rawHits", &istRawHit, "channelId/I:geoId:ladder:sensor:column:row:maxTimeBin:idTruth:EventId:charge/F:chargeErr");
	istQaTree->Branch("hits", &istHit, "hitId/I:ladder:sensor:idTruth:EventId:maxTimeBin:clusteringType:nRawHits:nRawHitsZ:nRawHitsRPhi:meanColumn/F:meanRow:localX:localY:localZ:x:y:z:charge:chargeErr");

	numOfRawHits_SensorId = new TH2F("numOfRawHits_SensorId", "The number of RawHits vs. sensorID", 144, 1, 145, 768, 0, 768);
        numOfRawHits_SensorId->GetXaxis()->SetTitle("Sensor ID");
        numOfRawHits_SensorId->GetYaxis()->SetTitle("Number of Raw Hits");

	char buffer[100];
	for(int iTimeBin=0; iTimeBin<kIstNumTimeBins; iTimeBin++) {
	   sprintf(buffer, "rawHitCharge_TimeBin%d", iTimeBin);
	   rawHitCharge_TimeBin[iTimeBin] = new TH2F(buffer, "ADC of raw hits over all time bins vs. channel ID", 110592, 1, 110593, 512, 0, kIstMaxAdc);
	   rawHitCharge_TimeBin[iTimeBin]->GetXaxis()->SetTitle("Channel ID");
	   rawHitCharge_TimeBin[iTimeBin]->GetYaxis()->SetTitle("ADC of Raw Hits");
	}

	rawHitChargeErr = new TH2F("rawHitChargeErr", "RMS noise of raw hits vs. channel ID", 110592, 1, 110593, 128, 0, 64);
	rawHitChargeErr->GetXaxis()->SetTitle("Channel ID");
	rawHitChargeErr->GetYaxis()->SetTitle("RMS noise of Raw Hits");

	numOfHits_SensorId = new TH2F("numOfHits_SensorId", "The number of Hits vs. sensorID", 144, 1, 145, 768, 0, 768);
        numOfHits_SensorId->GetXaxis()->SetTitle("Sensor ID");
        numOfHits_SensorId->GetYaxis()->SetTitle("Number of Hits");

	hitCharge_SensorId = new TH2F("hitCharge_SensorId", "ADC of raw hits vs. sensor ID", 144, 1, 145, 512, 0, kIstMaxAdc);
	hitCharge_SensorId->GetXaxis()->SetTitle("Sensor ID");
	hitCharge_SensorId->GetYaxis()->SetTitle("ADC of Hits");

	hitChargeErr_SensorId = new TH2F("hitChargeErr_SensorId", "RMS noise of hits vs. sensor ID", 144, 1, 145, 128, 0, 64);
        hitChargeErr_SensorId->GetXaxis()->SetTitle("Sensor ID");
        hitChargeErr_SensorId->GetYaxis()->SetTitle("RMS noise of Hits");

	maxTimeBin_SensorId = new TH2F("maxTimeBin_SensorId", "Max time bin of hits vs. channel ID", 144, 1, 145, kIstNumTimeBins, 0, kIstNumTimeBins);
        maxTimeBin_SensorId->GetXaxis()->SetTitle("Sensor ID");
        maxTimeBin_SensorId->GetYaxis()->SetTitle("Max Time Bin Index");

	clusterSize_SensorId = new TH2F("clusterSize_SensorId", "Cluster size of Hits vs. sensorID", 144, 1, 145, 20, 0, 20);
        clusterSize_SensorId->GetXaxis()->SetTitle("Sensor ID");
        clusterSize_SensorId->GetYaxis()->SetTitle("Cluster Size of Hits");

	clusterSizeZ_SensorId = new TH2F("clusterSizeZ_SensorId", "Cluster size in Z of Hits vs. sensorID", 144, 1, 145, 20, 0, 20);
        clusterSizeZ_SensorId->GetXaxis()->SetTitle("Sensor ID");
        clusterSizeZ_SensorId->GetYaxis()->SetTitle("Cluster Size in Z of Hits");

	clusterSizeRPhi_SensorId = new TH2F("clusterSizeRPhi_SensorId", "Cluster size in r-#phi of Hits vs. sensorID", 144, 1, 145, 20, 0, 20);
        clusterSizeRPhi_SensorId->GetXaxis()->SetTitle("Sensor ID");
        clusterSizeRPhi_SensorId->GetYaxis()->SetTitle("Cluster Size in r-#phi of Hits");

        hitMapOfIST = new TH2F("hitMapOfIST", "IST hit map in r-phi vs. Z", 1536, 1, 1537, 72, 1, 73);
	hitMapOfIST->GetXaxis()->SetTitle("Row in r-#phi");
	hitMapOfIST->GetYaxis()->SetTitle("Column in Z");

	hitGlobalXY = new TH2F("hitGlobalXY", "Global X vs. Global Y", 150, -15, 15, 150, -15, 15);
	hitGlobalXY->GetXaxis()->SetTitle("Global X [cm]");
	hitGlobalXY->GetYaxis()->SetTitle("Global Y [cm]");

	hitGlobalPhiZ = new TH2F("hitGlobalPhiZ", "Global #phi vs. Global Z", 100, -3.14159, 3.14159, 300, -30, 30);
        hitGlobalPhiZ->GetXaxis()->SetTitle("Global #phi [rad.]");
        hitGlobalPhiZ->GetYaxis()->SetTitle("Global Z [cm]");

	char histtitle[128];
	for(int iLadder=0; iLadder<kIstNumLadders; iLadder++) {
	   for(int iSensor=0; iSensor<kIstNumSensorsPerLadder; iSensor++) {
	      sprintf(histtitle, "Raw Hit column vs. row: Ladder %d Sensor %d", iLadder+1, iSensor+1);
	      sprintf(buffer,"rawHitMap_Sensor%d", iLadder*6+iSensor+1);
	      rawHitMap[iLadder*6+iSensor] = new TH2F(buffer, histtitle, 12, 1, 13, 64, 1, 65);
	      rawHitMap[iLadder*6+iSensor]->GetXaxis()->SetTitle("Column");
	      rawHitMap[iLadder*6+iSensor]->GetYaxis()->SetTitle("Row");

	      sprintf(histtitle, "Number of raw hits vs. EventID: Ladder %d Sensor %d", iLadder+1, iSensor+1);
              sprintf(buffer,"numOfRawHitsVsEventId_Sensor%d", iLadder*6+iSensor+1);
	      numOfRawHits_EventId[iLadder*6+iSensor] = new TProfile(buffer, histtitle, 10000, 0, 10000);
              numOfRawHits_EventId[iLadder*6+iSensor]->GetXaxis()->SetTitle("EventID (Time)");
              numOfRawHits_EventId[iLadder*6+iSensor]->GetYaxis()->SetTitle("<rawhits>");

	      sprintf(histtitle, "Hit mean column vs. mean row: Ladder %d Sensor %d", iLadder+1, iSensor+1);
              sprintf(buffer,"hitMap_Sensor%d", iLadder*6+iSensor+1);
              hitMap[iLadder*6+iSensor] = new TH2F(buffer, histtitle, 12, 1, 13, 64, 1, 65);
              hitMap[iLadder*6+iSensor]->GetXaxis()->SetTitle("Mean Column");
              hitMap[iLadder*6+iSensor]->GetYaxis()->SetTitle("Mean Row");
	   }		
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
   istRawHit.channelId = istRawHit.geoId = istRawHit.ladder = istRawHit.sensor = istRawHit.column = istRawHit.row = istRawHit.maxTimeBin = istRawHit.idTruth = istRawHit.EventId = -1;
   istRawHit.charge = istRawHit.chargeErr = 0.;

   istHit.hitId = istHit.ladder = istHit.sensor = istHit.idTruth = istHit.EventId = istHit.maxTimeBin = istHit.clusteringType = istHit.nRawHits = istHit.nRawHitsZ = istHit.nRawHitsRPhi = -1;
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
      	  //LOG_DEBUG << "event index: " << mEventCounter << endm; 
	  cout << "event index: " << mEventCounter << endl;

      for(unsigned int ladderIdx=0; ladderIdx<istHitCollection->numberOfLadders() && ladderIdx<kIstNumLadders; ladderIdx++ )	{
	  StIstLadderHitCollection* ladderHitCollection = istHitCollection->ladder(ladderIdx);
	  for(unsigned int sensorIdx=0; sensorIdx<ladderHitCollection->numberOfSensors() && sensorIdx<kIstNumSensorsPerLadder; sensorIdx++)	{
	      StIstSensorHitCollection* sensorHitCollection = ladderHitCollection->sensor(sensorIdx);
              for(int idx=0; idx<(int)sensorHitCollection->hits().size(); idx++ ){
		  StIstHit* hit = sensorHitCollection->hits()[idx];
		  if(hit)	{
			const StThreeVectorF &P = hit->position();
			istHit.hitId 		= (int)hit->id();
			istHit.ladder		= (int)hit->getLadder();
			istHit.sensor		= (int)hit->getSensor();
			istHit.maxTimeBin	= (int)hit->getMaxTimeBin();
			istHit.clusteringType	= (int)hit->getClusteringType();
			istHit.nRawHits		= (int)hit->getNRawHits();
			istHit.nRawHitsZ	= (int)hit->getNRawHitsZ();
			istHit.nRawHitsRPhi	= (int)hit->getNRawHitsRPhi();
			istHit.idTruth		= (int)hit->idTruth();
			istHit.EventId		= (int)eventPtr->id();
			istHit.meanColumn	= (float)hit->getMeanColumn();
			istHit.meanRow		= (float)hit->getMeanRow();
			istHit.localX          	= (float)hit->localPosition(0);
			istHit.localY		= (float)hit->localPosition(1);
			istHit.localZ		= (float)hit->localPosition(2);
			istHit.x		= (float)P.x();
			istHit.y		= (float)P.y();
			istHit.z		= (float)P.z();
			istHit.charge		= (float)hit->charge();
			istHit.chargeErr	= (float)hit->getChargeErr();

			hitMap[ladderIdx*6+sensorIdx]->Fill((float)hit->getMeanColumn(), (float)hit->getMeanRow());
			hitMapOfIST->Fill(ladderIdx*64+(int)hit->getMeanRow(), sensorIdx*12+(int)hit->getMeanColumn());
			hitGlobalXY->Fill((float)P.x(), (float)P.y());
			hitGlobalPhiZ->Fill((float)P.phi(), (float)P.z());
			hitCharge_SensorId->Fill(ladderIdx*6+sensorIdx+1, hit->charge());
			hitChargeErr_SensorId->Fill(ladderIdx*6+sensorIdx+1, hit->getChargeErr());
			maxTimeBin_SensorId->Fill(ladderIdx*6+sensorIdx+1, (int)hit->getMaxTimeBin());
			clusterSize_SensorId->Fill(ladderIdx*6+sensorIdx+1, (int)hit->getNRawHits());
			clusterSizeZ_SensorId->Fill(ladderIdx*6+sensorIdx+1, (int)hit->getNRawHitsZ());
			clusterSizeRPhi_SensorId->Fill(ladderIdx*6+sensorIdx+1, (int)hit->getNRawHitsRPhi());
		}//hit cut
            }//end loop over hits
	    numOfHits_SensorId->Fill(ladderIdx*6+sensorIdx+1, sensorHitCollection->hits().size());
	}//loop over sensors
      }//loop over ladders 

      for( unsigned char ladderIdx=0; ladderIdx<istCollectionPtr->getNumLadders() && ladderIdx<kIstNumLadders; ++ladderIdx ){
         StIstRawHitCollection *rawHitCollectionPtr = istCollectionPtr->getRawHitCollection( ladderIdx );
         if( rawHitCollectionPtr ){
            std::vector<StIstRawHit*>& rawHitVec = rawHitCollectionPtr->getRawHitVec();
            std::vector< StIstRawHit* >::iterator rawHitIter;
	    
	    unsigned char ladder = 0, sensor = 0, maxTimeBin = 0;
	    int sensorId = 0;
	    int counter[6];
	    for(int iS=0; iS<6; iS++)
	        counter[iS] = 0;

            for( rawHitIter = rawHitVec.begin(); rawHitIter != rawHitVec.end(); ++rawHitIter ){
		StIstRawHit* rawHit = *rawHitIter;

		ladder = rawHit->getLadder();
		sensor = rawHit->getSensor();
		maxTimeBin = rawHit->getMaxTimeBin();
		sensorId = (ladder-1)*6+sensor;
		counter[sensor-1]++;

                for( unsigned char timeBin = 0; timeBin < kIstNumTimeBins; ++timeBin )
		   rawHitCharge_TimeBin[timeBin]->Fill(rawHit->getChannelId(), rawHit->getCharge( timeBin ));

		rawHitChargeErr->Fill(rawHit->getChannelId(), rawHit->getChargeErr( maxTimeBin ));
		rawHitMap[sensorId-1]->Fill((float)rawHit->getColumn(), (float)rawHit->getRow());

		istRawHit.channelId 	= (int)rawHit->getChannelId();
		istRawHit.geoId         = (int)rawHit->getGeoId();
		istRawHit.ladder	= (int)rawHit->getLadder();
		istRawHit.sensor	= (int)rawHit->getSensor();
		istRawHit.column	= (int)rawHit->getColumn();
		istRawHit.row		= (int)rawHit->getRow();
		istRawHit.maxTimeBin    = (int)maxTimeBin;
		istRawHit.charge	= (float)rawHit->getCharge(maxTimeBin);
		istRawHit.chargeErr	= (float)rawHit->getChargeErr(maxTimeBin);
		istRawHit.idTruth	= (int)rawHit->getIdTruth();
		istRawHit.EventId	= (int)eventPtr->id();

            }//loop over raw hits
            for(int iS=0; iS<6; iS++) {
	    	numOfRawHits_SensorId->Fill(ladderIdx*6+iS+1, counter[iS]);
		numOfRawHits_EventId[ladderIdx*6+iS]->Fill((int)eventPtr->id()/100+1, counter[iS]);
	    }
         }//end raw hit collection
      }//loop over ladders
      istQaTree->Fill();
   }
  
   mEventCounter++;

   return ierr;
}

Int_t StIstQAMaker::Finish(){
   Int_t ierr = kStOk;

   //saving histograms 
   myRootFile->Write();
   myRootFile->Close();

   return ierr;
}

ClassImp( StIstQAMaker );
