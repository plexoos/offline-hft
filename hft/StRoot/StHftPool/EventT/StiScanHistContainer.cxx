#include <cmath>

#include "StiScanHistContainer.h"

#include "TCanvas.h"
#include "TH2S.h"
#include "TProfile2D.h"
#include "TROOT.h"
#include "TVector3.h"


StiScanHistContainer::StiScanHistContainer() : TDirectoryFile(), mHs(), mNodeZMin(249.5), mNodeZMax(-250.5)
{
   BookHists();
}


StiScanHistContainer::StiScanHistContainer(const char* name, const char* title, Option_t* option, TDirectory* motherDir) :
   TDirectoryFile(name, title, option, motherDir), mHs(), mNodeZMin(249.5), mNodeZMax(-250.5)
{
   BookHists();
}


StiScanHistContainer::~StiScanHistContainer()
{
   while (!mHs.empty()) {
      delete mHs.begin()->second;
      mHs.erase(mHs.begin());
   }
}


void StiScanHistContainer::BookHists()
{
   this->cd();

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
   mHs["hAllVolELossVsEtaVsPhi"] = h = new TProfile2D("hAllVolELossVsEtaVsPhi", " ; #eta; #phi, rad; Energy Losses in Select Volumes, keV", 50, -2, 2, 120, -M_PI, M_PI);
   h->SetOption("colz");

   mHs["hAllVolELossVsZVsPhi"]   = h = new TProfile2D("hAllVolELossVsZVsPhi", " ; z, cm; #phi, rad; Energy Losses in All Volumes, keV", 500, -mNodeZMin, -mNodeZMax, 120, -M_PI, M_PI);
   h->SetOption("colz");

   mHs["hAllVolELossVsZVsR"]     = h = new TProfile2D("hAllVolELossVsZVsR", " ; z, cm; r, cm; Energy Losses in All Volumes, keV", 500, -mNodeZMin, -mNodeZMax, 100, 0, 30);
   h->SetOption("colz");

   mHs["hAllVolELossVsPhiVsR"]   = h = new TProfile2D("hAllVolELossVsPhiVsR", " ; #phi, rad; r, cm; Energy Losses in All Volumes, keV", 120, -M_PI, M_PI, 100, 0, 30);
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

   mHs["hSelectVolELossVsPhiVsR"]   = h = new TProfile2D("hSelectVolELossVsPhiVsR", " ; #phi, rad; r, cm; Energy Losses in Select Volumes, keV", 120, -M_PI, M_PI, 100, 0, 30);
   h->SetOption("colz");
}


void StiScanHistContainer::FillHists(const EventT &eventT, const std::set<std::string> *volumeList)
{
   std::vector<TStiKalmanTrack>::const_iterator iTStiKTrack = eventT.fTStiKalmanTracks.begin();

   for ( ; iTStiKTrack != eventT.fTStiKalmanTracks.end(); ++iTStiKTrack)
   {
      const TStiKalmanTrack &kalmTrack = *iTStiKTrack;

      FillHists(kalmTrack, volumeList);
   }
}


void StiScanHistContainer::FillHists(const EventG &eventG)
{
   TIter iGeantStep(eventG.steps);

   while (StepG* stepG = (StepG*) iGeantStep())
   {
      stepG->dEstep *= 1e6; // convert GeV to keV

      TVector3 step_pos(stepG->x, stepG->y, stepG->z);
      ((TProfile2D*) mHs["hAllVolELossVsZVsPhi"])->Fill(step_pos.Z(),   step_pos.Phi(),  stepG->dEstep, 1);
      ((TProfile2D*) mHs["hAllVolELossVsZVsR"])  ->Fill(step_pos.Z(),   step_pos.Perp(), stepG->dEstep, 1);
      ((TProfile2D*) mHs["hAllVolELossVsPhiVsR"])->Fill(step_pos.Phi(), step_pos.Perp(), stepG->dEstep, 1);
   }
}


void StiScanHistContainer::FillDerivedHists()
{
   this->cd();

   TH1* h;

   TProfile2D* prof2D = (TProfile2D*) mHs["hAllVolELossVsPhiVsR"];

   mHs["hAllVolELossVsPhiVsR_px"]  = h = prof2D->ProjectionX();
   h->SetOption("XY");

   mHs["hAllVolELossVsPhiVsR_pfx"] = h = prof2D->ProfileX("_pfx", 1, -1, "g");
   h->SetOption("XY");

   mHs["hAllVolELossVsPhiVsR_pfy"] = h = prof2D->ProfileY("_pfy", 1, -1, "g");
   h->SetOption("XY");
}


