#include "StPxlRun.h"
#include <assert.h>
#include <iostream>
#include<fstream>
#include <sstream>
#include <vector>
#include <string>
ClassImp(StPxlRun);
void StPxlRun::SetRunName(Int_t name, Char_t *s){
  //std::cout<<"Setting run name!"<<endl;
  run=(name-15000000)%1000;
  day=(name-(15000000+run))/1000;
  stamp=s;
  //cout<<"Stamp at beg"<<s<<"/"<<stamp<<endl;
  return;
}
void StPxlRun::ClearRunStats(){
  for(Int_t i=0; i<7; i++){
    Flags[i]=0;
  }
  tot_entries=0;
  //2014--------------------
  totalBadPix=0;
  //------------------------
  return;
}
Int_t* StPxlRun::getFlags(){
  Int_t *pointer=Flags;
  return(pointer);
}
