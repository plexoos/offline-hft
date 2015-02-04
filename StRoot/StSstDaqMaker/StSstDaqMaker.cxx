/*!
 * \class StSstDaqMaker
 * \author Long Zhou, Nov 2013, according codes from PXL
 * \Initial Revision.
 */
/***************************************************************************
 *
 * $Id: StSstDaqMaker.cxx,v 1.12 2015/02/04 22:27:33 zhoulong Exp $
 *
 * Author: Long Zhou, Nov 2013
 ***************************************************************************
 *
 * Description:
 * Read ssd strip info from daq format.
 *
 *
 ***************************************************************************
 *
 * $Log: StSstDaqMaker.cxx,v $
 * Revision 1.12  2015/02/04 22:27:33  zhoulong
 * merged DAQ maker for Run14 and Run15.
 *
 * Revision 1.11  2015/02/02 22:58:31  zhoulong
 * Corrected the default ordering issue
 *
 * Revision 1.10  2015/01/10 20:18:18  zhoulong
 * 1>remove constant shift. 2>fixed delete pedestal table issue
 *
 * Revision 1.9  2015/01/05 22:07:31  smirnovd
 * StSstDaqMaker: Use STAR framework return codes
 *
 * In this case we better return an error code
 *
 * Revision 1.8  2015/01/05 22:07:23  smirnovd
 * StSstDaqMaker: Removed quite pointless overriding methods
 *
 * Revision 1.7  2015/01/05 21:58:48  smirnovd
 * StSstDaqMaker/: Updated style with astyle -s3 -p -H -A3 -k3 -O -o -y -Y -f
 *
 * Revision 1.6  2014/11/10 14:46:37  zhoulong
 * Fixed delete spa_strip table issue
 *
 * Revision 1.5  2014/09/30 18:00:10  zhoulong
 * Fixed alot of issue, and remove histograms, ntuple.
 *
 * Revision 1.4  2014/06/05 14:55:23  zhoulong
 * Added some code to compatible Thorsten's LC FPGA and correct readout channel shift in old LC FPGA
 *
 *
 ***************************************************************************
 * StSstDaqMaker.cxx v 1.1
 * Revision 1.1 2014/1/23 11:18:00 Long Zhou
 * <1>Change Sector to Rdo
 * <2>change the main structure(depend on test DAQ data sample.)
 *
 * StsstDaqMaker.cxx,v 1.0
 * Revision 1.0 2014/1/21 10:46:00 Long Zhou
 * Initial version
 ****************************************************************************/
#include "StSstDaqMaker.h"
#include "StRoot/St_base/StMessMgr.h"
#include "RTS/src/DAQ_SST/daq_sst.h"
#include "RTS/src/DAQ_READER/daq_dta.h"
#include "RTS/src/DAQ_READER/daqReader.h"
#include "StEventTypes.h"
#include "StMessMgr.h"
#include "StRtsTable.h"
#include "StSsdDbMaker/StSsdDbMaker.h"
#include "StSsdDbMaker/StSstDbMaker.h"
#include "tables/St_spa_strip_Table.h"
#include "tables/St_ssdConfiguration_Table.h"
#include "tables/St_ssdPedStrip_Table.h"
#include "tables/St_ssdChipCorrect_Table.h"
#include "tables/St_ssdStripCalib_Table.h"
#include "tables/St_ssdNoise_Table.h"
#include "StSsdUtil/StSsdConfig.hh"
#include "StIOMaker/StIOMaker.h"
#include <map>
#include <vector>
using std::vector;
ClassImp(StSstDaqMaker)

//-----------------------------------------------
//Ladder cards number in each RDO channel.
const Int_t StSstDaqMaker::RDO2LADDER[5][8] = { {1, 2, 3, 4, 5, 6, 7, 8},
   {9, 10, 11, 12, 13, 14, 15, 16},
   {17, 18, 19, 20, 1, 2, 3, 4},
   {5, 6, 7, 8, 9, 10, 11, 12},
   {13, 14, 15, 16, 17, 18, 19, 20}
};

//silicon strip number ordered by ALICE128 readout order
const Int_t StSstDaqMaker::ReadOutMap[128] = {
   97, 96, 98, 95, 99, 94, 100, 93,
   101, 92, 102, 91, 103, 90, 104, 89,
   105, 88, 106, 87, 107, 86, 108, 85,
   109, 84, 110, 83, 111, 82, 112, 81,
   113, 80, 114, 79, 115, 78, 116, 77,
   117, 76, 118, 75, 119, 74, 120, 73,
   121, 72, 122, 71, 123, 70, 124, 69,
   125, 68, 126, 67, 127, 66, 128, 65,
   1, 64, 2, 63, 3, 62, 4, 61,
   5, 60, 6, 59, 7, 58, 8, 57,
   9, 56, 10, 55, 11, 54, 12, 53,
   13, 52, 14, 51, 15, 50, 16, 49,
   17, 48, 18, 47, 19, 46, 20, 45,
   21, 44, 22, 43, 23, 42, 24, 41,
   25, 40, 26, 39, 27, 38, 28, 37,
   29, 36, 30, 35, 31, 34, 32, 33
};
//-----------------------------------------------
StSstDaqMaker::StSstDaqMaker(const Char_t *name)
   : StRTSBaseMaker("sst", name), spa_strip(0)
{
}
//-----------------------------------------------
StSstDaqMaker::~StSstDaqMaker()
{
}
//-----------------------------------------------
Int_t StSstDaqMaker::InitRun(Int_t runumber)
{
   mEventrunumber = runumber;
   mReverse = 0; // reverse=0 reverse P-side ; reverse=1 reverse N-side
   mReverseChip = 0; //0 -- Default(1-97), 1 -- Reverse(128-97) ;
   mUseChipCorrect = 1;
   mUsePedSubtraction = 1;
   mUseIntrinsicNoise = 1;
   LOG_INFO << "InitRun(Int_t runumber) - Read now Databases" << endm;
   Int_t run = (runumber / 1000000) - 1;

   if (mEventrunumber > 16000000) {
      (mUsePedSubtraction == 1) ? FillReadOutPedTable() : FillDefaultReadOutPedTable();
      (mUseIntrinsicNoise == 1) ? FillNoiseTable() : FillDefaultNoiseTable();
   }
   else {
     (mUseChipCorrect==1)?FillChipNoiseTable():FillDefaultChipNoiseTable();
   }

   St_ssdConfiguration *configuration = (St_ssdConfiguration *) GetDataBase("Geometry/ssd/ssdConfiguration");

   if (!configuration) {
      LOG_ERROR << "InitRun(" << runumber << ") - ERROR - ssdConfiguration==0" << endm;
      return kStErr;
   }

   ssdConfiguration_st *config  = (ssdConfiguration_st *) configuration->GetTable() ;

   if (!config) {
      LOG_ERROR << "InitRun(" << runumber << ") - ERROR - config==0" << endm;
      return kStErr;
   }

   mConfig = new StSsdConfig();

   Int_t totLadderPresent = 0;

   for (Int_t ladder = 1; ladder <= config->nMaxLadders; ladder++) {
      LOG_INFO << " on sector = " << config->ladderIsPresent[ladder - 1];

      if (config->ladderIsPresent[ladder - 1] != 0)
         totLadderPresent++;

      mConfig->setLadderIsActive(ladder, config->ladderIsPresent[ladder - 1]);
   }

   PrintConfiguration(run, config);
   mConfig->setNumberOfLadders(totLadderPresent);
   mConfig->setNumberOfWafers(config->nMaxWafers / config->nMaxLadders);
   mConfig->setNumberOfHybrids(2);
   mConfig->setTotalNumberOfHybrids(nSstSide * nSstWaferPerLadder * totLadderPresent);
   mConfig->setTotalNumberOfLadders(config->nMaxLadders);
   mConfig->setNumberOfStrips(nSstStripsPerWafer);
   mConfig->setConfiguration();
   mEventnumber = 0;

   LOG_INFO << "_____________________________" << endm;
   LOG_INFO << "       Via  Datababase......." << endm;
   LOG_INFO << ".......numberOfSectors =     " << config->nMaxSectors << endm;
   LOG_INFO << ".......numberOfLadders =     " << totLadderPresent << endm;
   LOG_INFO << " .numberOfWafersPerLadder =  " << config->nMaxWafers / config->nMaxLadders << endm;
   LOG_INFO << "_____________________________" << endm;
   LOG_INFO << "      InitRun() - Done       " << endm;
   LOG_INFO << "StSstDaqMaker initialization." << endm;

   return kStOK;
}

