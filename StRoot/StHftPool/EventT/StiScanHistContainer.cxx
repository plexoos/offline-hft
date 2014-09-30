#include <cmath>

#include "TCanvas.h"
#include "TH2S.h"
#include "TProfile2D.h"
#include "TVector3.h"

#include "StHftPool/EventT/StiScanHistContainer.h"
#include "StHftPool/EventT/TStiKalmanTrackNode.h"


StiScanHistContainer::StiScanHistContainer(StiScanPrgOptions& prgOpts) : TDirectoryFile(),
   fPrgOptions(prgOpts), mHs(), mNodeZMin(-250), mNodeZMax(250),
   mNodeRMin(0), mNodeRMax(30),
   hSelectVolNStepsVsPhiVsR_buf(nullptr),
   hSelectVolNStepsVsPhiVsR(nullptr)
{
   InitRange();
   BookHists();
}


StiScanHistContainer::StiScanHistContainer(StiScanPrgOptions& prgOpts, const char* name, const char* title, Option_t* option, TDirectory* motherDir) :
   TDirectoryFile(name, title, option, motherDir),
   fPrgOptions(prgOpts),
   mHs(), mNodeZMin(-250), mNodeZMax(250),
   mNodeRMin(0), mNodeRMax(30),
   hSelectVolNStepsVsPhiVsR_buf(nullptr),
   hSelectVolNStepsVsPhiVsR(nullptr)
{
   InitRange();
   BookHists();
}


StiScanHistContainer::~StiScanHistContainer()
{
   while (!mHs.empty()) {
      delete mHs.begin()->second;
      mHs.erase(mHs.begin());
   }
}


/** The default limits will be used if user provided values for min >= max. */
void StiScanHistContainer::InitRange()
{
   if (fPrgOptions.GetHistZMin() < fPrgOptions.GetHistZMax() ) {
      mNodeZMin = fPrgOptions.GetHistZMin();
      mNodeZMax = fPrgOptions.GetHistZMax();
   }

   if (fPrgOptions.GetHistRMin() < fPrgOptions.GetHistRMax() ) {
      mNodeRMin = fPrgOptions.GetHistRMin();
      mNodeRMax = fPrgOptions.GetHistRMax();
   }
}


