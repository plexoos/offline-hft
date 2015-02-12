// $Id: StSsdPointMaker.cxx,v 1.3 2015/02/12 15:24:25 bouchet Exp $
//
// $Log: StSsdPointMaker.cxx,v $
// Revision 1.3  2015/02/12 15:24:25  bouchet
// protection against missing spa_strip when writing pedestal file
//
// Revision 1.2  2015/02/08 17:30:30  bouchet
// read noise table aacording year and mode
//
// Revision 1.1  2015/01/29 19:51:41  bouchet
// 1st revision of PointMaker for run 14 and beyond
//
// Revision 1.1  2014/10/18 18:17:36  bouchet
// *** empty log message ***
//
// Revision 1.63  2012/11/07 21:54:37  fisyak
// Remove check for .histos
//
// Revision 1.62  2009/02/18 21:31:22  bouchet
// fix bug for printing the number of hits/ladder and filling histograms
//
// Revision 1.61  2008/10/20 19:32:39  bouchet
// use of new writePointToContainer method for the hit quality calculation
//
// Revision 1.60  2008/07/16 21:01:57  bouchet
// calculation of hits quality removed : call of default writePointToContainer
//
// Revision 1.59  2008/05/20 03:05:54  bouchet
// fix improper STAR logger(#1185) ; thanks to Valeri
//
// Revision 1.58  2008/05/07 22:45:24  bouchet
// add mcEvent dependence for embedding
//
// Revision 1.57  2008/04/15 21:05:22  bouchet
// remove latest change
//
// Revision 1.56  2008/04/12 14:20:38  bouchet
// Add a switch to use constant noise and pedestal ; remove some printing
//
// Revision 1.55  2008/01/15 13:48:58  bouchet
// Set a default value for uninitialized variable
//
// Revision 1.54  2008/01/11 10:39:39  bouchet
// add method to read the Wafer configuration table
//
// Revision 1.53  2007/09/25 13:40:46  bouchet
// Use m_Mode to switch between pedestals used in real data/simulation ; move some message to DEBUG
//
// Revision 1.52  2007/08/20 06:47:37  bouchet
// ssdStripCalib table taken for simulation
//
// Revision 1.51  2007/07/14 14:29:44  bouchet
// forget the Debug condition for the declaration of the tuples
//
// Revision 1.50  2007/07/14 13:52:16  bouchet
// add method to fill with default pedestal/noise values if no table is found
//
// Revision 1.49  2007/07/13 06:19:43  bouchet
// display of number of reconstructed hits corrected
//
// Revision 1.48  2007/07/12 17:07:18  bouchet
// add switch to read old ssdStripCalib Table and new ssdNoise Table
//
// Revision 1.47  2007/07/02 20:01:03  bouchet
// bug fixed for the normalization of reconstruction efficiency histos
//
// Revision 1.46  2007/07/01 16:18:41  bouchet
// add a normalization for the reconstruction efficiency histograms
//
// Revision 1.45  2007/06/23 04:53:50  bouchet
// add 0's to Timestamp which size is less than 6 digits
//
// Revision 1.44  2007/06/19 18:30:24  bouchet
// Add a method to evaluate the reconstruction efficiency (defined as the ratio of the number of matched clusters with all reconstructed clusters) ; some clean-up
//
// Revision 1.43  2007/06/19 01:19:15  bouchet
// cosmetic changes
//
// Revision 1.42  2007/04/28 17:56:58  perev
// Redundant StChain.h removed
//
// Revision 1.41  2007/04/17 05:09:25  perev
// GetTFile()==>StMaker. Jerome request
//
// Revision 1.40  2007/04/11 22:45:22  perev
// 1/0 avoided
//
// Revision 1.39  2007/03/27 23:15:09  bouchet
// Add a switch to use the gain calibration
//
// Revision 1.38  2007/03/27 18:30:04  fisyak
// recover lost access to ssdStripCalib table
//
// Revision 1.37  2007/03/21 17:19:12  fisyak
// use TGeoHMatrix for coordinate transformation, eliminate ssdWafersPostion, ake NTuples only for Debug()>1
//
// Revision 1.36  2007/03/08 23:04:42  bouchet
// add WriteMatchedStrips() method : fill the characteristics of the strips from matched clusters ; Small change for the writing of tuples
//
// Revision 1.35  2007/03/01 22:19:21  bouchet
// add a protection when ssdStripCalib is filled with empty values
//
// Revision 1.34  2007/02/21 20:36:17  bouchet
// add a method WriteMatchedClusters :\ instead of WriteScfTuple() method that fill all the reconstructed clusters,\ this one store the clusters associated to the hits
//
// Revision 1.33  2007/02/15 14:40:27  bouchet
// bug fixed for filling makeScmCtrlHistograms() method
//
// Revision 1.32  2007/02/14 11:49:12  bouchet
// Added control histograms and updated the Cluster and Point Tuple
//
// Revision 1.31  2007/02/02 20:24:15  bouchet
// WriteStripTuple method added, WriteScmTuple method updated
//
// Revision 1.30  2007/02/02 17:46:58  bouchet
// Few changes for the new Logger
//
// Revision 1.29  2007/01/16 18:01:52  bouchet
// Replace printf,cout,gMessMgr with LOG statements
//
// Revision 1.28  2006/10/16 16:27:49  bouchet
// Unify classes ; Methods for all classes (StSsdStrip, StSsdCluster, StSsdPoint) are now in StSsdUtil
//
// Revision 1.27  2006/09/15 21:03:14  bouchet
// id_mctrack is using for setIdTruth and propagated to the hit
//
// Revision 1.26  2005/12/31 01:43:22  perev
// Mack/Upack simplified
//
// Revision 1.25  2005/12/20 10:35:51  lmartin
// ReadStrip method updated and some cosmetic changes
//
// Revision 1.24  2005/12/20 09:23:35  lmartin
// ssdStripCalib table read from the mysql db
//
// Revision 1.23  2005/09/30 14:28:30  lmartin
// add a 0 to myTime if GetTime()<100000
//
// Revision 1.22  2005/09/26 15:49:54  bouchet
// adding a method to the poInt_t maker to check which ssdStripCalib is picked
//
// Revision 1.21  2005/08/11 13:51:39  lmartin
// PrintStripDetails, PrintPackageDetails and PrintPointDetails methods added
//
// Revision 1.20  2005/06/24 10:19:46  lmartin
// preventing crashes if ssdStripCalib is missing
//
// Revision 1.19  2005/06/16 14:29:22  bouchet
// no more makeSsdPedestalHistograms() method
//
// Revision 1.18  2005/06/14 12:09:15  bouchet
// add a histo for the pedestal and new name of the class : SsdPoint
//
// Revision 1.14  2005/06/07 16:24:47  lmartin
// InitRun returns kStOk
//
// Revision 1.13  2005/06/07 12:04:46  reinnart
// Make Stuff moved to Initrun
//
// Revision 1.12  2005/06/07 11:55:08  reinnart
// Initrun and good database connection
//
// Revision 1.11  2005/04/25 14:13:23  bouchet
// new method makeScfCtrlHistograms and makeScmCtrlHistograms and Clusternoise is coded as a float
//
// Revision 1.10  2005/04/23 08:56:20  lmartin
// physics and pedestal data processing separated
//
// Revision 1.9  2005/03/23 16:07:26  lmartin
// PrintClusterSummary and PrintPointSummary methods added
//
// Revision 1.8  2005/03/22 13:46:43  lmartin
// PrintStripSummary method added
//
// Revision 1.7  2005/03/22 10:38:51  lmartin
// HighCut value taken from the db
//
// Revision 1.6  2005/03/18 13:35:40  lmartin
// Partly missing cvs header added
//
// Revision 1.5  2005/03/18 10:16:34  lmartin
// positionSize argument added to the initLadders method
//
// Revision 1.4  2004/11/04 15:10:19  croy
// use the IAttr(".histos") to control histogramming and modification of the SsdHitCollection creation
//
// Revision 1.3  2004/08/13 07:07:23  croy
// Updates to read SSD databases
//
// Revision 1.2  2004/07/20 14:04:02  croy
// Use of new database structure definitions related to SSD config
//
// Revision 1.1  2004/03/12 06:12:37  jeromel
// Peer review closed. Yuri/Frank.
//
// Revision 1.3  2002/03/25 20:13:05  hippolyt
// Merged the two former makers 
//
//
#include "StSsdPointMaker.h"