//-------------------------------------------------
Int_t StSstDaqMaker::Make()
{
   LOG_INFO << "StSstDaqMaker Start Make..." << endm;
   StRtsTable *rts_table = 0;
   mEventnumber++;

   //raw data mode
   while (rts_table = GetNextDaqElement("sst/raw")) {
      //each DAQ element record one RDO data
      //one event will loop all the 5 RDOs
      mMode = 0;//0-physic run,1-pedestal run
      mRdoData = (UInt_t *)rts_table->At(0);
      mRdoDataLength = rts_table->GetNRows();
      LOG_INFO << "Found sst RDO raw data at " << hex << mRdoData << dec << " , length in byte: "
               << mRdoDataLength << " in UInt_t: " << mRdoDataLength / sizeof(UInt_t) << endm;
      DecodeRdoData();
      DecodeHitsData();
   }

   //pedestal mode

   while (rts_table = GetNextDaqElement("sst/pedrms")) {
      mMode    = 1;
      mRdoData = (UInt_t *)rts_table->At(0);
      mRdoDataLength = rts_table->GetNRows();
      LOG_INFO << "Found sst Pedestal Run data at " << hex << mRdoData
               << dec << " , length in byte: " << mRdoDataLength << endm;
      Int_t flag = 0;
      ssdPedStrip = (St_ssdPedStrip *) m_DataSet->Find("ssdPedStrip");

      if (!ssdPedStrip) {
         ssdPedStrip = new St_ssdPedStrip("ssdPedStrip", nSstStripsPerWafer * nSstWaferPerLadder);
         m_DataSet->Add(ssdPedStrip);
      }

      ssdPedStrip_st out_ped_strip;
      Int_t id_side, ladder, strip_number, id_wafer;
      Int_t ladderCountN[20] = {0};
      Int_t ladderCountP[20] = {0};
      Int_t count = 1;
      Int_t c_correct;
      mSec     = rts_table->Sector();
      mRDO     = rts_table->Rdo();
      mFiber   = rts_table->Pad();

      if (mSec == 1) mRDO = mRDO; //sector 1
      else mRDO = mRDO + 3; //sector 2

      if (mRDO < 1 || mRDO > 5)     flag = 1;

      if (mSec < 1 || mSec > 3)     flag = 1;

      if (mFiber < 0 || mFiber > 7) flag = 1;

      if (flag == 1) {
         LOG_WARN << "BAD pedestal data. Sector: " << mSec << " RDO: "
                  << mRDO << " fiber: " << mFiber << endm;
         continue;
      }

      FindLadderSide(mRDO, mFiber, ladder, id_side);

      for (StRtsTable::iterator it = rts_table->begin(); it != rts_table->end(); ++it) {
         daq_sst_pedrms_t *f = (daq_sst_pedrms_t *)*it;

         for (Int_t c = 0; c < nSstStripsPerWafer; c++) {
            //channel
            c_correct = (Int_t)c;

            if (!Shift(mEventrunumber, c_correct)) {
               LOG_INFO << "First readout channel in old LC FPGA is not usable." << endm;
               continue;
            }

            for (Int_t h = 0; h < nSstWaferPerLadder; h++) {
               //wafer
               Int_t s = c_correct;
               FindStripNumber(s);
               mPed  = (Float_t)f->ped[h][c_correct] - 375; //Pedestal from data have a constant shift (375)
               mRms  = (Float_t)f->rms[h][c_correct] / 16.;
               strip_number = s + 1;

               if (id_side == 0) {
                  id_wafer = 7000 + 100 * (nSstWaferPerLadder - h) + ladder + 1;
               }
               else {
                  id_wafer = 7000 + 100 * (h + 1) + ladder + 1;
               }

               out_ped_strip.id         = count;
               out_ped_strip.id_strip   = 10000 * (10 * strip_number + id_side) + id_wafer;
               out_ped_strip.noise      = mRms;
               out_ped_strip.pedestal   = mPed;
               ssdPedStrip->AddAt(&out_ped_strip);
               count = count + 1;

               if (id_side == 0) ladderCountP[ladder]++;
               else            ladderCountN[ladder]++;
            }
         }
      }

      if (StMaker::GetDebug()) {
         LOG_DEBUG << "Make()/Counts (p-side): ";

         for (Int_t i = 0; i < nSstLadder; i++) {
            LOG_DEBUG.width(5);
            LOG_DEBUG << ladderCountP[i] << " ";
         }

         LOG_DEBUG << endm;
         LOG_DEBUG << "Make()/Counts (n-side): ";

         for (Int_t i = 0; i < nSstLadder; i++) {
            LOG_DEBUG.width(5);
            LOG_DEBUG << ladderCountN[i] << " ";
         }

         LOG_DEBUG << endm;

         if (ssdPedStrip && ssdPedStrip->GetNRows() != 0) {
            LOG_DEBUG << "Make()/ Read Pedestal & Noise" << endm;
            LOG_DEBUG << "Make()/ssdPedStrip->NRows= " << ssdPedStrip->GetNRows() << endm;
         }
      }

      //delete ssdPedStrip;
   }

   return kStOk;
}
//-------------------------------------------------
void StSstDaqMaker::DecodeRdoData()
{
   Int_t index = 0;
   mRDO               = 0;
   mTrigger           = 0;
   mHeaderData        = 0;
   mTrailerData       = 0;
   mTrailerDataLength = 0;
   mRdoFlag           = 1;
   mEventTime         = 0;
   mPEventTime        = 0;

   for (Int_t f = 0; f < 8; f++) mFiberFlag[f] = 1; //flag=0-->bad, flag=1-->good

   if (mRdoDataLength == 0 || !mRdoData) {
      LOG_WARN << "NO RDO OUTPUT DATA!" << endm;
      mRdoFlag = 0;
      return;
   }

   if (mRdoData[0] != HEADER_TOKEN) {
      LOG_WARN << "SST DAQ DATA HEADER_TOKEN is not correct!!" << endm;
      mRdoFlag = 0;
      return;
   }
   else {
      LOG_DEBUG << "SST DAQ DATA HEADER_TOKEN correct: 0x" << hex << mRdoData[0] << dec << endm;
   }

   mEventTime  = (Float_t)mRdoData[2];
   mPEventTime = (Float_t)mRdoData[4];

   mHeaderData = mRdoData + index;

   if (StMaker::GetDebug()) {
      for (UInt_t h = 0; h < HEADER_LENGTH; h++) {
         if (h == 0) LOG_DEBUG << "Start Print Event HEADER info... " << endm;

         LOG_DEBUG << "0x" << hex << mHeaderData[h] << dec << endm;
      }
   }

   mTrigger = Mid(TRIG_START, TRIG_END, mRdoData[1]);
   LOG_DEBUG << "Current Event Trigger words is " << mTrigger << endm;
   mRDO = Mid(RDO_START, RDO_END, mRdoData[3]);

   if (mRDO == 6) mRDO = 1; //20140311,luis chenged this serial number

   LOG_DEBUG << "Current RDO number is :" << mRDO << endm;

   if (mRDO < 1 || mRDO > nSstRdo) {
      LOG_WARN << "RDO number is BAD number(<1 || >5), reject this RDO" << endm;
      mRdoFlag = 0;
      return;
   }

   index += HEADER_LENGTH;

   for (Int_t i = 0; i < 8; i++) {
      if (i == 0) {
         mAdcHeader[i] = (mRdoData + index);
         mAdc[i] = mAdcHeader[i] + FIBER_HEADER_LENGTH;
      }
      else {
         mAdc[i] = mAdc[i - 1] + mAdcLength[i - 1];
         mAdcHeader[i] = (mAdcHeader[i - 1] + mAdcLength[i - 1]);
      }

      mAdcLength[i] = Mid(ADC_START, ADC_END, mAdcHeader[i][1]);
      mDataMode[i]  = Mid(DATAMODE_START, DATAMODE_END, mAdcHeader[i][1]);
      mChannel[i]   = Mid(FIBER_START, FIBER_END, mAdcHeader[i][2]);
      mFlag[i]      = Mid(FLAG_START, FLAG_END, mAdcHeader[i][1]);

      if (mAdcHeader[i][0] == FIBER_LINK_TOKEN) {
         LOG_DEBUG << "Fiber [" << mChannel[i] << "]: link Token correct : 0x"
                   << hex << mAdcHeader[i][0] << dec << endm;
      }
      else {
         LOG_WARN << "Fiber [" << mChannel[i] << "]: Link Token wrong :0x"
                  << hex << mAdcHeader[i][0] << dec << endm;
         mFiberFlag[i] = 0;
         continue;
      }

      if (mDataMode[i] == RAWMODE) {
         LOG_DEBUG << "Fiber [" << mChannel[i] << "]:Data mode is RAW data. ADC Length "
                   << mAdcLength[i] << endm;
      }
      else if (mDataMode[i] == COMPRESSEDMODE) {
         LOG_DEBUG << "Fiber [" << mChannel[i] << "]:Data mode is COMPRESSED data. ADC Length "
                   << mAdcLength[i] << endm;
      }
      else {
         LOG_WARN << "Fiber [" << mChannel[i] << "]: DO not have this data mode; error data mode = "
                  << mDataMode[i] << endm;
         mFiberFlag[i] = 0;
         continue;
      }

      if (mFlag[i] == NODATA) {
         LOG_DEBUG << "Fiber [" << mChannel[i] << "] Flag:NO DATA FLAG,reject this fiber data" << endm;

         if (mAdcLength[i] == FIBER_HEADER_LENGTH)
            LOG_DEBUG << "Fiber [" << mChannel[i] << "] Flag and adc length is consistent! let's look at next fiber.." << endm;

         if (mAdcLength[i] != FIBER_HEADER_LENGTH) {
            LOG_WARN << "Fiber [" << mChannel[i] << "] Flag and adc length is not consistent,Stop !" << endm;
            mFiberFlag[i] = 0;
            continue;
         }
      }

      if (mFlag[i] == OVERFLOWFLAG) {
         LOG_WARN << "Fiber [" << mChannel[i] << "] Flag:Over Flow" << endm;
         mFiberFlag[i] = 0;
         continue;
      }

      if (mFlag[i] == EARLY_ABORT) {
         LOG_WARN << "Fiber [" << mChannel[i] << "] Flag:Abort Early!" << endm;
         mFiberFlag[i] = 0;
         continue;
      }

      if (mFlag[i] == WRONG_PIPE_MODE) {
         LOG_WARN << "FIBER [" << mChannel[i] << "] Flag:Wrong pipe mode!" << endm;
         mFiberFlag[i] = 0;
         continue;
      }

      LOG_DEBUG << "Fiber [" << mChannel[i] << "]: ADC Length = " << mAdcLength[i] << endm;
   }

   //check the end token,TCD end token TCD header
   if (mRdoData[mRdoDataLength / sizeof(UInt_t) - 1] == END_TOKEN) {
      LOG_DEBUG << "End Token Correct: 0x" << hex << mRdoData[mRdoDataLength / sizeof(UInt_t) - 1] << dec << endm;
   }
   else {
      LOG_WARN << "End Token Wrong : 0x" << hex << mRdoData[mRdoDataLength / sizeof(UInt_t) - 1] << dec << endm;
      mRdoFlag = 0;
      return;
   }

   //tcd end token
   if (mRdoData[mRdoDataLength / sizeof(UInt_t) - 3] == TCD_END_TOKEN) {
      LOG_DEBUG << "TCD End Token Correct: 0x" << hex << mRdoData[mRdoDataLength / sizeof(UInt_t) - 3] << dec << endm;
   }
   else {
      LOG_WARN << "TCD End Token Wrong : 0x" << hex << mRdoData[mRdoDataLength / sizeof(UInt_t) - 3] << dec << endm;
      mRdoFlag = 0;
      return;
   }

   //tcd token
   mTrailerData = mAdcHeader[7] + mAdcLength[7];

   if (mTrailerData[0] == TCD_TOKEN) {
      LOG_DEBUG << "TCD Token Correct: 0x" << hex << mTrailerData[0] << dec << endm;
   }
   else {
      LOG_WARN << "TCD Token Wrong : 0x" << hex << mTrailerData[0] << dec << endm;
      mRdoFlag = 0;
      return;
   }

   if (StMaker::GetDebug()) {
      for (Int_t t = 0; t < 10; t++) {
         if (t == 0) LOG_DEBUG << "Start Print Trailer info... " << endm;

         LOG_DEBUG << "0x" << hex << mTrailerData[t] << dec << endm;

         if (mTrailerData[t] == END_TOKEN) break;
      }
   }
}
//-------------------------------------------------
Int_t StSstDaqMaker::GetHitsDataLength(Int_t FiberNumber)
{
   return mAdcLength[FiberNumber];
}
//------------------------------------------------
void StSstDaqMaker::DecodeHitsData()
{
   if (mRdoFlag != 1) return;

   LOG_DEBUG << "START Decoding RDO [" << mRDO << "] data, rdo flag is " << mRdoFlag << endm;

   for (Int_t j = 0; j < 8; j++) {
      if (mFiberFlag[j] != 1) continue;

      if (mChannel[j] < 0 || mChannel[j] > (nSstFiberPerRdo - 1)) {
         LOG_WARN << "Fiber number is BAD (<0 || >7), reject this Fiber" << endm;
         continue;
      }

      Int_t temp = mAdcLength[j] - FIBER_HEADER_LENGTH;

      if (temp == 0 || !mAdc[j]) {
         LOG_WARN << "Fiber [" << mChannel[j] << "]: do not have any data..." << endm;
         continue;
      }

      if (mFlag[j] != NORMAL) {
         LOG_WARN << "Fiber[" << mChannel[j] << "]: data flag is not normal, reject this fiber" << endm;
         continue;
      }

      if (mDataMode[j] == RAWMODE) {
         if (mEventrunumber < 16000000) DecodeRawWords(mAdc[j], temp, mChannel[j]);
         else DecodeRawWords_r15(mAdc[j], temp, mChannel[j]);
      }

      if (mDataMode[j] == COMPRESSEDMODE) DecodeCompressedWords(mAdc[j], temp, mChannel[j]);
   }
}
//-------------------------------------------------
void StSstDaqMaker::DecodeRawWords(UInt_t *val, Int_t vallength, Int_t channel)
{
   Int_t strip_number[3]    = {0};
   Int_t id_wafer[3]        = {0};
   Int_t ladderCountN[20]   = {0};
   Int_t ladderCountP[20]   = {0};
   Int_t data[3]            = {0};
   Int_t wafer[3]           = {0};
   Int_t hybrid[3]          = {0};
   Int_t strip[3]           = {0};
   Int_t readout[3]         = {0};
   Int_t readout_correct[3] = {0};
   Int_t ladder             = 0;
   Int_t id_side            = 0;
   Int_t count              = 0;

   //initialize St_spa_strip and St_ssdPedStrip table.
   //St_spa_strip *spa_strip = (St_spa_strip *) m_DataSet->Find("spa_strip");
   spa_strip = (St_spa_strip *) m_DataSet->Find("spa_strip");

   if (!spa_strip) {
      spa_strip   = new St_spa_strip("spa_strip", vallength);
      m_DataSet->Add(spa_strip);
   }

   spa_strip_st   out_strip;
   count = 1;
   LOG_DEBUG << "DECODING RAW MODE data....." << endm;
   //grab ladder and side
   FindLadderSide(mRDO, channel, ladder, id_side);

   //RAW data format
   //Each 32-bit word will record 3 ADC words.
   //Data store structure:
   //
   // 32-bit word   3 ADCs bit 9-0:readout 0; hybrid 0
   //                      bit 19-10:readout 0; hybrid 1
   //                      bit 29-20:readout 0; hybrid 2
   //                      .
   //                      .
   // 32-bit word   3 ADCs bit 9-0:readout 0; hybrid 15
   //                      bit 19-10:readout 1; hybrid 0
   //                      bit 29-20:readout 1; hybrid 1

   for (Int_t i = 0; i < vallength; i++) {
      if (i == 0) hybrid[0] = 0;
      else hybrid[0] = hybrid[2] + 1;

      if (hybrid[0] >= 16) hybrid[0] = hybrid[0] % 16;

      hybrid[1] = hybrid[0] + 1;

      if (hybrid[1] >= 16) hybrid[1] = hybrid[1] % 16;

      hybrid[2] = hybrid[0] + 2;

      if (hybrid[2] >= 16) hybrid[2] = hybrid[2] % 16;

      LOG_DEBUG << "Three hybrid number in current word is :"
                << hybrid[0] << "," << hybrid[1] << "," << hybrid[2] << endm;

      if (i != 0) {
         if (hybrid[0] == 0) readout[0] = readout[0] + 1;

         if (readout[2] > readout[0]) readout[0] = readout[2];

         if (hybrid[1] == 0) readout[1] = readout[1] + 1;

         if (readout[0] > readout[1]) readout[1] = readout[0];

         if (hybrid[2] == 0) readout[2] = readout[2] + 1;

         if (readout[1] > readout[2]) readout[2] = readout[1];
      }

      LOG_DEBUG << "[adc_pointer,readout_0,hybrid_0] = [" << i << ","
                << readout[0] << "," << hybrid[0] << "]" << endm;
      LOG_DEBUG << "[adc_pointer,readout_1,hybrid_1] = [" << i << ","
                << readout[1] << "," << hybrid[1] << "]" << endm;
      LOG_DEBUG << "[adc_pointer,readout_2,hybrid_2] = [" << i << ","
                << readout[2] << "," << hybrid[2] << "]" << endm;

      data[0] = Mid(HYBRID_ONE_START, HYBRID_ONE_END, val[i]);
      data[1] = Mid(HYBRID_TWO_START, HYBRID_TWO_END, val[i]);
      data[2] = Mid(HYBRID_THREE_START, HYBRID_THREE_END, val[i]);

      for (Int_t n = 0; n < 3; n++) {
         readout_correct[n] = readout[n];

         if (!Shift(mEventrunumber, readout_correct[n])) {
            LOG_DEBUG << "First readout channel in old LC FPGA is not usable." << endm;
            continue;
         }

         strip[n] = readout[n];
         wafer[n] = hybrid[n];
         FindStripNumber(strip[n]);

         if (id_side == 0) {
            id_wafer[n] = 7000 + 100 * (nSstWaferPerLadder - wafer[n]) + ladder + 1;
            strip_number[n] = strip[n] + 1; //strip[n]+1 . in mapping, strip[1-128];
         }
         else {
            id_wafer[n] = 7000 + 100 * ((wafer[n]) + 1) + ladder + 1;
            strip_number[n] = nSstStripsPerWafer - strip[n];
         }

         out_strip.id          = count;
         out_strip.adc_count   = data[n];
         out_strip.id_strip    = 10000 * (10 * strip_number[n] + id_side) + id_wafer[n]; //id_side:0-->p,1-->N
         out_strip.id_mchit[0] = 0 ;
         out_strip.id_mchit[1] = 0 ;
         out_strip.id_mchit[2] = 0 ;
         out_strip.id_mchit[3] = 0 ;
         out_strip.id_mchit[4] = 0 ;

         spa_strip->AddAt(&out_strip);

         if (id_side == 0) {
            ladderCountP[ladder]++;
         }
         else {
            ladderCountN[ladder]++;
         }

         count = count + 1;
      }
   }

   LOG_DEBUG << "Last readout number: [readout[0] , readout[1] , readout[2]] = [" << readout[0] << ","
             << readout[1] << "," << readout[2] << "]" << endm;

   if (readout[0] > nSstStripsPerWafer || readout[1] > nSstStripsPerWafer || readout[2] > nSstStripsPerWafer) {
      LOG_WARN << "Strip number is larger than 768." << endm;
      return;
   }

   if (StMaker::GetDebug()) {
      LOG_DEBUG << "Make()/Counts (p-side): ";

      for (Int_t i = 0; i < nSstLadder; i++) {
         LOG_DEBUG.width(5);
         LOG_DEBUG << ladderCountP[i] << " ";
      }

      LOG_DEBUG << endm;
      LOG_DEBUG << "Make()/Counts (n-side): ";

      for (Int_t i = 0; i < nSstLadder; i++) {
         LOG_DEBUG.width(5);
         LOG_DEBUG << ladderCountN[i] << " ";
      }

      LOG_DEBUG << endm;

      if (spa_strip->GetNRows() != 0) {
         LOG_DEBUG << "Make()/ Read Signal from Physics Run" << endm;
         LOG_DEBUG << "Make()/  spa_strip->NRows= " << spa_strip->GetNRows() << endm;
      }
   }

   //delete spa_strip;
}
//-------------------------------------------------
void StSstDaqMaker::DecodeRawWords_r15(UInt_t *val, Int_t vallength, Int_t channel)
{
   vector<vector<int> > vadc(16, vector<int>(768)); //2-D Vector
   vector<vector<float> > vcmnoise(16, vector<float>(6)); //Save CM Noise
   vector<int> vtemp(128);

   for (int i = 0; i < 16; i++) {
      vadc[i].resize(768);
      vcmnoise[i].resize(6);
   }

   Int_t data[3]            = {0};
   Int_t wafer[3]           = {0};
   Int_t hybrid[3]          = {0};
   Int_t strip[3]           = {0};
   Int_t readout[3]         = {0};
   Int_t readout_correct[3] = {0};
   Int_t ladder             = 0;
   Int_t id_side            = 0;
   Int_t count              = 0;
   Int_t readoutindex       = 0;

   count = 1;
   LOG_DEBUG << "DECODING RAW MODE data....." << endm;
   //grab ladder and side
   FindLadderSide(mRDO, channel, ladder, id_side);

   for (Int_t i = 0; i < vallength; i++) {
      if (i == 0) hybrid[0] = 0;
      else hybrid[0] = hybrid[2] + 1;

      if (hybrid[0] >= 16) hybrid[0] = hybrid[0] % 16;

      hybrid[1] = hybrid[0] + 1;

      if (hybrid[1] >= 16) hybrid[1] = hybrid[1] % 16;

      hybrid[2] = hybrid[0] + 2;

      if (hybrid[2] >= 16) hybrid[2] = hybrid[2] % 16;

      LOG_DEBUG << "Three hybrid number in current word is :"
                << hybrid[0] << "," << hybrid[1] << "," << hybrid[2] << endm;

      if (i != 0) {
         if (hybrid[0] == 0) readout[0] = readout[0] + 1;

         if (readout[2] > readout[0]) readout[0] = readout[2];

         if (hybrid[1] == 0) readout[1] = readout[1] + 1;

         if (readout[0] > readout[1]) readout[1] = readout[0];

         if (hybrid[2] == 0) readout[2] = readout[2] + 1;

         if (readout[1] > readout[2]) readout[2] = readout[1];
      }

      LOG_DEBUG << "[adc_pointer,readout_0,hybrid_0] = [" << i << ","
                << readout[0] << "," << hybrid[0] << "]" << endm;
      LOG_DEBUG << "[adc_pointer,readout_1,hybrid_1] = [" << i << ","
                << readout[1] << "," << hybrid[1] << "]" << endm;
      LOG_DEBUG << "[adc_pointer,readout_2,hybrid_2] = [" << i << ","
                << readout[2] << "," << hybrid[2] << "]" << endm;

      data[0] = Mid(HYBRID_ONE_START, HYBRID_ONE_END, val[i]);
      data[1] = Mid(HYBRID_TWO_START, HYBRID_TWO_END, val[i]);
      data[2] = Mid(HYBRID_THREE_START, HYBRID_THREE_END, val[i]);

      for (Int_t n = 0; n < 3; n++) {
         readout_correct[n] = readout[n];

         if (!Shift(mEventrunumber, readout_correct[n])) {
            LOG_DEBUG << "First readout channel in old LC FPGA is not usable." << endm;
            continue;
         }

         strip[n] = readout_correct[n];
         wafer[n] = hybrid[n];
         readoutindex = id_side * nSstLadder * nSstWaferPerLadder * nSstStripsPerWafer + ladder * nSstWaferPerLadder * nSstStripsPerWafer + wafer[n] * nSstStripsPerWafer + strip[n];

         if (data[n] > 700) data[n] = data[n] - 1024 - mReadOutPed[readoutindex]; //rape-around correction.
         else
            data[n] = data[n] - mReadOutPed[readoutindex];

         vadc[wafer[n]][strip[n]] = data[n];
      }
   }

   for (int i = 0; i < 16; i++) {
      for (int j = 0; j < 768; j++) {
         vtemp[j % 128] = vadc[i][j];

         if ((j + 1) % 128 == 0) {
            vcmnoise[i][j / 128] = CalculateCommonModeNoiseSimple(vtemp);
         }
      }
   }

   FillData(vadc, vcmnoise, id_side, ladder, vallength);

   LOG_DEBUG << "Last readout number: [readout[0] , readout[1] , readout[2]] = [" << strip[0] << ","
             << strip[1] << "," << strip[2] << "]" << endm;

   if (strip[0] > nSstStripsPerWafer || strip[1] > nSstStripsPerWafer || strip[2] > nSstStripsPerWafer) {
      LOG_WARN << "Strip number is larger than 768." << endm;
      return;
   }

   vadc.clear();
   vcmnoise.clear();
   vtemp.clear();
}
//-------------------------------------------------
void StSstDaqMaker::DecodeCompressedWords(UInt_t *val, Int_t vallength, Int_t channel)
{
   Int_t ladderCountN[20] = {0};
   Int_t ladderCountP[20] = {0};
   Int_t strip_number     = 0;
   Int_t id_side          = 0;
   Int_t id_wafer         = 0;
   Int_t count            = 0;
   Int_t data             = 0;
   Int_t wafer            = 0;
   Int_t strip            = 0;
   Int_t readout          = 0;
   Int_t ladder           = 0;
   Int_t chip             = 0;
   Int_t chipIndex        = 0;
   LOG_INFO << "Current Event data length : " << vallength << endm;
   //St_spa_strip *spa_strip = (St_spa_strip *) m_DataSet->Find("spa_strip");
   spa_strip = (St_spa_strip *) m_DataSet->Find("spa_strip");

   if (!spa_strip) {
      spa_strip   = new St_spa_strip("spa_strip", vallength);
      m_DataSet->Add(spa_strip);
   }

   spa_strip_st  out_strip;
   count = 1;
   //grab ladder and side
   FindLadderSide(mRDO, channel, ladder, id_side); //convert channel to real Ladder number and side

   for (Int_t i = 0; i < vallength; i++) {
      wafer = Mid(HYBRID_START, HYBRID_END, val[i]);
      strip = Mid(STRIP_START, STRIP_END, val[i]);
      data  = Mid(COM_ADC_START, COM_ADC_END, val[i]);

      if (!Shift(mEventrunumber, strip)) {
         LOG_INFO << "First readout channel in old LC FPGA is not usable." << endm;
         continue;
      }

      if(mEventrunumber<16000000) {
	//chip correction 
	chip = strip/nSstStripsPerChip;
	chipIndex = ladder*nSstWaferPerLadder*nSstChipPerWafer + wafer*nSstChipPerWafer + chip;
      
	if(data<mNoiseCut[chipIndex][id_side]) data = 0; //remove noise.
	else data = data - mCorrectFactor[chipIndex][id_side]; //data correction.
      }

      readout = strip;

      FindStripNumber(strip);//convert to physic strip number

      if (id_side == 0) {
         id_wafer = 7000 + 100 * (nSstWaferPerLadder - wafer) + ladder + 1;
         strip_number = strip + 1;
      }
      else {
         id_wafer = 7000 + 100 * ((wafer) + 1) + ladder + 1;
         strip_number = nSstStripsPerWafer - strip;
      }

      out_strip.id          = count;
      out_strip.adc_count   = data;
      out_strip.id_strip    = 10000 * (10 * strip_number + id_side) + id_wafer;
      out_strip.id_mchit[0] = 0 ;
      out_strip.id_mchit[1] = 0 ;
      out_strip.id_mchit[2] = 0 ;
      out_strip.id_mchit[3] = 0 ;
      out_strip.id_mchit[4] = 0 ;
      spa_strip->AddAt(&out_strip);

      if (id_side == 0) {
         ladderCountP[ladder]++;
      }
      else {
         ladderCountN[ladder]++;
      }

      count = count + 1;
   }

   if (readout > nSstStripsPerWafer) {
      LOG_WARN << "Strip number is larger than 768, ERROR" << endm;
      return;
   }

   if (StMaker::GetDebug()) {
      LOG_DEBUG << "Make()/Counts (p-side): ";

      for (Int_t i = 0; i < nSstLadder; i++) {
         LOG_DEBUG.width(5);
         LOG_DEBUG << ladderCountP[i] << " ";
      }

      LOG_DEBUG << endm;
      LOG_DEBUG << "Make()/Counts (n-side): ";

      for (Int_t i = 0; i < nSstLadder; i++) {
         LOG_DEBUG.width(5);
         LOG_DEBUG << ladderCountN[i] << " ";
      }

      LOG_DEBUG << endm;

      if (spa_strip->GetNRows() != 0) {
         LOG_DEBUG << "Make()/ Read Signal from Physics Run" << endm;
         LOG_DEBUG << "Make()/  spa_strip->NRows= " << spa_strip->GetNRows() << endm;
      }
   }

   //delete spa_strip;
}

