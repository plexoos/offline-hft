#ifndef StiScanHistsByVolume_h
#define StiScanHistsByVolume_h

#include <map>
#include <set>
#include <string>

#include "StHftPool/EventT/StiScanHistContainer.h"



class StiScanHistsByVolume : public StiScanHistContainer
{
public:

   StiScanHistsByVolume();
   StiScanHistsByVolume(const char* name, const char* title, Option_t* option = "", TDirectory* motherDir = 0);

protected:

   virtual void FillHists(const TStiKalmanTrack &kalmTrack, const std::set<std::string> *volumeList=0);

   ClassDef(StiScanHistsByVolume, 1)
};

#endif