#include "TFile.h"
#include "TH1.h"
#include "TH2.h"
#include "TDataSetIter.h"
#include "StMessMgr.h"
#include "TNtuple.h"
#include "StSsdUtil/StSsdPoint.hh"
#include "StSsdUtil/StSsdPackage.hh"
#include "StSsdUtil/StSsdCluster.hh"
#include "StSsdUtil/StSsdStripList.hh"
#include "StSsdUtil/StSsdClusterList.hh"
#include "StSsdUtil/StSsdPointList.hh"
#include "StSsdUtil/StSsdPackageList.hh"
#include "StSsdUtil/StSsdWafer.hh"
#include "StSsdUtil/StSsdLadder.hh"
#include "StSsdUtil/StSsdBarrel.hh"
#include "StSsdUtil/StSsdStrip.hh"
#include "tables/St_spa_strip_Table.h" 
#include "tables/St_ssdPedStrip_Table.h"
#include "tables/St_scf_cluster_Table.h"
#include "tables/St_scm_spt_Table.h"
#include "tables/St_slsCtrl_Table.h"
#include "tables/St_clusterControl_Table.h"
#include "tables/St_ssdDimensions_Table.h"
#include "tables/St_ssdConfiguration_Table.h"
#include "tables/St_ssdWafersPosition_Table.h"
#include "tables/St_ssdLaddersPosition_Table.h"
#include "tables/St_ssdSectorsPosition_Table.h"
#include "tables/St_ssdBarrelPosition_Table.h"
#include "tables/St_ssdStripCalib_Table.h"
#include "tables/St_ssdGainCalibWafer_Table.h"
#include "tables/St_ssdNoise_Table.h"
#include "tables/St_ssdWaferConfiguration_Table.h"
#include "StEvent.h"
#include "StSsdHitCollection.h"
#include "StSsdDbMaker/StSsdDbMaker.h"
#include "StSsdDbMaker/StSstDbMaker.h"
#include "TMath.h"
ClassImp(StSsdPointMaker);
  
