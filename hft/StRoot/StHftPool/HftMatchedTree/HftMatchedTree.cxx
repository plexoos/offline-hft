#include <stdlib.h>

#include "StHftPool/HftMatchedTree/HftMatchedTree.h"

#include "TROOT.h"
#include "TSystem.h"
#include "TFile.h"
#include "TTree.h"
#include "TBranch.h"
#include "StEvent.h"
#include "StBFChain.h"

#include "Sti/StiToolkit.h"
#include "Sti/StiTrackContainer.h"
#include "Sti/StiKalmanTrack.h"
#include "StiMaker/StiMaker.h"

#include "StHftPool/EventT/EventT.h"
#include "StIstDbMaker/StIstDbMaker.h"
#include "StPxlDbMaker/StPxlDb.h"


HftMatchedTree::HftMatchedTree(const Char_t *name) : StMaker(name), fFile(0), fTree(0), fEvent(0), fMinNoHits(0),
   fpCut(0), fPxlDb(0)
{
}


HftMatchedTree::~HftMatchedTree()
{
   if (fFile) fFile->Close();

   delete fFile; fFile = 0;
   delete fTree; fTree = 0;
   delete fEvent; fEvent = 0;
}


Int_t HftMatchedTree::Init()
{
   SetTree();
   return kStOK;
}


Int_t HftMatchedTree::InitRun(Int_t runnumber)
{
   TObjectSet *pxlDbDataSet = (TObjectSet *)GetDataSet("pxl_db");

   if (pxlDbDataSet) {
      fPxlDb = (StPxlDb*) pxlDbDataSet->GetObject();
      assert(fPxlDb);
   }
   else {
      LOG_ERROR << "InitRun : not pxlDb" << endm;
      return kStErr;
   }

   // Dump Geometry matrix into root file
   StIstDbMaker *istDbMaker = (StIstDbMaker *)GetMaker("istDb");
   THashList *istRot = istDbMaker->GetRotations();

   TFile *file = new TFile("GeometryTables.root", "Recreate");

   for (int i = 0; i < 24; i++) {
      for (int j = 0; j < 6; j++) {
         int id = 1000 + i * 6 + j + 1;
         TGeoHMatrix *comb = (TGeoHMatrix *)istRot->FindObject(Form("R%04i", id));
         comb->Write();
      }
   }

   for (int i = 0; i < 10; i++) {
      for (int j = 0; j < 4; j++) {
         for (int k = 0; k < 10; k++) {
            int id = i * 40 + j * 10 + k + 1;
            TGeoHMatrix *comb = (TGeoHMatrix *) fPxlDb->geoHMatrixSensorOnGlobal(i + 1, j + 1, k + 1);
            comb->SetName(Form("R%03i", id));
            comb->Write();
         }
      }
   }

   file->Close();

   return kStOK;
}


Int_t HftMatchedTree::Finish()
{
   fFile = fTree->GetCurrentFile(); //just in case we switched to a new file
   fFile->Write();
   fTree->Print();
   return kStOK;
}


void HftMatchedTree::SetTree()
{
   StBFChain *bfChain = (StBFChain *) StMaker::GetChain();

   if (!bfChain) return;

   // Authorize Trees up to 2 Terabytes (if the system can do it)
   TTree::SetMaxTreeSize(1000 * Long64_t(2000000000));
   TString fileName( gSystem->BaseName(bfChain->GetFileIn().Data()) );

   fileName.ReplaceAll("st_physics", "");
   fileName.ReplaceAll(".event", "");
   fileName.ReplaceAll(".daq", "");
   fileName.ReplaceAll(".fz", "");

   if (fMinNoHits > 0) fileName += Form("_%i_%f2.1_", fMinNoHits, fpCut);

   fileName += ".evdisp.root";

   fFile = new TFile(fileName, "RECREATE", "TTree with HFT hits and tracks");
   fFile->SetCompressionLevel(1); // Set file compression level

   // Create a ROOT Tree and one superbranch
   fTree = new TTree("T", "TTree with HFT hits and tracks");
   fTree->SetAutoSave(1000000000);  // autosave when 1 Gbyte written

   Int_t bufsize = 64000;
   Int_t split   = 0;      // 0 means do not split event

   fEvent = new EventT();
   TBranch *branch = fTree->Branch("e", "EventT", &fEvent, bufsize, split);
   branch->SetAutoDelete(kFALSE);
}


Int_t HftMatchedTree::Make()
{
   StEvent *stEvent = (StEvent *) GetInputDS("StEvent");

   if (stEvent && !fEvent->Build(stEvent, fMinNoHits, fpCut, this, fPxlDb)) fTree->Fill(); //fill the tree

   return kStOK;
}
