/*!
 * \class StSstDaqMaker
 * \author Long Zhou, Nov 2013, according codes from PXL
 * \Initial Revision.
 */
/***************************************************************************
 *
 * $Id$
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
 * $Log$
 * Revision 1.2  2014/04/22 17:19:17  zhoulong
 * This version solved one bug(too many token error in one event)
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
//#include "StChain/StRtsTable.h"

#include "StEventTypes.h"  //StEvent/StEventTypes.h
#include "StMessMgr.h"
#include "StRtsTable.h"
#include "StSsdDbMaker/StSsdDbMaker.h"

#include "tables/St_spa_strip_Table.h"
#include "tables/St_ssdConfiguration_Table.h"
#include "tables/St_ssdPedStrip_Table.h"
#include "StSsdUtil/StSsdConfig.hh"
#include "TH1.h"
#include "TH2.h"
#include "TH3.h"
#include "TFile.h"
#include "TNtuple.h"
#include "StIOMaker/StIOMaker.h"
#include <map>

ClassImp(StSstDaqMaker)

#define HEADER_LENGTH        8
#define FIBER_HEADER_LENGTH  10

#define HEADER_TOKEN         0xAAAAAAAA
#define END_TOKEN            0xBBBBBBBB
#define TCD_TOKEN            0xCCCCCCCC
#define FIBER_LINK_TOKEN     0xDDDDDDDD
#define TCD_END_TOKEN        0xEEEEEEEE

#define RDO_START            24//RDO Number Start bit
#define RDO_END              28//RDO Number End bit
#define TRIG_START           0 //TCD trigger word start
#define TRIG_END             20//TCD trigger word end  
#define FIBER_START          28//Fiber Input Start bit
#define FIBER_END            31//Fiber Input End bit
#define HYBRID_ONE_START     0 //Hyirid 0 start bit
#define HYBRID_ONE_END       10//Hyirid 0 end bit
#define HYBRID_TWO_START     10
#define HYBRID_TWO_END       20
#define HYBRID_THREE_START   20
#define HYBRID_THREE_END     30
#define HYBRID_START         10
#define HYBRID_END           14
#define STRIP_START          14
#define STRIP_END            24
#define ADC_START            4    //adc length start bit
#define ADC_END              20   //adc length end bit
#define DATAMODE_START       0    //adc mode start bit
#define DATAMODE_END         4    //adc mode end bit
#define RAWMODE              0  //0x0 this is raw data mode
#define COMPRESSEDMODE       1  //0x1 this is compressed mode
#define FLAG_START           20     //flag start bit
#define FLAG_END             32     //flag end bit
#define NORMAL               0  //0x000  //normal flag
#define NODATA               1  //0x001  //no data
#define OVERFLOW             2  //0x002  //over flow
#define EARLY_ABORT          3  //0x003  //early abort 
#define WRONG_PIPE_MODE      4  //0x004  //wrong pipe mode
#define ADC_LENGTH_LIMIT     4106   //adc length limit,include fiber header
StSstDaqMaker::StSstDaqMaker(const Char_t *name):StRTSBaseMaker("sst",name)
{

}
//-----------------------------------------------                                        
Int_t StSstDaqMaker::InitRun(int runumber)
{
  LOG_INFO <<"InitRun(int runumber) - Read now Databases"<<endm;
  Int_t run = (runumber/1000000)-1;

  St_ssdConfiguration *configuration = (St_ssdConfiguration*) GetDataBase("Geometry/ssd/ssdConfiguration");
  if (!configuration){
    LOG_ERROR << "InitRun("<<runumber<<") - ERROR - ssdConfiguration==0"<<endm;
    return 0;
  }
  ssdConfiguration_st *config  = (ssdConfiguration_st*) configuration->GetTable() ;
  if (!config){
    LOG_ERROR <<"InitRun("<<runumber<<") - ERROR - config==0"<<endm;
    return 0;
  }

  mConfig = new StSsdConfig();

  int totLadderPresent = 0;

  for (int ladder = 1; ladder<=config->nMaxLadders;ladder++)
    {
      LOG_INFO<< " on sector = " << config->ladderIsPresent[ladder-1];
      if (config->ladderIsPresent[ladder-1] != 0)
        totLadderPresent++;
      mConfig->setLadderIsActive(ladder,config->ladderIsPresent[ladder-1]);
    }
  PrintConfiguration(run,config);
  mConfig->setNumberOfLadders(totLadderPresent);
  mConfig->setNumberOfWafers(config->nMaxWafers/config->nMaxLadders);
  mConfig->setNumberOfHybrids(2);
  mConfig->setTotalNumberOfHybrids(2*16*totLadderPresent);
  mConfig->setTotalNumberOfLadders(config->nMaxLadders);
  mConfig->setNumberOfStrips(768);

  mConfig->setConfiguration();

  mPedOut = 0;
  if(mPedOut)
    {
      DeclareNTuple();
    }
  eventnumber = 0;

  LOG_INFO << "_____________________________" << endm;
  LOG_INFO << "       Via  Datababase......." << endm;
  LOG_INFO << ".......numberOfSectors =     " << config->nMaxSectors << endm;
  LOG_INFO << ".......numberOfLadders =     " << totLadderPresent << endm;
  LOG_INFO << " .numberOfWafersPerLadder =  " << config->nMaxWafers/config->nMaxLadders << endm;
  LOG_INFO << "_____________________________" << endm;
  LOG_INFO << "      InitRun() - Done       "<<endm;

  LOG_INFO<<"StSstDaqMaker initialization..."<<endm;
}

//-----------------------------------------------
Int_t StSstDaqMaker::Init()
{
  LOG_INFO<<"StSstDaqMaker initialization..."<<endm;
 
  if(IAttr(".histos"))
    {
      for(int i=0;i<2;i++)
	{
	  for(int j=0;j<20;j++)
	    {
	      AdcStrip[i][j] = new TH2I(Form("AdcStrip%d%d",i,j),"Adc vs strip for each ladder;(Strip #)#times(Wafer #));Adc Value",12288,0,12288,1030,0,1030);	  
	    }
	}
    
    }
  LOG_INFO << "Init() - Done " << endm;
  return kStOk;
}
//------------------------------------------------
void StSstDaqMaker::DeclareNTuple(){
  string varlist = "eventno:RDO:fiber:side:ladder:wafer:strip:channel:ped:rms";
  pTuple     = new TNtuple("PedestalNTuple","Pedestal Ntuple",varlist.c_str());
  LOG_INFO << "StSsdDaqMaker::DeclareNtuple - Done - "<<endm;
}
//-------------------------------------------------
Int_t StSstDaqMaker::Make()
{
  LOG_INFO<<"StSstDaqMaker Start Make..."<<endm;
  StRtsTable *rts_table = 0;
  eventnumber++;
  //raw data mode 
  while(rts_table = GetNextDaqElement("sst/raw"))
    { //each DAQ element record one RDO data
      //one event will loop all the 5 RDOs
      m_mode = 0;//0-physic run,1-pedestal run
      m_rdoData = (UInt_t*)rts_table->At(0);
      m_rdoDataLength = rts_table->GetNRows();
      LOG_INFO<< "Found sst RDO raw data at "<<hex<<m_rdoData<<dec<<" , length in byte: "<<m_rdoDataLength<<" in UInt_t: "<<m_rdoDataLength/sizeof(UInt_t)<<endm;
      DecodeRdoData();
      DecodeHitsData();
    }

  //pedestal mode 
  
  while(rts_table = GetNextDaqElement("sst/pedrms"))
    { 
      m_mode    = 1;
      m_rdoData = (UInt_t *)rts_table->At(0);
      m_rdoDataLength = rts_table->GetNRows();
      LOG_INFO<< "Found sst Pedestal Run data at "<<hex<<m_rdoData<<dec<<" , length in byte: "<<m_rdoDataLength<<endm;
      int flag = 0;
      St_ssdPedStrip *ssdPedStrip = (St_ssdPedStrip *) m_DataSet->Find("ssdPedStrip");
      if(!ssdPedStrip)
	{
	  ssdPedStrip = new St_ssdPedStrip("ssdPedStrip",100000);
	  m_DataSet->Add(ssdPedStrip);
	}
      ssdPedStrip_st out_ped_strip;
      Int_t id_side,ladder,strip_number,id_wafer;
      int ladderCountN[20]={0};
      int ladderCountP[20]={0};
      Int_t count = 1;    
      
      m_sec     = rts_table->Sector();
      m_RDO     = rts_table->Rdo();
      m_fiber   = rts_table->Pad();

      if(m_sec==1) m_RDO = m_RDO; //sector 1
      else m_RDO = m_RDO + 3; //sector 2
      if(m_RDO<1 || m_RDO>5)     flag = 1; 
      if(m_sec<1 || m_sec>3)     flag = 1;
      if(m_fiber<0 || m_fiber>7) flag = 1;
      if(flag==1)
	{
	  LOG_WARN<<"BAD pedestal data. Sector: "<<m_sec<<" RDO: "<<m_RDO<<" fiber: "<<m_fiber<<endm;
	  continue;
	}
      FindLadderSide(m_RDO,m_fiber,ladder,id_side);
  
      for(StRtsTable::iterator it=rts_table->begin();it!=rts_table->end();it++)
	{
	  daq_sst_pedrms_t *f = (daq_sst_pedrms_t *)*it;
	  for(int h=0;h<nSstWaferPerLadder;h++)
	    { //wafer
	    for(int c=0;c<nSstStripsPerWafer;c++)
	      {//channel
		int s = c;
		FindStripNumber(s);
		m_ped  = (Float_t)f->ped[h][c];
		m_rms  = (Float_t)f->rms[h][c]/16.;
		strip_number=s+1;
		if (id_side==0)
		  id_wafer=7000+100*(nSstWaferPerLadder-h)+ladder+1;
		else
		  id_wafer=7000+100*(h+1)+ladder+1;
		out_ped_strip.id         = count;
		out_ped_strip.id_strip   = 10000*(10*strip_number+id_side)+id_wafer;
		out_ped_strip.noise      = m_rms;
		out_ped_strip.pedestal   = m_ped;
		ssdPedStrip->AddAt(&out_ped_strip);
		count = count + 1;

		if(mPedOut){
		  PedestalNTuple[0]=eventnumber;
		  PedestalNTuple[1]=m_RDO;
		  PedestalNTuple[2]=m_fiber;
		  PedestalNTuple[3]=id_side;
		  PedestalNTuple[4]=ladder;
		  PedestalNTuple[5]=h;
		  PedestalNTuple[6]=s;
		  PedestalNTuple[7]=c;
		  PedestalNTuple[8]=m_ped;
		  PedestalNTuple[9]=m_rms;
		  pTuple->Fill(PedestalNTuple);
		}
		if(id_side ==0) ladderCountP[ladder]++;
		else            ladderCountN[ladder]++;
	      }
	    }
	}
      LOG_DEBUG << "Make()/Counts (p-side): ";
      for (int i=0;i<nSstLadder;i++){ 
	LOG_DEBUG.width(5);
	LOG_DEBUG <<ladderCountP[i]<<" ";
      } 
      LOG_DEBUG << endm;
      LOG_DEBUG << "Make()/Counts (n-side): "; 
      for (int i=0;i<nSstLadder;i++){
	LOG_DEBUG.width(5);
	LOG_DEBUG <<ladderCountN[i]<<" ";
      } 
      LOG_DEBUG<<endm;    
  
      if(ssdPedStrip && ssdPedStrip->GetNRows()!=0){
	LOG_DEBUG << "Make()/ Read Pedestal & Noise"<< endm;
	LOG_DEBUG << "Make()/ssdPedStrip->NRows= "<<ssdPedStrip->GetNRows()<<endm;
      }

    }
  return kStOk;
}
//-------------------------------------------------
void StSstDaqMaker::DecodeRdoData()
{
  int index = 0;
  m_RDO = 0;
  m_trigger = 0;
  m_headerData = 0;
  m_trailerData = 0;
  m_trailerDataLength = 0;
  m_rdoflag=1;

  for(int f=0;f<8;f++) m_fiberflag[f]=1;//flag=0-->bad, flag=1-->good  
  if(m_rdoDataLength==0 || !m_rdoData) {
    LOG_WARN<<"NO RDO OUTPUT DATA!"<<endm;
    m_rdoflag=0;
    return;
  }

  if(m_rdoData[0]!=HEADER_TOKEN)
    {
      LOG_WARN<<"SST DAQ DATA HEADER_TOKEN is not correct!!"<<endm;
      m_rdoflag=0;
      return;
    }
  else
    {
      LOG_DEBUG<<"SST DAQ DATA HEADER_TOKEN correct: 0x"<<hex<<m_rdoData[0]<<dec<<endm;
    }
  m_headerData = m_rdoData + index;
 
  for(int h=0;h<HEADER_LENGTH;h++)
    {
      if(h==0) LOG_DEBUG<<"#Start Print Event HEADER info... "<<endm;
      LOG_DEBUG<<"#"<<h<<":0x"<<hex<<m_headerData[h]<<dec<<endm;
    }
 
  m_trigger = Mid(TRIG_START,TRIG_END,m_rdoData[1]);
  LOG_DEBUG<<"#Current Event Trigger words is "<<m_trigger<<endm;
  m_RDO = Mid(RDO_START,RDO_END,m_rdoData[3]);

  if(m_RDO==6) m_RDO=1;//20140311,luis chenged this serial number

  LOG_DEBUG<<"Current RDO number is :"<<m_RDO<<endm;

  if(m_RDO<1 || m_RDO >nSstRdo) {LOG_WARN<<"RDO number is BAD number(<1 || >5), reject this RDO"<<endm; m_rdoflag=0;return;}

  index += HEADER_LENGTH;
 
  for(int i=0;i<8;i++)
    {
      if(i == 0) 
	{
	  m_adcHeader[i] = (m_rdoData+index);
	  m_adc[i] = m_adcHeader[i] + FIBER_HEADER_LENGTH;
	  m_adcLength[i] = Mid(ADC_START,ADC_END,m_adcHeader[i][1]);
	  m_dataMode[i] = Mid(DATAMODE_START,DATAMODE_END,m_adcHeader[i][1]);
	  m_channel[i] = Mid(FIBER_START,FIBER_END,m_adcHeader[i][2]);
	  m_flag[i] = Mid(FLAG_START,FLAG_END,m_adcHeader[i][1]);
	  if(m_adcHeader[i][0]==FIBER_LINK_TOKEN)
	    {
	      LOG_DEBUG<<"#Fiber ["<<i<< "] link Token correct : 0x"<<hex<<m_adcHeader[i][0]<<dec<<endm;
	    }
            else
	      {
		LOG_WARN<<"Fiber ["<<i<<"] Link Token wrong :0x"<<hex<<m_adcHeader[i][0]<<dec<<endm;
		m_fiberflag[i]=0;
		continue;
	      }
	  if(m_dataMode[i]==RAWMODE)
	  
	    {
	      LOG_INFO<<"Current data mode is RAW data"<<endm;
	    }
	  else if(m_dataMode[i]==COMPRESSEDMODE)
	    {
	      LOG_INFO<<"Current data mode is COMPRESSED data"<<endm;
	    }
	  else 
	    {
	      LOG_WARN<<"DO not have this data mode; error mode = "<<m_dataMode[i]<<endm;
	      m_fiberflag[i]=0;
	      continue;
	    }
	  if(m_flag[i]==NODATA)
	    {
	      LOG_DEBUG<<"Fiber Flag:NO DATA FLAG,reject this fiber data"<<endm;
	      if(m_adcLength[i]==FIBER_HEADER_LENGTH)
		LOG_DEBUG<<"Fiber Flag and adc length is consistent! let's look at next fiber.."<<endm;
	      if(m_adcLength[i]!=FIBER_HEADER_LENGTH)
                {
		  LOG_WARN<<"Fiber Flag and adc length is not consistent,Stop !"<<endm;
		  m_fiberflag[i]=0;
		  continue;
                }
	    }
	  if(m_flag[i]==OVERFLOW) {LOG_WARN<<"Fiber Flag:Over Flow"<<endm; m_fiberflag[i]=0; continue;}
	  if(m_flag[i]==EARLY_ABORT) {LOG_WARN<<"Fiber Flag:Abort Early!"<<endm; m_fiberflag[i]=0;continue;}
	  if(m_flag[i]==WRONG_PIPE_MODE) {LOG_WARN<<"FIBER Flag:Wrong pipe mode!"<<endm; m_fiberflag[i]=0; continue;}
	}
      else  
	{
	  m_adc[i] = m_adc[i-1]+ m_adcLength[i-1];
	  m_adcHeader[i] = (m_adcHeader[i-1] + m_adcLength[i-1]);			
	  m_adcLength[i] = Mid(ADC_START,ADC_END,m_adcHeader[i][1]);
	  m_dataMode[i] = Mid(DATAMODE_START,DATAMODE_END,m_adcHeader[i][1]);
	  m_channel[i] = Mid(FIBER_START,FIBER_END,m_adcHeader[i][2]);
	  m_flag[i] = Mid(FLAG_START,FLAG_END,m_adcHeader[i][1]);
	
	  if(m_adcHeader[i][0]==FIBER_LINK_TOKEN)
	    {
	      LOG_DEBUG<<"#Fiber ["<<i<< "] link Token correct : 0x"<<hex<<m_adcHeader[i][0]<<dec<<endm;
	    }
	  else 
	    {
	      LOG_WARN<<"Fiber ["<<i<<"] Link Token wrong :0x"<<hex<<m_adcHeader[i][0]<<dec<<endm;
	      m_fiberflag[i]=0;
	      continue;
	    }
	  if(m_dataMode[i]==RAWMODE)
	    {
	      LOG_INFO<<"Current data mode is RAW data"<<endm;
	    }
	  else if(m_dataMode[i]==COMPRESSEDMODE)
	    {
	      LOG_INFO<<"Current data mode is COMPRESSED data"<<endm;
	    }
	  else 
	    {
	      LOG_WARN<<"DO not have this data mode; error data mode = "<<m_dataMode[i]<<endm;
	      m_fiberflag[i]=0;
	      continue;
	    }
	  if(m_flag[i]==NODATA)
	    {
	      LOG_DEBUG<<"Fiber Flag:NO DATA FLAG,reject this fiber data"<<endm;
	      if(m_adcLength[i]==FIBER_HEADER_LENGTH)
		LOG_DEBUG<<"Fiber Flag and adc length is consistent! let's look at next fiber.."<<endm;
	      if(m_adcLength[i]!=FIBER_HEADER_LENGTH)
                {
		  LOG_WARN<<"Fiber Flag and adc length is not consistent,Stop !"<<endm;
		  m_fiberflag[i]=0;
		  continue;
                }
	    }
	  if(m_flag[i]==OVERFLOW) {LOG_WARN<<"Fiber Flag:Over Flow"<<endm; m_fiberflag[i]=0; continue;}
	  if(m_flag[i]==EARLY_ABORT) {LOG_WARN<<"Fiber Flag:Abort Early!"<<endm; m_fiberflag[i]=0;continue;}
	  if(m_flag[i]==WRONG_PIPE_MODE) {LOG_WARN<<"FIBER Flag:Wrong pipe mode!"<<endm; m_fiberflag[i]=0;continue;}
	}
	   
      LOG_DEBUG<<"Fiber["<<i<<"]: ADC Length = "<<m_adcLength[i]<<endm;
    }
  
    //check the end token,TCD end token TCD header
    if(m_rdoData[m_rdoDataLength/sizeof(UInt_t)-1]==END_TOKEN)
    {
      LOG_DEBUG<<"End Token Correct: 0x"<<hex<<m_rdoData[m_rdoDataLength/sizeof(UInt_t)-1]<<dec<<endm;
    }
  else
    {
      LOG_WARN<<"End Token Wrong : 0x"<<hex<<m_rdoData[m_rdoDataLength/sizeof(UInt_t)-1]<<dec<<endm;
      m_rdoflag=0;
      return;
    }
  //tcd end token
  if(m_rdoData[m_rdoDataLength/sizeof(UInt_t)-3]==TCD_END_TOKEN)
    {
      LOG_DEBUG<<"TCD End Token Correct: 0x"<<hex<<m_rdoData[m_rdoDataLength/sizeof(UInt_t)-3]<<dec<<endm;
    }
  else
    {
      LOG_WARN<<"TCD End Token Wrong : 0x"<<hex<<m_rdoData[m_rdoDataLength/sizeof(UInt_t)-3]<<dec<<endm;
      m_rdoflag=0;
      return;
    }
    //tcd token
  m_trailerData = m_adcHeader[7]+m_adcLength[7];
  if(m_trailerData[0]==TCD_TOKEN)
    {
      LOG_DEBUG<<"TCD Token Correct: 0x"<<hex<<m_trailerData[0]<<dec<<endm;
    }
  else
    {
      LOG_WARN<<"TCD Token Wrong : 0x"<<hex<<m_trailerData[0]<<dec<<endm;
      m_rdoflag=0;
      return;
    }
  for(int t=0;t<10;t++)
    {
      if(t==0) LOG_DEBUG<<"#Start Print Trailer info... "<<endm;
      LOG_DEBUG<<"#"<<t<<":0x"<<hex<<m_trailerData[t]<<dec<<endm;
      if(m_trailerData[t]==END_TOKEN) break;
    }
   
  //set data flag(bad or good) 0-bad 1-good
  LOG_DEBUG<<"m_RDO "<<m_RDO<< " m_rdoflag "<<m_rdoflag<<endm;
}
//-------------------------------------------------
Int_t StSstDaqMaker::GetHitsDataLength(int FiberNumber)
{
  return m_adcLength[FiberNumber];
}
//------------------------------------------------    
void StSstDaqMaker::DecodeHitsData()
{
  if(m_rdoflag!=1) return;
  LOG_INFO<<"#START Decoding RDO " <<m_RDO<<" data, rdo flag is "<<m_rdoflag<<endm;
  for(Int_t j=0;j<8;j++)
    {
      if(m_fiberflag[j]!=1) continue;
      if(m_channel[j]<0 || m_channel[j] >(nSstFiberPerRdo-1)) {LOG_WARN<<"Fiber number is BAD (<0 || >7), reject this Fiber"<<endm; continue;}
      int temp = m_adcLength[j] - FIBER_HEADER_LENGTH;
      if(temp==0 || !m_adc[j]) 
	{
     	  LOG_WARN<<"Fiber ["<<m_channel[j]<<"] do not have any data..."<<endm;
	  continue;
	}

      if(m_flag[j]!=NORMAL) {LOG_WARN<<"Fiber["<<m_channel[j]<<"] data flag is not normal, reject this fiber"<<endm; continue;}
      if(m_dataMode[j]==RAWMODE) {
	DecodeRawWords(m_adc[j],temp,m_channel[j]);
      }
      if(m_dataMode[j]==COMPRESSEDMODE) DecodeCompressedWords(m_adc[j],temp,m_channel[j]);
    }
}
//-------------------------------------------------
void StSstDaqMaker::DecodeRawWords(UInt_t* val,int vallength,int channel)
{
  int strip_number[3],id_wafer[3],id_side,count;
  int ladderCountN[20]={0};
  int ladderCountP[20]={0};
  int data[3],pedestal[3],wafer[3],hybrid[3],noise[3],strip[3],readout[3],ladder;
  for(int i=0;i<3;i++)
    {
      data[i]     = 0;
      pedestal[i] = 0;
      hybrid[i]    = 0;
      readout[i]  = 0;
      noise[i]    = 0;
    }
//-----initialize St_spa_strip and St_ssdPedStrip table.
St_spa_strip *spa_strip = (St_spa_strip *) m_DataSet->Find("spa_strip");  
 if(!spa_strip)
 {
   spa_strip   = new St_spa_strip("spa_strip",100000);
   m_DataSet->Add(spa_strip);
 }

 spa_strip_st	out_strip;
 count = 1;
 LOG_DEBUG<<"DECODEING RAW MODE data....."<<endm;
 //grab ladder and side 
 FindLadderSide(m_RDO,channel,ladder,id_side);
 //----
for(int i=0;i<vallength;i++)
{
  if(i%1000==0) LOG_DEBUG<<"Fiber["<<channel<<"] :PROCESSING "<<(float)i/(float)vallength*100.<<"%"<<endm;  
  if(i==0) hybrid[0] = 0;
  else  hybrid[0] = hybrid[2] + 1;
  if(hybrid[0]>=16) hybrid[0] = hybrid[0]%16;
  hybrid[1] = hybrid[0] + 1;
  if(hybrid[1]>=16) hybrid[1] = hybrid[1]%16;
  hybrid[2] = hybrid[0] + 2;
  if(hybrid[2]>=16) hybrid[2] = hybrid[2]%16;
  LOG_DEBUG<<"Three hybrid number in current word is :"<<hybrid[0]<<","<<hybrid[1]<<","<<hybrid[2]<<endm;
  
if(i==0)
  {
    for(int j=0;j<3;j++) 
      {
	readout[j] = 0;

      }
    LOG_DEBUG<<"[adc_pointer,readout_0,hybrid_0] = ["<<i<<","<<readout[0]<<","<<hybrid[0]<<"]"<<endm;
    LOG_DEBUG<<"[adc_pointer,readout_1,hybrid_1] = ["<<i<<","<<readout[1]<<","<<hybrid[1]<<"]"<<endm;
    LOG_DEBUG<<"[adc_pointer,readout_2,hybrid_2] = ["<<i<<","<<readout[2]<<","<<hybrid[2]<<"]"<<endm;
  }
else
  {
    
    if(hybrid[0]==0) readout[0] = readout[0] + 1;
    if(readout[2]>readout[0]) readout[0] = readout[2];
    if(hybrid[1]==0) readout[1] = readout[1] + 1;
    if(readout[0]>readout[1]) readout[1] = readout[0];
    if(hybrid[2]==0) readout[2] = readout[2] + 1;
    if(readout[1]>readout[2]) readout[2] = readout[1];
    LOG_DEBUG<<"[adc_pointer,readout_0,hybrid_0] = ["<<i<<","<<readout[0]<<","<<hybrid[0]<<"]"<<endm;
    LOG_DEBUG<<"[adc_pointer,readout_1,hybrid_1] = ["<<i<<","<<readout[1]<<","<<hybrid[1]<<"]"<<endm;
    LOG_DEBUG<<"[adc_pointer,readout_2,hybrid_2] = ["<<i<<","<<readout[2]<<","<<hybrid[2]<<"]"<<endm;
  }
 data[0] = Mid(HYBRID_ONE_START,HYBRID_ONE_END,val[i]);
 data[1] = Mid(HYBRID_TWO_START,HYBRID_TWO_END,val[i]);
 data[2] = Mid(HYBRID_THREE_START,HYBRID_THREE_END,val[i]);
 
 for(int n=0;n<3;n++)
   {
     strip[n] = readout[n];
     wafer[n] = hybrid[n];
     FindStripNumber(strip[n]);
    
     if(id_side==0) AdcStrip[0][ladder]->Fill(strip[n]+wafer[n]*nSstStripsPerWafer,data[n]);
     if(id_side==1) AdcStrip[1][ladder]->Fill(strip[n]+wafer[n]*nSstStripsPerWafer,data[n]);
     /*if(id_side==0) AdcStrip[0][ladder]->Fill(readout[n]+wafer[n]*nSstStripsPerWafer,data[n]);
       if(id_side==1) AdcStrip[1][ladder]->Fill(readout[n]+wafer[n]*nSstStripsPerWafer,data[n]);*/
   }
