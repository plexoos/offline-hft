#include "St_base/Stypes.h"
#include "St_base/StMessMgr.h"
#include "StEvent/StEvent.h"
#include "StEvent/StTrack.h"
#include "StEvent/StTrackNode.h"
#include "StEvent/StTrackGeometry.h"
#include "StarClassLibrary/SystemOfUnits.h"
#include "StPrimaryVertex.h"
#include "StEventInfo.h"
#include "StEventSummary.h"
#include "StPrimaryTrack.h"
#include "StEvent/StGlobalTrack.h"
#include "StEvent/StTrackDetectorInfo.h"
#include "StTpcDedxPidAlgorithm.h"
#include "StPionPlus.hh"
#include "StTpcHit.h"
#include "StPxlHit.h"
#include "StIstHit.h"
#include "StSsdHit.h"
#include "StEvent/StEnumerations.h"
#include "StEvent/StPxlHitCollection.h"
#include "StEvent/StIstHitCollection.h"
#include "StEvent/StIstLadderHitCollection.h"
#include "StEvent/StIstSensorHitCollection.h"
#include "StEvent/StSsdHitCollection.h"
#include "StEvent/StSsdLadderHitCollection.h"
#include "StEvent/StSsdWaferHitCollection.h"
#include "StPxlDbMaker/StPxlDb.h"
#include "StIstDbMaker/StIstDb.h"
#include "StBTofCollection.h"
#include "StBTofHeader.h"

#include "StHftPool/EventT/EventT.h"

#include "TRVector.h"
#include "TRSymMatrix.h"


ClassImp(EventT);


EventT::EventT() : TObject(),
   fNPTracks(0),
   fNvertex(0), fNtrack(0), fNhit(0), fNmatchhit(0),
   fVertices(new TClonesArray("VertexT", 1000)),
   fTracks(new TClonesArray("TrackT", 1000)),
   fHits(new TClonesArray("HitT", 1000)),
   fMatchHits(new TClonesArray("HitMatchT", 1000)),
   fPxlDb(nullptr), fIstDb(nullptr)
{
}


EventT::~EventT()
{
   Clear();

   delete fVertices;  fVertices = nullptr;
   delete fTracks;    fTracks = nullptr;
   delete fHits;      fHits = nullptr;
   delete fMatchHits; fMatchHits = nullptr;
}