//_____________________________________________________________________________
Int_t StSsdPointMaker::Init(){
  return StMaker::Init();
}
//_____________________________________________________________________________
Int_t StSsdPointMaker::InitRun(Int_t runumber) {

  //mode= gStSsdDbMaker->GetMode();
  //temporary fixed for y2014
  mode= gStSstDbMaker->GetMode();
  LOG_INFO <<"m_Mode = " << mode << endm;
  NEvent         = 0;
  UseCalibration = 1;
  UseWaferConfig = 1;
  UseChipCorrect = 1;
  LOG_INFO<<Form("UseCalibration =%d UseWaferTable = %d UseCorrectionFactor = %d",UseCalibration,UseWaferConfig,UseChipCorrect)<<endm;
  St_slsCtrl* slsCtrlTable = (St_slsCtrl*) GetDataBase("Geometry/ssd/slsCtrl");
  if(! slsCtrlTable){LOG_ERROR << "InitRun : No access to slsCtrl table" << endm;}
  else  {
    mDynamicControl = new StSsdDynamicControl();
    slsCtrl_st*      control      = (slsCtrl_st*) slsCtrlTable->GetTable();
    mDynamicControl -> setnElectronInAMip(control->nElectronInAMip);
    mDynamicControl -> setadcDynamic(control->adcDynamic);
    mDynamicControl -> seta128Dynamic(control->a128Dynamic);
    mDynamicControl -> setnbitEncoding(control->nbitEncoding);
    mDynamicControl -> setnstripInACluster(control->nstripInACluster);
    mDynamicControl -> setpairCreationEnergy(control->pairCreationEnergy);
    mDynamicControl -> setparDiffP(control->parDiffP);
    mDynamicControl -> setparDiffN(control->parDiffN);
    mDynamicControl -> setparIndRightP(control->parIndRightP);
    mDynamicControl -> setparIndRightN(control->parIndRightN);
    mDynamicControl -> setparIndLeftP(control->parIndLeftP);
    mDynamicControl -> setparIndLeftN(control->parIndLeftN);
    mDynamicControl -> setdaqCutValue(control->daqCutValue);
    mDynamicControl -> printParameters();
  }
  St_clusterControl* clusterCtrlTable = (St_clusterControl*) GetDataBase("Geometry/ssd/clusterControl");
  if (!clusterCtrlTable) {LOG_ERROR << "InitRun : No access to clusterControl table" << endm;}
  else {
    mClusterControl = new StSsdClusterControl();
    clusterControl_st *clusterCtrl  = (clusterControl_st*) clusterCtrlTable->GetTable() ;
    mClusterControl -> setHighCut(clusterCtrl->highCut);  
    mClusterControl -> setTestTolerance(clusterCtrl->testTolerance);
    mClusterControl -> setClusterTreat(clusterCtrl->clusterTreat);
    mClusterControl -> setAdcTolerance(clusterCtrl->adcTolerance);
    mClusterControl -> setMatchMean(clusterCtrl->matchMean);
    mClusterControl -> setMatchSigma(clusterCtrl->matchSigma);
    mClusterControl -> printParameters();
  }    
  year = (GetDate()/10000)-2000;
  LOG_DEBUG <<Form("TimeStamp is %d Year is =%d\n",GetDate(),year)<<endm;
  switch(mode)
    {
    case 1: {
      m_noise2 = (St_ssdStripCalib*) GetDataBase("Calibrations/ssd/ssdStripCalib");
      if (!m_noise2) {LOG_ERROR << "InitRun : No access to ssdStripCalib - will use the default noise and pedestal values" << endm;} 
      else { 
	LOG_INFO<<"InitRun for simu : old Table (ssdStripCalib) is used"<<endm; 
      } 
      break;
    }
    case 0 :{
      if(year<7){
	m_noise2 = (St_ssdStripCalib*) GetDataBase("Calibrations/ssd/ssdStripCalib");
	if (!m_noise2) {LOG_ERROR << "InitRun : No access to ssdStripCalib - will use the default noise and pedestal values" << endm;}
	else {
	  LOG_INFO<<"InitRun for real data : old Table(ssdStripCalib) is used"<<endm;
	}
      }
      else {
	m_noise3 = (St_ssdNoise*)GetDataBase("Calibrations/ssd/ssdNoise");
	if (!m_noise3) {LOG_ERROR << "InitRun : No access to ssdNoise - will use the default noise and pedestal values" << endm;}
	else{
	  LOG_INFO << "InitRun for real data : new Table(ssdNoise) is used" << endm;}
      }
      break;
    }
    default : {printf("no real data nor simu");}
    }
  (UseCalibration==1)?FillCalibTable():FillDefaultCalibTable();
  (UseWaferConfig==1)?FillWaferTable():FillDefaultWaferTable();
  /*
    Init arrays for the reconstruction efficiency
  */
  for(Int_t ii=0 ;ii<20;ii++)
    {
      for(Int_t jj=0;jj<16;jj++)
	{
	  ratioP[ii][jj] = 0;
	  ratioN[ii][jj] = 0;
	}
    }
  return kStOk;
}
//_____________________________________________________________________________
Int_t StSsdPointMaker::Make()
{
  LOG_DEBUG << Form("Make : begin")<< endm;
  // 		Create output tables
  Int_t res = 0; 
  char* myLabel  = new char[100];
  char* myTime = new char[100]; 
  char* myDate = new char[100];
  if (GetTime()<999)
    sprintf(myTime,"000%d",GetTime());
  else
    if ((GetTime()<9999)&&(GetTime()>999))
      sprintf(myTime,"00%d",GetTime());
    else
      if ((GetTime()<99999)&&(GetTime()>9999))
	sprintf(myTime,"0%d",GetTime());
      else 
	sprintf(myTime,"%d",GetTime());
  sprintf(myDate,"%d%s",GetDate(),".");
  sprintf(myLabel,"%s%s",myDate,myTime);
  // two different tables can exist (physics data or pedestal data)
  
  TDataSet *SpaStrip = GetDataSet("sst_raw");
  //temporary fixed for y2014
  if (! SpaStrip) {
    LOG_ERROR << "Make : no input data set, wrong chain option" << endm;
    return kStErr;
  }
  St_spa_strip *spa_strip = dynamic_cast<St_spa_strip *> (SpaStrip->Find("spa_strip"));
  St_ssdPedStrip *spa_ped_strip = dynamic_cast<St_ssdPedStrip *> (SpaStrip->Find("ssdPedStrip"));
  
  if (!spa_strip || spa_strip->GetNRows()==0){
    {
      LOG_WARN << "Make : no input (fired strip for the SSD)"<<endm;
      LOG_WARN <<"Make : looking for a pedestal/noise tables"<<endm;
    }
    if (!spa_ped_strip || spa_ped_strip->GetNRows()==0) {
      LOG_WARN<<"Make : no pedestal/noise data..."<<endm;
      return kStWarn;
    }
    else 
      { LOG_WARN<<"Make : pedestal/noise data found : "<<spa_ped_strip->GetNRows()<<endm;}
  }
  
  St_scm_spt *scm_spt = new St_scm_spt("scm_spt",5000);
  m_DataSet->Add(scm_spt); 
  
  St_scf_cluster *scf_cluster = new St_scf_cluster("scf_cluster",5000);//09/13
  m_DataSet->Add(scf_cluster);
  
  mCurrentEvent = (StEvent*) GetInputDS("StEvent");
  if(mCurrentEvent) 
    {
      mSsdHitColl = mCurrentEvent->ssdHitCollection();
      if (!mSsdHitColl) {
	LOG_WARN << "Make : The SSD hit collection does not exist  - creating a new one" << endm;
	mSsdHitColl = new StSsdHitCollection;
	mCurrentEvent->setSsdHitCollection(mSsdHitColl);
      }
    }
  else              
    mSsdHitColl = 0;
  
  LOG_INFO<<"#################################################"<<endm;
  LOG_INFO<<"####     START OF NEW SSD POINT MAKER        ####"<<endm;
  LOG_INFO<<"####        SSD BARREL INITIALIZATION        ####"<<endm;
  LOG_INFO<<"####          BEGIN INITIALIZATION           ####"<<endm; 
  StSsdBarrel *mySsd =gStSstDbMaker->GetSsd();
  //temporary fixed for y2014
  mySsd->setClusterControl(mClusterControl);
  //The full SSD object is built only if we are processing physics data
  if((! spa_ped_strip || spa_ped_strip->GetNRows()==0) && (spa_strip->GetNRows()!=0))
    {
      Int_t stripTableSize = mySsd->readStripFromTable(spa_strip);
      LOG_INFO<<"####        NUMBER OF SPA STRIPS "<<stripTableSize<<"        ####"<<endm;
      mySsd->sortListStrip();
      PrintStripSummary(mySsd);
      noiseTableSize = 0; 
      noiseTableSize = ReadNoiseTable(mySsd,year);
      LOG_INFO<<"####       NUMBER OF DB ENTRIES "<<noiseTableSize<<"       ####"<<endm;
      Int_t nClusterPerSide[2];
      nClusterPerSide[0] = 0;
      nClusterPerSide[1] = 0;
      mySsd->doSideClusterisation(nClusterPerSide,WafStatus);
      LOG_INFO<<"####      NUMBER OF CLUSTER P SIDE "<<nClusterPerSide[0]<<"      ####"<<endm;
      LOG_INFO<<"####      NUMBER OF CLUSTER N SIDE "<<nClusterPerSide[1]<<"      ####"<<endm;
      mySsd->sortListCluster();
      Int_t nClusterWritten = mySsd->writeClusterToTable(scf_cluster,spa_strip);
      LOG_INFO<<"####   -> "<<nClusterWritten<<" CLUSTERS WRITTEN INTO TABLE       ####"<<endm;
      PrintClusterSummary(mySsd);
      //PrintStripDetails(mySsd,8310);
      //PrintClusterDetails(mySsd,8310); 
      //debugUnPeu(mySsd);
      Int_t nPackage = mySsd->doClusterMatching(CalibArray);
      LOG_INFO<<"####   -> "<<nPackage<<" PACKAGES IN THE SSD           ####"<<endm;
      mySsd->convertDigitToAnalog(mDynamicControl);
      mySsd->convertUFrameToOther();
      PrintPointSummary(mySsd);
      //Int_t nSptWritten = mySsd->writePointToContainer(scm_spt,mSsdHitColl);
      if(Debug()){
	for(Int_t i=1;i<=20;i++)
	  {
	    for(Int_t j=1;j<=16;j++)
	      {
		//PrintStripDetails(mySsd,7000+(100*j)+i);
		//PrintClusterDetails(mySsd,7000+(100*j)+i);
		//PrintPointDetails(mySsd,7000+(100*j)+i);
		//PrintPackageDetails(mySsd,7000+(100*j)+i);
	      }
	  }
      }
      //get McEvent here
      Int_t nSptWritten = 0;
      StMcEvent* mcEvent = 0;
      mcEvent = (StMcEvent*) GetDataSet("StMcEvent");
      if(mcEvent)
	{	
	  LOG_DEBUG << " mcEvent exists " << endm;
	  nSptWritten = mySsd->writePointToContainer(scm_spt,mSsdHitColl,scf_cluster,spa_strip,mDynamicControl,mcEvent);
	}
      else{
	nSptWritten = mySsd->writePointToContainer(scm_spt,mSsdHitColl,scf_cluster);
      }
      LOG_INFO<<"####   -> "<<nSptWritten<<" HITS WRITTEN INTO TABLE       ####"<<endm;
      if(mSsdHitColl){
	if (mSsdHitColl->numberOfHits()>0) {
	  NEvent++;
	  LOG_INFO<<"####   -> "<<mSsdHitColl->numberOfHits()<<" HITS WRITTEN INTO CONTAINER   ####"<<endm;
	  //clean data stream
	  scf_cluster->Purge();
	  scm_spt->Purge();
	}
	else {
	  LOG_INFO<<" ######### NO SSD HITS WRITTEN INTO CONTAINER   ####"<<endm;
	}
      }
      LOG_INFO<<"####        END OF SSD NEW POINT MAKER       ####"<<endm;
      LOG_INFO<<"#################################################"<<endm;
      if (nSptWritten) res = kStOK;
    }
  else
    {
      if((!spa_strip|| spa_strip->GetNRows()==0)&&(spa_ped_strip && spa_ped_strip->GetNRows()!=0))
	{ 
	  LOG_INFO <<"###### WRITING SSD PEDESTAL HISTOGRAMS##########"<<endm;
	  if(year<7 || year>13){
	    mySsd->writeNoiseToFile(spa_ped_strip,myLabel);}
	  else{mySsd->writeNewNoiseToFile3(spa_ped_strip,myLabel);//new method
	    printf("done\n");
	  }
	}
    }
  mySsd->Reset();
  if(res!=kStOK){
    LOG_WARN <<"Make : no output" << endm;;
    return kStWarn;
  }
  return kStOK;
}
//_____________________________________________________________________________

