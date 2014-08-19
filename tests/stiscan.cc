#include <fstream>
#include <string>
#include <unordered_map>

#include "TChain.h"
#include "TError.h"
#include "TGeoNavigator.h"
#include "TRandom.h"
#include "TROOT.h"

#include "StHftPool/EventT/EventT.h"
#include "StarGenerator/STEP/AgUStep.h"
#include "StHftPool/EventT/StiScanPrgOptions.h"
#include "StHftPool/EventT/StiScanRootFile.h"

typedef Event EventG;
typedef std::unordered_map<size_t, std::string> Hash2StringMap;


// These globals are not used but required since we link against TGiant3.o
int    Margc=0;
char** Margv=NULL;


void loop_hftree(StiScanPrgOptions &poProc);
void create_volume_hash_map(TGeoNavigator &geoNav, Hash2StringMap &hash2PathMap);


int main(int argc, char **argv)
{
   const std::string hftTreeName = "t";
   const std::string geantStepTreeName = "stepping";

   StiScanPrgOptions poProc(argc, argv, hftTreeName, geantStepTreeName);
   poProc.ProcessOptions();

   loop_hftree(poProc);

   return EXIT_SUCCESS;
}


void loop_hftree(StiScanPrgOptions &poProc)
{
   TChain *hftChain       = poProc.GetHftChain();
   TChain *geantStepChain = poProc.GetGeantStepChain();

   // Create a new output file
   std::string outFileName = poProc.GetHftreeFile();

   std::string postfix("hftree.root");
   std::size_t postfix_pos = outFileName.find(postfix);

   if (postfix_pos != std::string::npos)
   {
      outFileName.replace( postfix_pos, postfix.length(), "stiscan.root");
   } else {
      outFileName += "_stiscan.root";
   }

   StiScanRootFile outRootFile( outFileName.c_str(), "recreate");

   int nTreeEvents = hftChain->GetEntries();
   int nProcEvents = 0;

   Info("loop_hftree", "Found tree/chain with nTreeEvents: %d", nTreeEvents);

   EventT *eventT = new EventT();
   hftChain->SetBranchAddress("e.", &eventT);
   hftChain->SetBranchStatus("e.*", false);
   hftChain->SetBranchStatus("e.fTStiKalmanTracks*", true);

   EventG *eventG = new EventG();

   if (poProc.DoGeantStepTree())
      geantStepChain->SetBranchAddress("Event", &eventG);

   TRandom myRandom;

   for (int iEvent = 1; iEvent <= nTreeEvents; iEvent++, nProcEvents++)
   {
      if ( nTreeEvents >= 10 && iEvent % int(nTreeEvents*0.1) == 0 )
         Info("loop_hftree", "Analyzing event %d", iEvent);

      if (myRandom.Rndm() > poProc.GetSparsity()) continue;

      hftChain->GetEntry(iEvent-1);

      outRootFile.FillHists(*eventT, &poProc.GetVolumeList());

      if (poProc.DoGeantStepTree()) {
         geantStepChain->GetEntry(iEvent-1);
         outRootFile.FillHists(*eventG);
      }
   }

   delete eventT;

   outRootFile.FillDerivedHists();

   if (poProc.SaveGraphics()) {
      gROOT->Macro(poProc.GetStyleMacro().c_str());
      outRootFile.SaveAllAs("./images");
   }

   outRootFile.Write();
   outRootFile.Close();
}


void create_volume_hash_map(TGeoNavigator &geoNav, Hash2StringMap &hash2PathMap)
{
   TGeoNode* currNode = geoNav.GetCurrentNode();

   if (!currNode) {
      Warning("create_volume_hash_map", "Invalid TGeoNode provided as input. Skipping...");
      return;
   }

   // Keep track of current level with respect to the original node
   static int level = 0;

   std::string currentPath( geoNav.GetPath() );

   int nDaughters = currNode->GetVolume()->GetNdaughters();

   if (nDaughters) {

      TGeoNode* motherNode = currNode;
      for (int iDaughter = 0; iDaughter < nDaughters; iDaughter++)
      {
         TGeoNode *daughter = motherNode->GetVolume()->GetNode(iDaughter);

         geoNav.CdDown(daughter);
         level++;

         create_volume_hash_map(geoNav, hash2PathMap);
      }

   } else { // We are here if this node is a leaf, i.e. no daughters

      // Add this volume to the hash map
      std::string hashedPath(currentPath);

      // Remove TpcRefSys_1/ substring as it not relevant for geometry trees used in simulation 
      size_t first_pos = hashedPath.find("TpcRefSys_1/");
      hashedPath.replace(first_pos, std::string("TpcRefSys_1/").length(), "");

      std::hash<std::string> hash_fn;
      std::size_t hash_value = hash_fn(hashedPath);

      std::pair<size_t, std::string> hash2Path(hash_value, hashedPath);

      hash2PathMap.insert(hash2Path);

      geoNav.cd(currentPath.c_str());
   }

   if (level > 0) {
      geoNav.CdUp();
      level--;
   }
}
