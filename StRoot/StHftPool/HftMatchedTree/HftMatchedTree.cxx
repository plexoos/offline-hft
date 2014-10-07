#include <stdlib.h>

#include "StHftPool/HftMatchedTree/HftMatchedTree.h"

#include "TSystem.h"
#include "TFile.h"
#include "TTree.h"
#include "TBranch.h"

#include "StEvent/StEvent.h"
#include "StBFChain/StBFChain.h"
#include "StHftPool/EventT/EventT.h"
#include "StIstDbMaker/StIstDb.h"
#include "StPxlDbMaker/StPxlDb.h"

ClassImp(HftMatchedTree)


HftMatchedTree::HftMatchedTree(const Char_t *name) : StMaker(name),
   fTree(new TTree("t", "TTree with HFT hits and tracks")),
   fEvent(0),
   fFile(0),
   fMinNoHits(0),
   fpCut(0)
{
   fTree->SetAutoSave(1000000000);  // autosave when 1 Gbyte written
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
   if (Debug() >= 2)
      SetDebug(StMessMgr::kDebug);

   StBFChain *bfChain = (StBFChain *) StMaker::GetChain();

   if (!bfChain) return kStErr;

   // Authorize Trees up to 2 Terabytes (if the system can do it)
   TTree::SetMaxTreeSize(1000 * Long64_t(2000000000));

   TString fileName( gSystem->BaseName(bfChain->GetFileIn().Data()) );

   fileName.ReplaceAll("st_physics", "");
   fileName.ReplaceAll(".event", "");
   fileName.ReplaceAll(".daq", "");
   fileName.ReplaceAll(".fz", "");

   if (fMinNoHits > 0) fileName += Form("_%i_%f2.1_", fMinNoHits, fpCut);

   fileName += ".hftree.root";

   fFile = new TFile(fileName, "RECREATE", "TTree with HFT hits and tracks");
   fFile->SetCompressionLevel(1); // Set file compression level

   SetTree();

   return kStOK;
}


Int_t HftMatchedTree::InitRun(Int_t runnumber)
{
   TObjectSet *pxlDbDataSet = (TObjectSet *)GetDataSet("pxl_db");
   StPxlDb* stPxlDb = nullptr;

   if (pxlDbDataSet) {
      stPxlDb = (StPxlDb*) pxlDbDataSet->GetObject();
      assert(stPxlDb);
   }
   else {
      LOG_ERROR << "InitRun : Dataset \"pxl_db\" not found" << endm;
      return kStErr;
   }

   TObjectSet *istDbDataSet = (TObjectSet*) GetDataSet("ist_db");
   StIstDb* stIstDb = nullptr;

   if (istDbDataSet) {
      stIstDb = (StIstDb*) istDbDataSet->GetObject();
      assert(stIstDb);
   }
   else {
      LOG_ERROR << "InitRun : Dataset \"ist_db\" not found" << endm;
      return kStErr;
   }

   // Update pointers to the detector spatial transformations at new run transition
   fEvent->SetDbDatasets(stPxlDb, stIstDb);

   // Dump geometry matrix into root file
   TFile *file = new TFile("GeometryTables.root", "recreate");

   for (int i = 0; i < 24; i++) {
      for (int j = 0; j < 6; j++) {
         int id = 1000 + i * 6 + j + 1;
         TGeoHMatrix *comb = (TGeoHMatrix *) stIstDb->getRotations()->FindObject(Form("R%04i", id));
         comb->Write();
      }
   }

   for (int i = 0; i < 10; i++) {
      for (int j = 0; j < 4; j++) {
         for (int k = 0; k < 10; k++) {
            int id = i * 40 + j * 10 + k + 1;
            TGeoHMatrix *comb = (TGeoHMatrix *) stPxlDb->geoHMatrixSensorOnGlobal(i + 1, j + 1, k + 1);
            comb->SetName(Form("R%03i", id));
            comb->Write();
         }
      }
   }

   file->Close();
   delete file;

   return kStOK;
}


Int_t HftMatchedTree::Finish()
{
   fFile = fTree->GetCurrentFile(); //just in case we switched to a new file
   fFile->Write();

   if (GetDebug() >= 1)
      fTree->Print();

   return kStOK;
}


void HftMatchedTree::SetTree()
{

   Int_t bufsize = 64000;
   Int_t split   = 99;      // 0 means do not split event

   fEvent = new EventT();
   TBranch *branch = fTree->Branch("e.", "EventT", &fEvent, bufsize, split);
   branch->SetAutoDelete(kFALSE);
}


Int_t HftMatchedTree::Make()
{
   // Fill the rest of event with information from StEvent
   StEvent *stEvent = (StEvent*) GetInputDS("StEvent");

   LOG_DEBUG << "stEvent id: " << stEvent->id() << endm;

   if (stEvent && fEvent->Build(stEvent, fMinNoHits, fpCut) == kStOk) {
      fTree->Fill();
   }

   return kStOK;
}
