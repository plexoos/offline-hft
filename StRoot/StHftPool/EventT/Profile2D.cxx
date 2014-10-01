#include "StRoot/StHftPool/EventT/Profile2D.h"

#include <exception>
#include "TH1.h"

class DifferentNumberOfBins: public std::exception {};
class DifferentAxisLimits: public std::exception {};
class DifferentBinLimits: public std::exception {};
class DifferentLabels: public std::exception {};


ClassImp(Profile2D)


Profile2D::Profile2D() : TProfile2D()
{
}


Profile2D::Profile2D(std::string name, std::string title, Int_t nbinsx, Double_t xlow, Double_t xup,
   Int_t nbinsy, Double_t ylow, Double_t yup, std::string options) :
   TProfile2D(name.c_str(), title.c_str(), nbinsx, xlow, xup, nbinsy, ylow, yup, options.c_str())
{
   SetOption(options.c_str());
}


void Profile2D::ResetBinEntries(double val)
{
   for (int iBin=0; iBin<fNcells; ++iBin)
   {
      double bc = GetBinContent(iBin);
      double be = GetBinError(iBin);

      // Skip empty bins
      if (!bc && !be) continue;

      fBinEntries.fArray[iBin] = val;
   }
}


Bool_t Profile2D::Multiply(const Profile2D &prof2)
{
   try {
      CheckConsistency((TH1*) this, (TH1*) &prof2);
   } catch(DifferentNumberOfBins&) {
      Error("Multiply","Attempt to multiply histograms with different number of bins");
      return kFALSE;
   } catch(DifferentAxisLimits&) {
      Warning("Multiply","Attempt to multiply histograms with different axis limits");
   } catch(DifferentBinLimits&) {
      Warning("Multiply","Attempt to multiply histograms with different bin limits");
   } catch(DifferentLabels&) {
      Warning("Multiply","Attempt to multiply histograms with different labels");
   }

   // Loop on bins (including underflows/overflows)
   for (Int_t i = 0; i < fNcells; ++i) {
      Double_t c1 = prof2.GetBinContent(i);
      fArray[i] *= c1;
   }
   return kTRUE;
}