if(id_side == 0)
{
for(int i=0;i<3;i++)
	id_wafer[i]=7000+100*(nSstWaferPerLadder-wafer[i])+ladder+1;
}
else
{
for(int i=0;i<3;i++)
    id_wafer[i]=7000+100*((wafer[i])+1)+ladder+1;
}
for(int i=0;i<3;i++)
  strip_number[i] = strip[i]+1;//strip[i]+1 . in mapping, strip[1-128];

for(int j=0;j<3;j++)
{
out_strip.id          = count + j;
out_strip.adc_count   = data[j];
out_strip.id_strip    = 10000*(10*strip_number[j]+id_side)+id_wafer[j];//id_side:0-->p,1-->N
out_strip.id_mchit[0] = 0 ;
out_strip.id_mchit[1] = 0 ;
out_strip.id_mchit[2] = 0 ;
out_strip.id_mchit[3] = 0 ;
out_strip.id_mchit[4] = 0 ;

spa_strip->AddAt(&out_strip);
if(id_side ==0) ladderCountP[ladder]++;
 else            ladderCountN[ladder]++;
}
count = count + 3;

 }
LOG_DEBUG<<"last readout number readout[0]:readout[1]:readout[2] = ["<<readout[0]<<","<<readout[1]<<","<<readout[2]<<"]"<<endm;

