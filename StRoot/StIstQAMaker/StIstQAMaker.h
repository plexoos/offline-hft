/***************************************************************************
*
* $Id: StIstQAMaker.h,v 1.8 2014/03/18 02:19:37 ypwang Exp $
*
* Author: Yaping Wang, October 2013
****************************************************************************
* Description: 
* Generate several histograms and trees for IST raw hits and hits.
****************************************************************************
*
* $Log: StIstQAMaker.h,v $
* Revision 1.8  2014/03/18 02:19:37  ypwang
* add two 2D histogram sets for max ADC time bin vs APV/Group electronics ID
*
* Revision 1.7  2014/03/13 22:10:12  smirnovd
* Move some constants from StIstUtil/StIstConsts.h to StEvent/StEnumerations.h to avoid external dependance of StEvent on StIstUtil
*
* Revision 1.6  2014/03/11 17:51:32  ypwang
* Histogram type changed from TH2F to TH2S, and reduce several histogram's bin number
*
* Revision 1.5  2014/03/05 16:54:14  ypwang
* minor update on istRawHit branch to include all time bins' charge and chargeErr informations
*
* Revision 1.3  2014/02/06 12:28:35  ypwang
* updating scripts
*
*
****************************************************************************
* StIstQAMaker.h,v 1.0
* Revision 1.0 2013/11/04 16:20:30 Yaping
* Initial version
****************************************************************************/

#ifndef StIstQAMaker_hh     
#define StIstQAMaker_hh

#include <string>

#include "StMaker.h"
#include "StIOMaker/StIOMaker.h"
#include "StEvent/StEnumerations.h"
#include "StIstUtil/StIstConsts.h"
#include "TString.h"
#include "TNtuple.h"
#include "TH2F.h"
#include "TProfile.h"

using namespace StIstConsts;

class StIOMaker;
class StEvent;
class StTrack;

class StIstQAMaker : public StMaker {
public:
  StIstQAMaker(const char *name="ist_Qa");     // constructor
  Int_t Init();
  Int_t  Make();                      		// invoked for every event
  Int_t  Finish();                    		// called once at the end
  virtual const char *GetCVS() const {
    static const char cvs[]="Tag $Name:  $ $Id: StIstQAMaker.h,v 1.8 2014/03/18 02:19:37 ypwang Exp $ built "__DATE__" "__TIME__ ; 
    return cvs;
  };

protected:
  TFile* myRootFile;
  TTree *istRawHitTree;
  TTree *istHitTree;

  struct rawHitInfo {
	int channelId, geoId, ladder, sensor, column, row, maxTimeBin, rdo, arm, apv, channel, idTruth, EventId;
	float charge[kIstNumTimeBins], chargeErr[kIstNumTimeBins];
  } istRawHit;
  
  struct hitInfo {
	int hitId, ladder, sensor, apv, idTruth, EventId, maxTimeBin, clusteringType, nRawHits, nRawHitsZ, nRawHitsRPhi;
	float meanColumn, meanRow, localX, localY, localZ, x, y, z, charge, chargeErr;
  } istHit;

  //position
  TH2S* rawHitMap[kIstNumSensors]; // raw hit column vs row per sensor
  TH2S* hitMap[kIstNumSensors];    // hit mean column vs mean row per sensor
  TH2S* hitMapOfIST;		   // hit map of IST layer in r-phi vs. Z area
  TH2S* hitMapOfAPV;		   // hit map in ladder vs. APV geometry Id
  TH2F* hitGlobalXY;		   // hit global x vs y
  TH2F* hitGlobalPhiZ;		   // hit global z
  //Charge
  TH2S* rawHitCharge_TimeBin[kIstNumTimeBins]; // Charge (ADC) vs channel ID over all time bins
  TH2S* rawHitChargeErr;  // RMS noise vs channel ID
  TH2S* rawHitMaxTimeBin_Section;   // Raw hit max ADC time bin vs section electronics ID [12*(ARC-1)+2*ARM+GROUP]
  TH2S* rawHitMaxTimeBin_APV;       // Raw hit max ADC time bin vs APV electronics ID [144*(ARC-1)+24*ARM+APV]
  TH2S* hitCharge_SensorId;         // Charge vs sensorID
  TH2S* hitChargeErr_SensorId;      // Charge uncertainty vs sensorID
  TH2S* maxTimeBin_SensorId;	    // hit max ADC time bin vs sensorID
  //hit or raw hit number
  TH2S* numOfRawHits_SensorId;      // number of raw hits vs sensor Id
  TH2S* numOfHits_SensorId;	    // number of hits vs sensor Id
  TProfile* numOfRawHits_EventId[kIstNumSensors];
  //cluster size
  TH2S* clusterSize_SensorId;	    // hit cluster size
  TH2S* clusterSizeZ_SensorId;	    // hit cluster size in Z direction
  TH2S* clusterSizeRPhi_SensorId;   // hit cluster size in R-Phi direction

  Int_t  mEventCounter;             // Event countter

private:
  ClassDef(StIstQAMaker,1);
};
#endif
