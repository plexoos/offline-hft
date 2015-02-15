#ifndef STAR_StSsdMONMaker
#define STAR_StSsdMONMaker
#include "Riostream.h"
#include "StMaker.h"
#include "StSsdUtil/StSsdDynamicControl.h"
#include "StSsdUtil/StSsdClusterControl.h"

class TFile;
class TH1S;
class TH2S;
class TH2D;
class TDirectory;

class St_scf_cluster;
class St_scm_spt;
class St_spa_strip;

class St_ssdDimensions;
class St_ssdConfiguration;
class St_ssdWaferConfiguration;
class ssdDimensions_st;
class ssdConfiguration_st;

class StSsdMONMaker : public StMaker {
 public:
  StSsdMONMaker(const char *name="SsdMON");
  virtual       ~StSsdMONMaker() {}
  virtual Int_t  Init();
  virtual Int_t  Make();
  virtual Int_t  Finish();
  virtual void   PrintInfo();

 private:
  void  makeSpaCtrlHistograms(St_spa_strip *spa_strip);        //!
  void  makeScfCtrlHistograms(St_scf_cluster *scf_cluster);        //!
  void  makeScmCtrlHistograms(St_scm_spt *scm_point);        //!
  void  writeHists();
  //helper functions
  Int_t idWaferToWaferNumb(Int_t idWafer); //  idwafer = layer*1000+waf*100+ladder => waferNumb = mNWaferPerLadder*(ladder-1) + waf - 1
  Int_t idWaferToLadderNumb(Int_t idWafer);//  idwafer => ladder-1
  Int_t waferNumbToIdWafer(Int_t waferNumb);// waferNumb = mNWaferPerLadder*(ladder-1) + waf - 1 => idwafer
  Int_t idWaferToWafer(Int_t idWafer) {return (idWafer-7000)/100-1;}
 protected:

  TFile* myRootFile;
  StSsdDynamicControl    *mDynamicControl; //!

  //definition of histograms
  TH2S  *m_strip_ADC_CHAN_sideP[320];
  TH2S  *m_strip_ADC_CHAN_sideN[320];
  TH2S  *m_cluster_ADC_sideP[320];
  TH1S  *m_cluster_Size_sideP[320];
  TH2S  *m_cluster_ADC_sideN[320];
  TH1S  *m_cluster_Size_sideN[320];
  TH2D  *m_matchisto[320];
  TH2S  *globalMatchisto; 
  TH1S  *globalDeviation;
  TH2S  *globalMatchistoP; 
  TH1S  *globalDeviationP;
  TH2D  *globalPosition;
  TH2D  *localPosition;
  TH2D  *rphiZ;
  TH2S  *LadderWaferHits;
  TH1S  *package; 
  //constant
  Float_t convMeVToADC;
  Int_t mEventCounter;

  virtual const char *GetCVS() const 
  {static const char cvs[]="Tag $Name:  $ $Id: StSsdMONMaker.h,v 1.2 2015/02/15 16:10:45 bouchet Exp $ built "__DATE__" "__TIME__ ; return cvs;}
  
  ClassDef(StSsdMONMaker, 1)   //StAF chain virtual base class for Makers
    };
#endif
