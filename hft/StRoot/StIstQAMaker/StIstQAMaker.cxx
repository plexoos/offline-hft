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
* Revision 1.7  2014/02/19 08:38:50  ypwang
* update several histograms binning
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
    istRawHitTree->Branch("rawHits", &istRawHit, "channelId/I:geoId:ladder:sensor:column:row:maxTimeBin:rdo:arm:apv:channel:idTruth:EventId:charge/F:chargeErr");

    istHitTree = new TTree("istHits", "istHits_QA");
    istHitTree->Branch("hits", &istHit, "hitId/I:ladder:sensor:apv:idTruth:EventId:maxTimeBin:clusteringType:nRawHits:nRawHitsZ:nRawHitsRPhi:meanColumn/F:meanRow:localX:localY:localZ:x:y:z:charge:chargeErr");

    numOfRawHits_SensorId = new TH2F("numOfRawHits_SensorId", "The number of RawHits vs. sensor ID", 144, 1, 145, 128, 0, 128);
    numOfRawHits_SensorId->GetXaxis()->SetTitle("Sensor ID");
    numOfRawHits_SensorId->GetYaxis()->SetTitle("Number of Raw Hits");

    char buffer[100];
    for(int iTimeBin=0; iTimeBin<kIstNumTimeBins; iTimeBin++) {
	sprintf(buffer, "rawHitCharge_TimeBin%d", iTimeBin);
	rawHitCharge_TimeBin[iTimeBin] = new TH2F(buffer, Form("ADC of raw hits at time bin %d vs. channel geometry ID",iTimeBin), 110592, 1, 110593, 512, 0, kIstMaxAdc);
	rawHitCharge_TimeBin[iTimeBin]->GetXaxis()->SetTitle("Channel ID");
	rawHitCharge_TimeBin[iTimeBin]->GetYaxis()->SetTitle("ADC of Raw Hits");
    }

    rawHitChargeErr = new TH2F("rawHitChargeErr", "RMS noise of raw hits vs. channel geometry ID", 110592, 1, 110593, 128, 0, 64);
    rawHitChargeErr->GetXaxis()->SetTitle("Channel ID");
    rawHitChargeErr->GetYaxis()->SetTitle("RMS noise of Raw Hits");

    numOfHits_SensorId = new TH2F("numOfHits_SensorId", "The number of hits vs. sensor ID", 144, 1, 145, 128, 0, 128);
    numOfHits_SensorId->GetXaxis()->SetTitle("Sensor ID");
    numOfHits_SensorId->GetYaxis()->SetTitle("Number of Hits");

    hitCharge_SensorId = new TH2F("hitCharge_SensorId", "ADC of hits vs. sensor ID", 144, 1, 145, 512, 0, kIstMaxAdc);
    hitCharge_SensorId->GetXaxis()->SetTitle("Sensor ID");
    hitCharge_SensorId->GetYaxis()->SetTitle("ADC of Hits");

    hitChargeErr_SensorId = new TH2F("hitChargeErr_SensorId", "RMS noise of hits vs. sensor ID", 144, 1, 145, 128, 0, 64);
    hitChargeErr_SensorId->GetXaxis()->SetTitle("Sensor ID");
    hitChargeErr_SensorId->GetYaxis()->SetTitle("RMS noise of Hits");

    maxTimeBin_SensorId = new TH2F("maxTimeBin_SensorId", "Max time bin of hits vs. sensor ID", 144, 1, 145, kIstNumTimeBins, 0, kIstNumTimeBins);
    maxTimeBin_SensorId->GetXaxis()->SetTitle("Sensor ID");
    maxTimeBin_SensorId->GetYaxis()->SetTitle("Max Time Bin Index");

    clusterSize_SensorId = new TH2F("clusterSize_SensorId", "Cluster size of hits vs. sensor ID", 144, 1, 145, 20, 0, 20);
    clusterSize_SensorId->GetXaxis()->SetTitle("Sensor ID");
    clusterSize_SensorId->GetYaxis()->SetTitle("Cluster Size of Hits");

    clusterSizeZ_SensorId = new TH2F("clusterSizeZ_SensorId", "Cluster size in Z of hits vs. sensor ID", 144, 1, 145, 20, 0, 20);
    clusterSizeZ_SensorId->GetXaxis()->SetTitle("Sensor ID");
    clusterSizeZ_SensorId->GetYaxis()->SetTitle("Cluster Size in Z of Hits");

    clusterSizeRPhi_SensorId = new TH2F("clusterSizeRPhi_SensorId", "Cluster size in r-#phi of hits vs. sensor ID", 144, 1, 145, 20, 0, 20);
    clusterSizeRPhi_SensorId->GetXaxis()->SetTitle("Sensor ID");
    clusterSizeRPhi_SensorId->GetYaxis()->SetTitle("Cluster Size in r-#phi of hits");

    hitMapOfIST = new TH2F("hitMapOfIST", "IST hit map in r-phi vs. Z", 1536, 0.5, 1536.5, 72, 0.5, 72.5);
    hitMapOfIST->GetXaxis()->SetTitle("Row in r-#phi");
    hitMapOfIST->GetYaxis()->SetTitle("Column in Z");

    hitMapOfAPV = new TH2F("hitMapOfAPV", "IST hit map in APV geometry Id vs. ladder", 36, 1, 37, 24, 1, 25);
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
            hitMap[iLadder*6+iSensor] = new TH2F(buffer, histtitle, 12, 0.5, 12.5, 64, 0.5, 64.5);
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
   istRawHit.charge = istRawHit.chargeErr = 0.;

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
         for(unsigned int ladderIdx=0; ladderIdx<kIstNumLadders; ladderIdx++ )	{
	    StIstLadderHitCollection* ladderHitCollection = istHitCollection->ladder(ladderIdx);
	    for(unsigned int sensorIdx=0; sensorIdx<ladderHitCollection->numberOfSensors() && sensorIdx<kIstNumSensorsPerLadder; sensorIdx++)	{
	       StIstSensorHitCollection* sensorHitCollection = ladderHitCollection->sensor(sensorIdx);
	       int sensorIdxTemp = 0;
               for(int idx=0; idx<(int)sensorHitCollection->hits().size(); idx++ ){
		  StIstHit* hit = sensorHitCollection->hits()[idx];
		  if(hit)	{
			const StThreeVectorF &P = hit->position();

			istHit.hitId 		= (int)hit->id();
			istHit.ladder		= (int)hit->getLadder();
			istHit.sensor		= (int)hit->getSensor();
			istHit.apv		= (int)hit->getApv();
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

			istHitTree->Fill();

			sensorIdxTemp = ((int)hit->getLadder()-1)*kIstNumSensorsPerLadder + (int)hit->getSensor();
			hitMap[sensorIdxTemp-1]->Fill(hit->getMeanColumn(), hit->getMeanRow());
			hitMapOfIST->Fill(((float)hit->getLadder()-1)*kIstNumRowsPerSensor+hit->getMeanRow(), ((float)hit->getSensor()-1)*kIstNumColumnsPerSensor+hit->getMeanColumn());
			hitMapOfAPV->Fill(((int)hit->getSensor()-1)*6 + (int)hit->getApv(), (int)hit->getLadder());
			hitGlobalXY->Fill((float)P.x(), (float)P.y());
			hitGlobalPhiZ->Fill((float)P.phi(), (float)P.z());

			hitCharge_SensorId->Fill(sensorIdxTemp, hit->charge());
			hitChargeErr_SensorId->Fill(sensorIdxTemp, hit->getChargeErr());
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

         for( unsigned char ladderIdx=0; ladderIdx<kIstNumLadders; ++ladderIdx ){
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

                   for( unsigned char timeBin = 0; timeBin < kIstNumTimeBins; ++timeBin )
		      rawHitCharge_TimeBin[timeBin]->Fill(rawHit->getGeoId(), rawHit->getCharge( timeBin ));

		   rawHitChargeErr->Fill(rawHit->getGeoId(), rawHit->getChargeErr( maxTimeBin ));
		   rawHitMap[sensorId-1]->Fill((float)rawHit->getColumn(), (float)rawHit->getRow());

		   istRawHit.channelId 	= (int)rawHit->getChannelId();
		   istRawHit.geoId      = (int)rawHit->getGeoId();
		   istRawHit.ladder	= (int)rawHit->getLadder();
		   istRawHit.sensor	= (int)rawHit->getSensor();
		   istRawHit.column	= (int)rawHit->getColumn();
		   istRawHit.row	= (int)rawHit->getRow();
		   istRawHit.maxTimeBin = (int)maxTimeBin;
		   istRawHit.charge	= (float)rawHit->getCharge(maxTimeBin);
		   istRawHit.chargeErr	= (float)rawHit->getChargeErr(maxTimeBin);
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
