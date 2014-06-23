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

   mHs["hTrackCountVsEtaVsPhi"] = h = new TH2S("hTrackCountVsEtaVsPhi", " ; #eta; #phi, rad; Num. of Tracks", 101, -1, 1, 101, -M_PI, M_PI);
   h->SetOption("colz");
   mHs["hTrackCountVsZVsPhi"]   = h = new TH2S("hTrackCountVsZVsPhi", " ; z, cm; #phi, rad; Num. of Tracks", 50, -25.5, 24.5, 101, -M_PI, M_PI);
   h->SetOption("colz");

   mHs["hTotalELossVsEtaVsPhi"] = h = new TProfile2D("hTotalELossVsEtaVsPhi", " ; #eta; #phi, rad; Total Energy Losses, keV", 101, -1, 1, 101, -M_PI, M_PI);
   h->SetOption("colz");
   mHs["hTotalELossVsZVsPhi"]   = h = new TProfile2D("hTotalELossVsZVsPhi", " ; z, cm; #phi, rad; Total Energy Losses, keV", 50, -25.5, 24.5, 101, -M_PI, M_PI);
   h->SetOption("colz");

   // Histograms for all track nodes/volumes
   mHs["hVolELossVsEtaVsPhi"] = h = new TProfile2D("hVolELossVsEtaVsPhi", " ; #eta; #phi, rad; Energy Losses in Select Volumes, keV", 21, -1, 1, 101, -M_PI, M_PI);
   h->SetOption("colz");
   mHs["hVolELossVsZVsPhi"]   = h = new TProfile2D("hVolELossVsZVsPhi", " ; z, cm; #phi, rad; Energy Losses in All Volumes, keV", 50, -25.5, 24.5, 101, -M_PI, M_PI);
   h->SetOption("colz");
   mHs["hVolELossVsZVsR"]     = h = new TProfile2D("hVolELossVsZVsR", " ; z, cm; r, cm; Energy Losses in All Volumes, keV", 50, -25.5, 24.5, 30, 0, 30);
   h->SetOption("colz");

   // Histograms for selected track nodes/volumes only
   mHs["hSelectVolELossVsTrackEtaVsPhi"] = h = new TProfile2D("hSelectVolELossVsTrackEtaVsPhi", " ; Track #eta; Track #phi, rad; Energy Losses in Select Volumes, keV", 21, -1, 1, 101, -M_PI, M_PI);
   h->SetOption("colz");
   mHs["hSelectVolELossVsEtaVsPhi"] = h = new TProfile2D("hSelectVolELossVsEtaVsPhi", " ; #eta; #phi, rad; Energy Losses in Select Volumes, keV", 21, -1, 1, 101, -M_PI, M_PI);
   h->SetOption("colz");
   mHs["hSelectVolELossVsZVsPhi"]   = h = new TProfile2D("hSelectVolELossVsZVsPhi", " ; z, cm; #phi, rad; Energy Losses in Select Volumes, keV", 50, -25.5, 24.5, 101, -M_PI, M_PI);
   h->SetOption("colz");
   mHs["hSelectVolELossVsZVsR"]     = h = new TProfile2D("hSelectVolELossVsZVsR", " ; z, cm; r, cm; Energy Losses in Select Volumes, keV", 50, -25.5, 24.5, 30, 0, 30);
   h->SetOption("colz");
}


void StiScanRootFile::FillHists(const EventT &eventT, const std::set<std::string> *volumeList)
{
   std::vector<TStiKalmanTrack>::const_iterator iTStiKTrack = eventT.fTStiKalmanTracks.begin();

   for ( ; iTStiKTrack != eventT.fTStiKalmanTracks.end(); ++iTStiKTrack)
   {
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

      ((TProfile2D*) mHs["hVolELossVsEtaVsPhi"])->Fill(kalmNode.GetPosition().Z(), kalmNode.GetPosition().Phi(), fabs(kalmNode.GetEnergyLosses()), 1);
      ((TProfile2D*) mHs["hVolELossVsZVsPhi"])->Fill(kalmNode.GetPosition().Z(), kalmNode.GetPosition().Phi(), fabs(kalmNode.GetEnergyLosses()), 1);
      ((TProfile2D*) mHs["hVolELossVsZVsR"])->Fill(kalmNode.GetPosition().Z(), kalmNode.GetPosition().Perp(), fabs(kalmNode.GetEnergyLosses()), 1);

      if (!volumeList || !kalmNode.MatchedVolName(*volumeList) ) continue;

      ((TProfile2D*) mHs["hSelectVolELossVsTrackEtaVsPhi"])->Fill(kalmNode.GetTrackP().Eta(), kalmNode.GetTrackP().Phi(), fabs(kalmNode.GetEnergyLosses()), 1);
      ((TProfile2D*) mHs["hSelectVolELossVsEtaVsPhi"])->Fill(kalmNode.GetPosition().Eta(), kalmNode.GetPosition().Phi(), fabs(kalmNode.GetEnergyLosses()), 1);
      ((TProfile2D*) mHs["hSelectVolELossVsZVsPhi"])->Fill(kalmNode.GetPosition().Z(), kalmNode.GetPosition().Phi(), fabs(kalmNode.GetEnergyLosses()), 1);
      ((TProfile2D*) mHs["hSelectVolELossVsZVsR"])->Fill(kalmNode.GetPosition().Z(), kalmNode.GetPosition().Perp(), fabs(kalmNode.GetEnergyLosses()), 1);
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
