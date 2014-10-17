#ifndef StPxlRun_hh
#define StPxlRun_hh
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
class StPxlRun{
 private:
  Int_t run, day,Flags[7], tot_entries;
  std::vector <int> badPixinCol[400];
  Int_t totalBadPix;
  Char_t *stamp;
 public:
  StPxlRun(){};
  ~StPxlRun(){};
  Int_t Day(){return(day);};
  Int_t Run(){return(run);};
  void addFlag(Int_t flag){Flags[flag]+=1;};
  Char_t* rStamp(){return(stamp);};
  void SetRunName(Int_t name, Char_t *s);
  void setEntries(Int_t sentries){tot_entries+=sentries; return;};
  void ClearRunStats();
  Int_t *getFlags();
  Int_t Entries(){return(tot_entries);};
  std::vector<int> colPix(Int_t id){return(badPixinCol[id]);};
  Int_t SensorStats[7];
  void badPixCount(Int_t badPix){totalBadPix+=badPix; return;};
  Int_t getPixCount(){return(totalBadPix);};
  //
  void setBadPixinCol(Int_t id, Int_t value){badPixinCol[id].push_back(value); return;};
  void clearBadPixIn(int id){badPixinCol[id].clear(); return;}
  ClassDef(StPxlRun, 1)  
};
#endif
