#include <cmath>

#include "StiScanRootFile.h"

#include "TCanvas.h"
#include "TH2S.h"
#include "TProfile2D.h"
#include "TROOT.h"


StiScanRootFile::StiScanRootFile() : TFile()
{
   BookHists();
}


StiScanRootFile::StiScanRootFile(const char *fname, Option_t *option, const char *ftitle, Int_t compress) :
   TFile(fname, option, ftitle, compress)
{
   printf("Created ROOT file: %s\n", GetName());

   BookHists();
}


void StiScanRootFile::BookHists()
{
}


void StiScanRootFile::FillHists(const EventT &eventT, const std::set<std::string> *volumeList)
{
}


void StiScanRootFile::FillHists(const TStiKalmanTrack &kalmTrack, const std::set<std::string> *volumeList)
{
}


void StiScanRootFile::Close(Option_t *option)
{
   Info("Close", "%s", GetName());
   TFile::Close(option);
}


void StiScanRootFile::SaveAllAs()
{

         continue;
      }

   }
}
