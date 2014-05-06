#include <stdlib.h>

#include "HftMatchedTree.h"

#include "TROOT.h"
#include "TSystem.h"
#include "TFile.h"
#include "TTree.h"
#include "TBranch.h"
#include "StEvent.h"
#include "StBFChain.h"

#include "StIstDbMaker/StIstDbMaker.h"
#include "StPxlDbMaker/StPxlDb.h"
#include "StPxlDbMaker/StPxlDbMaker.h"
#include "StHftPool/EventT/EventT.h"


HftMatchedTree::HftMatchedTree(const Char_t *name) : StMaker(name), fFile(0), fTree(0), fEvent(0)
{
   SetMinNoHits();
   SetpCut();
   SetOut();
   mPxlDb = 0;
}


Int_t HftMatchedTree::Init()
{
   SetTree();
   return kStOK;
}


Int_t HftMatchedTree::InitRun(Int_t runnumber)
{

   TObjectSet *pxlDbDataSet = (TObjectSet *)GetDataSet("pxlDb");

   if (pxlDbDataSet) {
      mPxlDb = (StPxlDb *)pxlDbDataSet->GetObject();
      if(!mPxlDb) {
         LOG_ERROR << "InitRun : Found the dataset, but no pxlDb content" << endm;

         StPxlDbMaker *pxlDbMaker = (StPxlDbMaker *)GetMaker("pxl_db");
         mPxlDb = (StPxlDb *)pxlDbMaker->GetPxlDb();
         if(!mPxlDb) {
            LOG_ERROR << "InitRun : through dbMaker direct function, but no pxlDb content" << endm;
         }
      }
   }
   else {
      LOG_ERROR << "InitRun : not pxlDb" << endm;
      return kStErr;
   }

   // Dump Geometry matrix into root file
   StIstDbMaker *istDbMaker = (StIstDbMaker *)GetMaker("istDb");
   THashList *istRot = istDbMaker->GetRotations();

   TFile *fout = new TFile("GeometryTables.root", "Recreate");

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
            TGeoHMatrix *comb = (TGeoHMatrix *)mPxlDb->geoHMatrixSensorOnGlobal(i + 1, j + 1, k + 1);
            comb->SetName(Form("R%03i", id));
            comb->Write();
         }
      }
   }

   fout->Close();

   return kStOK;
}

Int_t HftMatchedTree::Finish()
{
   fFile = fTree->GetCurrentFile(); //just in case we switched to a new file
   fFile->Write();
   fTree->Print();
   return kStOK;
}

//________________________________________________________________________________
void HftMatchedTree::SetTree()
{
   StBFChain *chain = (StBFChain *) StMaker::GetChain();

   if (! chain) return;

   // root.exe
   Int_t split  = 99;       // by default, split Event in sub branches
   Int_t comp   = 1;       // by default file is compressed
   Int_t branchStyle = 1; //new style by default

   if (split < 0) {branchStyle = 0; split = -1 - split;}

   Int_t bufsize;
   //Authorize Trees up to 2 Terabytes (if the system can do it)
   TTree::SetMaxTreeSize(1000 * Long64_t(2000000000));
   TString FName(fOut);

   if (fMinNoHits > 0) FName += Form("_%i_%f2.1_", fMinNoHits, fpCut);

   FName += gSystem->BaseName(chain->GetFileIn().Data());
   FName.ReplaceAll("st_physics", "");
   FName.ReplaceAll(".event", "");
   FName.ReplaceAll(".daq", ".root");
   FName.ReplaceAll(".fz", ".root");
   fFile = new TFile(FName, "RECREATE", "TTree with HFT hits and tracks");
   fFile->SetCompressionLevel(comp);
   // Create a ROOT Tree and one superbranch
   fTree = new TTree("T", "TTree with HFT hits and tracks");
   fTree->SetAutoSave(1000000000);  // autosave when 1 Gbyte written
   bufsize = 64000;

   if (split)  bufsize /= 4;

   fEvent = new EventT();
   TTree::SetBranchStyle(branchStyle);
   TBranch *branch = fTree->Branch("EventT", &fEvent, bufsize, split);
   branch->SetAutoDelete(kFALSE);
}
//________________________________________________________________________________
Int_t HftMatchedTree::Make()
{
   StEvent *pEvent = (StEvent *) GetInputDS("StEvent");

   if (pEvent && !fEvent->Build(pEvent, fMinNoHits, fpCut, this, mPxlDb)) fTree->Fill(); //fill the tree

   return kStOK;
}