void StSsdPointMaker::PrintStripSummary(StSsdBarrel *mySsd)
{
  Int_t ladderCountN[20]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0} ;
  Int_t ladderCountP[20]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0} ;
  for (Int_t i=0;i<20;i++) 
    if (mySsd->isActiveLadder(i)>0) {
      for (Int_t j=0; j<mySsd->mLadders[i]->getWaferPerLadder();j++) {
	ladderCountP[i]=ladderCountP[i]+mySsd->mLadders[i]->mWafers[j]->getStripP()->getSize();
	ladderCountN[i]=ladderCountN[i]+mySsd->mLadders[i]->mWafers[j]->getStripN()->getSize();
      }
    }
  
  LOG_INFO <<"PrintStripSummary : Number of raw data in the SSD" << endm;
  LOG_INFO << "PrintStripSummary : Active Ladders : " << endm;
  for (Int_t i=0;i<20;i++) 
    if (mySsd->isActiveLadder(i)>0) {
      LOG_DEBUG.width(5);
      LOG_DEBUG<<i+1;
    }
  
  LOG_DEBUG<<endm;
  LOG_INFO << "PrintStripSummary : Counts (p-side): " << endm;
  for (Int_t i=0;i<20;i++)
    if (mySsd->isActiveLadder(i)>0) {
      LOG_DEBUG.width(5);
      LOG_DEBUG <<ladderCountP[i];
    }
  LOG_DEBUG<<endm;
  LOG_INFO << "PrintStripSummary : Counts (n-side): " << endm;
  for (Int_t i=0;i<20;i++)
    if (mySsd->isActiveLadder(i)>0) {
      LOG_DEBUG.width(5);
      LOG_DEBUG <<ladderCountN[i];
    }
  LOG_DEBUG<<endm;
}

