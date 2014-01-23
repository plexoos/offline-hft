/***************************************************************************
*
* $Id$
*
* Author: Yaping Wang, August 2012
****************************************************************************
* Description: 
* Fast simulation maker for IST.
****************************************************************************
*
* $Log$
* Revision 1.1  2014/01/23 20:11:30  ypwang
* adding scripts
*
*
****************************************************************************
* StIstFastSimMaker.h,v 1.0
* Revision 1.0 2013/11/04 16:25:30 Yaping
* Initial version
****************************************************************************/

#ifndef STAR_StIstFastSimMaker
#define STAR_StIstFastSimMaker
#ifndef StMaker_H
#include "StMaker.h"
#endif
#include "StThreeVectorF.hh"
#include "StThreeVectorD.hh"
#include <vector>

class StEvent;
class StMcEvent;
class StRandom;
class StMcIstHitCollection;
class StIstHitCollection;
class St_g2t_ist_hit;

class TString;

class StIstFastSimMaker : public StMaker {
 public:

  StIstFastSimMaker(const char *name="istFastSim") :StMaker(name){}
  virtual       ~StIstFastSimMaker();

  virtual Int_t  Make();
  virtual Int_t Finish();
  virtual int Init();
  virtual int InitRun( int);
  
  //Routine to smear hit by resolution with gaussian, mean zero and width res
  double distortHit(double x, double res, double detLength);

  virtual const char *GetCVS() const
  {
    static const char cvs[]="Tag $Name$ $Id$ built "__DATE__" "__TIME__ ; 
    return cvs;
  }

 protected:
  StRandom* myRandom;

  Double_t resXIst1;
  Double_t resZIst1;
  Int_t mSmear; //to turn smearing on and off

  private:

  ClassDef(StIstFastSimMaker,1)   //StAF chain virtual base class for Makers
};
#endif



