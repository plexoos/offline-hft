#include <cmath>

#include "StiScanRootFile.h"

#include "TCanvas.h"
#include "TH2S.h"
#include "TProfile2D.h"
#include "TROOT.h"


StiScanRootFile::StiScanRootFile() : TFile(), mHs(), mNodeZMin(249.5), mNodeZMax(-250.5)
{
   BookHists();
}


StiScanRootFile::StiScanRootFile(const char *fname, Option_t *option, const char *ftitle, Int_t compress) :
   TFile(fname, option, ftitle, compress), mHs(), mNodeZMin(249.5), mNodeZMax(-250.5)
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

   mHs["hTrackCountVsEtaVsPhi"] = h = new TH2S("hTrackCountVsEtaVsPhi", " ; #eta; #phi, rad; Num. of Tracks", 50, -2, 2, 120, -M_PI, M_PI);
   h->SetOption("colz");

   mHs["hTrackCountVsZVsPhi"]   = h = new TH2S("hTrackCountVsZVsPhi", " ; z, cm; #phi, rad; Num. of Tracks", 500, -mNodeZMin, -mNodeZMax, 120, -M_PI, M_PI);
   h->SetOption("colz");

   mHs["hTotalELossVsEtaVsPhi"] = h = new TProfile2D("hTotalELossVsEtaVsPhi", " ; #eta; #phi, rad; Total Energy Losses, keV", 50, -2, 2, 120, -M_PI, M_PI);
   h->SetOption("colz");

   mHs["hTotalELossVsZVsPhi"]   = h = new TProfile2D("hTotalELossVsZVsPhi", " ; z, cm; #phi, rad; Total Energy Losses, keV", 500, -mNodeZMin, -mNodeZMax, 120, -M_PI, M_PI);
   h->SetOption("colz");

   // Histograms for all track nodes/volumes
   mHs["hVolELossVsEtaVsPhi"] = h = new TProfile2D("hVolELossVsEtaVsPhi", " ; #eta; #phi, rad; Energy Losses in Select Volumes, keV", 50, -2, 2, 120, -M_PI, M_PI);
   h->SetOption("colz");

   mHs["hVolELossVsZVsPhi"]   = h = new TProfile2D("hVolELossVsZVsPhi", " ; z, cm; #phi, rad; Energy Losses in All Volumes, keV", 500, -mNodeZMin, -mNodeZMax, 120, -M_PI, M_PI);
   h->SetOption("colz");

   mHs["hVolELossVsZVsR"]     = h = new TProfile2D("hVolELossVsZVsR", " ; z, cm; r, cm; Energy Losses in All Volumes, keV", 500, -mNodeZMin, -mNodeZMax, 100, 0, 30);
   h->SetOption("colz");

   mHs["hVolELossVsPhiVsR"]     = h = new TProfile2D("hVolELossVsPhiVsR", " ; #phi, rad; r, cm; Energy Losses in All Volumes, keV", 120, -M_PI, M_PI, 100, 0, 30);
   h->SetOption("colz");

   // Histograms for selected track nodes/volumes only
   mHs["hSelectVolELossVsEtaVsPhi_trk"] = h = new TProfile2D("hSelectVolELossVsEtaVsPhi_trk", " ; Track #eta; Track #phi, rad; Energy Losses in Select Volumes, keV", 50, -2, 2, 120, -M_PI, M_PI);
   h->SetOption("colz");

   mHs["hSelectVolELossVsEtaVsPhi"] = h = new TProfile2D("hSelectVolELossVsEtaVsPhi", " ; #eta; #phi, rad; Energy Losses in Select Volumes, keV", 50, -2, 2, 120, -M_PI, M_PI);
   h->SetOption("colz");

   mHs["hSelectVolELossVsZVsPhi"]   = h = new TProfile2D("hSelectVolELossVsZVsPhi", " ; z, cm; #phi, rad; Energy Losses in Select Volumes, keV", 500, -mNodeZMin, -mNodeZMax, 120, -M_PI, M_PI);
   h->SetOption("colz");

   mHs["hSelectVolELossVsZVsR"]     = h = new TProfile2D("hSelectVolELossVsZVsR", " ; z, cm; r, cm; Energy Losses in Select Volumes, keV", 500, -mNodeZMin, -mNodeZMax, 100, 0, 30);
   h->SetOption("colz");

   mHs["hSelectVolELossVsPhiVsR"]     = h = new TProfile2D("hSelectVolELossVsPhiVsR", " ; #phi, rad; r, cm; Energy Losses in Select Volumes, keV", 120, -M_PI, M_PI, 100, 0, 30);
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

   mHs["hTrackCountVsZVsPhi"]->GetXaxis()->SetRangeUser(floor(mNodeZMin)-0.5, ceil(mNodeZMax)-0.5);
   ((TProfile2D*) mHs["hTotalELossVsZVsPhi"])->GetXaxis()->SetRangeUser(floor(mNodeZMin)-0.5, ceil(mNodeZMax)-0.5);

   ((TProfile2D*) mHs["hVolELossVsZVsPhi"])->GetXaxis()->SetRangeUser(floor(mNodeZMin)-0.5, ceil(mNodeZMax)-0.5);
   ((TProfile2D*) mHs["hVolELossVsZVsR"])->GetXaxis()->SetRangeUser(floor(mNodeZMin)-0.5, ceil(mNodeZMax)-0.5);

   ((TProfile2D*) mHs["hSelectVolELossVsZVsPhi"])->GetXaxis()->SetRangeUser(floor(mNodeZMin)-0.5, ceil(mNodeZMax)-0.5);
   ((TProfile2D*) mHs["hSelectVolELossVsZVsR"])->GetXaxis()->SetRangeUser(floor(mNodeZMin)-0.5, ceil(mNodeZMax)-0.5);
}