void StiScanHistContainer::FillHists(const TStiKalmanTrack &kalmTrack, const std::set<std::string> *volumeList)
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
   }
}


void StiScanHistContainer::FillHists_trk(const TStiKalmanTrack &kalmTrack, const std::set<std::string> *volumeList)
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

      ((TProfile2D*) mHs["hAllVolELossVsEtaVsPhi"])->Fill(kalmNode.GetPosition().Z(),   kalmNode.GetPosition().Phi(),  kalmNode.GetEnergyLosses(), 1);
      ((TProfile2D*) mHs["hAllVolELossVsZVsPhi"])  ->Fill(kalmNode.GetPosition().Z(),   kalmNode.GetPosition().Phi(),  kalmNode.GetEnergyLosses(), 1);
      ((TProfile2D*) mHs["hAllVolELossVsZVsR"])    ->Fill(kalmNode.GetPosition().Z(),   kalmNode.GetPosition().Perp(), kalmNode.GetEnergyLosses(), 1);
      ((TProfile2D*) mHs["hAllVolELossVsPhiVsR"])  ->Fill(kalmNode.GetPosition().Phi(), kalmNode.GetPosition().Perp(), kalmNode.GetEnergyLosses(), 1);

      if (volumeList && volumeList->size() && !kalmNode.MatchedVolName(*volumeList) ) continue;

      ((TProfile2D*) mHs["hSelectVolELossVsEtaVsPhi_trk"])->Fill(kalmNode.GetTrackP().Eta(), kalmNode.GetTrackP().Phi(), kalmNode.GetEnergyLosses(), 1);

      ((TProfile2D*) mHs["hSelectVolELossVsEtaVsPhi"])->Fill(kalmNode.GetPosition().Eta(), kalmNode.GetPosition().Phi(),  kalmNode.GetEnergyLosses(), 1);
      ((TProfile2D*) mHs["hSelectVolELossVsZVsPhi"])  ->Fill(kalmNode.GetPosition().Z(),   kalmNode.GetPosition().Phi(),  kalmNode.GetEnergyLosses(), 1);
      ((TProfile2D*) mHs["hSelectVolELossVsZVsR"])    ->Fill(kalmNode.GetPosition().Z(),   kalmNode.GetPosition().Perp(), kalmNode.GetEnergyLosses(), 1);
      ((TProfile2D*) mHs["hSelectVolELossVsPhiVsR"])  ->Fill(kalmNode.GetPosition().Phi(), kalmNode.GetPosition().Perp(), kalmNode.GetEnergyLosses(), 1);
   }
}


void StiScanHistContainer::SaveAllAs(std::string prefix)
{
   PrettifyHists();

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

      char* opts = (char*) obj->GetOption();
      char* l = strstr(opts, "XY");

      if (l) canvas.SetLogy(true);
      else   canvas.SetLogy(false);

      obj->Draw();

      string sFileName = prefix + "/c_" + objName + ".png";
      canvas.SaveAs(sFileName.c_str());
   }
}


void StiScanHistContainer::PrettifyHists()
{
   mHs["hTrackCountVsZVsPhi"]->GetXaxis()->SetRangeUser(floor(mNodeZMin)-0.5, ceil(mNodeZMax)-0.5);
   ((TProfile2D*) mHs["hTotalELossVsZVsPhi"])->GetXaxis()->SetRangeUser(floor(mNodeZMin)-0.5, ceil(mNodeZMax)-0.5);

   ((TProfile2D*) mHs["hAllVolELossVsZVsPhi"])->GetXaxis()->SetRangeUser(floor(mNodeZMin)-0.5, ceil(mNodeZMax)-0.5);
   ((TProfile2D*) mHs["hAllVolELossVsZVsR"])->GetXaxis()->SetRangeUser(floor(mNodeZMin)-0.5, ceil(mNodeZMax)-0.5);

   ((TProfile2D*) mHs["hSelectVolELossVsZVsPhi"])->GetXaxis()->SetRangeUser(floor(mNodeZMin)-0.5, ceil(mNodeZMax)-0.5);
   ((TProfile2D*) mHs["hSelectVolELossVsZVsR"])->GetXaxis()->SetRangeUser(floor(mNodeZMin)-0.5, ceil(mNodeZMax)-0.5);
}
