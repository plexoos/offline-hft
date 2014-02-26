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
* Revision 1.4  2014/02/26 21:18:08  smirnovd
* Style corrected with astyle -s3 -p -H -A3 -k3 -O -o -y -Y -f
*
* Revision 1.3  2014/02/25 17:08:49  ypwang
* add mClusteringType and its accessory/modifier functions
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

class StIstHitCollection : public StObject
{
public:
   StIstHitCollection();
   ~StIstHitCollection();

   bool          addHit(StIstHit *);
   unsigned int  numberOfHits() const;
   unsigned int  numberOfLadders() const;

   unsigned char getClusteringType() const;
   void 	  setClusteringType(unsigned char clusteringType);

   StIstLadderHitCollection       *ladder(unsigned int);
   const StIstLadderHitCollection *ladder(unsigned int) const;

private:
   enum { mNumberOfLadders = 24 };
   StIstLadderHitCollection mLadders[mNumberOfLadders];
   UChar_t mClusteringType;  // clustering algorithm type

   ClassDef(StIstHitCollection, 1)
};
#endif
