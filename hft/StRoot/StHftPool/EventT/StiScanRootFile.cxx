#include <cmath>

#include "StiScanRootFile.h"

#include "TCanvas.h"
#include "TH2S.h"
#include "TProfile2D.h"
#include "TROOT.h"

#include "StHftPool/EventT/StiScanHistContainer.h"


StiScanRootFile::StiScanRootFile() : TFile(), mDirs()
{
   BookHists();
}


StiScanRootFile::StiScanRootFile(const char *fname, Option_t *option, const char *ftitle, Int_t compress) :
   TFile(fname, option, ftitle, compress), mDirs()
{
   printf("Created ROOT file: %s\n", GetName());

   BookHists();
}


void StiScanRootFile::BookHists()
{
   mDirs["sti"] = new StiScanHistContainer("sti", "sti", "", this);
   mDirs["gea"] = new StiScanHistContainer("gea", "gea", "", this);
}


void StiScanRootFile::FillHists(const EventT &eventT, const std::set<std::string> *volumeList)
{
}


void StiScanRootFile::FillHists(const TStiKalmanTrack &kalmTrack, const std::set<std::string> *volumeList)
{
}


void StiScanRootFile::Close(Option_t *option)
{
   TFile::Close(option);
}


void StiScanRootFile::SaveAllAs()
{
   for (TDirMapConstIter iDir=mDirs.begin() ; iDir!=mDirs.end(); ++iDir)
   {
      std::string  dirName = iDir->first;
      StiScanHistContainer *container = static_cast<StiScanHistContainer*> (iDir->second);

      if (!container) {
         Error("SaveAllAs", "No container/directory found for key %s. Skipping...", dirName.c_str());
         continue;
      }

      container->SaveAllAs();
   }
}
