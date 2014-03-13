/***************************************************************************
*
* $Id$
*
* Author: Yaping Wang, August 2013
****************************************************************************
* Description:
* See header file.
****************************************************************************
*
* $Log$
* Revision 1.4  2014/03/13 22:05:25  smirnovd
* Style issue: Function return types on same line
*
* Revision 1.3  2014/02/26 21:18:08  smirnovd
* Style corrected with astyle -s3 -p -H -A3 -k3 -O -o -y -Y -f
*
* Revision 1.2  2014/01/29 18:25:00  ypwang
* updating scripts
*
*
****************************************************************************
* StIstSensorHitCollection.cxx,v 1.0
* Revision 1.0 2013/11/04 15:25:30 Yaping
* Initial version
****************************************************************************/

#include "StIstSensorHitCollection.h"
#include "StIstHit.h"

static const char rcsid[] = "$Id$";

ClassImp(StIstSensorHitCollection)

StIstSensorHitCollection::StIstSensorHitCollection() { /* noop */ }

StIstSensorHitCollection::~StIstSensorHitCollection()
{
   for (unsigned int i = 0; i < mHits.size(); i++) {
      delete mHits[i];
      mHits[i] = 0;
   }
}

const StSPtrVecIstHit & StIstSensorHitCollection::hits() const { return mHits; }

StSPtrVecIstHit & StIstSensorHitCollection::hits() { return mHits; }
