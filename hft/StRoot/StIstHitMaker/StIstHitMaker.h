/***************************************************************************
*
* $Id$
*
* Author: Yaping Wang, March 2013
****************************************************************************
* Description: 
* Calculates hit global position, and writes IST hits to StIstHitCollection.
****************************************************************************
*
* $Log$
* Revision 1.5  2014/06/27 21:31:40  ypwang
* remove data member istHitCollection and related Clear() function
*
* Revision 1.4  2014/02/08 03:34:16  ypwang
* updating scripts
*
*
****************************************************************************
* StIstHitMaker.h,v 1.0
* Revision 1.0 2013/11/04 16:05:30 Yaping
* Initial version
****************************************************************************/

#ifndef StIstHitMaker_hh
#define StIstHitMaker_hh

#include "StMaker.h"

class StIstDbMaker;
class THashList;
class St_istControl;

class StIstHitMaker : public StMaker
{
 public:
  StIstHitMaker( const char* name="ist_hit" );
  Int_t Init();
  Int_t InitRun(Int_t runnumber);
  Int_t Make();

  virtual const char *GetCVS() const
  {static const char cvs[]="Tag $Name$ $Id$ built "__DATE__" "__TIME__ ; return cvs;}

 protected:
  THashList *listGeoMSensorOnGlobal;
  StIstDbMaker *mIstDbMaker;

  UShort_t mMinNumOfRawHits, mMaxNumOfRawHits;

 private:
  ClassDef(StIstHitMaker,1);
};
#endif
