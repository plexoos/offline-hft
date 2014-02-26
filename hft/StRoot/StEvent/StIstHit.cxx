/***************************************************************************
*
* $Id$
*
* Author: Yaping Wang, March 2013
****************************************************************************
* Description:
* See header file.
****************************************************************************
*
* $Log$
* Revision 1.5  2014/02/26 21:18:08  smirnovd
* Style corrected with astyle -s3 -p -H -A3 -k3 -O -o -y -Y -f
*
* Revision 1.4  2014/02/26 01:35:36  ypwang
* get rid of meanColumn/meanRow/Apv transformations and local position uncertainties to avoid external constants access
*
* Revision 1.3  2014/02/25 17:04:50  ypwang
* get rid of mClusteringType and its accessory/modifier functions
*
* Revision 1.2  2014/01/29 18:25:00  ypwang
* updating scripts
*
*
****************************************************************************
* StIstHit.cxx,v 1.0
* Revision 1.0 2013/11/04 15:25:30 Yaping
* Initial version
****************************************************************************/

#include "StIstHit.h"

StIstHit::StIstHit(unsigned char ladder, unsigned char sensor, float charge, float chargeErr, unsigned char maxTB, unsigned char nRawHits, unsigned char nRawHitsZ, unsigned char nRawHitsRPhi)
{
   StHit::setHardwarePosition((ladder - 1)*mIstNumSensorsPerLadder + sensor);
   StHit::setCharge(charge);
   mChargeErr = chargeErr;
   mMaxTimeBin = maxTB;
   mNRawHits = nRawHits;
   mNRawHitsZ = nRawHitsZ;
   mNRawHitsRPhi = nRawHitsRPhi;
   mLocalPosition[0] = 0.;
   mLocalPosition[1] = 0.;
   mLocalPosition[2] = 0.;
   mDetectorId = kIstId;
}

StIstHit::StIstHit(const StThreeVectorF &p,
                   const StThreeVectorF &e,
                   unsigned int hw, float q, unsigned char c) : StHit(p, e, hw, q, c)
{
   mChargeErr = 0.;
   mMaxTimeBin = -1;
   mNRawHits = -1;
   mNRawHitsZ = -1;
   mNRawHitsRPhi = -1;
   mLocalPosition[0] = 0.;
   mLocalPosition[1] = 0.;
   mLocalPosition[2] = 0.;
   mDetectorId = kIstId;
}

StDetectorId StIstHit::detector() const {return mDetectorId;}

void StIstHit::setDetectorId(StDetectorId id) {mDetectorId = id;}

void StIstHit::setLocalPosition(float vRPhi, float vY, float vZ)
{
   mLocalPosition[0] = vRPhi;
   mLocalPosition[1] = vY;
   mLocalPosition[2] = vZ;
}

float StIstHit::localPosition(unsigned int i) const
{
   if (i < 3)
      return mLocalPosition[i];
   else
      return 0;
}

ostream &operator<<(ostream &os, const StIstHit &hit)
{
   os << "IST Hit -I- ladder: " << static_cast<int>(hit.getLadder())
      << " sensor: " << static_cast<int>(hit.getSensor())
      << " localPosition[0]/localPosition[2] : " << hit.localPosition(0)
      << "/" << hit.localPosition(2)
      << " ADC : " << hit.charge() << "+-" << hit.getChargeErr()
      << " detector: " << hit.detector()
      << endl;
   return os;
}

ClassImp(StIstHit);