Int_t EventT::Build(StEvent *stEvent, UInt_t minNoHits, Double_t pCut)
{
   if (!stEvent) {
      LOG_ERROR << "Cannot build EventT: Missing StEvent object" << endm;
      return kStErr;
   }

   if (!fIstDb || !fPxlDb) {
      LOG_ERROR << "Cannot build EventT: Missing PXL or IST data from database" << endm;
      return kStErr;
   }

   const THashList *istRot = fIstDb->getRotations();

   UInt_t NprimVtx = stEvent->numberOfPrimaryVertices();
   StPrimaryVertex *pVertex = 0;
   StThreeVectorF xyzP(-999, -999, -999);
   StMatrixF vCM(3, 3);

   static const Int_t nFitPointCutForGoodTrackT = 15;

   if (NprimVtx) {
      Int_t ibest = -1;
      Int_t nBestTracks = -1;
      Int_t nGoodTpcTracks;

      for (UInt_t ipr = 0; ipr < NprimVtx ; ipr++) {
         pVertex = stEvent->primaryVertex(ipr);

         if (! pVertex) continue;

         const StThreeVectorF &vtxPos = pVertex->position();
         Double32_t vtxXYZ[] = {vtxPos.x(), vtxPos.y(), vtxPos.z()};
         VertexT *vtx = AddVertexT();
         vtx->SetVertex(vtxXYZ);

         UInt_t nDaughters = pVertex->numberOfDaughters();
         vtx->SetNtracks(nDaughters);
         LOG_DEBUG << "Number of daughters: " << nDaughters << endl;
         nGoodTpcTracks = 0;

         for (UInt_t i = 0; i < nDaughters; i++) {
            StTrack *pTrackT = pVertex->daughter(i);

            if ( pTrackT->fitTraits().numberOfFitPoints(kTpcId) >= nFitPointCutForGoodTrackT) nGoodTpcTracks++;
         }

         if (nBestTracks < nGoodTpcTracks) {nBestTracks = nGoodTpcTracks; ibest = ipr;}
      }

      LOG_DEBUG << "Number of tracks: "  << nGoodTpcTracks << endl;

      if (ibest >= 0) {
         pVertex = stEvent->primaryVertex(ibest);
         xyzP = pVertex->position();
         vCM = pVertex->covariantMatrix();
         fNPTracks = pVertex->numberOfDaughters();
      }
   }

   fVertex[0] = xyzP.x();
   fVertex[1] = xyzP.y();
   fVertex[2] = xyzP.z();
   fCovariantMatrix[0] = vCM(1, 1); // left triangular
   fCovariantMatrix[1] = vCM(1, 2);
   fCovariantMatrix[2] = vCM(2, 2);
   fCovariantMatrix[3] = vCM(1, 3);
   fCovariantMatrix[4] = vCM(2, 3);
   fCovariantMatrix[5] = vCM(3, 3);

   StBTofCollection *btofColl = stEvent->btofCollection();
   fVzVpd = -9999.;
   fNVpdHits = 0;

   if (btofColl) {
      StBTofHeader *tofHeader = btofColl->tofHeader();

      if (tofHeader) {
         fVzVpd = tofHeader->vpdVz();
         int nE = tofHeader->numberOfVpdHits(east);
         int nW = tofHeader->numberOfVpdHits(west);
         fNVpdHits = nE * 100 + nW;
      }
   }


   StEventInfo *info = stEvent->info();
   Int_t ev = 0, run = 0, time = 0;

   if (info) {
      ev   = info->id();
      run  = info->runId();
      time = info->time();
   }

   LOG_DEBUG << "Event Infos: ev/run/time: " << ev << "/" << run << "/" << time << endl;
   StEventSummary *summary = stEvent->summary();
   Double32_t field = 0;

   if (summary) field = summary->magneticField();

   SetHeader(ev, run, time, field);
   // Create and Fill the TrackT objects
   //LOG_DEBUG <<" # of daughter tracks : "<< fNPTracks << endl;

   UInt_t nT = 0;

   static const double IST_Width_X = 3.8016; // cm
   static const double IST_Width_Z = 7.53;   // cm
   static const double PXL_Width_X = 1.9210; // cm
   static const double PXL_Width_Z = 1.9872; // cm

   // Load hits - PXL
   StPxlHitCollection *stPxlHitCollection = stEvent->pxlHitCollection();

   if(!stPxlHitCollection) {
      LOG_WARN << "No valid PXL hit collection found in StEvent" << endl;
   }

   if (stPxlHitCollection) {
      LOG_DEBUG << "Number of PXL hits: " << stPxlHitCollection->numberOfHits() << endl;
      UInt_t numberOfSectors = stPxlHitCollection->numberOfSectors();

      for (UInt_t i = 0; i < numberOfSectors; i++) {
         StPxlSectorHitCollection *pxlSectorHitCollection = stPxlHitCollection->sector(i);

         if (!pxlSectorHitCollection) continue;

         UInt_t numberOfLadders = pxlSectorHitCollection->numberOfLadders();

         for (UInt_t j = 0; j < numberOfLadders; j++) {
            StPxlLadderHitCollection *pxlLadderHitCollection = pxlSectorHitCollection->ladder(j);

            if (!pxlLadderHitCollection) continue;

            UInt_t numberOfSensors = pxlLadderHitCollection->numberOfSensors();

            for (UInt_t l = 0; l < numberOfSensors; l++) {
               StPxlSensorHitCollection *pxlSensorHitCollection = pxlLadderHitCollection->sensor(l);

               if (!pxlSectorHitCollection) continue;

               StSPtrVecPxlHit &vec = pxlSensorHitCollection->hits();

               if (vec.size() <= 0) continue;

               LOG_DEBUG << "curr i/j/l (starting from 0) : " << i << "/" << j << "/" << l << " ==> StiPixelHitLoader - collection size: " << vec.size() << endl;

               UInt_t NoHits = vec.size();

               for (UInt_t ll = 0; ll < NoHits; ll++) {
                  StPxlHit *hit = vec[ll];

                  if (!hit) continue;

                  //hit->Print("");
                  //LOG_DEBUG <<" layer/sector/ladder/sensor/idTruth/detectorId : " << (int)hit->layer()<<"/"<<(int)hit->sector() <<"/"<< (int)hit->ladder() <<"/"<< (int)hit->sensor() <<"/"<<(int)hit->idTruth()<<"/"<<(int)hit->detector() <<endl;
                  int matId = ((int)hit->sector() - 1) * 40 + (int)(hit->ladder() - 1) * 10 + (int)(hit->sensor());
                  //LOG_DEBUG <<" -->matPix : " << matId << endl;
                  Double_t globalPixHitPos[3] = {hit->position().x(), hit->position().y(), hit->position().z()};
                  Double_t localPixHitPos[3]  = {hit->localPosition(0), hit->localPosition(1), hit->localPosition(2)};

                  //LOG_DEBUG << "globalPixHitPos = " << globalPixHitPos[0] << " " << globalPixHitPos[1] << " " << globalPixHitPos[2] << endl;
                  //LOG_DEBUG<< "localPixHitPos = " << localPixHitPos[0] << " " << localPixHitPos[1] << " " << localPixHitPos[2] << endl;

                  HitT *h = AddHitT();
                  h->SetId(matId);
                  h->Set(globalPixHitPos, localPixHitPos);
                  h->SetNRawHits((UInt_t)hit->nRawHits());
               }  // end loop ll hits
            } // end loop l sensor
         } // end loop j ladder
      } // end loop i (sector
   } // end if

   // load hits - IST
   StIstHitCollection *stIstHitCollection = stEvent->istHitCollection();

   if (stIstHitCollection) {
      LOG_DEBUG << "Number of IST hits: " << stIstHitCollection->numberOfHits() << endl;
      UInt_t numberOfLadders = kIstNumLadders;

      for (UInt_t i = 0; i < numberOfLadders; i++) {
         StIstLadderHitCollection *istLadderHitCollection = stIstHitCollection->ladder(i);

         if (!istLadderHitCollection) continue;

         for (int j = 0; j < kIstNumSensorsPerLadder; j++) {
            StIstSensorHitCollection *istSensorHitCollection = istLadderHitCollection->sensor(j);

            if (!istSensorHitCollection) continue;

            StSPtrVecIstHit &vec = istSensorHitCollection->hits();

            for (UInt_t l = 0; l < vec.size(); l++) {
               StIstHit *hit = vec[l];

               if (!hit) continue;

               int ladder = hit->getLadder();
               int sensor = hit->getSensor();
               int matId = (ladder - 1) * 6 + sensor + 1000;
               Double_t globalIstHitPos[3] = {hit->position().x(), hit->position().y(), hit->position().z()};
               Double_t localIstHitPos[3]  = {hit->localPosition(0), hit->localPosition(1), hit->localPosition(2)};

               //LOG_DEBUG << "globalIstHitPos = " << globalIstHitPos[0] << " " << globalIstHitPos[1] << " " << globalIstHitPos[2] << endl;
               //LOG_DEBUG<< "localIstHitPos = " << localIstHitPos[0] << " " << localIstHitPos[1] << " " << localIstHitPos[2] << endl;

               HitT *h = AddHitT();
               h->SetId(matId);
               h->Set(globalIstHitPos, localIstHitPos);
               h->SetNRawHits((UInt_t)hit->getNRawHits());
            } // end loop l hits
         } // end j sensor
      } // end i ladder
   } // end if

   // load hits - SSD
   StSsdHitCollection *stSsdHitCollection = stEvent->ssdHitCollection();

   if (stSsdHitCollection) {
      LOG_DEBUG << "Number of SSD hits: " << stSsdHitCollection->numberOfHits() << endl;
      UInt_t numberOfLadders = stSsdHitCollection->numberOfLadders();

      for (UInt_t i = 0; i < numberOfLadders; i++) {
         StSsdLadderHitCollection *ssdLadderHitCollection = stSsdHitCollection->ladder(i);

         if (!ssdLadderHitCollection) continue;

         UInt_t numberOfSensors = ssdLadderHitCollection->numberOfWafers();

         for (UInt_t j = 0; j < numberOfSensors; j++) {
            StSsdWaferHitCollection *ssdWaferHitCollection = ssdLadderHitCollection->wafer(j);

            if (!ssdWaferHitCollection) continue;

            StSPtrVecSsdHit &vec = ssdWaferHitCollection->hits();

            for (UInt_t l = 0; l < vec.size(); l++) {
               StSsdHit *hit = vec[l];

               if (!hit) continue;

               int ladder = hit->ladder();
               int wafer = hit->wafer();
               int matId = (ladder - 1) * 16 + wafer + 2000;
               Double_t globalSsdHitPos[3] = {hit->position().x(), hit->position().y(), hit->position().z()};
               Double_t localSsdHitPos[3]  = {hit->localPosition(0), 0, hit->localPosition(1)};

               //LOG_DEBUG << "globalSsdHitPos = " << globalSsdHitPos[0] << " " << globalSsdHitPos[1] << " " << globalSsdHitPos[2] << endl;
               //LOG_DEBUG << "localSsdHitPos = " << localSsdHitPos[0] << " " << localSsdHitPos[1] << " " << localSsdHitPos[2] << endl;

               HitT *h = AddHitT();
               h->SetId(matId);
               h->Set(globalSsdHitPos, localSsdHitPos);
            } // end loop l hits
         } // end j wafer
      } // end i ladder
   } // end if

   UInt_t onPXL2 = 0;
   UInt_t onPXL1 = 0;
   UInt_t onIST = 0;

   StSPtrVecTrackNode &nodes = stEvent->trackNodes();
   LOG_DEBUG << " TrackNode size = " << nodes.size() << endl;

   for (size_t trkIndx = 0; trkIndx < nodes.size(); trkIndx++)
   {
      LOG_DEBUG << " current track # :" << trkIndx+1 << "/" << nodes.size() << endl;

      StGlobalTrack *stGlobalTrack = dynamic_cast<StGlobalTrack *>(nodes[trkIndx]->track(global));

      if (!stGlobalTrack) continue;

      if (stGlobalTrack->fitTraits().numberOfFitPoints(kTpcId) < nFitPointCutForGoodTrackT) continue;

      StDcaGeometry *dcaGeometry = stGlobalTrack->dcaGeometry();

      if (!dcaGeometry) {
         LOG_WARN << "No valid StDcaGeometry object found in this global track. Skipping to next track... " << endl;
         continue;
      }

      StPrimaryTrack *pTrackT = dynamic_cast<StPrimaryTrack *>(nodes[trkIndx]->track(primary));

      if (pTrackT && pTrackT->vertex() != pVertex) pTrackT = 0;

      StTrackDetectorInfo *dInfo = stGlobalTrack->detectorInfo();

      if ( !dInfo ) continue;

      TrackT *track = AddTrackT();

      int ii = 0;
      StPtrVecHit tpcHits = dInfo->hits(kTpcId);

      for (size_t ih = 0; ih < tpcHits.size(); ih++)
      {
         StTpcHit *aHit = dynamic_cast<StTpcHit *>(tpcHits[ih]);

         if (!aHit) continue;

         Double_t xyz[] = {aHit->position().x(), aHit->position().y(), aHit->position().z()};
         track->SetTpcHit(ii, xyz);
         ii++;
      }

      ii = 0;
      UInt_t npattern_ssd[2] = {0, 0};
      StPtrVecHit ssdHits = dInfo->hits(kSsdId);

      for (size_t ih = 0; ih < ssdHits.size(); ih++) {
         StSsdHit *aHit = dynamic_cast<StSsdHit *>(ssdHits[ih]);

         if (!aHit) continue;

         npattern_ssd[ii] = aHit->wafer() + (aHit->ladder() - 1) * 16;
         Double_t xyz[] = {aHit->position().x(), aHit->position().y(), aHit->position().z()};
         track->SetSsdHit(ii, xyz);
         ii++;
      }

      UInt_t nssdpattern = npattern_ssd[0] + npattern_ssd[1] * 1000;
      track->SetSsdHitPattern(nssdpattern);

      ii = 0;
      UInt_t npattern_ist[2] = {0, 0};
      StPtrVecHit istHits = dInfo->hits(kIstId);

      for (size_t ih = 0; ih < istHits.size(); ih++) {
         StIstHit *aHit = dynamic_cast<StIstHit *>(istHits[ih]);

         if (!aHit) continue;

         npattern_ist[ii] = aHit->getSensor() + (aHit->getLadder() - 1) * 6;
         Double_t xyz[] = {aHit->position().x(), aHit->position().y(), aHit->position().z()};
         track->SetIstHit(ii, xyz);
         ii++;
      }

      UInt_t nistpattern = npattern_ist[0] + npattern_ist[1] * 1000;
      track->SetIstHitPattern(nistpattern);

      StPtrVecHit pxlHits = dInfo->hits(kPxlId);

      if (pxlHits.size() > 0) LOG_DEBUG << "Number of PXL hits on this track " << pxlHits.size() << endl;

      UInt_t npts_pxl1 = 0;  // first layer
      UInt_t npts_pxl2 = 0;  // second layer
      UInt_t npattern_pxl[3] = {0, 0, 0};

      for (size_t ih = 0; ih < pxlHits.size(); ih++) {
         StPxlHit *aHit = dynamic_cast<StPxlHit *>(pxlHits[ih]);

         if (!aHit) continue;

         Double_t xyz[] = {aHit->position().x(), aHit->position().y(), aHit->position().z()};
         track->SetPxlHit(ih, xyz);

         UInt_t sensorId = aHit->sensor() + (aHit->ladder() - 1) * 10 + (aHit->sector() - 1) * 40;

         if (aHit->ladder() == 1) {
            npts_pxl1++;
            npattern_pxl[0] = sensorId;
         }

         if (aHit->ladder() >= 2 && aHit->ladder() <= 4) {
            npts_pxl2++;
            npattern_pxl[npts_pxl2] = sensorId;
         }
      }

      UInt_t npxlpattern = npattern_pxl[0] + npattern_pxl[1] * 1000 + npattern_pxl[2] * 1000000;
      track->SetPxlHitPattern(npxlpattern);

      UInt_t npoints = dInfo->numberOfPoints(kTpcId) +
                       dInfo->numberOfPoints(kSsdId) * 100 +
                       dInfo->numberOfPoints(kIstId) * 1000 +
                       npts_pxl2 * 10000 +
                       npts_pxl1 * 100000;

      LOG_DEBUG << " NPoints = " << npoints << endl;

      LOG_DEBUG << " Number of Possible Points: tpc/ssd/ist/PXL = " << stGlobalTrack->numberOfPossiblePoints(kTpcId)
           << "/" << stGlobalTrack->numberOfPossiblePoints(kSsdId) << "/" << stGlobalTrack->numberOfPossiblePoints(kIstId)
           << "/" << stGlobalTrack->numberOfPossiblePoints(kPxlId) << endl;
      LOG_DEBUG << " Number of Points: tpc/ssd/ist/PXL = " << dInfo->numberOfPoints(kTpcId) << "/"
           << dInfo->numberOfPoints(kSsdId) << "/" << dInfo->numberOfPoints(kIstId) << "/"
           << dInfo->numberOfPoints(kPxlId) << endl;
      LOG_DEBUG << " Number of Points vector size: tpc/ssd/ist/PXL = " << tpcHits.size() << "/"
           << ssdHits.size() << "/" << istHits.size() << "/" << pxlHits.size() << endl;
      //    LOG_DEBUG << " Hit Patter: ssd/ist/PXL = " << nssdpattern << "/" << nistpattern << "/" << npxlpattern << endl;

      StPhysicalHelixD dcaG_helix = dcaGeometry->helix();
      StThreeVectorF dcaG_origin = dcaGeometry->origin();
      StThreeVectorF dcaG_mom = dcaGeometry->momentum();
      int dcaG_q = dcaGeometry->charge();

      StPhysicalHelixD gHelix = stGlobalTrack->geometry()->helix();
      StThreeVectorD origin = gHelix.origin();
      StThreeVectorD gmom = gHelix.momentum(field * kilogauss);

      static StTpcDedxPidAlgorithm PidAlgorithm;
      static StPionPlus *Pion = StPionPlus::instance();
      const StParticleDefinition *pd = stGlobalTrack->pidTraits(PidAlgorithm);
      double nsigmaPi = -999.;

      if (pd) {
         nsigmaPi = PidAlgorithm.numberOfSigma(Pion);
      }

      double dca2d = dcaG_helix.curvatureSignedDistance(xyzP.x(), xyzP.y());
      double dca3d = dcaG_helix.curvatureSignedDistance(xyzP);

      track->SetPxPyPz(gmom.x(), gmom.y(), gmom.z());
      track->SetDcaPxPyPz(dcaG_mom.x(), dcaG_mom.y(), dcaG_mom.z());

      if (pTrackT) {
         StThreeVectorD pmom = pTrackT->geometry()->momentum();
         track->SetPPxPyPz(pmom.x(), pmom.y(), pmom.z());
      }
      else {
         track->SetPPxPyPz(0., 0., 0.);
      }

      track->SetOxOyOz(origin.x(), origin.y(), origin.z());
      track->SetDcaOxOyOz(dcaG_origin.x(), dcaG_origin.y(), dcaG_origin.z());
      track->SetNpoint(npoints, dcaG_q);
      track->SetNsigmaPi(nsigmaPi);
      track->SetDca2D(dca2d);
      track->SetDca3D(dca3d);

      StThreeVectorF firstP = dInfo->firstPoint();

      // do Projections
      // first IST
      for (int i_ladder = 0; i_ladder < 24; i_ladder++) {
         for (int i_sensor = 0; i_sensor < 6; i_sensor++) {
            UInt_t id = 1000 + i_ladder * 6 + i_sensor + 1;
            TGeoHMatrix *comb = (TGeoHMatrix *)istRot->FindObject(Form("R%04i", id));

            Double_t *rot = comb->GetRotationMatrix();
            Double_t *tra = comb->GetTranslation();
            const StThreeVectorD normal(rot[1], rot[4], rot[7]);
            const StThreeVectorD middle(tra);
            Double_t sh = dcaG_helix.pathLength(middle, normal);

            if (sh <= 0 || sh > 1e3) continue; // dcaG geometry, projection pathLength should be positive

            StThreeVectorD xyzG = dcaG_helix.at(sh);
            Double_t xyzGPred[3] = {xyzG.x(), xyzG.y(), xyzG.z()};
            Double_t uvPred[3];
            comb->MasterToLocal(xyzGPred, uvPred);

            if (TMath::Abs(uvPred[0]) > IST_Width_X / 2. ) continue;
            if (TMath::Abs(uvPred[2]) > IST_Width_Z / 2. ) continue;

            onIST++;

            Double_t dirGPred[3] = {dcaG_helix.cx(sh), dcaG_helix.cy(sh), dcaG_helix.cz(sh)};
            Double_t dxyzL[3];
            comb->MasterToLocalVect(dirGPred, dxyzL);
            Double_t tuvPred[2] = {dxyzL[0] / dxyzL[1], dxyzL[2] / dxyzL[1]};

            if (!stEvent->istHitCollection()) continue;
            if (!stEvent->istHitCollection()->ladder(i_ladder)) continue;
            if (!stEvent->istHitCollection()->ladder(i_ladder)->sensor(i_sensor)) continue;

            StSPtrVecIstHit &vec = stEvent->istHitCollection()->ladder(i_ladder)->sensor(i_sensor)->hits();

            if (vec.size() <= 0) continue;

            for (size_t ih = 0; ih < vec.size(); ih++) {
               StIstHit *hit = (StIstHit *)vec[ih];

               if (!hit) continue;

               Double_t global[3] = {hit->position().x(), hit->position().y(), hit->position().z()};
               Double_t local[3]  = {hit->localPosition(0), hit->localPosition(1), hit->localPosition(2)};

               LOG_DEBUG << (*hit) << endl;

               HitMatchT *h = AddHitMatchT();
               h->Set(global, local);
               h->SetPred(xyzGPred, uvPred);
               h->SettuvPred(tuvPred[0], tuvPred[1]);
               h->SetDetId(id);
               h->SetNRawHits((UInt_t)hit->getNRawHits());
               h->SetIndex2Track(nT);
               h->SetIndex2Hit(0);  // need to map out later
               h->SetTrackMom(dcaG_mom.perp(), dcaG_mom.pseudoRapidity(), dcaG_mom.phi());
               h->SetTrackOrigin(dcaG_origin.x(), dcaG_origin.y(), dcaG_origin.z());
               h->SetTrackNpoint(npoints * dcaG_q);
               h->SetTrackFirstPointR(firstP.perp());
               h->SetTrackFirstPointZ(firstP.z());
            }
         }
      }


      // do projection to PXL
      for (int i_sector = 0; i_sector < 10; i_sector++) {
         for (int i_ladder = 0; i_ladder < 4; i_ladder++) {
            for (int i_sensor = 0; i_sensor < 10; i_sensor++)
            {
               UInt_t id = i_sector * 40 + i_ladder * 10 + i_sensor + 1;
               TGeoHMatrix *comb = (TGeoHMatrix*) fPxlDb->geoHMatrixSensorOnGlobal(i_sector + 1, i_ladder + 1, i_sensor + 1);

               Double_t *rot = comb->GetRotationMatrix();
               Double_t *tra = comb->GetTranslation();
               const StThreeVectorD normal(rot[1], rot[4], rot[7]);
               const StThreeVectorD middle(tra);
               Double_t sh = dcaG_helix.pathLength(middle, normal);

               if (sh <= 0 || sh > 1e3) continue; // dcaG geometry, projection pathLength should be positive

               StThreeVectorD xyzG = dcaG_helix.at(sh);
               Double_t xyzGPred[3] = {xyzG.x(), xyzG.y(), xyzG.z()};
               Double_t uvPred[3];
               comb->MasterToLocal(xyzGPred, uvPred);

               if (TMath::Abs(uvPred[0]) > PXL_Width_X / 2. ) continue;
               if (TMath::Abs(uvPred[2]) > PXL_Width_Z / 2. ) continue;

               if (i_ladder == 0) onPXL1++;
               else onPXL2++;

               Double_t dirGPred[3] = {dcaG_helix.cx(sh), dcaG_helix.cy(sh), dcaG_helix.cz(sh)};
               Double_t dxyzL[3];
               comb->MasterToLocalVect(dirGPred, dxyzL);
               Double_t tuvPred[2] = {dxyzL[0] / dxyzL[1], dxyzL[2] / dxyzL[1]};

               if (!stEvent->pxlHitCollection()) continue;
               if (!stEvent->pxlHitCollection()->sector(i_sector)) continue;
               if (!stEvent->pxlHitCollection()->sector(i_sector)->ladder(i_ladder)) continue;
               if (!stEvent->pxlHitCollection()->sector(i_sector)->ladder(i_ladder)->sensor(i_sensor)) continue;

               StSPtrVecPxlHit &vec = stEvent->pxlHitCollection()->sector(i_sector)->ladder(i_ladder)->sensor(i_sensor)->hits();

               if (vec.size() <= 0) continue;

               for (size_t ih = 0; ih < vec.size(); ih++) {
                  StPxlHit *hit = (StPxlHit *)vec[ih];

                  if (!hit) continue;

                  Double_t global[3] = {hit->position().x(), hit->position().y(), hit->position().z()};
                  Double_t local[3]  = {hit->localPosition(0), hit->localPosition(1), hit->localPosition(2)};

                  HitMatchT *h = AddHitMatchT();
                  h->Set(global, local);
                  h->SetPred(xyzGPred, uvPred);
                  h->SettuvPred(tuvPred[0], tuvPred[1]);
                  h->SetDetId(id);
                  h->SetNRawHits((UInt_t)hit->nRawHits());
                  h->SetIndex2Track(nT);
                  h->SetIndex2Hit(0);  // need to map out later
                  h->SetTrackMom(dcaG_mom.perp(), dcaG_mom.pseudoRapidity(), dcaG_mom.phi());
                  h->SetTrackOrigin(dcaG_origin.x(), dcaG_origin.y(), dcaG_origin.z());
                  h->SetTrackNpoint(npoints * dcaG_q);
                  h->SetTrackFirstPointR(firstP.perp());
                  h->SetTrackFirstPointZ(firstP.z());
               }
            }
         }
      }

      nT++;
   }

   UInt_t val[4] = {onPXL1, onPXL2, onIST, 0};
   SetNPredHFT(val);

   LOG_INFO << "Number of predicted hits on PXL1/PXL2/IST/SSD = " << onPXL1 << "/" << onPXL2 << "/" << onIST << "/" << 0 << endm;

   return kStOk;
}