//-------------------------------------------------
Int_t StSstDaqMaker::Shift(Int_t runnumber, Int_t &channel)
{
   if (runnumber < 15150058) {
      if (channel == 0) return 0;
      else {
         channel = channel - 1;
         return 1;
      }
   }
   else {
      return 1;
   }
}
//-------------------------------------------------
UInt_t StSstDaqMaker::Mid(Int_t start, Int_t end, UInt_t input)
{
   UInt_t buf;
   buf = input << (32 - end);
   buf = buf >> (32 - end);
   return buf >> start;
}

//--------------------------------------------------
void StSstDaqMaker::FindLadderSide(Int_t RDO, Int_t channel, Int_t &ladder, Int_t &side)
{
   ladder = RDO2LADDER[RDO - 1][channel] - 1; //ladder [0-19];

   if (ladder > 20) LOG_WARN << "Ladder >20. RDO Number is" << RDO << ", channel number :" << channel << endm;

   if (RDO < 3) side = 1;
   else if (RDO > 3) side = 0;
   else if (channel < 4) side = 1;
   else side = 0;
}
//--------------------------------------------------
void StSstDaqMaker::FindStripNumber(Int_t &strip)
{
   Int_t temp = (strip / 128) * 128 + ReadOutMap[strip % 128];
   strip = temp - 1;
}

