#ifndef StiScanHistContainer_h
#define StiScanHistContainer_h

#include <map>
#include <set>
#include <string>

#include "TDirectoryFile.h"
#include "TProfile2D.h"

#include "StHftPool/EventT/EventT.h"
#include "StHftPool/EventT/StiScanPrgOptions.h"
#include "StHftPool/EventT/TStiKalmanTrack.h"
#include "StHftPool/EventT/Profile2D.h"
#include "StarGenerator/STEP/AgUStep.h"

typedef Event EventG;
typedef Track TrackG;
typedef Step  StepG;

typedef std::map<std::string, TH1*>                    HistMap;
typedef std::map<std::string, TH1*>::iterator          HistMapIter;
typedef std::map<std::string, TH1*>::const_iterator    HistMapConstIter;


class StiScanHistContainer : public TDirectoryFile
{
public:

   StiScanHistContainer(StiScanPrgOptions& prgOpts);
   StiScanHistContainer(StiScanPrgOptions& prgOpts, const char* name, const char* title, Option_t* option = "", TDirectory* motherDir = 0);
   ~StiScanHistContainer();

   void FillHists(const EventT &eventT, const std::set<std::string> *volumeList=0);
   void FillHists(const EventG &eventG, const std::set<std::string> *volumeList=0);
   void FillDerivedHists();
   void SaveAllAs(std::string prefix="./");
   void SetZRange(double minZ, double maxZ) { mNodeZMin = minZ; mNodeZMax = maxZ; }
   double GetZMin() { return mNodeZMin; }
   double GetZMax() { return mNodeZMax; }
   void SetRRange(float minR, float maxR) { mNodeRMin = minR; mNodeRMax = maxR; }
   float GetRMin() const { return mNodeRMin; }
   float GetRMax() const { return mNodeRMax; }
   void PrettifyHists();

protected:

   void BookHists();
   virtual void FillHists(const TStiKalmanTrack &kalmTrack, const std::set<std::string> *volumeList=0);
   void FillHists(const TrackG &trackG, const std::set<std::string> *volumeList=0);

   StiScanPrgOptions& fPrgOptions; ///< Command line arguments and options requested by the user
   HistMap mHs;
   double  mNodeZMin;
   double  mNodeZMax;
   float   mNodeRMin;
   float   mNodeRMax;

private:

   void InitRange();

   Profile2D* hNStepsVsPhiVsR_buf;
   Profile2D* hNStepsVsPhiVsR;
};

#endif
