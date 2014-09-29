#include <cmath>

#include "TProfile2D.h"
#include "TVector3.h"

#include "StHftPool/EventT/StiScanHistsByVolume.h"
#include "StHftPool/EventT/TStiKalmanTrackNode.h"


StiScanHistsByVolume::StiScanHistsByVolume(StiScanPrgOptions& prgOpts) : StiScanHistContainer(prgOpts)
{
}


StiScanHistsByVolume::StiScanHistsByVolume(StiScanPrgOptions& prgOpts, const char* name, const char* title, Option_t* option, TDirectory* motherDir) :
   StiScanHistContainer(prgOpts, name, title, option, motherDir)
{
}


void StiScanHistsByVolume::FillHists(const TStiKalmanTrack &kalmTrack, const std::set<std::string> *volumeList)
{
   // Take the first node with the smallest radius
   const TStiKalmanTrackNode& dcaNode = kalmTrack.GetDcaNode();

   mHs["hTrackCountVsEtaVsPhi"]->Fill(dcaNode.GetTrackP().Eta(), dcaNode.GetTrackP().Phi());
   mHs["hTrackCountVsZVsPhi"]->Fill(dcaNode.GetPosition().Z(), dcaNode.GetTrackP().Phi());

   std::set<TStiKalmanTrackNode>::const_iterator iTStiKTrackNode = kalmTrack.GetNodes().begin();

   for ( ; iTStiKTrackNode != kalmTrack.GetNodes().end(); ++iTStiKTrackNode)
   {
      const TStiKalmanTrackNode &kalmNode = *iTStiKTrackNode;

      if (volumeList && volumeList->size() && !kalmNode.MatchedVolName(*volumeList) ) continue;

      ((TProfile2D*) mHs["hELossVsEtaVsPhi_trk"])->Fill(kalmNode.GetTrackP().Eta(), kalmNode.GetTrackP().Phi(), kalmNode.GetEnergyLosses(), 1);

      ((TProfile2D*) mHs["hELossVsEtaVsPhi"])->Fill(kalmNode.GetPosition().Eta(), kalmNode.GetNodeCenterRefAngle(), kalmNode.GetEnergyLosses(), 1);
      ((TProfile2D*) mHs["hELossVsZVsPhi"])->Fill(kalmNode.GetPosition().Z(), kalmNode.GetNodeCenterRefAngle(), kalmNode.GetEnergyLosses(), 1);
      ((TProfile2D*) mHs["hELossVsZVsR"])->Fill(kalmNode.GetPosition().Z(), kalmNode.GetNodeRadius(), kalmNode.GetEnergyLosses(), 1);
      ((TProfile2D*) mHs["hELossVsPhiVsR"])->Fill(kalmNode.GetNodeCenterRefAngle(), kalmNode.GetNodeRadius(), kalmNode.GetEnergyLosses(), 1);
      hELossVsPhiVsRVsZ->Fill(kalmNode.GetNodeCenterRefAngle(), kalmNode.GetNodeRadius(), kalmNode.GetPosition().Z(), kalmNode.GetEnergyLosses());
      ((TProfile2D*) mHs["hDensityVsPhiVsR"])->Fill(kalmNode.GetNodeCenterRefAngle(), kalmNode.GetNodeRadius(), kalmNode.GetNodeMaterialDensity(), 1);
      ((TProfile2D*) mHs["hRelRadLengthVsPhiVsR"])->Fill(kalmNode.GetNodeCenterRefAngle(), kalmNode.GetNodeRadius(), kalmNode.GetNodeRelRadLength(), 1);
   }
}
