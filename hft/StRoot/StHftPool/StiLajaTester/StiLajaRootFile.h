#ifndef StiLajaRootFile_h
#define StiLajaRootFile_h

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


class StiLajaRootFile : public TFile
{
public:

   StiLajaRootFile();
   StiLajaRootFile(const char* fname, Option_t* option="", const char* ftitle="", Int_t compress=1);
   ~StiLajaRootFile();

   void FillHists(const EventT &eventT, const std::set<std::string> *volumeList=0);
   void FillHists(const TStiKalmanTrack &kalmTrack, const std::set<std::string> *volumeList=0);
   void Close(Option_t *option="");

 private:

   void BookHists();

   HistMap mHs;

   ClassDef(StiLajaRootFile, 1)
};

#endif
