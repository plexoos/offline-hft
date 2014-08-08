/*!
 * \class  StPxlMonMaker
 * \author Shusu Shi, LBNL
 * \date   May 2013
 * \Initial Revision.
 */
/***************************************************************************
 *
 * $Id$
 *
 * Author: Shusu Shi, May 2013
 ***************************************************************************
 *
 * Description:
 *
 ***************************************************************************
 *
 * $Log$
 * Revision 1.30  2014/02/25 04:44:52  sss
 * *** empty log message ***
 *
 * Revision 1.29  2014/02/25 04:20:14  sss
 * *** empty log message ***
 *
 * Revision 1.28  2014/02/25 01:09:28  smirnovd
 * Fixed missing brackets
 *
 * Revision 1.27  2014/02/25 01:09:19  smirnovd
 * Remove unused code accessing cluster collection
 *
 * Revision 1.26  2014/02/25 01:09:12  smirnovd
 * Fill derivative histograms when the maker finishes
 *
 * Revision 1.25  2014/02/25 01:09:06  smirnovd
 * Minor fixes not affecting functionality
 *
 * Revision 1.24  2014/02/25 01:08:39  smirnovd
 * Reduced number of bins in event distribution histograms
 *
 * Revision 1.23  2014/02/07 00:15:00  smirnovd
 * Small stylistic improvement + doxygen comments
 *
 * Revision 1.22  2014/02/07 00:14:52  smirnovd
 * Organized output in subdirectories. The change intoduced by Shusu
 *
 * Revision 1.21  2014/02/07 00:14:43  smirnovd
 * Removed temporary constraint on processing only three pixel sectors
 *
 * Revision 1.20  2014/02/07 00:14:32  smirnovd
 * Change histogram type to conserve memory
 *
 * Revision 1.19  2014/02/01 19:03:36  smirnovd
 * Common index for composite sensor id
 *
 * Revision 1.18  2014/02/01 19:03:16  smirnovd
 * Fixed default parameter bug
 *
 * Revision 1.17  2014/02/01 02:37:18  smirnovd
 * This commit is intended to sync with what we had in StRoot/StiRnD
 *
 * Revision 1.9  2014/01/28 19:29:42  qiuh
 * *** empty log message ***
 *
 *
 **************************************************************************/

#include <iostream>
#include <fstream>
#include "StPxlMonMaker.h"
#include "StEventTypes.h"
#include "StMessMgr.h"
#if ROOT_VERSION_CODE < 334081
#include "TArrayL.h"
#else
#include "TArrayL64.h"
#endif
#include "StThreeVectorF.hh"
#include "StDetectorName.h"
#include "StPxlClusterMaker/StPxlCluster.h"
#include "StPxlClusterMaker/StPxlClusterCollection.h"
#include "StPxlRawHitMaker/StPxlRawHit.h"
#include "StPxlRawHitMaker/StPxlRawHitCollection.h"
#include "StIOMaker/StIOMaker.h"

ClassImp(StPxlMonMaker);


StPxlMonMaker::StPxlMonMaker(uint16_t numPixelsPerBin, const Char_t *name) : StMaker(name),
   mNumPixelsPerBin(numPixelsPerBin), mEventCounter(0), mNtupleWrite(0)
{}


Int_t StPxlMonMaker::Init()
{
   LOG_INFO << "StPxlMonMaker::Init()" << endm;

   mEventCounter = 0;
   mNtupleWrite = 0;
   bookHists();
   return kStOk;
}


