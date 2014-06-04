#include "StiLajaRootFile.h"

#include "TH2S.h"
#include "TProfile2D.h"
#include "TROOT.h"


StiLajaRootFile::StiLajaRootFile() : TFile(), mHs()
{
   BookHists();
}


StiLajaRootFile::StiLajaRootFile(const char *fname, Option_t *option, const char *ftitle, Int_t compress) :
   TFile(fname, option, ftitle, compress), mHs()
{
   printf("Created ROOT file: %s\n", GetName());

   BookHists();
}


StiLajaRootFile::~StiLajaRootFile()
{
   while (!mHs.empty()) {
      delete mHs.begin()->second;
      mHs.erase(mHs.begin());
   }
}


void StiLajaRootFile::BookHists()
{
   // Create the root histograms outside of the current root file, i.e. they will not be associated with the file
   // TDirectory and won't be deleted on TFile::Close()
   gROOT->cd();

   mHs["hTrackCountVsEtaVsPhi"] = new TH2S("hTrackCountVsEtaVsPhi", "hTrackCountVsEtaVsPhi", 11, -1, 1, 11, -M_PI, M_PI);
   mHs["hTotalELossVsEtaVsPhi"] = new TProfile2D("hTotalELossVsEtaVsPhi", "hTotalELossVsEtaVsPhi", 11, -1, 1, 11, -M_PI, M_PI);
}


void StiLajaRootFile::FillHists(EventT &eventT)
{
   std::vector<TStiKalmanTrack>::const_iterator iTStiKTrack = eventT.fTStiKalmanTracks.begin();

   for ( ; iTStiKTrack != eventT.fTStiKalmanTracks.end(); ++iTStiKTrack) {
      //iTStiKTrack->Print();
      const TStiKalmanTrack &kTrack = *iTStiKTrack;

      // Take the first node with the smallest radius
      const TStiKalmanTrackNode& dcaNode = kTrack.GetDcaNode();

      mHs["hTrackCountVsEtaVsPhi"]->Fill(dcaNode.GetTrackP().Eta(), dcaNode.GetTrackP().Phi());
      ((TProfile2D*) mHs["hTotalELossVsEtaVsPhi"])->Fill(dcaNode.GetTrackP().Eta(), dcaNode.GetTrackP().Phi(), kTrack.GetEnergyLosses(), 1);
   }
}


/** */
void StiLajaRootFile::Close(Option_t *option)
{
   this->cd();

   HistMapIter iHist = mHs.begin();

   for ( ; iHist!=mHs.end(); ++iHist) {
      if (iHist->second) iHist->second->Write();
   }

   Info("Close", "%s", GetName());
   TFile::Close(option);
}