if(readout[0]>nSstStripsPerWafer||readout[1]>nSstStripsPerWafer||readout[2]>nSstStripsPerWafer)
  {
    LOG_WARN<<"Strip number is larger than 768, ERROR"<<endm;
    return;
  }
 LOG_DEBUG << "Make()/Counts (p-side): ";
for (int i=0;i<nSstLadder;i++){ 
       LOG_DEBUG.width(5);
       LOG_DEBUG <<ladderCountP[i]<<" ";
    } 
LOG_DEBUG << endm;
LOG_DEBUG << "Make()/Counts (n-side): "; 
for (int i=0;i<nSstLadder;i++){
    
       LOG_DEBUG.width(5);
       LOG_DEBUG <<ladderCountN[i]<<" ";
    } 
LOG_DEBUG<<endm;
 if(spa_strip->GetNRows()!=0){
   LOG_DEBUG << "Make()/ Read Signal from Physics Run"<< endm;
   LOG_DEBUG << "Make()/  spa_strip->NRows= "<<spa_strip->GetNRows()<<endm;
 }
}
//-------------------------------------------------
void StSstDaqMaker::DecodeCompressedWords(UInt_t* val,int vallength,int channel)
{
  int strip_number,id_side,id_wafer,count;
  int ladderCountN[20]={0};
  int ladderCountP[20]={0};
  int data,wafer,strip,readout,ladder;

  St_spa_strip *spa_strip = (St_spa_strip *) m_DataSet->Find("spa_strip");  
  if(!spa_strip)
    {
      spa_strip   = new St_spa_strip("spa_strip",100000);
      m_DataSet->Add(spa_strip);
    }
  
  spa_strip_st	out_strip;
  count = 1;
  //grab ladder and side 
  FindLadderSide(m_RDO,channel,ladder,id_side);//convert channel to real Ladder number and side
  //----
  for(int i=0;i<vallength;i++)
    {
      wafer = Mid(HYBRID_START,HYBRID_END,val[i]);
      strip = Mid(STRIP_START,STRIP_END,val[i]);
      readout = strip;
      data  = Mid(ADC_START,ADC_END,val[i]);
     
      FindStripNumber(strip);//convert to physic strip number 
      if(id_side==0) AdcStrip[0][ladder]->Fill(strip+wafer*nSstStripsPerWafer,data);
      if(id_side==1) AdcStrip[1][ladder]->Fill(strip+wafer*nSstStripsPerWafer,data);
      /*
      if(id_side==0) AdcStrip[0][ladder]->Fill(readout+wafer*nSstStripsPerWafer,data);
      if(id_side==1) AdcStrip[1][ladder]->Fill(readout+wafer*nSstStripsPerWafer,data);*/
      
      if(id_side == 0)
	{
	  id_wafer=7000+100*(nSstWaferPerLadder-wafer)+ladder+1;
	}
      else 
	{
	  id_wafer=7000+100*((wafer)+1)+ladder+1;
	}
    
      strip_number = strip +1;
      out_strip.id          = count;
      out_strip.adc_count   = data;
      out_strip.id_strip    = 10000*(10*strip_number + id_side) + id_wafer;
      out_strip.id_mchit[0] = 0 ;
      out_strip.id_mchit[1] = 0 ;
      out_strip.id_mchit[2] = 0 ;
      out_strip.id_mchit[3] = 0 ;
      out_strip.id_mchit[4] = 0 ;
      spa_strip->AddAt(&out_strip);
      if(id_side ==0) ladderCountP[ladder]++;
      else            ladderCountN[ladder]++;
      count = count + 1;
    }

  if(readout>nSstStripsPerWafer)
    {
      LOG_WARN<<"Strip number is larger than 768, ERROR"<<endm;
      return;
    }
  LOG_DEBUG << "Make()/Counts (p-side): ";
  for (int i=0;i<nSstLadder;i++){
    LOG_DEBUG.width(5);
    LOG_DEBUG <<ladderCountP[i]<<" ";
  } 
  LOG_DEBUG << endm;
  LOG_DEBUG << "Make()/Counts (n-side): "; 
  for (int i=0;i<nSstLadder;i++){
    
    LOG_DEBUG.width(5);
    LOG_DEBUG <<ladderCountN[i]<<" ";
  } 
  LOG_DEBUG<<endm;
  if(spa_strip->GetNRows()!=0){
    LOG_DEBUG << "Make()/ Read Signal from Physics Run"<< endm;
    LOG_DEBUG << "Make()/  spa_strip->NRows= "<<spa_strip->GetNRows()<<endm;
  }
}
//-------------------------------------------------
UInt_t StSstDaqMaker::Mid(Int_t start,Int_t end, UInt_t input)
{
    UInt_t buf;
    buf=input<<(32-end);
    buf=buf>>(32-end);
    return buf>>start;
}

