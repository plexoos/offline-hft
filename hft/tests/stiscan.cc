
#include <fstream>
#include <string>
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/variables_map.hpp>
#include <boost/program_options/parsers.hpp>

#include "TChain.h"
#include "TError.h"

#include "StHftPool/EventT/EventT.h"
#include "StHftPool/StiLajaTester/StiLajaRootFile.h"


namespace po = boost::program_options;

void process_options(po::options_description &prgOptions, po::variables_map &prgOptionValues);
void loop_hftree(std::string inpFileName);


int main(int argc, char **argv)
{
   std::string inputRootFile;
   std::string stiGeoPatternFile;

   po::options_description prgOptions;
   po::variables_map       prgOptionValues;

   prgOptions.add_options()
      ("help,h",          "Print help message")
      ("inputfile,f",      po::value<std::string>(&inputRootFile), "Full path to input file with ROOT tree")
      ("stigeofile,s",     po::value<std::string>(&stiGeoPatternFile), "Full path to file with Sti/TGeo volume names")
   ;

   po::store(po::parse_command_line(argc, argv, prgOptions), prgOptionValues);
   po::notify(prgOptionValues);

   process_options(prgOptions, prgOptionValues);

   loop_hftree(inputRootFile);

   return EXIT_SUCCESS;
}


void process_options(po::options_description &prgOptions, po::variables_map &prgOptionValues)
{
   if (prgOptionValues.count("help"))
   {
      std::cout << prgOptions << std::endl;
      exit(EXIT_SUCCESS);
   }

   Info("process_options", "User provided options:");

   if (prgOptionValues.count("inputfile"))
   {
      std::string inputFile = boost::any_cast<std::string>(prgOptionValues["inputfile"].value());

      std::cout << "inputRootFile: " << inputFile << std::endl;
      std::ifstream tmpFileCheck(inputFile.c_str());
      if (!tmpFileCheck.good()) {
         Error("process_options", "File \"%s\" does not exist", inputFile.c_str());
         exit(EXIT_FAILURE);
      }

   } else {
      Error("process_options", "Input file not set");
      exit(EXIT_FAILURE);
   }
}


void loop_hftree(std::string inpFileName)
{
   TChain *myTreeFileChain = new TChain("t");
   myTreeFileChain->AddFile(inpFileName.c_str());

   // Create a new output file
   std::string outFileName = inpFileName;
   std::string postfix("hftree.root");
   outFileName.replace( outFileName.find(postfix), postfix.length(), "laja.root");

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
      eventT->Print();
      outRootFile.FillHists(*eventT);
   }

   delete eventT;
   outRootFile.Close();
}
