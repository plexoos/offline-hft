/***************************************************************************
* $Id$
* Author: Yaping Wang, March 2013
****************************************************************************
* Description: Virtual class for clustering algorithm implementation.
****************************************************************************
* $Log$
* Revision 1.2  2014/01/29 18:25:01  ypwang
* updating scripts
*
* Revision 1.0 2013/11/04 Yaping
* Initial version
****************************************************************************/


#ifndef StIstIClusterAlgo_hh
#define StIstIClusterAlgo_hh

#include "Stypes.h"
class StIstRawHitCollection;
class StIstClusterCollection;
class StIstCollection;

class StIstIClusterAlgo
{
 public:
  virtual Int_t doClustering(const StIstCollection& istCollection, StIstRawHitCollection&, StIstClusterCollection& )=0;
  virtual Int_t Init()=0;

  virtual ~StIstIClusterAlgo()=0;

  virtual void setUsedTimeBin(unsigned char tb)=0;
  virtual void setSplitFlag( bool splitFlag)=0;

 protected:

 private:

  ClassDef( StIstIClusterAlgo, 1 );
};
#endif