void StPxlMonMaker::bookHists()
{
   int nRowBins = kNumberOfPxlRowsOnSensor / mNumPixelsPerBin + (kNumberOfPxlRowsOnSensor % mNumPixelsPerBin ? 1 : 0);
   int nColBins = kNumberOfPxlColumnsOnSensor / mNumPixelsPerBin + (kNumberOfPxlColumnsOnSensor % mNumPixelsPerBin ? 1 : 0);

   if (mNtupleWrite) {
      m_hitNtuple = new TNtuple("hitNtuple", "hitNtuple", "sector:ladder:sensor:localX:localY:localZ:x:y:z:meanRow:meanColumn:layer:nRawHits:idTruth:EventId");
      m_rawHitNtuple = new TNtuple("rawHitNtuple", "rawHitNtuple", "sector:ladder:sensor:column:row:idTruth:EventId");
   }

   m_nRawHits_sensorID = new TH2S("nRawHits_sensorID", "The number of RawHits vs. sensorID", 400, 1, 401, 100, 0, 1200);
   m_nRawHits_sensorID->GetXaxis()->SetTitle("Sensor ID");
   m_nRawHits_sensorID->GetYaxis()->SetTitle("nRawHits");

   m_nHits_sensorID = new TH2S("nHits_sensorID", "The number of Hits vs. sensorID", 400, 1, 401, 100, 0, 1200);
   m_nHits_sensorID->GetXaxis()->SetTitle("Sensor ID");
   m_nHits_sensorID->GetYaxis()->SetTitle("nHits");

   m_hitnRawHits_sensorID = new TH2S("hitnRawHits_sensorID", "The number of RawHits per hit vs. sensorID", 400, 1, 401, 32, 0, 32);
   m_hitnRawHits_sensorID->GetXaxis()->SetTitle("Sensor ID");
   m_hitnRawHits_sensorID->GetYaxis()->SetTitle("RawHits per hit");

   for (int i = 0; i < 10; i++) {

      char ename[50];
      char etitle[100];

      sprintf(ename, "nRawHits_eachsector_sensorID_%d", i);
      sprintf(etitle, "RawHits vs. sensorNo: Sector %d", i + 1);
      m_nRawHits_eachsector_sensorID[i] = new TH2S(ename, etitle, 160, 1, 161, 1200, 0, 1200);
      m_nRawHits_eachsector_sensorID[i]->GetXaxis()->SetTitle("Sub-array");
      m_nRawHits_eachsector_sensorID[i]->GetYaxis()->SetTitle("nRawHits");

      sprintf(ename, "nHits_eachsector_sensorID_%d", i);
      sprintf(etitle, "Hits vs. sensorNo: Sector %d", i + 1);
      m_nHits_eachsector_sensorID[i] = new TH2S(ename, etitle, 40, 1, 41, 100, 0, 1200);
      m_nHits_eachsector_sensorID[i]->GetXaxis()->SetTitle("Sensor No.");
      m_nHits_eachsector_sensorID[i]->GetYaxis()->SetTitle("nHits");

      sprintf(ename, "hitnRawHits_eachsector_sensorID_%d", i);
      sprintf(etitle, "RawHits per hit vs. sensorNo: Sector %d", i + 1);
      m_hitnRawHits_eachsector_sensorID[i] = new TH2S(ename, etitle, 40, 1, 41, 32, 0, 32);
      m_hitnRawHits_eachsector_sensorID[i]->GetXaxis()->SetTitle("Sensor No.");
      m_hitnRawHits_eachsector_sensorID[i]->GetYaxis()->SetTitle("RawHits per hit");

      sprintf(ename, "innerhits_outerhits_%d", i);
      sprintf(etitle, "Inner hits vs. outer hits: Sector %d", i + 1);
      m_innerhits_outerhits[i] = new TH2S(ename, etitle, 600, 0, 6000, 1800, 0, 18000);
      m_innerhits_outerhits[i]->GetXaxis()->SetTitle("Inner hits");
      m_innerhits_outerhits[i]->GetYaxis()->SetTitle("Outer hits");

      sprintf(ename, "innerrawhits_outerrawhits_%d", i);
      sprintf(etitle, "Inner rawhits vs. outer rawhits: Sector %d", i + 1);
      m_innerrawhits_outerrawhits[i] = new TH2S(ename, etitle, 600, 0, 6000, 1800, 0, 18000);
      m_innerrawhits_outerrawhits[i]->GetXaxis()->SetTitle("Inner rawhits");
      m_innerrawhits_outerrawhits[i]->GetYaxis()->SetTitle("Outer rawhits");
   }

   m_globalx_y  = new TH2F("globalx_y", "Global X vs. Y", 200, -10, 10, 200, -10, 10);
   m_globalx_y->GetXaxis()->SetTitle("Global X");
   m_globalx_y->GetYaxis()->SetTitle("Global Y");

   m_globalz = new TH1F("globalz", "Global Z", 200, -10, 10);
   m_globalz->GetXaxis()->SetTitle("Global Z");
   m_globalz->GetYaxis()->SetTitle("Counts");

   m_globalphi_z_inner = new TH2F("globalphi_z_inner", "Global #phi vs. global Z, inner", 100, -3.14159, 3.14159, 100, -10, 10);
   m_globalphi_z_inner->GetXaxis()->SetTitle("Global #phi");
   m_globalphi_z_inner->GetYaxis()->SetTitle("Global Z");

   m_globalphi_z_outer = new TH2F("globalphi_z_outer", "Global #phi vs. global Z, outer", 100, -3.14159, 3.14159, 100, -10, 10);
   m_globalphi_z_outer->GetXaxis()->SetTitle("Global #phi");
   m_globalphi_z_outer->GetYaxis()->SetTitle("Global Z");

   for (int i = 0; i < 10; i++) {
      for (int j = 0; j < 4; j++) {
         for (int k = 0; k < 10; k++) {

            int sensorId = i * 40 + j * 10 + k;

            char ename[50];
            char etitle[100];

            sprintf(ename, "rawHit_rowvscolumn_sec%02d_ldr%d_snr%02d", i+1, j+1, k+1);
            sprintf(etitle, "Raw hit map: Sector %d, Ladder %d, Sensor %d; Row Id; Column Id;", i+1, j+1, k+1);
            m_rawHit_rowvscolumn[sensorId] = new TH2S(ename, etitle, nRowBins, 0, nRowBins*mNumPixelsPerBin, nColBins, 0, nColBins*mNumPixelsPerBin);

            sprintf(ename, "nRawHits_EventId_sec%02d_ldr%d_snr%02d", i+1, j+1, k+1);
            sprintf(etitle, "rawHits vs. Event Id: Sector %d Ladder %d Sensor %d; Event Id (Time); Num. of Raw Hits", i+1, j+1, k+1);
            m_nRawHits_EventId[sensorId] = new TProfile(ename, etitle, 10000, 0, 10000);
         }
      }
   }
}