TrackT *EventT::AddTrackT()
{
   // Add a new track to the list of tracks for this event.
   // To avoid calling the very time consuming operator new for each track,
   // the standard but not well know C++ operator "new with placement"
   // is called. If tracks[i] is 0, a new TrackT object will be created
   // otherwise the previous TrackT[i] will be overwritten.

   TClonesArray &tracks = *fTracks;
   TrackT *track = new(tracks[fNtrack++]) TrackT();
   //Save reference to last TrackT in the collection of Tracks
   return track;
}


HitT *EventT::AddHitT()
{
   // Add a new hit to the list of hits for this event.
   // To avoid calling the very time consuming operator new for each hit,
   // the standard but not well know C++ operator "new with placement"
   // is called. If hits[i] is 0, a new HitT object will be created
   // otherwise the previous HitT[i] will be overwritten.

   TClonesArray &hits = *fHits;
   HitT *hit = new(hits[fNhit++]) HitT();
   //Save reference to last HitT in the collection of Hits
   return hit;
}


HitMatchT *EventT::AddHitMatchT()
{
   // Add a new hit to the list of hits for this event.
   // To avoid calling the very time consuming operator new for each hit,
   // the standard but not well know C++ operator "new with placement"
   // is called. If hits[i] is 0, a new HitT object will be created
   // otherwise the previous HitT[i] will be overwritten.

   TClonesArray &matchhits = *fMatchHits;
   HitMatchT *hitmatch = new(matchhits[fNmatchhit++]) HitMatchT();
   //Save reference to last HitMatchT in the collection of Hits

   return hitmatch;
}