//_____________________________________________________________________________
void StSsdPointMaker::debugUnPeu(StSsdBarrel *mySsd)
{
  Int_t monladder,monwafer;
  monladder=7;
  monwafer=6;
  mySsd->debugUnPeu(monladder,monwafer);
}

//_____________________________________________________________________________
void StSsdPointMaker::PrintClusterSummary(StSsdBarrel *mySsd)
{
  Int_t ladderCountN[20]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0} ;
  Int_t ladderCountP[20]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0} ;
  for (Int_t i=0;i<20;i++) 
    if (mySsd->isActiveLadder(i)>0) {
      for (Int_t j=0; j<mySsd->mLadders[i]->getWaferPerLadder();j++) {
	ladderCountP[i]=ladderCountP[i]+mySsd->mLadders[i]->mWafers[j]->getClusterP()->getSize();
	ladderCountN[i]=ladderCountN[i]+mySsd->mLadders[i]->mWafers[j]->getClusterN()->getSize();
      }
    }
  
  LOG_INFO <<"PrintClusterSummary : Number of clusters in the SSD" << endm;
  LOG_INFO << "PrintClusterSummary : Active Ladders : " << endm;
  for (Int_t i=0;i<20;i++) 
    if (mySsd->isActiveLadder(i)>0) {
      LOG_DEBUG.width(5);
      LOG_DEBUG<<i+1;
    }
  
  LOG_DEBUG<<endm;
  LOG_INFO << "PrintClusterSummary : Counts (p-side): " << endm;
  for (Int_t i=0;i<20;i++)
    if (mySsd->isActiveLadder(i)>0) {
      LOG_DEBUG.width(5);
      LOG_DEBUG <<ladderCountP[i];
    }
  LOG_DEBUG<<endm;
  LOG_INFO << "PrintClusterSummary : Counts (n-side): " << endm;
  for (Int_t i=0;i<20;i++)
    if (mySsd->isActiveLadder(i)>0) {
      LOG_DEBUG.width(5);
      LOG_DEBUG <<ladderCountN[i];
    }
  LOG_DEBUG<<endm;
}
//_____________________________________________________________________________
void StSsdPointMaker::PrintPointSummary(StSsdBarrel *mySsd)
{
  Int_t ladderCount[20]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0} ;
  Int_t ladderCount11[20]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0} ;
  for (Int_t i=0;i<20;i++) 
    if (mySsd->isActiveLadder(i)>0) {
      for (Int_t j=0; j<mySsd->mLadders[i]->getWaferPerLadder();j++) {
	ladderCount[i]=ladderCount[i]+mySsd->mLadders[i]->mWafers[j]->getPoint()->getSize();
	StSsdPoint *pSpt = mySsd->mLadders[i]->mWafers[j]->getPoint()->first();
	while (pSpt){	
	  if (pSpt->getNMatched()==11) ladderCount11[i]++;
	  pSpt    = mySsd->mLadders[i]->mWafers[j]->getPoint()->next(pSpt);
	}
      }
    }
  
  LOG_INFO<<"PrintPointSummary : Number of hits in the SSD" << endm;
  LOG_INFO<< "PrintPointSummary : Active Ladders : " << endm;
  for (Int_t i=0;i<20;i++) 
    if (mySsd->isActiveLadder(i)>0) {
      LOG_DEBUG.width(5);
      LOG_DEBUG<<i+1;
    }
  
  LOG_DEBUG<<endm;
  LOG_INFO << "PrintPointSummary : Counts         : " << endm;
  for (Int_t i=0;i<20;i++)
    if (mySsd->isActiveLadder(i)>0) {
      LOG_DEBUG.width(5);
      LOG_DEBUG <<ladderCount[i];
    }
  LOG_DEBUG<<endm;
  LOG_INFO << "PrintPointSummary : Counts  (11)   : " << endm;
  for (Int_t i=0;i<20;i++)
    if (mySsd->isActiveLadder(i)>0) {
      LOG_DEBUG.width(5);
      LOG_DEBUG <<ladderCount11[i];
    }
  LOG_DEBUG<<endm;
}
//_____________________________________________________________________________
void StSsdPointMaker::PrintStripDetails(StSsdBarrel *mySsd, Int_t mywafer)
{
  //Int_t LadderIsActive[20]={1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1} ;
  Int_t found = 0 ;
  LOG_DEBUG <<"PrintStripDetails() - Wafer "<<mywafer<< endm;  
  for (Int_t i=0;i<20;i++) 
    if (mySsd->isActiveLadder(i)>0) {
      for (Int_t j=0; j<mySsd->mLadders[i]->getWaferPerLadder();j++) {
        if (mySsd->mLadders[i]->mWafers[j]->getIdWafer()==mywafer) {
          found=1;
          //Looking for the P-side strip informations
          if (mySsd->mLadders[i]->mWafers[j]->getStripP()->getSize()==0) {
            LOG_DEBUG <<"PrintStripDetails() - No strip on the P-side of this wafer "<< endm;  
          }
          else {
            LOG_DEBUG<<"PrintStripDetails() - "
		     <<mySsd->mLadders[i]->mWafers[j]->getStripP()->getSize()<<" strip(s) on the P-side of this wafer "<< endm;  
            LOG_DEBUG<<"PrintStripDetails() - Strip/Adc/Ped/Noise/Analog"<< endm;  
            StSsdStrip *pStripP = mySsd->mLadders[i]->mWafers[j]->getStripP()->first();
            while (pStripP){
              LOG_DEBUG<<"PrintStripDetails() - "
		       <<pStripP->getNStrip()<<" "
		       <<pStripP->getDigitSig()<<" "
		       <<pStripP->getPedestal()<<" "
		       <<pStripP->getSigma()<<" "
		       <<pStripP->getAnalogSig()<<" "
		       <<endm;  
	      for(Int_t e=0;e<5;e++){printf("e=%d idMcHit=%d idMcTrack=%d\n",e,pStripP->getIdMcHit(e),pStripP->getIdMcTrack(e));}
              pStripP    = mySsd->mLadders[i]->mWafers[j]->getStripP()->next(pStripP);
            }
	  }
          //Looking for the N-side strip informations
          if (mySsd->mLadders[i]->mWafers[j]->getStripN()->getSize()==0) {
            LOG_DEBUG <<"PrintStripDetails() - No strip on the N-side of this wafer "<< endm;  
          }
          else {
            LOG_DEBUG<<"PrintStripDetails() - "
		     <<mySsd->mLadders[i]->mWafers[j]->getStripN()->getSize()<<" strip(s) on the N-side of this wafer "<< endm;  
            LOG_DEBUG <<"StSsdPointMaker::PrintStripDetails() - Strip/Adc/Ped/Noise/Analog"<< endm;  
            StSsdStrip *pStripN = mySsd->mLadders[i]->mWafers[j]->getStripN()->first();
            while (pStripN){
              LOG_DEBUG<<"PrintStripDetails() - "
		       <<pStripN->getNStrip()<<" "
		       <<pStripN->getDigitSig()<<" "
		       <<pStripN->getPedestal()<<" "
		       <<pStripN->getSigma()<<" "
		       <<pStripN->getAnalogSig()<<" "
		       <<endm;  
	      for(Int_t e=0;e<5;e++){printf("e=%d idMcHit=%d idMcTrack=%d\n",e,pStripN->getIdMcHit(e),pStripN->getIdMcTrack(e));}
              pStripN    = mySsd->mLadders[i]->mWafers[j]->getStripN()->next(pStripN);
            }     
          }
        }
      }
    } 
  if (found==0) {LOG_DEBUG <<"PrintStripDetails() - Wafer not found !!!"<<endm;}
}
//_____________________________________________________________________________
void StSsdPointMaker::PrintClusterDetails(StSsdBarrel *mySsd, Int_t mywafer)
{
  //Int_t LadderIsActive[20]={1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1} ;
  Int_t found = 0;
  LOG_INFO <<"PrintClusterDetails() - Wafer "<<mywafer<< endm;  
  for (Int_t i=0;i<20;i++) 
    if (mySsd->isActiveLadder(i)>0) {
      for (Int_t j=0; j<mySsd->mLadders[i]->getWaferPerLadder();j++) {
        if (mySsd->mLadders[i]->mWafers[j]->getIdWafer()==mywafer) {
          found=1;
          //Looking for the P-side cluster informations
          if (mySsd->mLadders[i]->mWafers[j]->getClusterP()->getSize()==0) {
            LOG_INFO <<"PrintClusterDetails() - No cluster on the P-side of this wafer "<< endm;  
          }
          else {
            LOG_INFO<<"PrintClusterDetails() - "
		    <<mySsd->mLadders[i]->mWafers[j]->getClusterP()->getSize()<<" cluster(s) on the P-side of this wafer "<< endm;  
            LOG_INFO<<"PrintClusterDetails() - Cluster/Flag/Size/1st Strip/Strip Mean/TotAdc/1st Adc/Last Adc/TotNoise"<< endm;  
            StSsdCluster *pClusterP = mySsd->mLadders[i]->mWafers[j]->getClusterP()->first();
            while (pClusterP){
              LOG_INFO<<"PrintClusterDetails() - "
		      <<pClusterP->getNCluster()<<" "
		      <<pClusterP->getFlag()<<" "
		      <<pClusterP->getClusterSize()<<" "
		      <<pClusterP->getFirstStrip()<<" "
		      <<pClusterP->getStripMean()<<" "
		      <<pClusterP->getTotAdc()<<" "
		      <<pClusterP->getFirstAdc()<<" "
		      <<pClusterP->getLastAdc()<<" "
		      <<pClusterP->getTotNoise()<<" "
		      <<endm;  
	      for(Int_t e=0;e<5;e++){printf("e=%d idMcHit=%d \n",e,pClusterP->getIdMcHit(e));}
              pClusterP    = mySsd->mLadders[i]->mWafers[j]->getClusterP()->next(pClusterP);
            }
	  }
          //Looking for the N-side cluster informations
          if (mySsd->mLadders[i]->mWafers[j]->getClusterN()->getSize()==0) {
            LOG_INFO <<"PrintClusterDetails() - No cluster on the N-side of this wafer "<< endm;  
          }
          else {
            LOG_INFO<<"PrintClusterDetails() - "
		    <<mySsd->mLadders[i]->mWafers[j]->getClusterN()->getSize()<<" cluster(s) on the N-side of this wafer "<< endm;  
            LOG_INFO<<"PrintClusterDetails() - Cluster/Flag/Size/1st Strip/Strip Mean/TotAdc/1st Adc/Last Adc/TotNoise"<< endm;  
            StSsdCluster *pClusterN = mySsd->mLadders[i]->mWafers[j]->getClusterN()->first();
            while (pClusterN){
              LOG_INFO<<"PrintClusterDetails() - "
		      <<pClusterN->getNCluster()<<" "
		      <<pClusterN->getFlag()<<" "
		      <<pClusterN->getClusterSize()<<" "
		      <<pClusterN->getFirstStrip()<<" "
		      <<pClusterN->getStripMean()<<" "
		      <<pClusterN->getTotAdc()<<" "
		      <<pClusterN->getFirstAdc()<<" "
		      <<pClusterN->getLastAdc()<<" "
		      <<pClusterN->getTotNoise()<<" "
		      <<endm;  
	      for(Int_t e=0;e<5;e++){printf("e=%d idMcHit=%d \n",e,pClusterN->getIdMcHit(e));}
              pClusterN    = mySsd->mLadders[i]->mWafers[j]->getClusterN()->next(pClusterN);
            }     
          }
        }
      }
    }
  if (found==0){ LOG_INFO <<"PrintClusterDetails() - Wafer not found !!!"<<endm; }
}

