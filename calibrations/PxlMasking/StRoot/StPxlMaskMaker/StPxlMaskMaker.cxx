#include "StPxlMaskMaker.h"
#include <assert.h>
#include <iostream>
#include<fstream>
#include <sstream>
#include <vector>
#include "StMemStat.h"
#include "TTree.h"
#include "TStyle.h"
#include "TCanvas.h"
#include "TF1.h"
#include "Riostream.h"
#include "TFile.h"
#include "TSystem.h"
#include "TMath.h"
#include "TH1.h"
#include "TH2.h"
#include "TH3.h"
#include "TStyle.h"
#include "TF1.h"
#include "TProfile.h"
#include "TTree.h"
#include "TChain.h"
#include "TFile.h"
#include "TNtuple.h"
#include "TCanvas.h"
#include "TClassTable.h"
#include "TFileSet.h"
#include "TDataSetIter.h"
#include "TDataSet.h"
#include "TDataSetIter.h"
#include "TClassTable.h"


//StPxlMaskMaker::StPxlMaskMaker(){
//}
void StPxlMaskMaker::summarizeMasking(Int_t run_proc,Char_t *mytimestamp){
  Int_t *flagpointer=currRun.getFlags();
  char command[256];
  sprintf(command, "tail -n 400 pxlMon.%i.log | grep Jtag | cut -d : -f 3 > JtagFileVersion.txt", run_proc);
  gSystem->Exec(command);
  ifstream ifs("JtagFileVersion.txt");
  int jtagFileVersion = 0;
  ifs>>jtagFileVersion;
  cout<<"**************** Masking summary ****************"<<endl;
  cout<<"* \t run:\t\t"<<run_proc<<endl;
  sensor_summary<<"run "<<run_proc<<endl;
  cout<<"* \t timestamp:\t\t"<<mytimestamp<<"\t*"<<endl;
  sensor_summary<<"timestamp "<<mytimestamp<<endl;
  cout<<"* \t Jtag file version:\t\t"<<jtagFileVersion<<"\t*"<<endl;
  sensor_summary<<"Jtag_file_version "<<jtagFileVersion<<endl;
  cout<<"****************** Inner Layer ******************"<<endl;
  cout<<"* \t Perfect sensors:\t\t"<<perlayer[0][1]<<"\t*"<<endl;
  sensor_summary<<"Perfect_sensors_inner "<<perlayer[0][1]<<endl;
  cout<<"* \t Good but hot sensors:\t\t"<<perlayer[0][2]<<"\t*"<<endl;
  sensor_summary<<"Good_but_hot_sensors_inner "<<perlayer[0][2]<<endl;
  cout<<"* \t Non-uniform sensors:\t\t"<<perlayer[0][3]<<"\t*"<<endl;
  sensor_summary<<"Non_uniform_sensors_inner "<<perlayer[0][3]<<endl;
  cout<<"* \t Low efficiency sensors:\t"<<perlayer[0][4]<<"\t*"<<endl;
  sensor_summary<<"Low_efficiency_sensors_inner "<<perlayer[0][4]<<endl;
  cout<<"* \t Bad sensors:\t\t\t"<<perlayer[0][5]<<"\t*"<<endl;
  sensor_summary<<"Bad_sensors_inner "<<perlayer[0][5]<<endl;
  cout<<"* \t Dead sensors:\t\t\t"<<perlayer[0][0]<<"\t*"<<endl;
  sensor_summary<<"Dead_sensors_inner "<<perlayer[0][0]<<endl;
  cout<<"* \t Missing sensors:\t\t"<<perlayer[0][6]<<"\t*"<<endl;
  sensor_summary<<"Missing_sensors_inner "<<perlayer[0][6]<<endl;
  cout<<"****************** Outer Layer ******************"<<endl;
  cout<<"* \t Perfect sensors:\t\t"<<perlayer[1][1]<<"\t*"<<endl;
  sensor_summary<<"Perfect_sensors_outer "<<perlayer[1][1]<<endl;
  cout<<"* \t Good but hot sensors:\t\t"<<perlayer[1][2]<<"\t*"<<endl;
  sensor_summary<<"Good_but_hot_sensors_outer "<<perlayer[1][2]<<endl;
  cout<<"* \t Non-uniform sensors:\t\t"<<perlayer[1][3]<<"\t*"<<endl;
  sensor_summary<<"Non_uniform_sensors_outer "<<perlayer[1][3]<<endl;
  cout<<"* \t Low efficiency sensors:\t"<<perlayer[1][4]<<"\t*"<<endl;
  sensor_summary<<"Low_efficiency_sensors_outer "<<perlayer[1][4]<<endl;
  cout<<"* \t Bad sensors:\t\t\t"<<perlayer[1][5]<<"\t*"<<endl;
  sensor_summary<<"Bad_sensors_outer "<<perlayer[1][5]<<endl;
  cout<<"* \t Dead sensors:\t\t\t"<<perlayer[1][0]<<"\t*"<<endl;
  sensor_summary<<"Dead_sensors_outer "<<perlayer[1][0]<<endl;
  cout<<"* \t Missing sensors:\t\t"<<perlayer[1][6]<<"\t*"<<endl;
  sensor_summary<<"Missing_sensors_outer "<<perlayer[1][6]<<endl;
  sensor_summary<<"hot_columns_all "<<hot_columns_all<<endl;
  sensor_summary<<"hot_rows_all "<<hot_rows_all<<endl;
  cout<<"*************************************************"<<endl;
  return;
}
void StPxlMaskMaker::Init(){
  //Following used to test sensor status
  for(Int_t i=0;i<7;i++){
    perlayer[0][i]=0;
    perlayer[1][i]=0;
  }
  for (Int_t i=0;i<400;i++){
    mySensor[i].maskClear();		
  }
  //myoutMask=new TFile("Mask.root","RECREATE");
  Char_t *g=currRun.rStamp();
  cout<<"Output file Stamp: "<<g<<endl;
  TString FName=Form("ana_%s.root",currRun.rStamp());
  myoutfile=new TFile(FName,"RECREATE");
  //senTuple=new TNtuple("senTuple","runDay:runNo:sensorId:sector:ladder:sensor:status:entries:hot:missing","runDay:runNo:sensorId:sector:ladder:sensor:status:entries:hot:missing");
  senTuple=new TNtuple("senTuple","runDay:runNo:sensorId:sector:ladder:sensor:status:hotCol:hotRow:hotPix:entries","runDay:runNo:sensorId:sector:ladder:sensor:status:hotCol:hotRow:hotPix:entries");
  Char_t *temp=Form("Summary.%s.txt",currRun.rStamp());
  oSummary.open(temp);
  sensor_summary.open("sensor_summary.txt");
  return;
}
void StPxlMaskMaker::Finish(){
  //myoutTest->Close();
  myoutfile->cd();
  senTuple->Write();
  myoutfile->Close();
  return;
}
//The following have been added to create fresh/empty tables
void StPxlMaskMaker::beginFreshRCTable(){
  writeRowCol<<"TDataSet *CreateTable() {"<<endl;
  writeRowCol<<"\t"<<"// ===================================================================="<<endl;
  writeRowCol<<"\t"<<"// numbering is : (sector-1)*40 + (ladder-1)*10 + sensor"<<endl;
  writeRowCol<<"\t"<<"// ===================================================================="<<endl;
  writeRowCol<<"\t"<<"// ------  Test whether this table share library was loaded ------"<<endl;
  writeRowCol<<"\t"<<"if (!gROOT->GetClass(\"St_pxlRowColumnStatus\")) return 0;"<<endl;
  writeRowCol<<"\t\t"<<"pxlRowColumnStatus_st row;"<<endl;
  writeRowCol<<"\t\tSt_pxlRowColumnStatus *tableSet = new St_pxlRowColumnStatus(\"pxlRowColumnStatus\",1);"<<endl;
  return;
}
void StPxlMaskMaker::endFreshRCTable(){
  writeRowCol<<"\t"<<"// ----------------- end of code ---------------"<<endl;
  writeRowCol<<"tableSet->AddAt(&row,0);"<<endl;
  writeRowCol<<"\t\t"<<"return (TDataSet *)tableSet;"<<endl;
  writeRowCol<<"}"<<endl;
  writeRowCol.close();
  return;
}
void StPxlMaskMaker::beginFreshSensorTable(){
  outfile=Form("pxlSensorStatus.%s.C",currRun.rStamp());
  writeSensor.open(outfile);
  writeSensor<<"TDataSet *CreateTable() {"<<endl;
  writeSensor<<"\t"<<"// ==================================================================== "<<endl;
  writeSensor<<"\t"<<"// numbering is : (sector-1)*40 + (ladder-1)*10 + sensor "<<endl;
  writeSensor<<"\t"<<"// ==================================================================== "<<endl;
  writeSensor<<"\t"<<"// ------  Test whether this table share library was loaded ------"<<endl;
  writeSensor<<"\t"<<"if (!gROOT->GetClass(\"St_pxlSensorStatus\")) return 0;"<<endl<<"\t"<<"pxlSensorStatus_st row;"<<endl;
  writeSensor<<"\t"<<"St_pxlSensorStatus *tableSet = new St_pxlSensorStatus(\"pxlSensorStatus\",1);"<<endl;
  return;
}
void StPxlMaskMaker::endFreshSensorTable(){
  writeSensor<<"\t"<<"tableSet->AddAt(&row,0);"<<endl;
  writeSensor<<"\t"<<"// ----------------- end of code --------------- "<<endl;
  writeSensor<<"\t"<<"return (TDataSet *)tableSet;"<<endl<<"}"<<endl;
  writeSensor.close();
  return;
}
void StPxlMaskMaker::writeFreshTables(){
  beginFreshSensorTable();
  Int_t id;
  for(Int_t i=0; i<10; i++){
    for(Int_t j=0;j<4;j++){
      writeSensor<<"//Sector"<<i+1<<"//Ladder"<<j+1<<endl;
      for(Int_t k=0;k<10;k++){
	id=40*i+10*j+k;
	writeSensor<<"\t"<<"row.status["<<id<<"]=1;"<<endl;
      }
    }
  }
  endFreshSensorTable();
  FreshRCTable();
  return;
}
void StPxlMaskMaker::FreshRCTable(){
  //Lomnitz
  Char_t *outp=Form("pxlRowColumnStatus.%s.C",currRun.rStamp());
  //TFile *RCMaskTable=new TFile(outp,"RECREATE");
  writeRowCol.open(outp);
  cout<<"Filling row/column  mask table"<<endl;
  Int_t realSize;
  //memset(&temp,-1,sizeof(pxlRowColumnStatus_st)); 
  beginFreshRCTable(); 
  for(Int_t i=0;i<10;i++){
    for(int j=0;j<4;j++){
      for(Int_t k=0;k<10;k++){
	realSize=0;
	Int_t curr_ID=40*i+10*j+k;
	writeRowCol<<"// Sensor"<<curr_ID<<endl;
	for(Int_t ii=0; ii<=colMax;ii++){
	  writeRowCol<<"\t\trow.cols["<<curr_ID*(1+colMax)+ii<<"]=1;"<<endl;
	}
	for(Int_t ii=0; ii<=rowMax;ii++){
	  writeRowCol<<"\t\trow.rows["<<curr_ID*(1+rowMax)+ii<<"]=1;"<<endl;
	}
	//cout<<"\t Realsize:"<<realSize<<"\t Array size: "<<temp[curr_ID].nEntries<<endl;
      }
    }
  }
  endFreshRCTable();
  return;
}
void StPxlMaskMaker::MakeFreshTables(Char_t *s){
  currRun.SetRunName(0,s);
  cout<<"Writing tables"<<endl;
  writeFreshTables();
  cout<<"Done writing tables"<<endl;
  return;
}
//------- Opening and writing intial structure for pxlSensorStatus table --------
void StPxlMaskMaker::beginSensorTable(){
  outfile=Form("%s/pxlSensorStatus.%s.C",path,currRun.rStamp());
  writeSensor.open(outfile);
  writeSensor<<"TDataSet *CreateTable() {"<<endl;
  writeSensor<<"\t"<<"// ==================================================================== "<<endl;
  writeSensor<<"\t"<<"// numbering is : (sector-1)*40 + (ladder-1)*10 + sensor "<<endl;
  writeSensor<<"\t"<<"// ==================================================================== "<<endl;
  writeSensor<<"\t"<<"// ------  Test whether this table share library was loaded ------"<<endl;
  writeSensor<<"\t"<<"if (!gROOT->GetClass(\"St_pxlSensorStatus\")) return 0;"<<endl<<"\t"<<"pxlSensorStatus_st row;"<<endl;
  writeSensor<<"\t"<<"St_pxlSensorStatus *tableSet = new St_pxlSensorStatus(\"pxlSensorStatus\",1);"<<endl;
  //--------------------------------------------------------
  //Generating official DB files
  string tempString=currRun.rStamp();
  outfile=Form("%s/officialDB/writeSensorStatus_%s_%s.C",path,tempString.substr(0,8).c_str(),tempString.substr(9,6).c_str());
  oSensor.open(outfile);
  oSensor<<"void writeSensorStatus_"<<tempString.substr(0,8)<<"_"<<tempString.substr(9,6)<<"() {"<<endl;
  oSensor<<"\tgROOT->Macro(\"LoadLogger.C\");"<<endl;
  oSensor<<"\tgSystem->Load(\"St_base.so\");"<<endl;
  oSensor<<"\tgSystem->Load(\"libStDb_Tables.so\");"<<endl;
  oSensor<<"\tgSystem->Load(\"StDbLib.so\");"<<endl;
  oSensor<<"// Initialize db manager"<<endl;
  oSensor<<"\tStDbManager* mgr = StDbManager::Instance();"<<endl;
  oSensor<<"\tStDbConfigNode* node = mgr->initConfig(\"Calibrations_pxl\");"<<endl;
  oSensor<<"\tStDbTable* dbtable = node->addDbTable(\"pxlSensorStatus\");"<<endl;
  oSensor<<"\tTString storeTime = \""<<tempString.substr(0,4)<<"-"<<tempString.substr(4,2)<<"-"<<tempString.substr(6,2)<<" "<<tempString.substr(9,2)<<":"<<tempString.substr(11,2)<<":"<<tempString.substr(13,2)<<"\"; // beginTime timestamp in MySQL format: YYYY-MM-DD HH:mm:ss"<<endl;
  oSensor<<"\tmgr->setStoreTime(storeTime.Data());"<<endl;
  oSensor<<"//Create your c-struct"<<endl;
  oSensor<<"\tpxlSensorStatus_st row;"<<endl;
  return;
}
// ------ Closing and writing end structure for pxlSensorStatus table --------
void StPxlMaskMaker::endSensorTable(){
  writeSensor<<"\t"<<"tableSet->AddAt(&row,0);"<<endl;
  writeSensor<<"\t"<<"// ----------------- end of code --------------- "<<endl;
  writeSensor<<"\t"<<"return (TDataSet *)tableSet;"<<endl<<"}"<<endl;
  writeSensor.close();
  //Generating official DB tables
  oSensor<<"// Store data to the StDbTable"<<endl;
  oSensor<<"\tdbtable->SetTable((char*)&row, 1);"<<endl;
  oSensor<<"// uncomment next line to set \"sim\" flavor. \"ofl\" flavor is set by default, no need to set it."<<endl;
  oSensor<<"\t//dbtable->setFlavor(\"sim\");"<<endl;
  oSensor<<"// Store table to database"<<endl;
  oSensor<<"\tmgr->storeDbTable(dbtable);"<<endl;
  oSensor<<"}"<<endl;
  oSensor.close();
  return;
}
//------- Opening and writing intial structure for pxlSensorRowCol table --------
void StPxlMaskMaker::beginRCTable(){
  Char_t *outp=Form("%s/pxlBadRowColumns.%s.C",path,currRun.rStamp());
  //TFile *RCMaskTable=new TFile(outp,"RECREATE");
  writeRowCol.open(outp);
  writeRowCol<<"TDataSet *CreateTable() {"<<endl;
  writeRowCol<<"\t"<<"// ===================================================================="<<endl;
  writeRowCol<<"\t"<<"// numbering is : 100*(1000000*isCol+1000*(40*(sec-1)+10*(lad-1)+sen)+rowOrCol)+stat"<<endl;
  writeRowCol<<"\t"<<"// ===================================================================="<<endl;
  writeRowCol<<"\t"<<"// ------  Test whether this table share library was loaded ------"<<endl;
  writeRowCol<<"\t"<<"if (!gROOT->GetClass(\"St_pxlBadRowColumns\")) return 0;"<<endl;
  writeRowCol<<"\t\t"<<"pxlBadRowColumns_st row;"<<endl;
  writeRowCol<<"\t\tmemset(row.badRowColumns, 0, 10000*sizeof(unsigned int));"<<endl;
  writeRowCol<<"\t\tSt_pxlBadRowColumns *tableSet = new St_pxlBadRowColumns(\"pxlBadRowColumns\",1);"<<endl;
  //-----------------------------------------------------------------------------
  //GEnerating official DB tables
  string tempString=currRun.rStamp();
  outp=Form("%s/officialDB/writeBadRowColumns_%s_%s.C",path,tempString.substr(0,8).c_str(),tempString.substr(9,6).c_str());
  oRowCol.open(outp);
  oRowCol<<"void writeBadRowColumns_"<<tempString.substr(0,8)<<"_"<<tempString.substr(9,6)<<"() {"<<endl;
  oRowCol<<"// Load all required libraries"<<endl;
  oRowCol<<"\tgROOT->Macro(\"LoadLogger.C\");"<<endl;
  oRowCol<<"\tgSystem->Load(\"St_base.so\");"<<endl;
  oRowCol<<"\tgSystem->Load(\"libStDb_Tables.so\");"<<endl;
  oRowCol<<"\tgSystem->Load(\"StDbLib.so\");"<<endl;
  oRowCol<<"// Initialize db manager"<<endl;
  oRowCol<<"\tStDbManager* mgr = StDbManager::Instance();"<<endl;
  oRowCol<<"\tStDbConfigNode* node = mgr->initConfig(\"Calibrations_pxl\");"<<endl;
  oRowCol<<"\tStDbTable* dbtable = node->addDbTable(\"pxlBadRowColumns\");"<<endl;
  oRowCol<<"\tTString storeTime = \""<<tempString.substr(0,4)<<"-"<<tempString.substr(4,2)<<"-"<<tempString.substr(6,2)<<" "<<tempString.substr(9,2)<<":"<<tempString.substr(11,2)<<":"<<tempString.substr(13,2)<<"\"; // beginTime timestamp in MySQL format: YYYY-MM-DD HH:mm:ss"<<endl;
  oRowCol<<"\tmgr->setStoreTime(storeTime.Data());"<<endl;
  oRowCol<<"// Create your c-struct"<<endl;
  oRowCol<<"\tpxlBadRowColumns_st row;"<<endl;
  oRowCol<<"\tmemset(row.badRowColumns, 0, sizeof(unsigned int)*10000);"<<endl;
  
  return;
}
// ------ Closing and writing end structure for pxlSensorRowCol table --------
void StPxlMaskMaker::endRCTable(){
  writeRowCol<<"\t"<<"// ----------------- end of code ---------------"<<endl;
  writeRowCol<<"tableSet->AddAt(&row,0);"<<endl;
  writeRowCol<<"\t\t"<<"return (TDataSet *)tableSet;"<<endl;
  writeRowCol<<"}"<<endl;
  writeRowCol.close();
  //--------------------------------------------------------------------------
  //Generating official DB tables
  oRowCol<<"\tdbtable->SetTable((char*)&row, 1);"<<endl;
  oRowCol<<"// uncomment next line to set \"sim\" flavor. \"ofl\" flavor is set by default, no need to set it."<<endl;
  oRowCol<<"//\tdbtable->setFlavor(\"ofl\");"<<endl;    
  oRowCol<<"// Store table to database"<<endl;
  oRowCol<<"\tmgr->storeDbTable(dbtable);"<<endl<<"}"<<endl;
  //oRowCol<<"\treturn;"<<endl<<"}"<<endl;
  oRowCol.close();
  return;
}
void StPxlMaskMaker::RCTable(){
  vector<int> order;
  order.push_back(0);
  for(int ii=1; ii<400; ii++){
    for(int jj=0; jj<order.size();jj++){
      if(jj==order.size()-1){
	order.push_back(ii);
	break;
      }
      if((mySensor[ii].noHotCol()+mySensor[ii].noHotRow())> mySensor[order.at(jj)].noHotCol()+mySensor[order.at(jj)].noHotRow()){
	std::vector<int>::iterator it;
	it=order.begin();
	order.insert(it+jj,ii);
	break;
      }
    }
  }
  //Lomnitz
  cout<<"Filling row/column  mask table"<<endl;
  int realSize;
  // memset(&temp,-1,sizeof(pxlRowColumnStatus_st)); 
  beginRCTable();
  hot_columns_all = 0;
  hot_rows_all = 0;
  uint jj=0;
  for(int i=0;i<400;i++){
    realSize=0;
    Int_t curr_ID=order.at(i);
    //Int_t curr_ID=40*i+10*j+k;
    writeRowCol<<"// Sensor"<<curr_ID<<endl;
    oRowCol<<"// Sensor"<<curr_ID<<endl;
    if(mySensor[curr_ID].Status()==255 || mySensor[curr_ID].Status()>10 )
      continue;
    vector<int> listBadCol=mySensor[curr_ID].getColList();
    vector<int> listBadRow=mySensor[curr_ID].getRowList();
    //cout<<"Sensor "<<curr_ID+1<<" listBadCol.size()= "<<listBadCol.size()<<endl;
    //cout<<"Sensor "<<curr_ID+1<<" listBadRow.size()= "<<listBadRow.size()<<endl;
    int temp=jj;
    for(uint ii=0; ii<listBadCol.size();ii++){
      if(listBadCol.at(ii)!=1){
	if(jj>9999){
	  break;
	}
	writeRowCol<<"\t\trow.badRowColumns["<<jj<<"]="<<100*(1000000+1000*(curr_ID+1)+ii)+listBadCol.at(ii)<<";"<<endl;
	oRowCol<<"\trow.badRowColumns["<<jj<<"]="<<100*(1000000+1000*(curr_ID+1)+ii)+listBadCol.at(ii)<<";"<<endl;
	jj+=1;
      }
    }
    cout<<"Done with columns, total in list: "<<jj<<endl;
    for(uint ii=0; ii<listBadRow.size();ii++){
      if(listBadRow.at(ii)!=1){
	if(jj>9999)
	  break;
	writeRowCol<<"\t\trow.badRowColumns["<<jj<<"]="<<100*(1000*(curr_ID+1)+ii)+listBadRow.at(ii)<<";"<<endl;
	oRowCol<<"\trow.badRowColumns["<<jj<<"]="<<100*(1000*(curr_ID+1)+ii)+listBadRow.at(ii)<<";"<<endl;
	jj+=1;
      }
    }
    temp=jj-temp;
    oRowCol<<"//Sensor:"<<curr_ID<<" with "<< temp<<" entries"<<endl;
    //cout<<"\t Realsize:"<<realSize<<"\t Array size: "<<temp[curr_ID].nEntries<<endl;
  }
  if(jj>9999)
    warning<<"RC table over limit day/run: "<<currRun.Day()<<" / "<<currRun.Run()<<endl;
  /*
  beginRCTable();
  for(int ii=0;ii<400000;ii++){
    writeRowCol<<"\trow.cols["<<ii<<"]="<<(int) temp.cols[ii]<<";"<<endl;
  }
  for(int ii=0;ii<400000;ii++){
    writeRowCol<<"\trow.rows["<<ii<<"]="<<(int) temp.cols[ii]<<";"<<endl;
    }*/
  endRCTable();
  /*RCMaskTable->cd();
  St_pxlRowColumnStatus *RCMask = new St_pxlRowColumnStatus("pxlRowColumnStatus");
  for(Int_t i=0;i<400;i++) {
   RCMask->AddAt(&temp);
  }
  RCMask->Write();
  RCMaskTable->Close();
  SafeDelete(RCMask);*/
  return;
}
//------- Opening and writing intial structure for indvpxl table --------
void StPxlMaskMaker::beginPxlTable(){
  Char_t *outp=Form("%s/pxlHotPixels.%s.C",path,currRun.rStamp());
  writePxl.open(outp);
  writePxl<<"TDataSet *CreateTable() {"<<endl;
  writePxl<<"\t"<<"// ==================================================================== "<<endl;
  writePxl<<"\t"<<"// numbering is : sensorId*1000000 + row*1000 + column "<<endl;
  writePxl<<"\t"<<"// ==================================================================== "<<endl;
  writePxl<<"\t"<<"// ------  Test whether this table share library was loaded ------"<<endl;
  writePxl<<"\t"<<"if (!gROOT->GetClass(\"St_pxlHotPixels\")) return 0;"<<endl<<"\t"<<"pxlHotPixels_st row;"<<endl;
  writePxl<<"\tmemset(row.hotPixel, 0, 10000*sizeof(unsigned int));"<<endl;
  writePxl<<"\t"<<"St_pxlHotPixels *tableSet = new St_pxlHotPixels(\"pxlHotPixels\",1);"<<endl;
  //-----------------------------------------------------------------------------
  //Generating official DB tables
  string tempString=currRun.rStamp();
  outp=Form("%s/officialDB/writeHotPixels_%s_%s.C",path,tempString.substr(0,8).c_str(),tempString.substr(9,6).c_str());
  oPxl.open(outp);
  oPxl<<"void writeHotPixels_"<<tempString.substr(0,8)<<"_"<<tempString.substr(9,6)<<"()"<<endl;
  oPxl<<"{"<<endl;
  oPxl<<"// Load all required libraries"<<endl;
  oPxl<<"\tgROOT->Macro(\"LoadLogger.C\");"<<endl;
  oPxl<<"\tgSystem->Load(\"St_base.so\");"<<endl;
  oPxl<<"\tgSystem->Load(\"libStDb_Tables.so\");"<<endl;
  oPxl<<"\tgSystem->Load(\"StDbLib.so\");"<<endl;
  oPxl<<"// Initialize db manager"<<endl;
  oPxl<<"\tStDbManager* mgr = StDbManager::Instance();"<<endl;
  oPxl<<"\tStDbConfigNode* node = mgr->initConfig(\"Calibrations_pxl\");"<<endl;
  oPxl<<"\tStDbTable* dbtable = node->addDbTable(\"pxlHotPixels\");"<<endl;
  //Lomnitz Fix
  oPxl<<"\tTString storeTime = \""<<tempString.substr(0,4)<<"-"<<tempString.substr(4,2)<<"-"<<tempString.substr(6,2)<<" "<<tempString.substr(9,2)<<":"<<tempString.substr(11,2)<<":"<<tempString.substr(13,2)<<"\"; // beginTime timestamp in MySQL format: YYYY-MM-DD HH:mm:ss"<<endl;
  oPxl<<"\tmgr->setStoreTime(storeTime.Data());"<<endl;
  oPxl<<"// Create your c-struct"<<endl;
  oPxl<<"\tpxlHotPixels_st row;"<<endl;
  oPxl<<"\tmemset(row.hotPixel, 0, sizeof(unsigned int)*10000);"<<endl;
  return;
}
// ------ Closing and writing end structure for pxlSensorPixelMask table --------
void StPxlMaskMaker::endPxlTable(){
  writePxl<<"\t"<<"// ----------------- end of code ---------------"<<endl;
  writePxl<<"tableSet->AddAt(&row,0);"<<endl;
  writePxl<<"\t\t"<<"return (TDataSet *)tableSet;"<<endl;
  writePxl<<"}"<<endl;
  writePxl.close();
  oPxl<<"// Store data to the StDbTable"<<endl;
  oPxl<<"\tdbtable->SetTable((char*)&row, 1);"<<endl;
  oPxl<<"// uncomment next line to set \"sim\" flavor. \"ofl\" flavor is set by default, no need to set it."<<endl;
  oPxl<<"\t//dbtable->setFlavor(\"sim\");"<<endl;
  oPxl<<"\tmgr->storeDbTable(dbtable);"<<endl<<"}"<<endl;
  //oPxl<<"\treturn;"<<endl<<"}"<<endl;
  oPxl.close();
  return;
}
void StPxlMaskMaker::PxlTable(){
  //Lomnitz
  vector<int> order;
  order.push_back(0);
  for(int ii=1; ii<400; ii++){
    for(int jj=0; jj<order.size();jj++){
      if(jj==order.size()-1){
	order.push_back(ii);
	break;
      }
      if(mySensor[ii].getPixList().size()> mySensor[order.at(jj)].getPixList().size()){
	std::vector<int>::iterator it;
	it=order.begin();
	order.insert(it+jj,ii);
	break;
      }
    }
  }
  cout<<"Filling individual pxl mask table"<<endl;
  int realSize=0;
  //memset(&temp,-1,400*sizeof(pxlSensorPixelMask_st)); 
  beginPxlTable();
  for(int i=0;i<400;i++){
    Int_t curr_ID=order.at(i);
    vector<int> badPixlist=mySensor[curr_ID].getPixList();
    writePxl<<"//Sensor"<<curr_ID+1<<"/400"<<endl;
    oPxl<<"//Sensor"<<curr_ID+1<<"/400"<<endl;
    if((int)badPixlist.size()>0){
      for(int ii=0; ii<(int)badPixlist.size();ii++){
	if(realSize>9999){
	  break;
	}
	writePxl<<"\t\trow.hotPixel["<<realSize<<"]="<<1000000*(curr_ID+1)+badPixlist.at(ii)<<";"<<endl;
	oPxl<<"\t\trow.hotPixel["<<realSize<<"]="<<1000000*(curr_ID+1)+badPixlist.at(ii)<<";"<<endl;
	++realSize;
      }
    }
    if(realSize>9999)
      warning<<"Pix table is over limit day/run: "<<currRun.Day()<<" / "<<currRun.Run()<<endl;
    //cout<<"\t Realsize:"<<realSize<<"\t Array size: "<<temp[curr_ID].nEntries<<endl;
  }
  /*for(Int_t ii=realSize; ii<10000; ii++){
    writePxl<<"\t\trow.hotPixel["<<ii<<"]=0;"<<endl;
    oPxl<<"\t\trow.hotPixel["<<ii<<"]=0;"<<endl;
    }*/
  endPxlTable();
  return;
}
//Still here Lomnitz
void StPxlMaskMaker::writeTables(){
  cout<<"Writing sensor table"<<endl;
  beginSensorTable();
  //cout<<"Lomnitz: Mid"<<endl;
  outfile=Form("%s/warning.txt",path);
  warning.open(outfile,std::fstream::app);
  Int_t id;
  for(Int_t i=0; i<10; i++){
    for(Int_t j=0;j<4;j++){
      writeSensor<<"//Sector"<<i+1<<"//Ladder"<<j+1<<endl;
      oSensor<<"//Sector"<<i+1<<"//Ladder"<<j+1<<endl;
      for(Int_t k=0;k<10;k++){
	id=40*i+10*j+k;
	writeSensor<<"\t"<<"row.status["<<id<<"]="<<mySensor[id].Status()<<";"<<endl;
	oSensor<<"\t"<<"row.status["<<id<<"]="<<mySensor[id].Status()<<";"<<endl;
      }
    }
  }
  //cout<<"Lomnitz: end"<<endl;
  endSensorTable();
  cout<<"Writing Row/Col table"<<endl;
  RCTable();
  cout<<"Writing pixel table"<<endl;
  PxlTable();
  warning.close();
  return;
}
void StPxlMaskMaker::maskSensor(Int_t id){
  Int_t badCol=0,badRow=0,badPix=0,flagPix[maxPix]={0},indBadPix=0;
  Double_t sAverage,sAverageR;
  mySensor[id].FlagClear();
  Int_t sector, ladder, sensor;
  sector=id/40+1;
  ladder=(id%40)/10+1;
  if(ladder==1){
    sAverage=inner;
    sAverageR=innerR;
    //hPix=hinner;
    //sAverage=1000;
  }
  else{
    sAverage=outer;
    sAverageR=outerR;
    //hPix=houter;
    //sAverage=1000;
  }
  //For cosmic runs
  if(opt=="cosmic"){
    sAverage=cosmicAverage;
    sAverageR=cosmicNoRow;
  }
  sensor=(id%10)+1;
  str=Form("rawHit_rowvscolumn_sec%02i_ldr%i_snr%02i",sector,ladder,sensor);
  TH2F *Name=(TH2F*)myfile->Get(str);
  TH2 *TEST=Name;
  //Dead sensor
  mySensor[id].SetId(id);
  //2014 Used for individual pixel masking
  mySensor[id].clearPixList();
  //------------------------------------
  if(!Name || Name->GetEntries()==0){
    //Used to status analysis
    mySensor[id].setStatus(0);
    currRun.addFlag(0);
    return;
  }
  //Added here for cosmics, might consider changing
  if(opt=="cosmic" && Name->GetEntries()/Events>1000){
    mySensor[id].setMaskedOut(500);
    mySensor[id].setStatus(12);
    currRun.addFlag(5);
    return;
  }
  Double_t pixcut;
  if(opt=="cosmic")
    pixcut=cosmichPix;
  else
    pixcut=hPix;
  for(Int_t binX=1;binX<=colMax+1;binX++){
    badPix=0;
    for(Int_t binY=1; binY<=rowMax+1; binY++){
      if(Name->GetBinContent(binX,binY)/Events>pixcut){
	if(badPix<=maxPix){
	  flagPix[badPix]=binY;
	}
	badPix++;
      }
    }
    //IF more than cut, erase full col.
    if(badPix>=maxPix){
      mySensor[id].setColStatus(2);
      badCol++;
      for(Int_t count=0; count<=rowMax+1;count++){
	//Have to manually adjust number of entries, otherwise root just adds an entry for every bin deleted
	TEST->SetEntries(TEST->GetEntries()-Name->GetBinContent(binX,count)-1);
	TEST->SetBinContent(binX,count,0);
      }
    }
    //Otherwise just mask out bad pxl's
    else{
      indBadPix+=badPix;
      mySensor[id].setColStatus(1);
      currRun.badPixCount(badPix);
      //cout<<"Lomnitz"<<badPix<<" "<<1000*binX+badPix<<endl;
      currRun.setBadPixinCol(id,1000*(binX-1)+badPix);
	//---------
      for(Int_t count=0; count<badPix;count++){
	//2014------------
	mySensor[id].addBadPix(1000*(flagPix[count]-1)+binX-1);
	//----------------
	TEST->SetEntries(TEST->GetEntries()-Name->GetBinContent(binX,flagPix[count]));
	TEST->SetBinContent(binX,flagPix[count],0);
	//Need to save these out later note than the coords in flagPix are holding histo coords and not actual axis value, need to subtract when writting out
      }
    }
  }
  TH1D *px=TEST->ProjectionX();
  for(Int_t binX=1;binX<=colMax+1;binX++){
    if((Double_t)px->GetBinContent(binX)>(Double_t)(px->GetEntries()/hCol) || (Double_t)(px->GetBinContent(binX)/Events)>sAverage){
      mySensor[id].setColList(binX-1,2);
      badCol++;
      for(Int_t count=0; count<=rowMax+1;count++){
	currRun.badPixCount(-badPix);
	currRun.clearBadPixIn(id);
	mySensor[id].clearPixList();
	//Have to manually adjust number of entries, otherwise root just adds an entry for every bin deleted
	TEST->SetEntries(TEST->GetEntries()-Name->GetBinContent(binX,count)-1);
	TEST->SetBinContent(binX,count,0);
      }
    }
  }
  TH1D *py=TEST->ProjectionY();
  for(Int_t binX=1;binX<=rowMax+1;binX++){
    if ((py->GetBinContent(binX))>(py->GetEntries())/hRow || (py->GetBinContent(binX)/Events)>sAverageR) {
      mySensor[id].setRowStatus(2);
      badRow++;
      /*if(badRow<200){
        mySensor[id].listBadRow.push_back(binX-1);
	//flagR[badRow]=binX-1;
	++badRow;
	}*/
      for(Int_t count=0; count<=colMax+1; count++){
	//Have to manually adjust number of entries, otherwise root just adds an entry for every bin deleted!    
	TEST->SetEntries(TEST->GetEntries()-Name->GetBinContent(count,binX)-1);
	TEST->SetBinContent(count,binX,0);
      }
    }
    else{
      mySensor[id].setRowStatus(1);
    }
  }
  //----------------------------Identifying missing columns
  px->Clear();
  px=TEST->ProjectionX();
  Double_t colAvg=(px->GetEntries())/(colMax);
  Int_t missing=0;
  //No missing columns in cosmics 
  if(opt=="cosmic")
    colAvg==-100;
  //----------------------------Loop will set the value of eliminated columns to -1 so they ares skipped
  for(Int_t i=0;i<colMax;i++){
    //Possible prob
    if(mySensor[id].getColList().at(i)==2){
      px->SetBinContent(i+1,-1.0);
    }
    //  cout<<"\t"<<flagC[i]+1<<endl;
  }
  for(Int_t binX=0; binX<colMax; binX++){
    if(px->GetBinContent(binX+1)!=-1){
      if((1.0)*px->GetBinContent(binX+1)<=(1.0)*colAvg/mCol){
	int mBlock[2]={binX,0};
	//cout<<"Found first m col"<<endl;
	while(px->GetBinContent(binX+mBlock[1]+1)<=colAvg/mCol && (binX+mBlock[1])<colMax){
	  mBlock[1]+=1;
	  //cout<<"Coords: "<<mBlock[0]<<" + "<<mBlock[1]<<" Has: "<<" limit "<<colAvg/mCol<<endl;
	}
	binX+=mBlock[1];
	//cout<<"Start :"<<mBlock[0]<<" length: "<<mBlock[1]<<endl;
	if(mBlock[1]>=block){
	  missing+=mBlock[1];
	  for(Int_t jj=0; jj<mBlock[1]; jj++)
	    mySensor[id].setColList(mBlock[0]+jj,0);
	}
      }
    }
  }
  mySensor[id].setMaskInfo(badCol,badRow,indBadPix,TEST->GetEntries(),missing);
  mySensor[id].setMaskedOut(missing+badCol+badRow+badPix/1000.0);
  oSummary<<"Sensor :"<<id+1<<"\t "<<badCol<<" | "<<badRow<<" | "<<indBadPix<<" | "<<missing;
  oSummary<<"\t Total masked out: "<<mySensor[id].getMaskedOut()<<"   :: colAvg/mCol"<<colAvg/mCol<<"\t\t ____"<<endl;  
  //----------------------------Following if's will help identify the possible sensor flags
  //Bad Sensor
  //Lomnitz issue here: Difference between cosmic and physcs running. For physics we are using 3/4 ofr sensor, for comsic half
  Double_t badTemp;
  if(opt=="cosmic")
    badTemp=500.0;
  else 
    badTemp=3*(colMax+1)/4;
  if(mySensor[id].getMaskedOut()/*+mySensor[id].missing*/>=badTemp || TEST->GetEntries()/Events>1000.0){
    mySensor[id].setStatus(12);
    currRun.addFlag(5);
    return;
  }
  //mySensor[id].setMaskInfo(badCol,badRow,indBadPix,TEST->GetEntries(),missing);
  currRun.setEntries(TEST->GetEntries());
  return;
}
// ----- Method flags bad col/row for given sensor ------
void StPxlMaskMaker::sensorStatus(){
  Double_t avg=0;
  for(int ii=0; ii<400; ii++)
      avg+=mySensor[ii].Entries()/400.0;
  for(Int_t id=0; id<400;id++){
    //cout<<"Sensor: "<<id<<"\t Col size"<< mySensor[id].listBadCol.size()<<endl;
    //Missing Sector
    cout<<"Sensor: "<< id <<" entries"<<mySensor[id].Entries()<<" avg "<<avg<<endl;
    Int_t sector, ladder,sensor;
    sector=(id)/40+1;
    ladder=(id)%40/10+1;
    sensor=(id)%10+1;
    Int_t layer;
    if(ladder==1) layer=1;
    else layer=2;
    if(mySensor[id].Status()==255){
      senTuple->Fill(currRun.Day(),currRun.Run(),id,sector,ladder,sensor,mySensor[id].Status(),0,0,0,0);
      mySensor[id].clearPixList();
      perlayer[layer-1][6]+=1;
      continue;
    }
    //Bad sensor
    Double_t hotCol=mySensor[id].noHotCol();
    Double_t hotRow=mySensor[id].noHotRow();
    Double_t hotPix=mySensor[id].noHotPix();
    if(mySensor[id].Status()==12 || (mySensor[id].Entries()>10.0*avg && opt=="cosmic")){
      mySensor[id].setStatus(12);
      senTuple->Fill(currRun.Day(),currRun.Run(),id,sector,ladder,sensor,mySensor[id].Status(),0,0,0,0);
      // here
      //cout<<"Lomnitz: Going to remove stuff from :"<<id<<endl;
      //cout<<"Bad Pix size: "<<-mySensor[id].getPixList().size()<<endl;
      currRun.badPixCount(-mySensor[id].getPixList().size());
      currRun.clearBadPixIn(id);
      mySensor[id].clearPixList();
      perlayer[layer-1][5]+=1;
      continue;
    }
    if(mySensor[id].Status()==0){ 
      senTuple->Fill(currRun.Day(),currRun.Run(),id,sector,ladder,sensor,mySensor[id].Status(),0,0,0,0);
      mySensor[id].clearPixList();
      perlayer[layer-1][0]+=1;
      continue;
    }
    //Low efficiency sensor
    Int_t *Flags=currRun.getFlags();
    if(10*mySensor[id].Entries()<currRun.Entries()/(400-Flags[0]-Flags[5]-Flags[6])){
      mySensor[id].setStatus(11);
      currRun.addFlag(4);
      senTuple->Fill(currRun.Day(),currRun.Run(),id,sector,ladder,sensor,mySensor[id].Status(),0,0,0,0);
      currRun.badPixCount(-mySensor[id].getPixList().size());
      currRun.clearBadPixIn(id);
      mySensor[id].clearPixList();
      perlayer[layer-1][4]+=1;
      continue;
    }
    //Good (Perfect) sensor
    if(mySensor[id].getMaskedOut()/*+ mySensor[id].missing*/<=5.0){
      //Used to status analysis
      mySensor[id].setStatus(1);
      senTuple->Fill(currRun.Day(),currRun.Run(),id,sector,ladder,sensor,mySensor[id].Status(),hotCol,hotRow,hotPix,mySensor[id].Entries()/Events);
      currRun.addFlag(1);
      perlayer[layer-1][1]+=1;
      continue;
    }
    //Good but hot Sensor
    if(mySensor[id].getMaskedOut()/*+ mySensor[id].missing*/<=50.0/* && mySensor.missing==0*/){
      //Used to status analysis
      mySensor[id].setStatus(2);
      senTuple->Fill(currRun.Day(),currRun.Run(),id,sector,ladder,sensor,mySensor[id].Status(),hotCol,hotRow,hotPix,mySensor[id].Entries()/Events);
      currRun.addFlag(2);
      perlayer[layer-1][2]+=1;
      continue;
    }
    //Non-uniform sensor
    if(mySensor[id].getMaskedOut()/*+mySensor[id].missing*/<3*(colMax+1)/4){
      //Used to status analysis
      mySensor[id].setStatus(3);
      senTuple->Fill(currRun.Day(),currRun.Run(),id,sector,ladder,sensor,mySensor[id].Status(),hotCol,hotRow,hotPix,mySensor[id].Entries()/Events);
      currRun.addFlag(3);
      perlayer[layer-1][3]+=1;
      continue;
    }
    cout<<"Masking debugg: id/status/hotCol/hotRow/hotPix: "<<id<<"/"<<mySensor[id].Status()<<"/"<<hotCol<<"/"<<hotRow<<"/"<<hotPix<<endl;
  }
  //Lomnitz
  return;
}
// ------ Method manages required outputs to tables for absent sector -------
void StPxlMaskMaker::noSector(Int_t curr_sector){
  for(Int_t j=0; j<4;j++){
    Int_t id;
    for(Int_t k=0;k<10;k++){
      id=40*curr_sector+10*j+k;
      mySensor[id].SetId(id);
      mySensor[id].setStatus(255);
      currRun.addFlag(6);
      //Used to status analysis
      //senTuple->Fill(currRun.Day(),currRun.Run(),mySensor.id,mySensor.sector+1,mySensor.ladder+1,mySensor.sensor+1,255);
    }
  }
  return;
}
// ----- Method manages flagging for sensors in sector -----
void StPxlMaskMaker::flagSector(Int_t curr_sector){
  for(Int_t j=0; j<4; j++){
    Int_t id;
    oSummary<<"// Ladder "<<j+1<<endl;
    oSummary<<"\t HotCol | HotRow | HotPix | Missing Col "<<endl;
      for(Int_t k=0; k<10;k++){
	//cout<<"\tcurrent mem used:"<<mem.Used()<<endl;
	id=40*curr_sector+10*j+k;
	mySensor[id].SetId(id);
	//Michael Lomnitz Uber TEst
	//if(id!=11) continue;
	maskSensor(id);
      }
    } 
  return;
}
// ------ Method manages loops over sector no -------
void StPxlMaskMaker::runMask(){
  str=Form("nRawHits_EventId_sec%02i_ldr1_snr01",1);
  TH2F *Name=(TH2F *)myfile->Get(str);
  Events=Name->GetEntries();
  getSaverage();
  for(Int_t i=0; i<10;i++){
    oSummary<<"// Sector "<<i+1<<endl;
    if(!Name){
      noSector(i);
      continue;
    }
    flagSector(i);
  }
  cout<<"About to start sensor Status"<<endl;
  sensorStatus();
  return;
}
void StPxlMaskMaker::getSaverage(){
  int ind=0;
  int outd=0;
  inner=0;
  outer=0;
  innerR=0;
  outerR=0;
  for(Int_t id=0; id<400; id++){
    Int_t sector, ladder, sensor;
    sector=id/40+1;
    ladder=(id%40)/10+1;
    sensor=(id%10)+1;
    str=Form("rawHit_rowvscolumn_sec%02i_ldr%i_snr%02i",sector,ladder,sensor);
    TH2F *Name=(TH2F*)myfile->Get(str);
    TH2 *TEST=Name;
    if(Name->GetEntries()==0) continue;
    if(ladder==1){
      inner+=10.0*Name->GetEntries()/(Events*960);
      innerR+=10.0*Name->GetEntries()/(Events*928);
      ind+=1;
    }
    else{
      outer+=10.0*Name->GetEntries()/(Events*960);
      outerR+=10.0*Name->GetEntries()/(Events*928);
      outd+=1;
    }
  }  
  inner=inner/ind;
  outer=outer/outd;
  innerR=innerR/ind;
  outerR=outerR/outd;
  cout<<"Calculated sAverage inner: "<<inner<<" outer: "<<outer<<endl;
  // cout<<"Calculated hPix inner: "<<hinner<<" outer: "<<houter<<endl;
  return;
}
void StPxlMaskMaker::removeEntries(Int_t remove){
  Int_t max=0;
  Int_t sensorR, colR;
  cout<<"Entries to remove "<<remove<<endl;
  vector <int> order;
  for(Int_t id=0; id<400; id++){
    for(Int_t col=0; col<(int)currRun.colPix(id).size();col++){
      if(currRun.colPix(id).at(col)%1000>0){
	int pos;
	Int_t temp=1000000*id+currRun.colPix(id).at(col);
	for(int ll=0; ll<order.size();ll++){
	  if (temp%1000>=order.at(ll)%1000) {
	    pos=ll;
	    break;
	  }
	  pos=ll;
	}
	if(order.size()==0 || pos==order.size()-1)
	  order.push_back(temp);
	else
	  order.insert(order.begin()+pos,temp);
      }
    }
  }
  //cout<<"Done ordering size "<<order.size()<<endl;
  int removed=0;
  for(int t=0; t<order.size();t++){
    cout<<"Current removed "<< removed <<" from pix list:"<<endl;
    if(removed>=remove) break;
    int temp=order.at(t);
    cout<<"Removing: "<<temp<<"Wich translates to id/col/no "<<temp/1000000<<" / "<<(temp%1000000)/1000<<" / "<<temp%1000<<endl;
    removed+=temp%1000;
    int id=temp/1000000;
    int col=temp%1000000/1000;
    cout<<"Column id Before"<<mySensor[id].getColList().at(col);
    //mySensor[id].setColList(col,2);
    cout<<" and after "<<mySensor[id].getColList().at(col)<<endl;
    currRun.badPixCount(-removed);
    for(Int_t counter=0; counter<(int)mySensor[id].getPixList().size(); counter++){
      //cout<<"Id "<< id <<" List has "<<(int)mySensor[id].getPixList().size()<<" entries"<<endl;
      Int_t tempCol=(mySensor[id].getPixList().at(counter))%1000;
      if(tempCol==temp%1000000/1000){
	cout<<"Found column"<<tempCol<<" Erasing "<<temp%1000<<"starting from: "<<counter<<endl;
	/*for(int i=0; i<temp%1000; i++)
	  cout<<counter+i<<"Pixel: "<<mySensor[id].getPixList().at(counter+i)<<endl;*/
	mySensor[id].removePxls(counter,temp%1000);
	break;
      }
    }
  }
  return;
}
// ------- Makes masking ---------
void StPxlMaskMaker::Make(Int_t run_proc,Char_t *mytimestamp,Char_t *yourPath, string runopt=""){
  //inner=l1;
  //outer=l2;
  StMemStat mem;
  currRun.SetRunName(run_proc,mytimestamp);
  Init();
  cout<<"Current memory used:"<<mem.Used()<<endl;
  cout<<"Reading File: "<<run_proc<<endl;
  path=yourPath;
  if (runopt=="")
    opt="physics";
  else
    opt=runopt;
  //Linking files to Jonathan's directory-2013 Engineering run
  //str=Form("/star/institutions/ksu/bouchet/Run13Production/all_files/%i.root",run_proc[i]);
  //Linking fles to 2014 Shusu directory
  //str=Form(" /star/u/sss/lbl/Analysis/Cosmic_Data/%i.root",run_proc[i]);
  //str=Form("/star/u/mlomnitz/Calibrations/Masking/%i.root",run_proc[i]);
  //need to set the output path
  cout<<"Writting tables to path: "<<path<<endl;
  str=fileloc;
  //cout<<"Reading File: "<<str<<currRun.Day()<<endl;
  /*if(opt=="cosmic")
    str=Form("/star/u/sss/lbl_prod/mask_table/rootfiles/cosmics_153_runs/%i.pxlMon.root",run_proc);
  else
  str=Form("/star/u/sss/lbl_prod/mask_table/rootfiles/%i.pxlMon.root",run_proc);*/
  //str=Form("%s/%i.pxlMon.root",yourPath,run_proc);
  cout<<"Reading file: "<<str<<endl;
  currRun.ClearRunStats();
  myfile=new TFile(str);
  myfile->cd();
  if(!myfile){
    cout<<"File: "<<str<<"not found"<<endl;
    return;
  }
  cout<<"Running mask"<<endl;
  runMask();
  //cout<<"Inidividual Pixel Mask table size: "<<currRun.totalBadPix<<endl;
  if(currRun.getPixCount()>10000){
    cout<<"Removing entries form pxl list. Has: "<<currRun.getPixCount()<<endl;
    removeEntries(currRun.getPixCount()-10000);
    cout<<"Inidividual Pixel Mask table size: "<<currRun.getPixCount()<<endl;
  }
  writeTables();
  summarizeMasking(run_proc,mytimestamp);
  myfile->Close(); 
  Finish();
  oSummary.close();
  sensor_summary.close();
  return;
}
