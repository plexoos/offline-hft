#include "StiLajaRootFile.h"


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
   while (!mHs.empty()) delete mHs.end()->second, mHs.erase(mHs.end());
}


void StiLajaRootFile::BookHists()
{
   mHs["hVolumeMatchedTracks"] = new TH2S("hVolumeMatchedTracks", "hVolumeMatchedTracks", 10, -1, 1, 10, -M_PI, M_PI);
}


void StiLajaRootFile::FillHists()
{
   //mHs["hVolumeMatchedTracks"]->Fill();
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