//_____________________________________________________________________________
void StSsdPointMaker::PrintPackageDetails(StSsdBarrel *mySsd, Int_t mywafer)
{
  //Int_t LadderIsActive[20]={1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1} ;
  Int_t found;
  found=0;
  LOG_INFO <<"PrintPackageDetails() - Wafer "<<mywafer<< endm;  
  for (Int_t i=0;i<20;i++) 
    if (mySsd->isActiveLadder(i)>0) {
      for (Int_t j=0; j<mySsd->mLadders[i]->getWaferPerLadder();j++) {
        if (mySsd->mLadders[i]->mWafers[j]->getIdWafer()==mywafer) {
          found=1;
          if (mySsd->mLadders[i]->mWafers[j]->getPackage()->getSize()==0) {
            LOG_INFO <<"PrintPackageDetails() - No package in this wafer "<< endm;  
          }
          else {
            LOG_INFO <<"PrintPackageDetails() - "<<mySsd->mLadders[i]->mWafers[j]->getPackage()->getSize()<<" package(s) in this wafer "<< endm;  
            LOG_INFO <<"PrintPackageDetails() - Package/Kind/Size"<< endm;  
            StSsdPackage *pPack = mySsd->mLadders[i]->mWafers[j]->getPackage()->first();
            while (pPack){
              LOG_INFO<<"PrintPackageDetails() - "<<pPack->getNPackage()<<" "
		      <<pPack->getKind()<<" "
		      <<pPack->getSize()<<" "<<endm;
              for (Int_t k=0;k<pPack->getSize();k++) {
                LOG_INFO<<"PrintPackageDetails() - "<<k<<" "<<pPack->getMatched(k)<<" "<<pPack->getMatched(k)->getNCluster()<<endm;
              }
              pPack    = mySsd->mLadders[i]->mWafers[j]->getPackage()->next(pPack);
            }     
          }
        }
      }
    }
  if (found==0){ LOG_INFO <<"PrintPackageDetails() - Wafer not found !!!"<<endm;}
}

