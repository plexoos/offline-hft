/***************************************************************************
*
* $Id$
*
* Author: Yaping Wang, August 2013
****************************************************************************
* Description: 
* Data collection for IST hits, and is saved to StEvent.
****************************************************************************
*
* $Log$
* Revision 1.1  2014/01/23 20:11:28  ypwang
* adding scripts
*
*
****************************************************************************
* StIstHitCollection.h,v 1.0
* Revision 1.0 2013/11/04 15:25:30 Yaping
* Initial version
****************************************************************************/

#ifndef StIstHitCollection_hh
#define StIstHitCollection_hh

#include "StObject.h"
#include "StIstLadderHitCollection.h"
class StIstHit;

class StIstHitCollection : public StObject {
public:
    StIstHitCollection();
    ~StIstHitCollection();
    
    bool          addHit(StIstHit*);
    unsigned int  numberOfHits() const;
    unsigned int  numberOfLadders() const;
    
    StIstLadderHitCollection*       ladder(unsigned int);
    const StIstLadderHitCollection* ladder(unsigned int) const;

private:
    enum { mNumberOfLadders = 24 };
    StIstLadderHitCollection mLadders[mNumberOfLadders];
    
    ClassDef(StIstHitCollection,1)
};
#endif
