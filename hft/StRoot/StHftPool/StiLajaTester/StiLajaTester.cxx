
#include "StHftPool/StiLajaTester/StiLajaTester.h"

#include <algorithm>

#include "TSystem.h"

#include "Sti/StiHitLoader.h"
#include "StChain/StChainOpt.h"
#include "Sti/StiTrackContainer.h"
#include "Sti/StiKalmanTrack.h"
#include "StiMaker/StiMaker.h"
#include "StBFChain/StBFChain.h"

ClassImp(StiLajaTester)


StiLajaTester::StiLajaTester(const Char_t *name) : StMaker(name),
   mOutRootFile(0)
{
   StBFChain* chain = (StBFChain*) StMaker::GetChain();
   assert(chain);

   TString inpFileName(gSystem->BaseName( chain->GetFileIn().Data() ));

   inpFileName.ReplaceAll(".event", "");
   inpFileName.ReplaceAll(".daq", "");
   inpFileName.ReplaceAll(".fz", "");

   mOutRootFile = new StiLajaRootFile( (inpFileName + ".laja.root").Data(), "recreate");
}


StiLajaTester::~StiLajaTester()
{
   mOutRootFile->Close();
   delete mOutRootFile;
}


Int_t StiLajaTester::Make()
{
   Info("Make", "XXX kalman tracks");

   StiMaker* stiMaker = (StiMaker*) GetMaker("Sti");

   Info("Make", "Found stiMaker: %p", stiMaker);

   //StiToolkit *_toolkit = StiToolkit::instance();
   StiToolkit *_toolkit = stiMaker->getToolkit();
   StiTrackContainer *_trackContainer = _toolkit->getTrackContainer();

   for (vector<StiTrack*>::iterator trackIt = _trackContainer->begin(); trackIt!=_trackContainer->end(); ++trackIt) 
   {
      StiKalmanTrack* kalmanTrack = static_cast<StiKalmanTrack*>(*trackIt);

      if (!kalmanTrack) {
         Info("Make", "Invalid kalman kTrack");
         continue;
      }

      Info("Make", "P: %f", kalmanTrack->getP());

      kalmanTrack->print("XYZEPTC");

      mOutRootFile->FillHists();
   }

   return kStOK;
}


Int_t StiLajaTester::Finish()
{
   mOutRootFile->Close();
   return kStOK;
}