//_____________________________________________________________________________
void StSsdPointMaker::PrintPointDetails(StSsdBarrel *mySsd, Int_t mywafer)
{
  //Int_t LadderIsActive[20]={1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1} ;
  Int_t found;
  found=0;
  Float_t convMeVToAdc = (int)pow(2.0,mDynamicControl->getnbitEncoding())/(mDynamicControl->getpairCreationEnergy()*mDynamicControl->getadcDynamic()*mDynamicControl->getnElectronInAMip());
  LOG_INFO <<"PrintPointDetails() - Wafer "<<mywafer<< endm;  
  for (Int_t i=0;i<20;i++) 
    if (mySsd->isActiveLadder(i)>0) {
      for (Int_t j=0; j<mySsd->mLadders[i]->getWaferPerLadder();j++) {
        if (mySsd->mLadders[i]->mWafers[j]->getIdWafer()==mywafer) {
          found=1;
          if (mySsd->mLadders[i]->mWafers[j]->getPoint()->getSize()==0) {
            LOG_INFO <<"PrintPointDetails() - No hit in this wafer "<< endm;  
          }
          else {
            LOG_INFO<<"PrintPointDetails() - "<<mySsd->mLadders[i]->mWafers[j]->getPoint()->getSize()<<" hit(s) in this wafer "<< endm; 
 
            LOG_INFO<<"PrintPointDetails() - Hit/Flag/NMatched/IdClusP/IdClusN/idMcHit[0]/idMcHit[1]/idMcHit[2]/idMcHit[3]/idMcHit[4]/Xg[0]/Xg[1]/Xg[2]/Xl[0]/Xl[1]/Xl[2]/a/b"<<endm;  
            StSsdPoint *pSpt = mySsd->mLadders[i]->mWafers[j]->getPoint()->first();
            while (pSpt){
	      Float_t a = 0, b = 0;
	      a = convMeVToAdc*(pSpt->getDe(0)+pSpt->getDe(1));
	      b = convMeVToAdc*(pSpt->getDe(0)-pSpt->getDe(1));
              LOG_INFO<<"PrintPointDetails() - "
		      <<pSpt->getNPoint()    <<" "
		      <<pSpt->getFlag()      <<" "
		      <<pSpt->getNMatched()  <<" "
		      <<pSpt->getIdClusterP()<<" "
		      <<pSpt->getIdClusterN()<<" "
		      <<pSpt->getNMchit(0)   <<" "
		      <<pSpt->getNMchit(1)   <<" "
		      <<pSpt->getNMchit(2)   <<" "
		      <<pSpt->getNMchit(3)   <<" "
		      <<pSpt->getNMchit(4)   <<" "
		      <<pSpt->getXg(0)       <<" "
		      <<pSpt->getXg(1)       <<" "
		      <<pSpt->getXg(2)       <<" "
		      <<pSpt->getXl(0)       <<" "
		      <<pSpt->getXl(1)       <<" "
		      <<pSpt->getXl(2)       <<" "
		      <<a                    <<" "
		      <<b                    <<" "
		      <<endm;
	      printf("pulseP =%f pulseN = %f\n",a,b);  
              pSpt    = mySsd->mLadders[i]->mWafers[j]->getPoint()->next(pSpt);
            }     
          }
        }
      }
    }
  if (found==0) {LOG_INFO <<"PrintPointDetails() - Wafer not found !!!"<<endm; }
}

//_____________________________________________________________________________
void StSsdPointMaker::PrintInfo()
{
  if (Debug()) StMaker::PrintInfo();
}
//_____________________________________________________________________________
void StSsdPointMaker::Read_Strip(St_ssdStripCalib *strip_calib)
{
  ssdStripCalib_st *noise = strip_calib->GetTable();
  Int_t  mSsdLayer = 7;
  LOG_INFO << "Read_Strip : printing few pedestal/noise values " << endm;
  Int_t idWaf  = 0;
  Int_t iWaf   = 0;
  Int_t iLad   = 0;
  Int_t iZero  = 0;
  Int_t nStrip = 0;
  Int_t iSide  = 0;
  Int_t iOver  = 0;
  Int_t iUnder = 0;
  Int_t iGood  = 0;
  for (Int_t i = 0 ; i < strip_calib->GetNRows(); i++)
    {
      if (noise[i].id>0 && noise[i].id<=76818620) {
	nStrip  = (int)(noise[i].id/100000.);
	idWaf   = noise[i].id-10000*((int)(noise[i].id/10000.));
	iWaf    = (int)((idWaf - mSsdLayer*1000)/100 - 1);
	iLad    = (int)(idWaf - mSsdLayer*1000 - (iWaf+1)*100 - 1);
	iSide   = (noise[i].id - nStrip*100000 - idWaf)/10000;
	if (iLad==11 && iWaf==8 && nStrip <10) {
	  LOG_DEBUG<<"ReadStrip: iLad,idWaf,nStrip,iSide,pedestal,rms = "<<iLad
		  <<" "<<idWaf
		  <<" "<<nStrip
		  <<" "<<iSide
		  <<" "<<(float)(noise[i].pedestals)
		  <<" "<<(float)(noise[i].rms)<<endm;
	  iGood++;
	}
      }
      else {
	if (noise[i].id<0) iUnder++;
	else {
	  if (noise[i].id==0) iZero++;
	  else iOver++;
	}
      }
    }
  LOG_INFO<<"ReadStrip: Number of rows in the table : "<<strip_calib->GetNRows()<<endm;
  LOG_INFO<<"ReadStrip: Number of good id : "<<iGood<<endm;
  LOG_INFO<<"ReadStrip: Number of id = 0  : "<<iZero<<endm;
  if (iUnder>0){
    LOG_WARN <<"ReadStrip: Number of underf  : "<<iUnder<<endm;}
  if (iOver>0){
    LOG_WARN <<"ReadStrip: Number of overf   : "<<iOver<<endm;}
 }