void StiScanHistContainer::BookHists()
{
   const double minZBinWidth = 1;   // desired bin width in cm
   const double minRBinWidth = 0.2; // desired bin width in cm

   int nZBins = ceil( (mNodeZMax - mNodeZMin) / minZBinWidth );
   int nRBins = ceil( (mNodeRMax - mNodeRMin) / minRBinWidth );

   nRBins = nRBins > 150 ? 150 : nRBins;
   nRBins = nRBins <  50 ?  50 : nRBins;

   nZBins = nZBins > 500 ? 500 : nZBins;

   this->cd();

   TH1* h;

   mHs["hTrackCountVsEtaVsPhi"] = h = new TH2S("hTrackCountVsEtaVsPhi", " ; #eta; #phi, rad; Num. of Tracks", 50, -2, 2, 120, -M_PI, M_PI);
   h->SetOption("colz");

   mHs["hTrackCountVsZVsPhi"]   = h = new TH2S("hTrackCountVsZVsPhi", " ; z, cm; #phi, rad; Num. of Tracks", nZBins, mNodeZMin, mNodeZMax, 120, -M_PI, M_PI);
   h->SetOption("colz");

   // Histograms for selected track nodes/volumes only
   hSelectVolNStepsVsPhiVsR_buf = new Profile2D("hSelectVolNStepsVsPhiVsR_buf", " ; #phi, rad; r, cm; Num. of Steps", 120, -M_PI, M_PI, nRBins, mNodeRMin, mNodeRMax);

   hSelectVolNStepsVsPhiVsR = new Profile2D("hSelectVolNStepsVsPhiVsR", " ; #phi, rad; r, cm; Num. of Steps", 120, -M_PI, M_PI, nRBins, mNodeRMin, mNodeRMax);
   mHs["hSelectVolNStepsVsPhiVsR"] = hSelectVolNStepsVsPhiVsR;
   hSelectVolNStepsVsPhiVsR->SetOption("colz");

   mHs["hSelectVolELossVsEtaVsPhi_trk"] = h = new TProfile2D("hSelectVolELossVsEtaVsPhi_trk", " ; Track #eta; Track #phi, rad; Energy Losses in Select Volumes, keV", 50, -2, 2, 120, -M_PI, M_PI);
   h->SetOption("colz");

   mHs["hSelectVolELossVsEtaVsPhi"] = h = new TProfile2D("hSelectVolELossVsEtaVsPhi", " ; #eta; #phi, rad; Energy Losses in Select Volumes, keV", 50, -2, 2, 120, -M_PI, M_PI);
   h->SetOption("colz");

   mHs["hSelectVolELossVsZVsPhi"]   = h = new TProfile2D("hSelectVolELossVsZVsPhi", " ; z, cm; #phi, rad; Energy Losses in Select Volumes, keV", nZBins, mNodeZMin, mNodeZMax, 120, -M_PI, M_PI);
   h->SetOption("colz");

   mHs["hSelectVolELossVsZVsR"]     = h = new TProfile2D("hSelectVolELossVsZVsR", " ; z, cm; r, cm; Energy Losses in Select Volumes, keV", nZBins, mNodeZMin, mNodeZMax, nRBins, mNodeRMin, mNodeRMax);
   h->SetOption("colz");

   mHs["hSelectVolELossVsPhiVsR"]   = h = new TProfile2D("hSelectVolELossVsPhiVsR", " ; #phi, rad; r, cm; Energy Losses in Select Volumes, keV", 120, -M_PI, M_PI, nRBins, mNodeRMin, mNodeRMax);
   h->SetOption("colz");

   mHs["hSelectVolDensityVsPhiVsR"] = h = new TProfile2D("hSelectVolDensityVsPhiVsR", " ; #phi, rad; r, cm; Material Density, g/cm^{3}", 120, -M_PI, M_PI, nRBins, mNodeRMin, mNodeRMax);
   h->SetOption("colz");

   mHs["hSelectVolRelRadLengthVsPhiVsR"] = h = new TProfile2D("hSelectVolRelRadLengthVsPhiVsR", " ; #phi, rad; r, cm; Rel. Radiation Length, %", 120, -M_PI, M_PI, nRBins, mNodeRMin, mNodeRMax);
   h->SetOption("colz");

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


void StiScanHistContainer::FillHists(const EventG &eventG, const std::set<std::string> *volumeList)
{
   TIter iGeantTrack(eventG.tracks);

   while (TrackG* trackG = (TrackG*) iGeantTrack())
   {
      FillHists(*trackG, volumeList);
   }
}


void StiScanHistContainer::FillDerivedHists()
{
   this->cd();

   TH1* h;
   TProfile2D* prof2D;

   // Create a projection from hSelectVolNStepsVsPhiVsR
   prof2D = (TProfile2D*) mHs["hSelectVolNStepsVsPhiVsR"];

   mHs["hSelectVolNStepsVsPhiVsR_px"]  = h = prof2D->ProjectionX();
   h->SetTitle(" ; #phi, rad; Num. of Steps (from ProjectionX)");

   Profile2D* hProf = (Profile2D*) mHs["hSelectVolELossVsPhiVsR"];
   hProf->Multiply(*hSelectVolNStepsVsPhiVsR);

   // Create a projection from hSelectVolELossVsPhiVsR
   prof2D = (TProfile2D*) mHs["hSelectVolELossVsPhiVsR"];

   mHs["hSelectVolELossVsPhiVsR_px"]  = h = prof2D->ProjectionX();
   h->SetTitle(" ; #phi, rad; Energy Losses in Select Volumes, keV");
   h->SetOption("XY");

   // Create a projection from hSelectVolRelRadLengthVsPhiVsR
   prof2D = (TProfile2D*) mHs["hSelectVolRelRadLengthVsPhiVsR"];

   mHs["hSelectVolRelRadLengthVsPhiVsR_px"] = h = prof2D->ProjectionX();
   h->SetTitle(" ; #phi, rad; Rel. Radiation Length, %");
   h->SetOption("XY");

   prof2D = (TProfile2D*) mHs["hSelectVolDensityVsPhiVsR"];

   mHs["hSelectVolDensityVsPhiVsR_px"] = h = prof2D->ProjectionX();
   h->SetTitle(" ; #phi, rad; \"Total\" (added) Material Density, g/cm^{3}");
}


void StiScanHistContainer::FillHists(const TStiKalmanTrack &kalmTrack, const std::set<std::string> *volumeList)
{
   // Take the first node with the smallest radius
   const TStiKalmanTrackNode& dcaNode = kalmTrack.GetDcaNode();

   mHs["hTrackCountVsEtaVsPhi"]->Fill(dcaNode.GetTrackP().Eta(), dcaNode.GetTrackP().Phi());
   mHs["hTrackCountVsZVsPhi"]->Fill(dcaNode.GetPosition().Z(), dcaNode.GetTrackP().Phi());


   std::set<TStiKalmanTrackNode>::const_iterator iTStiKTrackNode = kalmTrack.GetNodes().begin();
   hSelectVolNStepsVsPhiVsR_buf->Reset("ICES"); // reset the histogram

   for ( ; iTStiKTrackNode != kalmTrack.GetNodes().end(); ++iTStiKTrackNode)
   {
      const TStiKalmanTrackNode &kalmNode = *iTStiKTrackNode;

      if (volumeList && volumeList->size() && !kalmNode.MatchedVolName(*volumeList) ) continue;

      if (kalmNode.GetNodeMaterialDensity() <= 0) continue;

      hSelectVolNStepsVsPhiVsR_buf->Fill(kalmNode.GetPosition().Phi(), kalmNode.GetPosition().Perp(), 1, 1);
      ((TProfile2D*) mHs["hSelectVolELossVsEtaVsPhi_trk"])->Fill(kalmNode.GetTrackP().Eta(), kalmNode.GetTrackP().Phi(), kalmNode.GetEnergyLosses(), 1);

      ((TProfile2D*) mHs["hSelectVolELossVsEtaVsPhi"])->Fill(kalmNode.GetPosition().Eta(), kalmNode.GetPosition().Phi(),  kalmNode.GetEnergyLosses(), 1);
      ((TProfile2D*) mHs["hSelectVolELossVsZVsPhi"])  ->Fill(kalmNode.GetPosition().Z(),   kalmNode.GetPosition().Phi(),  kalmNode.GetEnergyLosses(), 1);
      ((TProfile2D*) mHs["hSelectVolELossVsZVsR"])    ->Fill(kalmNode.GetPosition().Z(),   kalmNode.GetPosition().Perp(), kalmNode.GetEnergyLosses(), 1);
      ((TProfile2D*) mHs["hSelectVolELossVsPhiVsR"])  ->Fill(kalmNode.GetPosition().Phi(), kalmNode.GetPosition().Perp(), kalmNode.GetEnergyLosses(), 1);
      ((TProfile2D*) mHs["hSelectVolDensityVsPhiVsR"])->Fill(kalmNode.GetPosition().Phi(), kalmNode.GetPosition().Perp(), kalmNode.GetNodeMaterialDensity(), 1);
      ((TProfile2D*) mHs["hSelectVolRelRadLengthVsPhiVsR"])->Fill(kalmNode.GetPosition().Phi(), kalmNode.GetPosition().Perp(), kalmNode.GetNodeRelRadLength(), 1);
   }

   hSelectVolNStepsVsPhiVsR_buf->ResetBinEntries(1); // Force number of entries per bin to 1
   hSelectVolNStepsVsPhiVsR->Add(hSelectVolNStepsVsPhiVsR_buf); // Calculate the average number of steps by adding the temp profile
}


void StiScanHistContainer::FillHists(const TrackG &trackG, const std::set<std::string> *volumeList)
{
   hSelectVolNStepsVsPhiVsR_buf->Reset("ICES"); // reset the histogram

   TIter iGeantStep(&trackG.steps);

   while (StepG* stepG = (StepG*) iGeantStep())
   {
      double dEStep = stepG->dEstep * 1e6; // convert GeV to keV

      TVector3 step_pos(stepG->x, stepG->y, stepG->z);

      if (volumeList && volumeList->size() && !stepG->MatchedVolName(*volumeList) ) continue;

      hSelectVolNStepsVsPhiVsR_buf->Fill(step_pos.Phi(), step_pos.Perp(), 1, 1);

      ((TProfile2D*) mHs["hSelectVolELossVsZVsPhi"])->Fill(step_pos.Z(),   step_pos.Phi(),  dEStep, 1);
      ((TProfile2D*) mHs["hSelectVolELossVsZVsR"])  ->Fill(step_pos.Z(),   step_pos.Perp(), dEStep, 1);
      ((TProfile2D*) mHs["hSelectVolELossVsPhiVsR"])->Fill(step_pos.Phi(), step_pos.Perp(), dEStep, 1);
      ((TProfile2D*) mHs["hSelectVolDensityVsPhiVsR"])->Fill(step_pos.Phi(), step_pos.Perp(), stepG->dens, 1);
      ((TProfile2D*) mHs["hSelectVolRelRadLengthVsPhiVsR"])->Fill(step_pos.Phi(), step_pos.Perp(), stepG->relRadLength, 1);
   }

   hSelectVolNStepsVsPhiVsR_buf->ResetBinEntries(1); // Force number of entries per bin to 1
   hSelectVolNStepsVsPhiVsR->Add(hSelectVolNStepsVsPhiVsR_buf); // Calculate the average number of steps by adding the temp profile
}


void StiScanHistContainer::SaveAllAs(std::string prefix)
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

   ((TProfile2D*) mHs["hSelectVolELossVsZVsPhi"])->GetXaxis()->SetRangeUser(floor(mNodeZMin)-0.5, ceil(mNodeZMax)-0.5);

   ((TProfile2D*) mHs["hSelectVolELossVsZVsR"])->GetXaxis()->SetRangeUser(floor(mNodeZMin)-0.5, ceil(mNodeZMax)-0.5);
   ((TProfile2D*) mHs["hSelectVolELossVsZVsR"])->GetYaxis()->SetRangeUser(floor(mNodeRMin)-0.5, ceil(mNodeRMax)-0.5);

   ((TProfile2D*) mHs["hSelectVolELossVsPhiVsR"])->GetYaxis()->SetRangeUser(floor(mNodeRMin)-0.5, ceil(mNodeRMax)-0.5);
}