//--------------------------------------------------
void StSstDaqMaker::FindLadderSide(int RDO,int channel,int &ladder,int &side)
{
  ladder = RDO2LADDER[RDO-1][channel]-1;//ladder [0-19];
  if(ladder>20) LOG_WARN<<"Ladder >20. RDO Number is"<<RDO<<", channel number :"<<channel<<endm;
  if(RDO<3) side = 1; 
  else if(RDO>3) side = 0;
  else if(channel<4) side = 1;
  else side = 0;
}
//--------------------------------------------------
void StSstDaqMaker::FindStripNumber(int &strip)
{
  int temp = (strip/128)*128 + ReadOutMap[strip%128];
  strip = temp-1;
}

//--------------------------------------------------
                                                      
void StSstDaqMaker::PrintConfiguration(Int_t runumber,ssdConfiguration_st *config)
{
  switch(runumber){
  case 4 : {
    LOG_INFO <<"Configuration of ladders for run IV" <<endm;
    break;
  }
  case 5 : {
    LOG_INFO <<"Configuration of ladders for run V" << endm;
    break;
  }
  case 6 : {
    LOG_INFO <<"Configuration of ladders  for run VI"<< endm;
    break;
  }
  case 7 : {
    LOG_INFO <<"Configuration of ladders  for run VII" << endm;
    break;
  }
  }
  Int_t i =0;
  Int_t totladderPresent =0;
  LOG_INFO << "PrintLadderSummary:ladder id                 :";
  for (i=1;i<=config->nMaxLadders;i++){
    LOG_INFO.width(3);
    LOG_INFO << i;
  }
  LOG_INFO <<endm;
  LOG_INFO << "PrintLadderSummary:Active Ladders on sectors: ";
  for (i=1;i<=config->nMaxLadders;i++){
    LOG_INFO.width(3);
    LOG_INFO <<mConfig->getLadderIsActive(i);
    if(mConfig->getLadderIsActive(i)>0)totladderPresent++;

  }
  LOG_INFO << endm;
  LOG_INFO << "totLadderActive = "<<totladderPresent<<endm;
}
//------------------------------------------------                                                          
Int_t StSstDaqMaker::Finish()
{
  LOG_INFO << Form("Finish()") << endm;
  return kStOK;
}
