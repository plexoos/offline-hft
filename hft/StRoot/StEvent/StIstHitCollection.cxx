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
* StIstHitCollection.cxx,v 1.0
* Revision 1.0 2013/11/04 15:25:30 Yaping
* Initial version
****************************************************************************/

#include "StIstHitCollection.h"
#include "StIstLadderHitCollection.h"
#include "StIstSensorHitCollection.h"
#include "StIstHit.h"

static const char rcsid[] = "$Id$";

ClassImp(StIstHitCollection)

StIstHitCollection::StIstHitCollection() { /* noop */ }

StIstHitCollection::~StIstHitCollection() { /* noop */ }
    
unsigned int
StIstHitCollection::numberOfLadders() const { return mNumberOfLadders; }

bool
StIstHitCollection::addHit(StIstHit* hit)
{
    unsigned int l, w;
    l = (unsigned int)hit->getLadder()-1;
    w = (unsigned int)hit->getSensor()-1;

    if (hit &&
        l < mNumberOfLadders &&
        w < mLadders[l].numberOfSensors()) {
        mLadders[l].sensor(w)->hits().push_back(hit);
        return kTRUE;
    }
    else {
        return kFALSE;
    }
}

unsigned int
StIstHitCollection::numberOfHits() const
{
    unsigned int sum = 0;
    for (unsigned int i=0; i<mNumberOfLadders; i++)
        for (unsigned int j=0; j<mLadders[i].numberOfSensors(); j++)
                sum += mLadders[i].sensor(j)->hits().size();
    return sum;
}

StIstLadderHitCollection*
StIstHitCollection::ladder(unsigned int i)
{
    if (i < mNumberOfLadders)
        return &(mLadders[i]);
    else
        return 0;
}

const StIstLadderHitCollection*
StIstHitCollection::ladder(unsigned int i) const
{
    if (i < mNumberOfLadders)
        return &(mLadders[i]);
    else
        return 0;
}

