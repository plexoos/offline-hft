#ifndef StiLajaTester_h
#define StiLajaTester_h

#include "StMaker.h"
#include "StHftPool/StiLajaTester/StiLajaRootFile.h"


class StiLajaTester : public StMaker
{
public:

   StiLajaTester(const Char_t *name="StiLajaTester");
   ~StiLajaTester();

   virtual Int_t Make();
   virtual Int_t Finish();

private:

   StiLajaRootFile *mOutRootFile;

   ClassDef(StiLajaTester, 1)
};

#endif
