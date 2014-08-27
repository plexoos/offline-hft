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

   ((TProfile2D*) mHs["hTotalELossVsEtaVsPhi"])->Fill(dcaNode.GetTrackP().Eta(), dcaNode.GetTrackP().Phi(), kalmTrack.GetEnergyLosses(), 1);
   ((TProfile2D*) mHs["hTotalELossVsZVsPhi"])->Fill(dcaNode.GetPosition().Z(), dcaNode.GetTrackP().Phi(), kalmTrack.GetEnergyLosses(), 1);

   std::set<TStiKalmanTrackNode>::const_iterator iTStiKTrackNode = kalmTrack.GetNodes().begin();

   for ( ; iTStiKTrackNode != kalmTrack.GetNodes().end(); ++iTStiKTrackNode)
   {
      const TStiKalmanTrackNode &kalmNode = *iTStiKTrackNode;

      ((TProfile2D*) mHs["hAllVolELossVsEtaVsPhi"])->Fill(kalmNode.GetPosition().Z(), kalmNode.GetNodeCenterRefAngle(), kalmNode.GetEnergyLosses(), 1);
      ((TProfile2D*) mHs["hAllVolELossVsZVsPhi"])->Fill(kalmNode.GetPosition().Z(), kalmNode.GetNodeCenterRefAngle(), kalmNode.GetEnergyLosses(), 1);
      ((TProfile2D*) mHs["hAllVolELossVsZVsR"])->Fill(kalmNode.GetPosition().Z(), kalmNode.GetNodeRadius(), kalmNode.GetEnergyLosses(), 1);
      ((TProfile2D*) mHs["hAllVolELossVsPhiVsR"])->Fill(kalmNode.GetNodeCenterRefAngle(), kalmNode.GetNodeRadius(), kalmNode.GetEnergyLosses(), 1);

      if (volumeList && volumeList->size() && !kalmNode.MatchedVolName(*volumeList) ) continue;

      ((TProfile2D*) mHs["hSelectVolELossVsEtaVsPhi_trk"])->Fill(kalmNode.GetTrackP().Eta(), kalmNode.GetTrackP().Phi(), kalmNode.GetEnergyLosses(), 1);

      ((TProfile2D*) mHs["hSelectVolELossVsEtaVsPhi"])->Fill(kalmNode.GetPosition().Eta(), kalmNode.GetNodeCenterRefAngle(), kalmNode.GetEnergyLosses(), 1);
      ((TProfile2D*) mHs["hSelectVolELossVsZVsPhi"])->Fill(kalmNode.GetPosition().Z(), kalmNode.GetNodeCenterRefAngle(), kalmNode.GetEnergyLosses(), 1);
      ((TProfile2D*) mHs["hSelectVolELossVsZVsR"])->Fill(kalmNode.GetPosition().Z(), kalmNode.GetNodeRadius(), kalmNode.GetEnergyLosses(), 1);
      ((TProfile2D*) mHs["hSelectVolELossVsPhiVsR"])->Fill(kalmNode.GetNodeCenterRefAngle(), kalmNode.GetNodeRadius(), kalmNode.GetEnergyLosses(), 1);
      ((TProfile2D*) mHs["hSelectVolDensityVsPhiVsR"])->Fill(kalmNode.GetNodeCenterRefAngle(), kalmNode.GetNodeRadius(), kalmNode.GetNodeMaterialDensity(), 1);
      ((TProfile2D*) mHs["hSelectVolRelRadLengthVsPhiVsR"])->Fill(kalmNode.GetNodeCenterRefAngle(), kalmNode.GetNodeRadius(), kalmNode.GetNodeRelRadLength(), 1);
   }
}