void StiScanRootFile::FillHists(const TStiKalmanTrack &kalmTrack, const std::set<std::string> *volumeList)
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

      double node_z = kalmNode.GetPosition().Z();

      if (node_z < mNodeZMin) mNodeZMin = node_z;
      if (node_z > mNodeZMax) mNodeZMax = node_z;

      ((TProfile2D*) mHs["hVolELossVsEtaVsPhi"])->Fill(kalmNode.GetPosition().Z(), kalmNode.GetNodeCenterRefAngle(), kalmNode.GetEnergyLosses(), 1);
      ((TProfile2D*) mHs["hVolELossVsZVsPhi"])->Fill(kalmNode.GetPosition().Z(), kalmNode.GetNodeCenterRefAngle(), kalmNode.GetEnergyLosses(), 1);
      ((TProfile2D*) mHs["hVolELossVsZVsR"])->Fill(kalmNode.GetPosition().Z(), kalmNode.GetNodeRadius(), kalmNode.GetEnergyLosses(), 1);
      ((TProfile2D*) mHs["hVolELossVsPhiVsR"])->Fill(kalmNode.GetNodeCenterRefAngle(), kalmNode.GetNodeRadius(), kalmNode.GetEnergyLosses(), 1);

      if (volumeList && volumeList->size() && !kalmNode.MatchedVolName(*volumeList) ) continue;

      ((TProfile2D*) mHs["hSelectVolELossVsEtaVsPhi_trk"])->Fill(kalmNode.GetTrackP().Eta(), kalmNode.GetTrackP().Phi(), kalmNode.GetEnergyLosses(), 1);

      ((TProfile2D*) mHs["hSelectVolELossVsEtaVsPhi"])->Fill(kalmNode.GetPosition().Eta(), kalmNode.GetNodeCenterRefAngle(), kalmNode.GetEnergyLosses(), 1);
      ((TProfile2D*) mHs["hSelectVolELossVsZVsPhi"])->Fill(kalmNode.GetPosition().Z(), kalmNode.GetNodeCenterRefAngle(), kalmNode.GetEnergyLosses(), 1);
      ((TProfile2D*) mHs["hSelectVolELossVsZVsR"])->Fill(kalmNode.GetPosition().Z(), kalmNode.GetNodeRadius(), kalmNode.GetEnergyLosses(), 1);
      ((TProfile2D*) mHs["hSelectVolELossVsPhiVsR"])->Fill(kalmNode.GetNodeCenterRefAngle(), kalmNode.GetNodeRadius(), kalmNode.GetEnergyLosses(), 1);
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


void StiScanRootFile::SaveAllAs()
{
   TCanvas canvas("canvas", "canvas", 1400, 600);
   canvas.UseCurrentStyle();
   canvas.SetGridx(true);
   canvas.SetGridy(true);
   canvas.SetLogz(true);

   HistMapIter iHist = mHs.begin();

   for ( ; iHist!=mHs.end(); ++iHist) {
      // For shorthand
      string   objName = iHist->first;
      TObject *obj      = iHist->second;

      if (!obj) {
         Error("SaveAllAs", "No object found for key %s. Skipping...", objName.c_str());
         continue;
      }

      obj->Draw();

      string sFileName =  "c_" + objName + ".png";
      canvas.SaveAs(sFileName.c_str());
   }
}
