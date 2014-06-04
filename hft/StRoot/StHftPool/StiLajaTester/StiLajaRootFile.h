#ifndef StiLajaRootFile_h
#define StiLajaRootFile_h

#include <map>

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

   void FillHists(EventT &eventT);
   void Close(Option_t *option="");

 private:

   void BookHists();

   HistMap mHs;

   ClassDef(StiLajaRootFile, 1)
};

#endif
