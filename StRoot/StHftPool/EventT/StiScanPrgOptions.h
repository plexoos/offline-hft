#ifndef StiScanPrgOptions_h
#define StiScanPrgOptions_h

#include <set>
#include <string>

#include <boost/program_options/options_description.hpp>
#include <boost/program_options/variables_map.hpp>
#include <boost/program_options/parsers.hpp>

#include "TObject.h"
#include "TChain.h"

#include "StHftPool/EventT/PrgOptionProcessor.h"


namespace {
   namespace po = boost::program_options;
}


/**
 * Processes and controls user options provided in the command line.
 */
class StiScanPrgOptions : public PrgOptionProcessor
{
public:

   StiScanPrgOptions();
   StiScanPrgOptions(int argc, char **argv, const std::string& hftTreeName="t", const std::string& geantStepTreeName="stepping");
   ~StiScanPrgOptions();

   bool  DoGeantStepTree() const { return fDoGeantStepTree; }
   TChain* GetGeantStepChain() { return fGeantStepChain; }

protected:

   virtual void VerifyOptions();
   virtual void AddToInputChains(std::string hftTreeRootFileName);

   bool  fDoGeantStepTree; ///< A flag to process geant tree if set
   TChain *fGeantStepChain;

private:

   void InitOptions();

};

#endif
