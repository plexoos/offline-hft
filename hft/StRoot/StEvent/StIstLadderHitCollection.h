/***************************************************************************
*
* $Id$
*
* Author: Yaping Wang, August 2013
****************************************************************************
* Description:
* Data collection for IST hits, and one instance corresponds to one ladder.
****************************************************************************
*
* $Log$
* Revision 1.3  2014/02/26 21:18:08  smirnovd
* Style corrected with astyle -s3 -p -H -A3 -k3 -O -o -y -Y -f
*
* Revision 1.2  2014/01/29 18:25:00  ypwang
* updating scripts
*
*
****************************************************************************
* StIstLadderHitCollection.h,v 1.0
* Revision 1.0 2013/11/04 15:25:30 Yaping
* Initial version
****************************************************************************/

#ifndef StIstLadderHitCollection_hh
#define StIstLadderHitCollection_hh

#include "StObject.h"
#include "StIstSensorHitCollection.h"

class StIstLadderHitCollection : public StObject
{
public:
   StIstLadderHitCollection();
   ~StIstLadderHitCollection();

   unsigned int  numberOfHits() const;
   unsigned int  numberOfSensors() const;

   StIstSensorHitCollection       *sensor(unsigned int);
   const StIstSensorHitCollection *sensor(unsigned int) const;

private:
   enum { mMaxNumberOfSensors = 6 };
   StIstSensorHitCollection  mSensors[mMaxNumberOfSensors];

   ClassDef(StIstLadderHitCollection, 1)
};
#endif
