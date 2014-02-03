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
* Revision 1.3  2014/02/03 16:12:19  ypwang
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
class StIstHitCollection;

class StIstHitMaker : public StMaker
{
 public:
  StIstHitMaker( const char* name="ist_hit" );
  virtual ~StIstHitMaker();
  virtual Int_t Init();
  virtual Int_t InitRun(Int_t runnumber);
  virtual Int_t Make();
  virtual void Clear( Option_t *opts = "" );

  virtual const char *GetCVS() const
  {static const char cvs[]="Tag $Name$ $Id$ built "__DATE__" "__TIME__ ; return cvs;}

 protected:
  THashList *listGeoMSensorOnGlobal;
  StIstDbMaker *mIstDbMaker;

  UShort_t mMinNumOfRawHits, mMaxNumOfRawHits;

  StIstHitCollection *istHitCollection;

 private:
  ClassDef(StIstHitMaker,1);
};
#endif
