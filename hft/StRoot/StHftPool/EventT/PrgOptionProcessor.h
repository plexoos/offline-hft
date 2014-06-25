#ifndef PrgOptionProcessor_h
#define PrgOptionProcessor_h

#include <set>
#include <string>

#include <boost/program_options/options_description.hpp>
#include <boost/program_options/variables_map.hpp>
#include <boost/program_options/parsers.hpp>

#include "TObject.h"
#include "TChain.h"


namespace {
   namespace po = boost::program_options;
}


/**
 * Processes and controls user options provided in the command line.
 */
class PrgOptionProcessor : public TObject
{
public:

   PrgOptionProcessor();
   PrgOptionProcessor(int argc, char **argv);

   std::string  GetHftreeFile() const { return fHftreeFile; }
   const std::set<std::string>&  GetVolumeList() const { return fVolumeList; }
   unsigned int GetMaxEventsUser() const;
   float GetSparsity() const { return fSparsity; }
   bool  SaveGraphics() const { return fSaveGraphics; }

   void ProcessOptions(int argc, char **argv);
   bool MatchedVolName(std::string & volName) const;
   TChain* BuildHftreeChain(std::string name);

protected:

   po::options_description fOptions;
   po::variables_map       fOptionsValues;
   std::string             fHftreeFile;
   std::string             fVolumeListFile;  ///< Full path to a text file with Sti/TGeo volume names
   std::string             fVolumePattern;   ///< Regex pattern provided by the user in the command line
   std::set<std::string>   fVolumeList;      ///< A list of volume names to consider
   unsigned int            fMaxEventsUser;
   float                   fSparsity;        ///< Approximate fraction of events to read and process
   bool                    fSaveGraphics;    ///< A flag to create images when set

private:
   
   void InitOptions();
};

#endif
