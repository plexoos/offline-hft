#ifndef StPxlSensor_hh
#define StPxlSensor_hh
#include <assert.h>
#include <exception>
#include <vector>
#include <iostream>
#include<fstream>
#include <sstream>
#include<cmath>
#include <string>
#include "TChain.h"
#include "TMath.h"
#include "TSystem.h"
#include "TFileSet.h"
#include "TDataSetIter.h"
#include "TDataSet.h"
#include "TClassTable.h"
class StPxlSensor{
 private:
  Int_t ladder, sector,sensor,ind_badPix, entries,status,missing;
  Double_t masked_out,hotCol,hotRow,hotPix;
  std::vector<int> listBadPix, listBadCol,listBadRow;
 public:
  StPxlSensor();
  ~StPxlSensor(){};
  void SetId(Int_t ID);
  void FlagClear();
  void maskClear();
  Int_t Status(){return(status);};
  Int_t Entries(){return(entries);}; 
  Int_t noHotCol(){return(hotCol);};
  Int_t noHotRow(){return(hotRow);};
  Int_t noHotPix(){return(hotPix);};
  //
  void setStatus(Int_t stat){status=stat;};
  void setMaskedOut(Double_t masked){masked_out=masked;};
  Double_t getMaskedOut(){return(masked_out);};
  void setMaskInfo(Int_t col, Int_t row, Int_t pix, Int_t entries, Int_t missing);
 //Pix list manip
  void addBadPix(Int_t id){listBadPix.push_back(id);};
  void pixClear(){listBadPix.clear(); };
  void clearPixList(){listBadPix.clear();};
  void removePxls(int counter, int total){listBadPix.erase(listBadPix.begin()+counter,listBadPix.begin()+counter+total); return;};
  std::vector<int> getPixList(){return(listBadPix);};
  //  
  void setColList(int place, int val){listBadCol.at(place)=val; return;};
  void setColStatus( Int_t stat){listBadCol.push_back(stat);};
  std::vector<int> getColList(){return(listBadCol);};
  //
  void setRowList(int place, int val){listBadRow.at(place)=val; return;};
  void setRowStatus(Int_t stat){listBadRow.push_back(stat);};
  std::vector<int> getRowList(){return(listBadRow);};
  
  ClassDef(StPxlSensor,1)
};
#endif
