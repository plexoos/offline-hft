#include <iostream>
#include <fstream>
#include "StSsdMONMaker.h"
#include "TFile.h"
#include "TH1.h"
#include "TH2.h"
#include "TDataSetIter.h"
#include "StMessMgr.h"
#include "tables/St_spa_strip_Table.h"
#include "tables/St_scf_cluster_Table.h"
#include "tables/St_scm_spt_Table.h"
#include "tables/St_slsCtrl_Table.h"
#include "StSsdDbMaker/StSsdDbMaker.h"
#include "TMath.h"
#include "StIOMaker/StIOMaker.h"
ClassImp(StSsdMONMaker);

 // The constructor. Initialize you data members here.
StSsdMONMaker::StSsdMONMaker(const Char_t *name) : StMaker(name)
{}
//_____________________________________________________________________________
Int_t StSsdMONMaker::Init(){
  Int_t ierr    = kStOk;
  Int_t counter = 0;
  Int_t currLad = 0 ;
  Int_t currWaf = 0;
  mEventCounter = 0;
  
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
  convMeVToADC = (int)pow(2.0,mDynamicControl->getnbitEncoding())/(mDynamicControl->getpairCreationEnergy()*mDynamicControl->getadcDynamic()*mDynamicControl->getnElectronInAMip());
  
  LOG_INFO << "Init() : Defining the histograms" << endm;
  for(Int_t ii=0;ii<20;++ii){
    for(Int_t jj=0;jj<16;++jj){
      counter = ii*16+jj;
      currLad = ii+1;
      currWaf = jj+1;
      char Title[200];
      char Name[200];
      
      std::cout << " counter : " << counter << " currLad/currWaf : " << currLad <<" " << currWaf << std::endl;
      
      //side P strips  
      sprintf(Title,"Strip Adc vs. channels for ladder P : %i wafer : %i",currLad,currWaf);
      sprintf(Name,"stripADCCHANsideP%i",counter);
      m_strip_ADC_CHAN_sideP[counter] =  new TH2S(Name,Title,770,0,770,1050,0,1050);
      m_strip_ADC_CHAN_sideP[counter]->SetXTitle("channels");	
      m_strip_ADC_CHAN_sideP[counter]->SetYTitle("signal [adc counts]");	
      std::cout <<" name/title : " << Name <<" " << Title << std::endl;
      
      //side N strips   
      sprintf(Title,"Strip adc vs. channels for ladder N : %i wafer : %i",currLad,currWaf);
      sprintf(Name,"stripADCCHANsideN%i",counter);
      m_strip_ADC_CHAN_sideN[counter] =  new TH2S(Name,Title,770,0,770,1050,0,1050);
      m_strip_ADC_CHAN_sideN[counter]->SetXTitle("channels");	
      m_strip_ADC_CHAN_sideN[counter]->SetYTitle("signal [adc counts]");	
      std::cout <<" name/title : " << Name <<" " << Title << std::endl;
      
      //side P clusters 
      sprintf(Title,"Cluster ADC for ladder P : %i wafer : %i",currLad,currWaf);
      sprintf(Name,"clusterADCSideP%i",counter);
      m_cluster_ADC_sideP[counter] =  new TH2S(Name,Title,1050,0,1050,100,0,100);
      m_cluster_ADC_sideP[counter]->SetXTitle("signal [adc counts]");
      m_cluster_ADC_sideP[counter]->SetYTitle("rms [adc counts]");
      sprintf(Title,"Cluster Size for ladder P : %i wafer : %i",currLad,currWaf);
      sprintf(Name,"clusterSizeSideP%i",counter);
      m_cluster_Size_sideP[counter] =  new TH1S(Name,Title,100,0,100);
      m_cluster_Size_sideP[counter]->SetXTitle("size [# of strips]");	
      
      //side N clusters   
      sprintf(Title,"Cluster ADC for ladder N : %i wafer : %i",currLad,currWaf);
      sprintf(Name,"clusterADCSideN%i",counter);
      m_cluster_ADC_sideN[counter] =  new TH2S(Name,Title,1050,0,1050,100,0,100);
      m_cluster_ADC_sideN[counter]->SetXTitle("signal [adc counts]");
      m_cluster_ADC_sideN[counter]->SetYTitle("rms [adc counts]");
      sprintf(Title,"Cluster Size for ladder N : %i wafer : %i",currLad,currWaf);
      sprintf(Name,"clusterSizeSideN%i",counter);
      m_cluster_Size_sideN[counter] =  new TH1S(Name,Title,100,0,100);
      m_cluster_Size_sideN[counter]->SetXTitle("size [# of strips]");	
      
      //points
      sprintf(Title,"matching ADC for ladder : %i wafer :%i ",currLad,currWaf);
      sprintf(Name,"matchingADC%i",counter);
      m_matchisto[counter] =  new TH2D(Name,Title,525,0,1050,525,0,1050);
      m_matchisto[counter]->SetXTitle("side P [adc counts]");
      m_matchisto[counter]->SetYTitle("side N [adc counts]");
    }
  }
  
  //global histograms
  globalMatchisto = new TH2S("globalMatchisto","matching ADC for all ladders",525,0,1050,525,0,1050);
  globalMatchisto->SetXTitle("ADC clusters P-side [adc counts]");
  globalMatchisto->SetYTitle("ADC clusters N-side [adc counts]");
  globalDeviation = new TH1S("globalDeviation","ADC cluster P - ADC clusterN for all ladders",200,-100,100);
  globalDeviation->SetXTitle("ADC clusters PSide - ADC clusters NSide [adc counts]");
  LadderWaferHits = new TH2S("LadderWaferHits","number of reconstructed hits per wafer",20,1,21,16,1,17);
  LadderWaferHits->SetXTitle("Ladder Id");
  LadderWaferHits->SetYTitle("Wafer Id");
  globalPosition  = new TH2D("globalPosition","global Y vs. global X positions of reconstructed points",250,-25,25,250,-25,25);
  globalPosition->SetXTitle(" global X position [cm] ");
  globalPosition->SetYTitle(" global Y position [cm] ");
  localPosition   = new TH2D("localPosition","local U vs. local V positions of reconstructed points",375,-3.75,3.75,200,-2.5,2.5);
  localPosition->SetXTitle(" local U position [cm] ");
  localPosition->SetYTitle(" local V position [cm] ");
  rphiZ           = new TH2D("rphiZ","azimuth vs. Z positions of reconstructed points",160,-40,40,160,-3.2,3.2);
  rphiZ->SetXTitle(" global Z position [cm]");
  rphiZ->SetYTitle(" r/#phi coordinate ");
  return ierr;
}
//____________________________________________________________________________
Int_t StSsdMONMaker::Make()
{
  //get pointers to SSD clusters and points tables
  mEventCounter++;

  St_spa_strip *spa_strip = (St_spa_strip *) GetDataSet("spa_strip");
  if (!spa_strip) {
    LOG_ERROR << "Make : no strip ADC data set" << endm;
    return kStErr;
  }
  spa_strip_st *on_strip = spa_strip->GetTable();
  makeSpaCtrlHistograms(spa_strip);
  if(Debug()>2){
    LOG_INFO <<" size of strip tables : " << spa_strip->GetNRows() << endm;
    for(Int_t j = 0 ; j < spa_strip->GetNRows(); j++){
      //LOG_INFO <<" strip : " << j << " ADC :  "<< on_strip[j].adc_count << endm;
    }
  }
  St_scf_cluster *scf_cluster = (St_scf_cluster *) GetDataSet("scf_cluster");
  if (!scf_cluster) {
    LOG_ERROR << "Make : no cluster data set" << endm;
    return kStErr;
  }
  scf_cluster_st *on_cluster = scf_cluster->GetTable();
  makeScfCtrlHistograms(scf_cluster);
  if(1){//Debug()>2){
    LOG_INFO <<" size of clusters tables : " << scf_cluster->GetNRows() << endm;
    for(Int_t j = 0 ; j < scf_cluster->GetNRows(); j++){
      //LOG_INFO <<" cluster : " << j << " ADC :  "<< on_cluster[j].adc_count << endm;
    }
  }
  St_scm_spt *scm_spt = (St_scm_spt *) GetDataSet("scm_spt");
  if (!scm_spt) {
    LOG_ERROR << "Make : no point_ts data set" << endm;
    return kStErr;
  }
  scm_spt_st *on_point = scm_spt->GetTable(); 
  makeScmCtrlHistograms(scm_spt);
  if(1){//Debug()>2){
    LOG_INFO <<" size of points tables : " << scm_spt->GetNRows() << endm;
    for(Int_t j = 0 ; j < scm_spt->GetNRows(); j++){
      //LOG_INFO <<" points : " << j << " x/y/z :  "<< on_point[j].x[0] <<" "<< on_point[j].x[1] << " " <<on_point[j].x[2] << endm;
    }
  }
  //delete clusters and points tables
  scf_cluster->Purge();
  scm_spt->Purge();
  
  return kStOK;
}
//_____________________________________________________________________________
void StSsdMONMaker::makeSpaCtrlHistograms(St_spa_strip *spa_strip){
  //stripDir->cd();
  Int_t idWaf,iSide,iWaf,iLad,nstrip;
  spa_strip_st *on_strip = spa_strip->GetTable();
  for(Int_t i = 0 ; i < spa_strip->GetNRows(); i++){
    nstrip    = (int)(on_strip[i].id_strip/100000.);
    idWaf     = on_strip[i].id_strip-10000*((int)(on_strip[i].id_strip/10000.));
    iWaf      = idWaferToWafer(idWaf);
    iLad      = (int)(idWaf - 7*1000 - (iWaf+1)*100 - 1);
    iSide     = (on_strip[i].id_strip - nstrip*100000 - idWaf)/10000;
    if(iSide==0){
      m_strip_ADC_CHAN_sideP[iLad*16+iWaf]->Fill((int)nstrip,(int)on_strip[i].adc_count);
    }
    else{
      m_strip_ADC_CHAN_sideN[iLad*16+iWaf]->Fill((int)nstrip,(int)on_strip[i].adc_count);
    }
  }
}
//_____________________________________________________________________________
void StSsdMONMaker::makeScfCtrlHistograms(St_scf_cluster *scf_cluster){
  //clusDir->cd();
  float nStripMean;
  Int_t idWaf,iSide,iWaf,iLad,nFirstStrip,nStrip,nFirstAdc,nLastAdc,nAdcCount,nNoiseCount,nFlag,nCluster;
  scf_cluster_st *on_cluster = scf_cluster->GetTable();
  for(Int_t i = 0 ; i < scf_cluster->GetNRows(); i++){
    //std::cout <<" i/nCluster_id/id_cluster : " << i << " " << (int)(on_cluster[i].id) <<" " << (int)(on_cluster[i].id_cluster) << std::endl;
    nCluster    = (int)(on_cluster[i].id_cluster/100000.); 
    idWaf       = (on_cluster[i].id_cluster-10000*((int)(on_cluster[i].id_cluster/10000.))); 
    iSide       = (on_cluster[i].id_cluster-idWaf-nCluster*100000)/10000; 
    iWaf        = idWaferToWafer(idWaf); 
    iLad        = (int)(idWaf - 7*1000 - (iWaf+1)*100 - 1); 
    //std::cout <<" idWaf/iWaf/iLad/iSide : " << idWaf << " " << iWaf <<" " << iLad <<" "<< iSide << std::endl;
    if(iSide==0){
      m_cluster_ADC_sideP[iLad*16+iWaf]->Fill(on_cluster[i].adc_count,on_cluster[i].noise_count);
      m_cluster_Size_sideP[iLad*16+iWaf]->Fill(on_cluster[i].n_strip);
    }
    else
      {    
	m_cluster_ADC_sideN[iLad*16+iWaf]->Fill(on_cluster[i].adc_count,on_cluster[i].noise_count);
	m_cluster_Size_sideN[iLad*16+iWaf]->Fill(on_cluster[i].n_strip);
      }
  }
}
//_____________________________________________________________________________
void StSsdMONMaker::makeScmCtrlHistograms(St_scm_spt *scm_spt){
  //pointDir->cd();
  scm_spt_st *on_point = scm_spt->GetTable();
  Int_t a = 0;
  Int_t b = 0;
  int iLad =0;
  int iWaf =0;
  for(Int_t i = 0 ; i < scm_spt->GetNRows(); i++){
    a = convMeVToADC*((float)on_point[i].de[0] + (float)on_point[i].de[1]);
    b = convMeVToADC*((float)on_point[i].de[0] - (float)on_point[i].de[1]);
    iLad = idWaferToLadderNumb((int)on_point[i].id_wafer);
    iWaf = idWaferToWafer((int)on_point[i].id_wafer);
    //std::cout <<" idWafer/iLad/iWaf/a/b/de0/de1 : " << (int)on_point[i].id_wafer <<" " << iLad <<" " << iWaf << " " << a <<" " << b << " " << on_point[i].de[0] << " " <<on_point[i].de[1] << std::endl;
    LadderWaferHits->Fill(iLad+1,iWaf+1,1);
    globalMatchisto->Fill(a,b);
    globalDeviation->Fill(a-b);
    m_matchisto[iLad*16+iWaf]->Fill(a,b);
    globalPosition->Fill(on_point[i].x[0],on_point[i].x[1]);
    rphiZ->Fill(on_point[i].x[2],TMath::ATan2(on_point[i].x[1],on_point[i].x[0]));
    localPosition->Fill(on_point[i].xl[0],on_point[i].xl[2]);
  }
}   
//____________________________________________________________________________
void StSsdMONMaker::PrintInfo()
{
  if (Debug()) StMaker::PrintInfo();
}
//_____________________________________________________________________________
Int_t StSsdMONMaker::Finish() {
  LOG_INFO << "StSsdMONMaker::Finish()" << endm;
  writeHists();
  gMessMgr->Info() << "StSsdMONMaker::Finish() "
		   << "Processed " << mEventCounter << " events." << endm;
  return kStOK;
}
//_____________________________________________________________________________
void StSsdMONMaker::writeHists() {  
  
  StIOMaker *ioMaker = (StIOMaker*) GetMaker("inputStream");
  if (!ioMaker) {
    gMessMgr->Warning() << "StSsdMONMaker::writeHists(): No StIOMaker found" << endm;
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
  
  filename.Replace(slash_index, filename.Length() - slash_index, ".ssdMon.root");
  filename.Insert(0, "output/");
  LOG_INFO << "StSsdMONMaker::writeHists(): filename: " << filename << endm;
  
  myRootFile = new TFile(filename.Data(), "RECREATE");

  for(int i=0;i<20*16;++i){
    myRootFile->WriteTObject(m_strip_ADC_CHAN_sideP[i]);
    myRootFile->WriteTObject(m_strip_ADC_CHAN_sideN[i]);
    myRootFile->WriteTObject(m_cluster_ADC_sideP[i]);
    myRootFile->WriteTObject(m_cluster_ADC_sideN[i]);
    myRootFile->WriteTObject(m_cluster_Size_sideP[i]);
    myRootFile->WriteTObject(m_cluster_Size_sideN[i]);
    myRootFile->WriteTObject(m_matchisto[i]);
  }
  myRootFile->WriteTObject(LadderWaferHits);
  myRootFile->WriteTObject(globalMatchisto);
  myRootFile->WriteTObject(globalDeviation);
  myRootFile->WriteTObject(globalPosition);
  myRootFile->WriteTObject(rphiZ); 
  myRootFile->WriteTObject(localPosition);
  myRootFile->Close();
}
//_____________________________________________________________________________
Int_t StSsdMONMaker::idWaferToWaferNumb(Int_t idWafer)
{
  // idwafer = layer*1000+waf*100+ladder
  /*
    Int_t iW = (int)((idWafer - mSsdLayer*1000)/100);
    Int_t iL = idWafer - mSsdLayer*1000 - iW*100;
  */
  Int_t iW = (int)((idWafer - 7*1000)/100);
  Int_t iL = idWafer - 7*1000 - iW*100;
  return ((iL-1)*16 + iW -1);
}
//_____________________________________________________________________________
Int_t StSsdMONMaker::idWaferToLadderNumb(Int_t idWafer)
{
  // idwafer = layer*1000+waf*100+ladder
  /*
    Int_t iW = (int)((idWafer - mSsdLayer*1000)/100);
    Int_t iL = idWafer - mSsdLayer*1000 - iW*100;
  */
  Int_t iW = (int)((idWafer - 7*1000)/100);
  Int_t iL = idWafer - 7*1000 - iW*100;
  return iL-1;
}
//_____________________________________________________________________________
Int_t StSsdMONMaker::waferNumbToIdWafer(Int_t waferNumb)
{
  /*
    Int_t iL = 1+(int)((waferNumb)/mNWaferPerLadder);
    Int_t iW = waferNumb-((iL-1)*mNWaferPerLadder)+1;
  */
  Int_t iL = 1+(int)((waferNumb)/16);
  Int_t iW = waferNumb-((iL-1)*16)+1;
  return 7*1000 + iW*100 + iL;
}
