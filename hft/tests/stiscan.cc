
#include <fstream>
#include <string>

#include "TChain.h"
#include "TError.h"

#include "StHftPool/EventT/EventT.h"
#include "StHftPool/HftMatchedTree/PrgOptionProcessor.h"
#include "StHftPool/StiLajaTester/StiLajaRootFile.h"


void loop_hftree(PrgOptionProcessor &poProc);


int main(int argc, char **argv)
{
   PrgOptionProcessor poProc(argc, argv);

   loop_hftree(poProc);

   return EXIT_SUCCESS;
}


void loop_hftree(PrgOptionProcessor &poProc)
{
   TChain *myTreeFileChain = poProc.BuildHftreeChain("t");

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

   StiLajaRootFile outRootFile( outFileName.c_str(), "recreate");

   int nTreeEvents = myTreeFileChain->GetEntries();
   int nProcEvents = 0;

   Info("loop_hftree", "Found tree/chain with nTreeEvents: %d", nTreeEvents);

   EventT *eventT = new EventT();
   myTreeFileChain->SetBranchAddress("e", &eventT);

   for (int iEvent = 1; iEvent <= nTreeEvents; iEvent++, nProcEvents++)
   {
      if ( nTreeEvents >= 10 && iEvent % int(nTreeEvents*0.1) == 0 )
         Info("loop_hftree", "Analyzing event %d", iEvent);

      myTreeFileChain->GetEntry(iEvent-1);

      outRootFile.FillHists(*eventT, &poProc.GetVolumeList());
   }

   delete eventT;
   outRootFile.Close();
}
