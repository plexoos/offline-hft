#include <fstream>
#include <string>

#include "TChain.h"
#include "TError.h"
#include "TRandom.h"
#include "TROOT.h"

#include "StHftPool/EventT/EventT.h"
#include "StarGenerator/STEP/AgUStep.h"
#include "StHftPool/EventT/PrgOptionProcessor.h"
#include "StHftPool/EventT/StiScanRootFile.h"

typedef Event EventG;


// These globals are not used but required since we link against TGiant3.o
int    Margc=0;
char** Margv=NULL;


void loop_hftree(PrgOptionProcessor &poProc);


int main(int argc, char **argv)
{
   const std::string hftTreeName = "t";
   const std::string geantStepTreeName = "stepping";

   PrgOptionProcessor poProc(argc, argv, hftTreeName, geantStepTreeName);

   loop_hftree(poProc);

   return EXIT_SUCCESS;
}


void loop_hftree(PrgOptionProcessor &poProc)
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

   if (poProc.SaveGraphics()) {
      outRootFile.SaveAllAs();
   }

   outRootFile.Close();
}
