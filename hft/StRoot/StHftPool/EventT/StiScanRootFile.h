#ifndef StiScanRootFile_h
#define StiScanRootFile_h

#include <map>
#include <set>
#include <string>

#include "TH2S.h"
#include "TFile.h"
#include "StMaker.h"

#include "StHftPool/EventT/EventT.h"




class StiScanRootFile : public TFile
{
public:

   StiScanRootFile();
   StiScanRootFile(const char* fname, Option_t* option="", const char* ftitle="", Int_t compress=1);

   void FillHists(const EventT &eventT, const std::set<std::string> *volumeList=0);
   void FillHists(const TStiKalmanTrack &kalmTrack, const std::set<std::string> *volumeList=0);
   void Close(Option_t *option="");
   void SaveAllAs();

 private:

   void BookHists();


   ClassDef(StiScanRootFile, 1)
};

#endif
