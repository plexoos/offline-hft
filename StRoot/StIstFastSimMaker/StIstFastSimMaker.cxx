/* $Id: StIstFastSimMaker.cxx,v 1.24 2015/02/18 16:32:16 smirnovd Exp $ */

#include "Stiostream.h"
#include "StIstFastSimMaker.h"
#include "StHit.h"
#include "StEventTypes.h"
#include "StEvent.h"
#include "StMcEvent.hh"
#include "StMcEvent/StMcHit.hh"
#include "StMcIstHit.hh"
#include "StRoot/StIstUtil/StIstConsts.h"
#include "StIstHit.h"
#include "StIstHitCollection.h"
#include "StIstDbMaker/StIstDb.h"
#include "StMcEvent/StMcIstHit.hh"
#include "StMcEvent/StMcIstHitCollection.hh"
#include "StMcEventTypes.hh"

#include <stdio.h>
#include "StThreeVectorF.hh"
#include "StThreeVectorD.hh"
#include <vector>
#include <exception>
#include <stdexcept>
#include "tables/St_g2t_ist_hit_Table.h"
#include "tables/St_HitError_Table.h"
#include "TGeoManager.h"
#include "TGeoMatrix.h"
#include "TDataSet.h"
#include "StarClassLibrary/StRandom.hh"

ClassImp(StIstFastSimMaker)

StIstFastSimMaker::StIstFastSimMaker( const Char_t *name ) : StMaker(name), mIstRot(NULL), mIstDb(NULL), mBuildIdealGeom(kTRUE),
   mRandom(new StRandom()), mSmear(kTRUE)
{
   Int_t seed = time(NULL);
   mRandom->setSeed(seed);
}

//____________________________________________________________
StIstFastSimMaker::~StIstFastSimMaker(){ 
   if (mRandom) delete mRandom; 
}

//____________________________________________________________
void StIstFastSimMaker::Clear(Option_t *) {
   StMaker::Clear();
}

//____________________________________________________________
Int_t StIstFastSimMaker::Init() {
   LOG_INFO << "StIstFastSimMaker::Init()" << endm;

   return kStOk;
}

//____________________________________________________________
Int_t StIstFastSimMaker::InitRun(int runNo)
{
   LOG_INFO << "StIstFastSimMaker::InitRun" << endm;

   TDataSet *calibDataSet = GetDataBase("Calibrations/tracker");
   St_HitError *istTableSet = (St_HitError *) calibDataSet->Find("ist1HitError");
   HitError_st *istHitError = istTableSet->GetTable();
   mResXIst1 = sqrt(istHitError->coeff[0]);
   mResZIst1 = sqrt(istHitError->coeff[3]);

   TObjectSet *istDbDataSet = (TObjectSet *)GetDataSet("ist_db");

   if (istDbDataSet) {
      mIstDb = (StIstDb *)istDbDataSet->GetObject();
   }
   else {
      LOG_ERROR << "InitRun : no mIstDb" << endm;
      return kStErr;
   }

   // geometry Db tables
   mIstRot = mIstDb->getRotations();

   return kStOk;
}


/**
 * Retrieves GEANT hit information from StMcEvent then fills the StEvent's
 * StIstHitCollection with (possibly smeared) hit positions in either ideal or
 * misaligned geometry. Thus created StIstHitCollection is used in tracking.
 */
