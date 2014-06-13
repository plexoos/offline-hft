#include "StiScanRootFile.h"

#include "TH2S.h"
#include "TProfile2D.h"
#include "TROOT.h"


StiScanRootFile::StiScanRootFile() : TFile(), mHs()
{
   BookHists();
}


StiScanRootFile::StiScanRootFile(const char *fname, Option_t *option, const char *ftitle, Int_t compress) :
   TFile(fname, option, ftitle, compress), mHs()
{
   printf("Created ROOT file: %s\n", GetName());

   BookHists();
}


StiScanRootFile::~StiScanRootFile()
{
   while (!mHs.empty()) {
      delete mHs.begin()->second;
      mHs.erase(mHs.begin());
   }
}


void StiScanRootFile::BookHists()
{
   // Create the root histograms outside of the current root file, i.e. they will not be associated with the file
   // TDirectory and won't be deleted on TFile::Close()
   gROOT->cd();

   TH1* h;

   mHs["hTrackCountVsEtaVsPhi"] = h = new TH2S("hTrackCountVsEtaVsPhi", " ; Eta; Phi, rad; Num. of Tracks", 101, -1, 1, 101, -M_PI, M_PI);
   h->SetOption("colz");
   mHs["hTrackCountVsZVsPhi"]   = h = new TH2S("hTrackCountVsZVsPhi", " ; Z, cm; Phi, rad; Num. of Tracks", 30, -15.5, 14.5, 101, -M_PI, M_PI);
   h->SetOption("colz");

   mHs["hTotalELossVsEtaVsPhi"] = h = new TProfile2D("hTotalELossVsEtaVsPhi", " ; Eta; Phi, rad; Total Energy Losses, keV", 101, -1, 1, 101, -M_PI, M_PI);
   h->SetOption("colz");
   mHs["hTotalELossVsZVsPhi"]   = h = new TProfile2D("hTotalELossVsZVsPhi", " ; Z, cm; Phi, rad; Total Energy Losses, keV", 30, -15.5, 14.5, 101, -M_PI, M_PI);
   h->SetOption("colz");

   mHs["hSelectVolELossVsEtaVsPhi"] = h = new TProfile2D("hSelectVolELossVsEtaVsPhi", " ; Eta; Phi, rad; Energy Losses in Select Volumes, keV", 101, -1, 1, 101, -M_PI, M_PI);
   h->SetOption("colz");
   mHs["hSelectVolELossVsZVsPhi"]   = h = new TProfile2D("hSelectVolELossVsZVsPhi", " ; Z, cm; Phi, rad; Energy Losses in Select Volumes, keV", 30, -15.5, 14.5, 101, -M_PI, M_PI);
   h->SetOption("colz");
}


void StiScanRootFile::FillHists(const EventT &eventT, const std::set<std::string> *volumeList)
{
   std::vector<TStiKalmanTrack>::const_iterator iTStiKTrack = eventT.fTStiKalmanTracks.begin();

   for ( ; iTStiKTrack != eventT.fTStiKalmanTracks.end(); ++iTStiKTrack) {
      //iTStiKTrack->Print();
      const TStiKalmanTrack &kalmTrack = *iTStiKTrack;

      FillHists(kalmTrack, volumeList);
   }
}


void StiScanRootFile::FillHists(const TStiKalmanTrack &kalmTrack, const std::set<std::string> *volumeList)
{
   // Take the first node with the smallest radius
   const TStiKalmanTrackNode& dcaNode = kalmTrack.GetDcaNode();

   mHs["hTrackCountVsEtaVsPhi"]->Fill(dcaNode.GetTrackP().Eta(), dcaNode.GetTrackP().Phi());
   mHs["hTrackCountVsZVsPhi"]->Fill(dcaNode.GetPosition().Z(), dcaNode.GetTrackP().Phi());

   ((TProfile2D*) mHs["hTotalELossVsEtaVsPhi"])->Fill(dcaNode.GetTrackP().Eta(), dcaNode.GetTrackP().Phi(), fabs(kalmTrack.GetEnergyLosses()), 1);
   ((TProfile2D*) mHs["hTotalELossVsZVsPhi"])->Fill(dcaNode.GetPosition().Z(), dcaNode.GetTrackP().Phi(), fabs(kalmTrack.GetEnergyLosses()), 1);

   std::set<TStiKalmanTrackNode>::const_iterator iTStiKTrackNode = kalmTrack.GetNodes().begin();

   for ( ; iTStiKTrackNode != kalmTrack.GetNodes().end(); ++iTStiKTrackNode)
   {
      const TStiKalmanTrackNode &kalmNode = *iTStiKTrackNode;

      if (!volumeList || !kalmNode.MatchedVolName(*volumeList) ) continue;

      ((TProfile2D*) mHs["hSelectVolELossVsEtaVsPhi"])->Fill(kalmNode.GetTrackP().Eta(), kalmNode.GetTrackP().Phi(), fabs(kalmNode.GetEnergyLosses()), 1);
      ((TProfile2D*) mHs["hSelectVolELossVsZVsPhi"])->Fill(kalmNode.GetPosition().Z(), kalmNode.GetTrackP().Phi(), fabs(kalmNode.GetEnergyLosses()), 1);
   }
}


/** */
void StiScanRootFile::Close(Option_t *option)
{
   this->cd();

   HistMapIter iHist = mHs.begin();

   for ( ; iHist!=mHs.end(); ++iHist) {
      if (iHist->second) iHist->second->Write();
   }

   Info("Close", "%s", GetName());
   TFile::Close(option);
}