//_____________________________________________________________________________
void StSsdPointMaker::Read_Strip(St_ssdNoise *strip)
{
  ssdNoise_st *noise = strip->GetTable();
  LOG_INFO << " ReadStrip : printing few pedestal/noise values "<< endm;
  Int_t  mNWaferPerLadder = 16;
  Int_t iWaf              = 0;
  Int_t iLad              = 0;
  for (Int_t i = 0 ; i < strip->GetNRows(); i++)
    {
      iWaf = noise[i].id-(noise[i].id/mNWaferPerLadder)*mNWaferPerLadder;
      iLad = noise[i].id/16;
      Int_t idWaf = noise[i].id;
      if (idWaf==1)
	{
	  for(Int_t nStrip=0;nStrip<10;nStrip++)
	    {
	      LOG_DEBUG<<"ReadStrip: iLad,idWaf,nStrip,rmsP,rmsN = "<<iLad+1
		      <<" "<<idWaf+1
		      <<" "<<nStrip
		      <<" "<<(int)(noise[i].rmsp[nStrip])
		      <<" "<<(int)(noise[i].rmsn[nStrip])<<endm;
	    }
	}
    }  
  LOG_INFO<<"ReadStrip: Number of rows in the table : "<<strip->GetNRows()<<endm;
 }
//_____________________________________________________________________________
void StSsdPointMaker::FillCalibTable(){
  mGain          = (St_ssdGainCalibWafer*)GetDataBase("Calibrations/ssd/ssdGainCalibWafer"); 
  if(mGain){ 
    ssdGainCalibWafer_st *g  = mGain->GetTable() ;
    Int_t size = mGain->GetNRows();
    LOG_INFO<<Form("Size of gain table = %d",(int)mGain->GetNRows())<<endm;
    for(Int_t i=0; i<size;i++){
      LOG_DEBUG<<Form(" Print entry %d : ladder=%d gain =%lf wafer=%d",i,g[i].nLadder,g[i].nGain,g[i].nWafer)<<endm;
      CalibArray[i] = g[i].nGain;
    }
  }
  else { 
    LOG_WARN << "InitRun : No access to Gain Calib - will use the default gain" << endm;
    LOG_WARN << "We will use the default table" <<endm;
    for(Int_t i=0; i<320;i++){
      CalibArray[i] = 1;
    }
  }
}
//_____________________________________________________________________________
void StSsdPointMaker::FillDefaultCalibTable(){
  LOG_INFO << " The calibration gain will not be used." << endm;
  for(Int_t i=0; i<320;i++){
    CalibArray[i] = 1;
    //LOG_INFO << Form("wafer=%d gain=%f",i,CalibArray[i])<<endm; 
  }
}
//_____________________________________________________________________________
void StSsdPointMaker::FillWaferTable(){
  mWafConfig    = (St_ssdWaferConfiguration*) GetDataBase("Calibrations/ssd/ssdWaferConfiguration"); 
  if(mWafConfig){ 
    ssdWaferConfiguration_st *g  = mWafConfig->GetTable() ;
    Int_t size = mWafConfig->GetNRows();
    for(Int_t i=0; i<size;i++){
      LOG_DEBUG<<Form(" Print entry=%d : ladder=%d wafer=%d status=%d",i,g[i].nLadder,g[i].nWafer,g[i].nStatus)<<endm;
      WafStatus[g[i].nLadder][g[i].nWafer] = g[i].nStatus; 
    }
  } 
  else { 
    LOG_WARN << "InitRun : No access to Wafer Config - will use the default wafer config" << endm;
    LOG_WARN << "We will use the default table" <<endm;
    for(Int_t i=0; i<20;i++){
      for(Int_t j=0; j<16;j++){ 
	WafStatus[i][j] = 1;
      }
    }
  }
}
//_____________________________________________________________________________
void StSsdPointMaker::FillDefaultWaferTable(){
  LOG_INFO << " The wafer configuration table will not be used." << endm;
  for(Int_t i=0; i<20;i++){ 
    for(Int_t j=0; i<16;j++){  
      WafStatus[i][j] = 1; 
      LOG_DEBUG << Form("wafer=%d gain=%f",i,CalibArray[i])<<endm; 
    }
  }
}
//_____________________________________________________________________________
void StSsdPointMaker::FillDefaultChipNoiseTable(){
  LOG_DEBUG <<" do something here " << endm;
}
//_____________________________________________________________________________
Int_t StSsdPointMaker::ReadNoiseTable(StSsdBarrel *mySsd,Int_t year){
  Int_t noiseTableSize = 0; 
  
  //ssdStripCalib is used for year <2007 and for the simulation
  if((year<7)||(mode==1))
    {
      if (!m_noise2)
	{
	  LOG_WARN << "Make : No pedestal and noise values (ssdStripCalib table missing), will use default values" <<endm;
	  noiseTableSize = mySsd->readNoiseDefault(mDynamicControl);
	}
      else
	{
	  if(Debug()) {Read_Strip(m_noise2);}
	  noiseTableSize = mySsd->readNoiseFromTable(m_noise2,mDynamicControl);
	}
    }
  else if (year>=7){
    if(!m_noise3)
      {
	LOG_WARN << "Make : No pedestal and noise values (ssdNoise table missing), will use default values" << endm;
	noiseTableSize = mySsd->readNoiseDefault(mDynamicControl);
      }
    else
      if(m_noise3)
	{
	  if (Debug()){Read_Strip(m_noise3);}
	  noiseTableSize = mySsd->readNoiseFromTable(m_noise3,mDynamicControl);
	}
  }
  return noiseTableSize;  
}
//____________________________________________________________________________
Int_t StSsdPointMaker::Finish() {
  LOG_INFO << Form("Finish()") << endm;
   return kStOK;
}