Int_t StIstFastSimMaker::Make()
{
   LOG_INFO << "StIstFastSimMaker::Make()" << endm;
   if (!mIstRot) {
      LOG_FATAL << "Make(): mIstRot is not initialized" << endm;
      return kStFatal;
   }

   // Get the input data structures from StEvent and StMcEvent
   StEvent *rcEvent =  (StEvent *) GetInputDS("StEvent");

   if (! rcEvent) {LOG_WARN << "Make() - StEvent not found" << endl; return kStWarn;}

   StMcEvent *mcEvent = (StMcEvent *) GetInputDS("StMcEvent");

   if (! mcEvent) {LOG_WARN << "Make() - StMcEvent not found" << endl; return kStWarn;}

   if ( mBuildIdealGeom && !gGeoManager ) {
      GetDataBase("VmcGeometry");
   }

   // Store hits into Ist Hit Collection
   StIstHitCollection *istHitCollection = 0;

   istHitCollection = rcEvent->istHitCollection();

   if (!istHitCollection) {
      gMessMgr->Info() << "StIstFastSimMaker -E- no istHitCollection!\n";
      istHitCollection = new StIstHitCollection;
      rcEvent->setIstHitCollection(istHitCollection);
      LOG_WARN << "Make() - Added new StIstHitCollection to StEvent" << endm;
   }

   StThreeVectorF mHitError(0., 0., 0.);

   //Get MC Ist hit collection. This contains all ist hits.
   const StMcIstHitCollection *istMcHitCol = mcEvent->istHitCollection();

   if (!istMcHitCol) {
      LOG_FATAL << "No Ist MC hits found." << endm;
      return kStFatal;
   }

   //new simulator for new 1-layer design
   Float_t smearedX = 0., smearedZ = 0.;

   LOG_INFO << "ist MC hit collection found" << endm;
   Int_t nIsthits = istMcHitCol->numberOfHits();

   if (istMcHitCol->layer(0)) {
      for (UInt_t kk = 0; kk < istMcHitCol->layer(0)->hits().size(); kk++) {
         StMcHit *mcH = istMcHitCol->layer(0)->hits()[kk];
         StMcIstHit *mcI = dynamic_cast<StMcIstHit *>(mcH);

         Int_t matIst = 1000 + (mcI->ladder() - 1) * 6 + mcI->wafer();
         cout << " matIst : " << matIst << endl;

         TGeoHMatrix *combI = NULL;
         //Access VMC geometry once no IST geometry Db tables available or using ideal geoemtry is set
         if( (!mIstRot || mBuildIdealGeom) && gGeoManager) {
  	  TString Path("HALL_1/CAVE_1/TpcRefSys_1/IDSM_1/IBMO_1");
  	  Path += Form("/IBAM_%d/IBLM_%d/IBSS_1", mcI->ladder(), mcI->wafer());
  	  gGeoManager->RestoreMasterVolume();
  	  gGeoManager->CdTop();
  	  gGeoManager->cd(Path);
  	  combI = (TGeoHMatrix *)gGeoManager->GetCurrentMatrix();
         }
         else { //using mis-aligned gemetry from IST geometry DB tables
  	  combI = (TGeoHMatrix *)mIstRot->FindObject(Form("R%04i", matIst));  
         }

         if (combI) {
            cout << " geometry matrix :" << endl;
            combI->Print();
         }

         //YPWANG: McIstHit stored local position
         Double_t globalIstHitPos[3] = {mcI->position().x(), mcI->position().y(), mcI->position().z()};
         Double_t localIstHitPos[3] = {mcI->position().x(), mcI->position().y(), mcI->position().z()};
         LOG_DEBUG << "Before Smearing" << endm;
         LOG_DEBUG << "localIstHitPos = " << localIstHitPos[0] << " " << localIstHitPos[1] << " " << localIstHitPos[2] << endm;

         if (mSmear) { // smearing on
            LOG_DEBUG << "Smearing start... " << endm;
            smearedX = distortHit(localIstHitPos[0], mResXIst1, kIstSensorActiveSizeRPhi / 2.0);
            smearedZ = distortHit(localIstHitPos[2], mResZIst1, kIstSensorActiveSizeZ / 2.0);

            localIstHitPos[0] = smearedX;
            localIstHitPos[2] = smearedZ;
            LOG_DEBUG << Form("Smearing done...") << endm;
         }
         else { //smearing off
            LOG_DEBUG << "No smearing, but discreting ... " << endm;
            //discrete hit local position (2D structure of IST sensor pads)
            Float_t rPhiPos   = kIstSensorActiveSizeRPhi / 2.0 - localIstHitPos[0];
            Float_t zPos      = localIstHitPos[2] + kIstSensorActiveSizeZ / 2.0;
            Short_t meanColumn  = (Short_t)floor( zPos / kIstPadPitchColumn ) + 1;
            Short_t meanRow     = (Short_t)floor( rPhiPos / kIstPadPitchRow ) + 1;
            rPhiPos = (meanRow - 1) * kIstPadPitchRow + 0.5 * kIstPadPitchRow; //unit: cm
            zPos    = (meanColumn - 1) * kIstPadPitchColumn + 0.5 * kIstPadPitchColumn; //unit: cm
            localIstHitPos[0] = kIstSensorActiveSizeRPhi / 2.0 - rPhiPos;
            localIstHitPos[2] = zPos - kIstSensorActiveSizeZ / 2.0;
         }

         //YPWANG: do local-->global transform with geometry table
         combI->LocalToMaster(localIstHitPos, globalIstHitPos);
         StThreeVectorF gistpos(globalIstHitPos);
         LOG_DEBUG << "smeared globalIstHitPos = " << globalIstHitPos[0] << " " << globalIstHitPos[1] << " " << globalIstHitPos[2] << endm;
         LOG_DEBUG << "smeared localIstHitPos = " << localIstHitPos[0] << " " << localIstHitPos[1] << " " << localIstHitPos[2] << endm;
         LOG_DEBUG << "hit position(ladder/sensor): " << mcI->ladder() << " " << mcI->wafer() << endm;

         UInt_t hw =  ( mcI->ladder() - 1 ) * 6 + mcI->wafer();
         StIstHit *tempHit = new StIstHit(gistpos, mHitError, hw, mcI->dE(), 0);
         tempHit->setDetectorId(kIstId);
         tempHit->setId(mcI->key());
         mcI->parentTrack()? tempHit->setIdTruth(mcI->parentTrack()->key(), 100): tempHit->setIdTruth(-999);
         tempHit->setLocalPosition(localIstHitPos[0], localIstHitPos[1], localIstHitPos[2]);
         istHitCollection->addHit(tempHit);

         LOG_DEBUG << "id() : " << tempHit->id()  << " idTruth: " << tempHit->idTruth() << endm;
         LOG_DEBUG << "from StMcIstHit x= " << mcI->position().x()   << "; y= " << mcI->position().y()   << "; z= " << mcI->position().z() << endm;
         LOG_DEBUG << "istHit location x= " << tempHit->position().x() << "; y= " << tempHit->position().y() << "; z= " << tempHit->position().z() << "; energy/charge = " << tempHit->charge() << endm;
      }//MC hits loop over
   }//end layer=0 cut

   LOG_DEBUG << "StIstFastSimMaker::Make() -I- Loaded " << nIsthits << " ist hits. \n";

   return kStOK;
}


/**
 * Calculates and returns new value for the local coordinate x by smearing it
 * acccording to a normal distribution N(mean, sigma) = N(x, res). The returned
 * value is constrained to be within the characteristic dimension detLength
 * provided by the user.
 */
Double_t StIstFastSimMaker::distortHit(const Double_t x, const Double_t res, const Double_t detLength) const
{
   // Do not smear x when it is outside the physical limits. Issue a warning instead
   if (fabs(x) > detLength) {
      LOG_WARN << "distortHit() - Generated hit is outside detector sensor plane" << endm;
      return x;
   }

   Double_t smeared_x;

   do {
      smeared_x = mRandom->gauss(x, res);
   } while ( fabs(smeared_x) > detLength);

   return smeared_x;
}
