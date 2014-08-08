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
   PrgOptionProcessor(int argc, char **argv, const std::string& hftTreeName="t", const std::string& geantStepTreeName="stepping");

   std::string  GetHftreeFile() const { return fHftreeFile; }
   bool  DoGeantStepTree() const { return fDoGeantStepTree; }
   const std::set<std::string>&  GetVolumeList() const { return fVolumeList; }
   unsigned int GetMaxEventsUser() const;
   float GetSparsity() const { return fSparsity; }
   bool  SaveGraphics() const { return fSaveGraphics; }
   TChain* GetHftChain() { return fHftChain; }
   TChain* GetGeantStepChain() { return fGeantStepChain; }
   std::string GetStyleMacro() const { return fEnvVars.find("OFFLINE_HFT_DIR")->second + "/offline/hft/tests/style_hists.C"; }

   void ProcessOptions(int argc, char **argv);
   bool MatchedVolName(std::string & volName) const;

protected:

   po::options_description fOptions;
   po::variables_map       fOptionsValues;
   /// Full path to either a root file with hft event tree or a text file with a list of such root files
   std::string             fHftreeFile;
   bool                    fDoGeantStepTree; ///< A flag to process geant tree if set
   std::string             fVolumeListFile;  ///< Full path to a text file with Sti/TGeo volume names
   std::string             fVolumePattern;   ///< Regex pattern provided by the user in the command line
   std::set<std::string>   fVolumeList;      ///< A list of volume names to consider
   unsigned int            fMaxEventsUser;
   float                   fSparsity;        ///< Approximate fraction of events to read and process
   bool                    fSaveGraphics;    ///< A flag to create images when set

private:
   
   void InitOptions();
   void InitEnvVars();
   void BuildInputChains(std::string hftTreeName, std::string geantStepTreeName);
   void AddToInputChains(std::string hftTreeRootFileName);

   std::map<std::string, std::string> fEnvVars;
   TChain *fHftChain;
   TChain *fGeantStepChain;
};

#endif