//--------------------------------------------------

void StSstDaqMaker::PrintConfiguration(Int_t runumber, ssdConfiguration_st *config)
{
   switch (runumber) {
      case 4 : {
         LOG_INFO << "Configuration of ladders for run IV" << endm;
         break;
      }

      case 5 : {
         LOG_INFO << "Configuration of ladders for run V" << endm;
         break;
      }

      case 6 : {
         LOG_INFO << "Configuration of ladders  for run VI" << endm;
         break;
      }

      case 7 : {
         LOG_INFO << "Configuration of ladders  for run VII" << endm;
         break;
      }
   }

   Int_t i = 0;
   Int_t totladderPresent = 0;
   LOG_INFO << "PrintLadderSummary:ladder id                 :";

   for (i = 1; i <= config->nMaxLadders; i++) {
      LOG_INFO.width(3);
      LOG_INFO << i;
   }

   LOG_INFO << endm;
   LOG_INFO << "PrintLadderSummary:Active Ladders on sectors: ";

   for (i = 1; i <= config->nMaxLadders; i++) {
      LOG_INFO.width(3);
      LOG_INFO << mConfig->getLadderIsActive(i);

      if (mConfig->getLadderIsActive(i) > 0)totladderPresent++;

   }

   LOG_INFO << endm;
   LOG_INFO << "totLadderActive = " << totladderPresent << endm;
}
//------------------------------------------------
void StSstDaqMaker::FillData(vector<vector<int> > vadc, vector<vector<float> > vcmnoise, Int_t id_side, Int_t ladder, Int_t vallength)
{
   Int_t   id_wafer         = 0;
   Int_t   strip_number     = 0;
   Int_t   count            = 1;
   Int_t   ladderCountN[20] = {0};
   Int_t   ladderCountP[20] = {0};
   Float_t adc              = 0;
   Float_t cmnoise          = 0;
   Int_t   intrinsicnoise   = 0;
   Int_t   stripindex       = 0;

   const Float_t Epsinon    = 0.00001;
   spa_strip = (St_spa_strip *) m_DataSet->Find("spa_strip");

   if (!spa_strip) {
      spa_strip   = new St_spa_strip("spa_strip", vallength);
      m_DataSet->Add(spa_strip);
   }

   spa_strip_st out_strip;

   for (int i = 0; i < 16; i++) {
      for (int j = 0; j < 768; j++) {
         Int_t s = j;
         adc = (Float_t)vadc[i][j];
         cmnoise = vcmnoise[i][j / 128];

         //----------------
         //Temporary masking

         if ((cmnoise >= -Epsinon) && (cmnoise <= Epsinon)) adc = 0;
         else {
            adc = adc - cmnoise;
         }

         if (j % 128 == 0) adc = 0; //first channel is not useable.

         if (id_side == 1) adc = -1 * adc; //Reverse N-side Signal Charge Sign.

         FindStripNumber(s);

         //---------------
         //hotchip masking
         if (gStSstDbMaker->chipHot(id_side, ladder, i, j / 128)) continue;

         if (id_side == mReverse) {
            id_wafer = 7000 + 100 * (nSstWaferPerLadder - i) + ladder + 1;
            strip_number = s + 1;
         }
         else {
            id_wafer = 7000 + 100 * (i + 1) + ladder + 1;
            strip_number = nSstStripsPerWafer - s;
         }

         //-------------------------
         //Offline Zero-Suppression
         stripindex = id_side * nSstLadder * nSstWaferPerLadder * nSstStripsPerWafer + ladder * nSstWaferPerLadder * nSstStripsPerWafer + i * nSstStripsPerWafer + s;
         intrinsicnoise = mIntrinsicRms[stripindex];

         if (adc < 3 * intrinsicnoise) continue;

         //mask dead strips.
         //if(intrinsicnoise<=1) continue;

         out_strip.id          = count;
         out_strip.adc_count   = adc; //Minus the Common mode noise
         out_strip.id_strip    = 10000 * (10 * strip_number + id_side) + id_wafer; //id_side:0-->p,1-->N
         out_strip.id_mchit[0] = 0 ;
         out_strip.id_mchit[1] = 0 ;
         out_strip.id_mchit[2] = 0 ;
         out_strip.id_mchit[3] = 0 ;
         out_strip.id_mchit[4] = 0 ;

         spa_strip->AddAt(&out_strip);

         if (id_side == 0) {
            ladderCountP[ladder]++;
         }
         else {
            ladderCountN[ladder]++;
         }
      }
   }

   if (StMaker::GetDebug()) {
      LOG_DEBUG << "Make()/Counts (p-side): ";

      for (Int_t i = 0; i < nSstLadder; i++) {
         LOG_DEBUG.width(5);
         LOG_DEBUG << ladderCountP[i] << " ";
      }

      LOG_DEBUG << endm;
      LOG_DEBUG << "Make()/Counts (n-side): ";

      for (Int_t i = 0; i < nSstLadder; i++) {
         LOG_DEBUG.width(5);
         LOG_DEBUG << ladderCountN[i] << " ";
      }

      LOG_DEBUG << endm;

      if (spa_strip->GetNRows() != 0) {
         LOG_DEBUG << "Make()/ Read Signal from Physics Run" << endm;
         LOG_DEBUG << "Make()/  spa_strip->NRows= " << spa_strip->GetNRows() << endm;
      }
   }
}