Int_t StPxlMonMaker::Finish()
{
   LOG_INFO << "StPxlMonMaker::Finish()" << endm;
   writeHists();
   gMessMgr->Info() << "StPxlMonMaker::Finish() "
                    << "Processed " << mEventCounter << " events." << endm;
   return kStOK;
}


Int_t StPxlMonMaker::Make()
{
   mEventCounter++;

   StEvent *event;
   event = (StEvent *) GetInputDS("StEvent");
   if (!event) {
      gMessMgr->Warning() << "StPxlMonMaker::Make : No StEvent" << endm;
      //return kStOK;        // if no event, we're done
   }

   fillHists();

   if (mEventCounter % 100 == 0) writeHists();
   return kStOK;
}


void StPxlMonMaker::fillHists()
{
   StEvent *pEvent = (StEvent *) StMaker::GetChain()->GetInputDS("StEvent");
   if (pEvent) {
      StPxlHitCollection *PxlHitCollection = pEvent->pxlHitCollection();
      if (! PxlHitCollection) { LOG_WARN << "No Pixel Hit Collection" << endm; return;}

      for (unsigned int i = 0; i < PxlHitCollection->numberOfSectors(); i++)
      {
         int hitnumber_inner = 0, hitnumber_outer = 0;
         StPxlSectorHitCollection *sectorHitCollection = PxlHitCollection->sector(i);
         for (unsigned int j = 0; j < sectorHitCollection->numberOfLadders(); j++)
         {
            StPxlLadderHitCollection *ladderHitCollection = sectorHitCollection->ladder(j);
            for (unsigned int k = 0; k < ladderHitCollection->numberOfSensors(); k++)
            {
               int sensorId = i * 40 + j * 10 + k;

               StPxlSensorHitCollection *sensorHitCollection = ladderHitCollection->sensor(k);
               for (unsigned int l = 0; l < sensorHitCollection->hits().size(); l++)
               {
                  StPxlHit *hit = sensorHitCollection->hits()[l];
                  if (!hit) continue;

                  const StThreeVectorF &P = hit->position();
                  if (mNtupleWrite) {
                     m_hitNtuple->Fill((float)hit->sector(), (float)hit->ladder(), (float)hit->sensor(),
                                       (float)hit->localPosition(0), (float)hit->localPosition(1), (float)hit->localPosition(2),
                                       (float)P.x(), (float)P.y(), (float)P.z(),
                                       (float)hit->meanRow(), (float)hit->meanColumn(), (float)hit->layer(),
                                       (float)hit->nRawHits(), (float)hit->idTruth(), (float)pEvent->id());
                  }

                  m_globalx_y->Fill((float)P.x(), (float)P.y());
                  m_globalz->Fill((float)P.z());
                  if (j == 0) m_globalphi_z_inner->Fill(P.phi(), (float)P.z());
                  if (j == 1 || j == 2 || j == 3) m_globalphi_z_outer->Fill(P.phi(), (float)P.z());
                  m_hitnRawHits_sensorID->Fill(sensorId + 1, (int)hit->nRawHits());
                  m_hitnRawHits_eachsector_sensorID[i]->Fill(j * 10 + k + 1, (int)hit->nRawHits());
               }

               m_nHits_sensorID->Fill(sensorId + 1, sensorHitCollection->hits().size());
               if (j == 0) hitnumber_inner += sensorHitCollection->hits().size();
               if (j == 1 || j == 2 || j == 3) hitnumber_outer += sensorHitCollection->hits().size();
            }
         }

         m_innerhits_outerhits[i]->Fill(hitnumber_inner, hitnumber_outer);
      }
   }

   TObjectSet *pxlRawHitDataSet = (TObjectSet *)GetDataSet("pxlRawHit");
   if (!pxlRawHitDataSet) {
      LOG_WARN << "StPxlClusterMaker::Make there is no pxlRawHitDataSet " << endm;
      return;
   }

   StPxlRawHitCollection *pxlRawHitCollection = (StPxlRawHitCollection *)pxlRawHitDataSet->GetObject();
   if (!pxlRawHitCollection) {
      LOG_WARN << "StPxlClusterMaker::Make() no pxlRawHitCollection." << endm;
      return;
   }

   for (int i = 0; i < kNumberOfPxlSectors; i++)
   {
      int rawhitnumber_inner = 0;
      int rawhitnumber_outer = 0;

      for (int j = 0; j < kNumberOfPxlLaddersPerSector; j++)
      {
         for (int k = 0; k < kNumberOfPxlSensorsPerLadder; k++)
         {
            int sensorId = i * 40 + j * 10 + k;
            unsigned int sensor_subarray[4] = {0};
            for (int l = 0; l < pxlRawHitCollection->numberOfRawHits(i + 1, j + 1, k + 1); l++) {
               const StPxlRawHit *rawHit = pxlRawHitCollection->rawHit(i + 1, j + 1, k + 1, l);
               if (mNtupleWrite) {
                  m_rawHitNtuple->Fill((float)i + 1, (float)j + 1, (float)k + 1, (float)rawHit->row(), (float)rawHit->column(), (float)rawHit->idTruth(), (float) pEvent->id());
               }
               m_rawHit_rowvscolumn[sensorId]->Fill(rawHit->column(), rawHit->row());
               if(rawHit->column()<240) sensor_subarray[0]++;
               else if(rawHit->column()<480) sensor_subarray[1]++;
               else if(rawHit->column()<720) sensor_subarray[2]++;
               else sensor_subarray[3]++;
            }

            m_nRawHits_sensorID->Fill(sensorId + 1, pxlRawHitCollection->numberOfRawHits(i + 1, j + 1, k + 1));
            m_nRawHits_eachsector_sensorID[i]->Fill(j*40+k*4+1, sensor_subarray[0]);
            m_nRawHits_eachsector_sensorID[i]->Fill(j*40+k*4+2, sensor_subarray[1]);
            m_nRawHits_eachsector_sensorID[i]->Fill(j*40+k*4+3, sensor_subarray[2]);
            m_nRawHits_eachsector_sensorID[i]->Fill(j*40+k*4+4, sensor_subarray[3]);
            m_nRawHits_EventId[sensorId]->Fill((int) pEvent->id() / 100 + 1, pxlRawHitCollection->numberOfRawHits(i + 1, j + 1, k + 1));

            if (j == 0) rawhitnumber_inner += pxlRawHitCollection->numberOfRawHits(i + 1, j + 1, k + 1);
            if (j == 1 || j == 2 || j == 3) rawhitnumber_outer += pxlRawHitCollection->numberOfRawHits(i + 1, j + 1, k + 1);
         }
      }

      m_innerrawhits_outerrawhits[i]->Fill(rawhitnumber_inner, rawhitnumber_outer);
   }
}


