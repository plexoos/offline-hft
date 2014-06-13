#include <iostream>
#include <fstream>
#include <algorithm>
#include <boost/regex.hpp>

#include "TFile.h"

#include "PrgOptionProcessor.h"


PrgOptionProcessor::PrgOptionProcessor() : TObject(),
   fOptions("Program options", 120), fOptionsValues(), fHftreeFile(), fVolumeListFile(),
   fVolumeList(), fMaxEventsUser(0)
{
   InitOptions();
}


PrgOptionProcessor::PrgOptionProcessor(int argc, char **argv) : TObject(),
   fOptions("Program options", 120), fOptionsValues(), fHftreeFile(), fVolumeListFile(),
   fVolumeList(), fMaxEventsUser(0)
{
   InitOptions();
   ProcessOptions(argc, argv);
}


void PrgOptionProcessor::InitOptions()
{
   // Declare supported options
   fOptions.add_options()
      ("help,h",              "Print help message")
      ("hftree-file,f",       po::value<std::string>(&fHftreeFile), "Full path to a ROOT file containing a 'hftree' TTree " \
       "OR a text file with a list of such ROOT files")
      ("volume-pattern-flist,p",   po::value<std::string>(&fVolumeListFile), "Full path to a text file with Sti/TGeo volume names")
      ("max-events,n",        po::value<unsigned int>(&fMaxEventsUser)->default_value(0), "Maximum number of events to process")
   ;
}


/**
 * Takes the standard command line arguments and parses them with the boost
 * program_options utility. Additional checks are implemented to verify the
 * validity of the supplied arguments.
 */
void PrgOptionProcessor::ProcessOptions(int argc, char **argv)
{
   using namespace std;

   po::store(po::parse_command_line(argc, argv, fOptions), fOptionsValues);
   po::notify(fOptionsValues);

   if (fOptionsValues.count("help"))
   {
      std::cout << fOptions << std::endl;
      exit(EXIT_FAILURE);
   }

   Info("ProcessOptions", "User provided options:");


   if (fOptionsValues.count("hftree-file"))
   {
      std::string hftreeFile = boost::any_cast<std::string>(fOptionsValues["hftree-file"].value());

      std::cout << "fHftreeFile: " << hftreeFile << std::endl;
      std::ifstream tmpFileCheck(hftreeFile.c_str());
      if (!tmpFileCheck.good()) {
         Error("process_options", "File \"%s\" does not exist", hftreeFile.c_str());
         tmpFileCheck.close();
         exit(EXIT_FAILURE);
      }
   } else {
      Error("process_options", "Input file not set");
      exit(EXIT_FAILURE);
   }

   if (fOptionsValues.count("volume-pattern-flist"))
   {
      std::string fileName = boost::any_cast<std::string>(fOptionsValues["volume-pattern-flist"].value());

      std::cout << "fVolumeListFile: " << fileName << std::endl;
      std::ifstream volListFile(fileName.c_str());

      if (!volListFile.good()) {
         Error("process_options", "File \"%s\" does not exist", fileName.c_str());
         volListFile.close();
         exit(EXIT_FAILURE);
      }

      std::string pattern;

      while ( volListFile.good() )
      {
         volListFile >> pattern;
         if (volListFile.eof()) break;

         fVolumeList.insert(pattern);
      }

      copy(fVolumeList.begin(), fVolumeList.end(), ostream_iterator<string>(cout, "\n"));

   } else // Default list of patterns
   {
      fVolumeList.insert("^.*IDSM_1/IBMO_1/IBAM_[\\d]+/ILSB.*$");
      fVolumeList.insert("^.*IDSM_1/PXMO_1/PXLA_[\\d]+/LADR_\\d/PXSI_[\\d]+/PLAC.*$");
   }


   if (fOptionsValues.count("max-events"))
   {
      std::cout << "max-events: " << fMaxEventsUser << std::endl;
   }
}


bool PrgOptionProcessor::MatchedVolName(std::string & volName) const
{
   if (volName.empty() || fVolumeList.empty())
      return false;

   std::set<std::string>::const_iterator iPattern = fVolumeList.begin();

   for( ; iPattern != fVolumeList.end(); ++iPattern )
   {
      boost::regex r(*iPattern);

      if ( boost::regex_match(volName, r) )
         return true;
   }

   return false;
}


TChain* PrgOptionProcessor::BuildHftreeChain(std::string name)
{
   TChain *chain = new TChain(name.c_str(), "READ");

   TFile file( fHftreeFile.c_str() );

   if ( file.IsZombie() )
   {
      Warning("BuildHftreeChain", "Not a root file: %s", fHftreeFile.c_str());

      std::ifstream hftreeListFile(fHftreeFile.c_str());
      std::string hftreeFile;

      while ( hftreeListFile.good() )
      {
         hftreeListFile >> hftreeFile;
         if (hftreeListFile.eof()) break;

         chain->AddFile( hftreeFile.c_str() );
      }
   } else
   {
      Info("BuildHftreeChain", "Good root file: %s", fHftreeFile.c_str());
      chain->AddFile( fHftreeFile.c_str() );
   }

   return chain;
}