VertexT *EventT::AddVertexT()
{
   // Add a new hit to the list of vertices for this event.
   // To avoid calling the very time consuming operator new for each hit,
   // the standard but not well know C++ operator "new with placement"
   // is called. If vertex[i] is 0, a new VertexT object will be created
   // otherwise the previous VertexT[i] will be overwritten.

   TClonesArray &vertices = *fVertices;
   VertexT *vertex = new(vertices[fNvertex++]) VertexT();
   //Save reference to last VertexT in the collection of Hits
   return vertex;
}


void EventT::Clear(Option_t * /*option*/)
{
   fTracks->Clear("C"); //will also call TrackT::Clear
   fHits->Clear("C"); //will also call HitT::Clear
   fVertices->Clear("C");
   fMatchHits->Clear("C");
}


void EventT::SetHeader(Int_t i, Int_t run, Int_t date, Double32_t field)
{
   fNtrack    = 0;
   fNhit      = 0;
   fNvertex   = 0;
   fNmatchhit = 0;
   fEvtHdr.Set(i, run, date, field);
}


void EventT::Print(Option_t *opt) const
{
   LOG_INFO << "Run/EventT\t" << fEvtHdr.GetRun() << "/" << fEvtHdr.GetEvtNum() << "\tDate " << fEvtHdr.GetDate()
        << "\tField " << fEvtHdr.GetField() << endl;
   LOG_INFO << "Total no. tracks " << GetTotalNoTracks() << "\tRecorded tracks " << GetNtrack()
        << "\tRecorded hits " << GetNhit() << endl;
   TRVector vertex(3, GetVertex());
   TRSymMatrix cov(3, GetCovMatrix());
   LOG_INFO << "Primary vertex " << vertex << endl;
   LOG_INFO << "Its cov. matrix " << cov << endl;

   for (UInt_t i = 0; i < GetNtrack(); i++) {LOG_INFO << i << "\t"; GetTrackT(i)->Print();}

   for (UInt_t i = 0; i < GetNhit(); i++) {LOG_INFO << i << "\t"; GetHitT(i)->Print();}

   //for (UInt_t i = 0; i < GetNvertex(); i++) {LOG_INFO << i << "\t"; GetVertexT(i)->Print();}
}