void StPxlMonMaker::writeHists()
{
   StIOMaker *ioMaker = (StIOMaker*) GetMaker("inputStream");
   if (!ioMaker) {
      gMessMgr->Warning() << "StPxlMonMaker::writeHists(): No StIOMaker found" << endm;
   }

   TString filename = TString(ioMaker->GetFile());
   int slash_index = filename.Last('/');

   if(slash_index >= 0){
      filename.Replace(0, slash_index-12, "");
   }

   TString daynumber( filename(0,3) );
   TString runnumber( filename(4,8) );
   gSystem->Exec(Form("mkdir output"));
   gSystem->Exec(Form("mkdir output/%s",daynumber.Data()));
   gSystem->Exec(Form("mkdir output/%s/%s",daynumber.Data(),runnumber.Data()));

   slash_index = filename.First(".");
   if (slash_index == 0) slash_index = filename.Length();

   filename.Replace(slash_index, filename.Length() - slash_index, ".pxlMon.root");
   filename.Insert(0, "output/");
   LOG_INFO << "StPxlMonMaker::writeHists(): filename: " << filename << endm;

   m_f1 = new TFile(filename.Data(), "RECREATE");

   m_f1->WriteTObject(m_hitNtuple);
   m_f1->WriteTObject(m_rawHitNtuple);

   m_f1->WriteTObject(m_nRawHits_sensorID);
   for (int i = 0; i < 10; i++) {
      m_f1->WriteTObject(m_nRawHits_eachsector_sensorID[i]);
   }

   m_f1->WriteTObject(m_nHits_sensorID);
   for (int i = 0; i < 10; i++) {
      m_f1->WriteTObject(m_nHits_eachsector_sensorID[i]);
   }

   m_f1->WriteTObject(m_hitnRawHits_sensorID);
   for (int i = 0; i < 10; i++) {
      m_f1->WriteTObject(m_hitnRawHits_eachsector_sensorID[i]);
   }

   m_f1->WriteTObject(m_globalx_y);
   m_f1->WriteTObject(m_globalz);
   m_f1->WriteTObject(m_globalphi_z_inner);
   m_f1->WriteTObject(m_globalphi_z_outer);

   for (int i = 0; i < 10; i++) {
      m_f1->WriteTObject(m_innerhits_outerhits[i]);
      m_f1->WriteTObject(m_innerrawhits_outerrawhits[i]);
   }

   for (int i = 0; i < 10; i++) {
      for (int j = 0; j < 4; j++) {
         for (int k = 0; k < 10; k++) {

            int sensorId = i * 40 + j * 10 + k;

            m_f1->WriteTObject(m_rawHit_rowvscolumn[sensorId]);
            m_f1->WriteTObject(m_nRawHits_EventId[sensorId]);
         }
      }
   }

   m_f1->Close();
}
