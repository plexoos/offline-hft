/***************************************************************************
*
* $Id$
*
* Author: Yaping Wang, August 2013
****************************************************************************
* Description: 
* Data collection for IST hits, and one instance corresponds to one sensor.
****************************************************************************
*
* $Log$
* Revision 1.1  2014/01/23 20:11:28  ypwang
* adding scripts
*
*
****************************************************************************
* StIstSensorHitCollection.h,v 1.0
* Revision 1.0 2013/11/04 15:25:30 Yaping
* Initial version
****************************************************************************/

#ifndef StIstSensorHitCollection_hh
#define StIstSensorHitCollection_hh

#include "StObject.h"
#include "StContainers.h"

class StIstHit;

class StIstSensorHitCollection : public StObject {
public:
    StIstSensorHitCollection();
    ~StIstSensorHitCollection();
    
    StSPtrVecIstHit&       hits();
    const StSPtrVecIstHit& hits() const;

private:
    StSPtrVecIstHit mHits;
    
    ClassDef(StIstSensorHitCollection,1)
};
#endif
