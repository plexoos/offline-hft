#ifndef Profile2D_h
#define Profile2D_h

#include <string>

#include "TProfile2D.h"


class Profile2D : public TProfile2D
{
public:

   Profile2D();
   Profile2D(std::string name, std::string title, Int_t nbinsx, Double_t xlow,
             Double_t xup, Int_t nbinsy, Double_t ylow, Double_t yup, std::string options="");

   void ResetBinEntries(double val);
   using TProfile2D::Multiply;
   Bool_t Multiply(const Profile2D &prof2);

   ClassDef(Profile2D, 1)
};

#endif
