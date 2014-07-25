#ifndef StiScanHistContainer_h
#define StiScanHistContainer_h

#include <map>
#include <set>
#include <string>

#include "TDirectoryFile.h"
#include "TH2S.h"
#include "StMaker.h"

#include "StHftPool/EventT/EventT.h"


typedef std::map<std::string, TH1*>                    HistMap;
typedef std::map<std::string, TH1*>::iterator          HistMapIter;
typedef std::map<std::string, TH1*>::const_iterator    HistMapConstIter;


class StiScanHistContainer : public TDirectoryFile
{
public:

   StiScanHistContainer();
   StiScanHistContainer(const char* name, const char* title, Option_t* option = "", TDirectory* motherDir = 0);
   ~StiScanHistContainer();

   void FillHists(const EventT &eventT, const std::set<std::string> *volumeList=0);
   void SaveAllAs();
   void SetZRange(double minZ, double maxZ) { mNodeZMin = minZ; mNodeZMax = maxZ; }
   double GetZMin() { return mNodeZMin; }
   double GetZMax() { return mNodeZMax; }
   void PrettifyHists();

 private:

   void BookHists();
   void FillHists(const TStiKalmanTrack &kalmTrack, const std::set<std::string> *volumeList=0);
   void FillHists_trk(const TStiKalmanTrack &kalmTrack, const std::set<std::string> *volumeList=0);

   HistMap mHs;
   double  mNodeZMin;
   double  mNodeZMax;

   ClassDef(StiScanHistContainer, 1)
};

#endif
