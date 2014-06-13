#ifndef StiScanRootFile_h
#define StiScanRootFile_h

#include <map>
#include <set>
#include <string>

#include "TH2S.h"
#include "TFile.h"
#include "StMaker.h"

#include "StHftPool/EventT/EventT.h"


typedef std::map<std::string, TH1*>                    HistMap;
typedef std::map<std::string, TH1*>::iterator          HistMapIter;
typedef std::map<std::string, TH1*>::const_iterator    HistMapConstIter;


class StiScanRootFile : public TFile
{
public:

   StiScanRootFile();
   StiScanRootFile(const char* fname, Option_t* option="", const char* ftitle="", Int_t compress=1);
   ~StiScanRootFile();

   void FillHists(const EventT &eventT, const std::set<std::string> *volumeList=0);
   void FillHists(const TStiKalmanTrack &kalmTrack, const std::set<std::string> *volumeList=0);
   void Close(Option_t *option="");

 private:

   void BookHists();

   HistMap mHs;

   ClassDef(StiScanRootFile, 1)
};

#endif
