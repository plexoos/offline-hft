#include <iostream>
#include <fstream>
#include <algorithm>
#include <boost/regex.hpp>

#include "PrgOptionProcessor.h"


PrgOptionProcessor::PrgOptionProcessor() : TObject(),
   fOptions("Allowed options"), fOptionsValues(), fHftreeFile(), fVolumeListFile(),
   fVolumeList(), fMaxEventsUser(0)
{
   InitOptions();
}


PrgOptionProcessor::PrgOptionProcessor(int argc, char **argv) : TObject(),
   fOptions("Allowed options"), fOptionsValues(), fHftreeFile(), fVolumeListFile(),
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
      ("hftree-file,f",       po::value<std::string>(&fHftreeFile), "Full name including path of input file with ROOT tree")
      ("volumelist,l",        po::value<std::string>(&fVolumeListFile), "Full path to file with Sti/TGeo volume names")
      ("max-events,n",        po::value<unsigned int>(&fMaxEventsUser)->default_value(0), "Maximum number of events to process")
   ;

   //fVolumeList.insert("^.*IDSM_1/PXMO_1/PXLA_1/.*$");
   //fVolumeList.insert("^.*IDSM_1/PXMO_1/PXLA_7/.*$");
}


std::string PrgOptionProcessor::GetHftreeFile() const { return fHftreeFile; }


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

   if (fOptionsValues.count("volumelist"))
   {
      std::string fileName = boost::any_cast<std::string>(fOptionsValues["volumelist"].value());

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