//------------------------------------------------
Float_t StSstDaqMaker::CalculateCommonModeNoise(vector<int> vadc)
{
   Float_t preSum  = 0;
   Float_t preMean = 0;
   Float_t sum     = 0;
   Float_t devcut  = 20; //deviation cut, default is 10 ADC
   Int_t   counter = 0;
   //For Masking
   TH1I *hadc = new TH1I("hadc", "adc in each chip", 768, -512, 1024);
   Float_t chipRms  = 0;

   for (int i = 1; i < 128; i++) { //index start from 1 is to avoid the edge effect.
      hadc->Fill(vadc[i]);
   }

   chipRms = hadc->GetRMS();

   if (chipRms > 15) {
      hadc->Delete();
      return 0.; //if Raw signal in one chip have very large RMS, we will mark it to bad
   }

   hadc->Delete();

   //----------------------
   //Self-tunning algorithm
   for (int i = 17; i < 49; i++) {
      preSum += vadc[i];
   }

   Float_t preThreshold = preSum / 32 + 50;
   preSum = 0;

   for (int i = 17; i < 49; i++) {
      if (vadc[i] > preThreshold) continue;

      counter = counter + 1;
      preSum += vadc[i];
   }

   preMean = preSum / counter;

   counter = 0;
   Float_t thresholdhig = preMean + devcut;
   Float_t thresholdlow = preMean - devcut;

   for (int i = 49; i <= 112; i++) {
      if (vadc[i] < thresholdlow || vadc[i] > thresholdhig) continue;
      else {
         sum += vadc[i];
         counter = counter + 1;
      }
   }

   LOG_INFO << "preMean:Counter = " << preMean << "," << counter << endm;
   return sum / counter;
}
//------------------------------------------------
Float_t StSstDaqMaker::CalculateCommonModeNoiseSimple(vector<int> vadc) //Simplify algorithm
{
   Float_t preSum  = 0;
   Float_t preMean = 0;
   Float_t sum     = 0;
   Float_t devcut  = 20; //deviation cut, default is 10 ADC
   Int_t   counter = 0;
   //For Masking
   TH1I *hadc = new TH1I("hadc", "adc in each chip", 768, -512, 1024);
   Float_t chipRms  = 0;

   for (int i = 1; i < 128; i++) { //index start from 1 is to avoid the edge effect.
      hadc->Fill(vadc[i]);
   }

   chipRms = hadc->GetRMS();

   if (chipRms > 15) {
      hadc->Delete();
      return 0.; //if Raw signal in one chip have very large RMS, we will mark it to bad
   }

   hadc->Delete();

   //------------------------------
   //upgraded Fix Threshold method.
   for (int i = 1; i < 128; i++) {
      preSum += vadc[i];
   }

   preMean = preSum / 127.;

   Float_t thresholdhig = preMean + devcut;
   Float_t thresholdlow = preMean - devcut;

   for (int i = 1; i < 128; i++) {
      if (vadc[i] < thresholdlow || vadc[i] > thresholdhig) continue;

      sum += vadc[i];
      counter = counter + 1;
   }

   return sum / counter;
}
//------------------------------------------------
void StSstDaqMaker::FillReadOutPedTable()
{
   strip_calib = (St_ssdStripCalib *) GetDataBase("Calibrations/ssd/ssdStripCalib");

   if (!strip_calib) {LOG_ERROR << "InitRun : No access to ssdStripCalib table - we will use the default noise and pedestal values" << endm;}
   else {
      LOG_INFO << "We will use the default Pedestal(=0) and Rms(=0) " << endm;
   }

   ssdStripCalib_st *noise = strip_calib->GetTable();

   Int_t side    = 0;
   Int_t ladder  = 0;
   Int_t wafer   = 0;
   Int_t idwaf   = 0;
   Int_t channel = 0;
   Int_t chip    = 0;
   Int_t ped     = 0;
   Int_t index   = 0;

   for (Int_t i = 0 ; i < strip_calib->GetNRows(); i++) {
      channel = (int)(noise[i].id / 100000.);
      chip    = (channel - 1) / 128;
      idwaf   = noise[i].id - 10000 * ((int)(noise[i].id / 10000.));
      wafer   = idWaferToWafer(idwaf);
      ladder  = idWaferToLadderNumb(idwaf);
      side    = (noise[i].id - channel * 100000 - idwaf) / 10000;
      index   = side * nSstLadder * nSstWaferPerLadder * nSstStripsPerWafer + ladder * nSstWaferPerLadder * nSstStripsPerWafer + wafer * nSstStripsPerWafer + channel;
      ped     = noise[i].pedestals;
      mReadOutPed[index] = ped;
   }

}
//------------------------------------------------
void StSstDaqMaker::FillDefaultReadOutPedTable()
{
   LOG_DEBUG << " Default is no Pedestal subtraction ." << endm;
   Int_t size = nSstSide * nSstLadder * nSstWaferPerLadder * nSstStripsPerWafer;

   for (Int_t i = 0; i < size; i++) {
      mReadOutPed[i] = 0;
   }
}
//------------------------------------------------
void StSstDaqMaker::FillNoiseTable()
{
   mNoise = (St_ssdNoise *)GetDataBase("Calibrations/ssd/ssdNoise");

   if (!mNoise) {LOG_ERROR << "InitRun : No access to ssdNoise - will use the default noise and pedestal values" << endm;}
   else {
      LOG_INFO << "InitRun for real data : new Table(ssdNoise) is used" << endm;
   }

   ssdNoise_st *noise = mNoise->GetTable();
   Int_t iWaf          = 0;
   Int_t iLad          = 0;
   Int_t iSide         = 0;
   Int_t index         = 0;
   printf("size of mNoise table = %d\n", (int)mNoise->GetNRows());

   for (Int_t i = 0 ; i < mNoise->GetNRows(); i++) {
      iWaf      = noise[i].id - (noise[i].id / nSstWaferPerLadder) * nSstWaferPerLadder;
      iLad      = noise[i].id / 16;

      for (int s = 0; s < nSstStripsPerWafer; s++) {
         //P-Side
         iSide = 0;
         index = iSide * nSstLadder * nSstWaferPerLadder * nSstStripsPerWafer + iLad * nSstWaferPerLadder * nSstStripsPerWafer + iWaf * nSstStripsPerWafer + s;
         mIntrinsicRms[index] = (Int_t)noise[i].rmsp[s];

         //N-Side
         iSide = 1;
         index = iSide * nSstLadder * nSstWaferPerLadder * nSstStripsPerWafer + iLad * nSstWaferPerLadder * nSstStripsPerWafer + iWaf * nSstStripsPerWafer + s;
         mIntrinsicRms[index] = (Int_t)noise[i].rmsn[s];

      }
   }
}
//------------------------------------------------
void StSstDaqMaker::FillDefaultNoiseTable()
{
   LOG_DEBUG << " Default is no Pedestal subtraction ." << endm;
   Int_t size = nSstSide * nSstLadder * nSstWaferPerLadder * nSstStripsPerWafer;

   for (Int_t i = 0; i < size; i++) {
      mIntrinsicRms[i] = 0;
   }
}
//------------------------------------------------
void StSstDaqMaker::FillChipNoiseTable(){
  mChipCorrect = (St_ssdChipCorrect*)GetDataBase("Calibrations/ssd/ssdChipCorrect");
  if(mChipCorrect){
    ssdChipCorrect_st *g  = mChipCorrect->GetTable() ;
    Int_t size = mChipCorrect->GetNRows();
    LOG_INFO<<"Size of gain table = "<<mChipCorrect->GetNRows()<<endm;
    for(Int_t i=0; i<size;i++){
      LOG_DEBUG<<Form(" Print entry %d : ladder=%d wafer=%d chip=%d side=%d correction=%d iter=%d",i,g[i].nLadder,g[i].nWafer,g[i].nChip,g[i].nSide,g[i].nCorrect,g[i].nLadder*96+g[i].nWafer*6+g[i].nChip)<<endm;
      mCorrectFactor[g[i].nLadder*96+g[i].nWafer*6+g[i].nChip][g[i].nSide] = g[i].nCorrect;
      mNoiseCut[g[i].nLadder*96+g[i].nWafer*6+g[i].nChip][g[i].nSide] = g[i].nCutPos;
    }
  }
}
//------------------------------------------------
void StSstDaqMaker::FillDefaultChipNoiseTable(){
  LOG_DEBUG <<" Default is no any correction ." << endm;
  Int_t size = nSstLadder*nSstWaferPerLadder*nSstChipPerWafer;
  for(Int_t s=0;s<nSstSide;s++) {
    for(Int_t i=0;i<size;i++) {
      mCorrectFactor[i][s] = 0;
      mNoiseCut[i][s]      = 0;
    }
  }
}
//------------------------------------------------
Int_t StSstDaqMaker::idWaferToWafer(Int_t idWafer)
{
   return (idWafer - 7000) / 100 - 1;
}
//------------------------------------------------
Int_t StSstDaqMaker::idWaferToLadderNumb(Int_t idWafer)
{
   Int_t iW = (int)((idWafer - 7 * 1000) / 100);
   Int_t iL = idWafer - 7 * 1000 - iW * 100;
   return iL - 1;
}
//------------------------------------------------
Int_t StSstDaqMaker::Finish()
{
   LOG_INFO << Form("Finish()") << endm;
   return kStOK;
}
//------------------------------------------------
void StSstDaqMaker::Clear(const Option_t *)
{
   if (spa_strip) {
      delete spa_strip;
      spa_strip = 0;
   }

   if (ssdPedStrip) {
      delete ssdPedStrip;
      ssdPedStrip = 0;
   }

   return StMaker::Clear();
}
