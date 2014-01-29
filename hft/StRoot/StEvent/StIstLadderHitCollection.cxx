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
* Revision 1.2  2014/01/29 18:25:00  ypwang
* updating scripts
*
*
****************************************************************************
* StIstLadderHitCollection.cxx,v 1.0
* Revision 1.0 2013/11/04 15:25:30 Yaping
* Initial version
****************************************************************************/

#include "StIstLadderHitCollection.h"

static const char rcsid[] = "$Id$";

ClassImp(StIstLadderHitCollection)

StIstLadderHitCollection::StIstLadderHitCollection() { /* noop */ }

StIstLadderHitCollection::~StIstLadderHitCollection() { /* noop */ }

unsigned int
StIstLadderHitCollection::numberOfSensors() const {return mMaxNumberOfSensors;}

unsigned int
StIstLadderHitCollection::numberOfHits() const
{
    unsigned int sum = 0;
    for (unsigned int j=0; j<numberOfSensors(); j++) {
        sum += mSensors[j].hits().size();
    }
    return sum;
}

StIstSensorHitCollection*
StIstLadderHitCollection::sensor(unsigned int i)
{
    if (i < numberOfSensors())
        return &(mSensors[i]);
    else
        return 0;
}

const StIstSensorHitCollection*
StIstLadderHitCollection::sensor(unsigned int i) const
{
    if (i < numberOfSensors())
        return &(mSensors[i]);
    else
        return 0;
}
    
