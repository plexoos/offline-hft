/*!
 * \class StSstDaqMaker
 * \author Long Zhou,Nov 2013, according codes from Hao Qiu
 * \Initial version.
 */
/***************************************************************************
 *
 * $Id$
 *
 * Author: Long Zhou, Nov 2013, according codes from Hao Qiu
 ***************************************************************************
 *
 * Description:
 * Read SST strips info from daq format. 
 *
 ***************************************************************************
 *
 * $Log$
 * Revision 1.4  2014/06/05 14:55:24  zhoulong
 * Added some code to compatible Thorsten's LC FPGA and correct readout channel shift in old LC FPGA
 *
 *
 **************************************************************************/
#ifndef STAR_StSstDaqMaker
#define STAR_StSstDaqMaker

#include "StMaker.h"
#include "StRTSBaseMaker.h"
#include "StSstConstant.h"

class St_spa_strip;
class St_ssdPedStrip;
class TH1F;
class TH1S;
class TH2S;
class TH2I;
class TH3I;
class TFile;
class TTree;
class TNtuple;
class StSsdConfig;
class St_ssdConfiguration;
class ssdConfiguration_st;

class StSstDaqMaker : public StRTSBaseMaker {
 public:
  StSstDaqMaker(const char *name="sst_raw");//here need to check 
  virtual Int_t Init();
  virtual Int_t InitRun(Int_t runumber);
  virtual Int_t Make();
  virtual Int_t Finish();
 
  Int_t GetHitsDataLength(int FiberNumber);
  void DecodeRdoData();
  void DecodeHitsData();
  void DecodeRawWords(UInt_t* val,int vallength,int channel);
  void DecodeCompressedWords(UInt_t* val,int valength,int channel);
  UInt_t Mid(Int_t start, Int_t end, UInt_t input);//used to grab the valuable info in middle of 32-bit .
  Int_t Shift(int runnumber,int &channel);
  void FindLadderSide(int RDO,int channel,int &ladder,int &side);//use RDO no and channel no to grab ladder info and p-side or n-side info
  void FindStripNumber(int &strip);
  virtual const char *GetCVS() const {
	  static const char cvs[]="Tag $Name$ $Id$ built "__DATE__" "__TIME__; 
	  return cvs;
	}
  
private:
  Float_t PedestalNTuple[10];
  TNtuple *pTuple;
  void DeclareNTuple(); 
  void PrintConfiguration(Int_t runumber,ssdConfiguration_st *config);

protected:
  StSsdConfig*  mConfig;
  TH2I*   AdcStrip[2][20]; //used to debug delay time ...
  int     mPedOut;          //to turn the fill of TNtuple on and off
  UInt_t* m_rdoData;
  Int_t   m_rdoDataLength;
  UInt_t* m_headerData;
  UInt_t* m_trailerData;
  Int_t   m_trailerDataLength;
  UInt_t* m_adc[8];//8 fiber data pointer
  UInt_t* m_adcHeader[8];
  Int_t   m_adcLength[8];// = {0}; // initialization 
  Int_t   m_mode;//m_mode=0. real data, m_mode=1 pedestal 
  Int_t   m_rdoflag;
  Int_t   m_fiberflag[8];
  Int_t   m_flag[8];//check fiber flag
  Int_t   m_trigger;
  //----------
  Int_t   m_sec;//used in Tonko's pedestal bank.
  Int_t   m_fiber;//used in Tonko's pedestal bank
  Float_t m_ped;
  Float_t m_rms;
  //---------- 
  Int_t   m_RDO;//RDO number 
  Int_t   m_wafer[16];
  Int_t   m_strip[768];
  Int_t   m_channel[8];
  Int_t   m_dataMode[8];// Raw or Compressed data .
  Int_t   eventnumber;
  Int_t   eventrunumber;
  ClassDef(StSstDaqMaker,1)   
};

#endif
 
