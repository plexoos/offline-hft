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

StIstHit::~StIstHit() { /* no op */ }

StIstHit::StIstHit(unsigned char ladder, unsigned char sensor, float charge, float chargeErr, unsigned char maxTB, unsigned char nRawHits, unsigned char nRawHitsZ, unsigned char nRawHitsRPhi, float meanColumn, float meanRow)
{
    StHit::setHardwarePosition((ladder-1)*kIstNumSensorsPerLadder + sensor);
    StHit::setCharge(charge);
    mChargeErr = chargeErr;
    mMaxTimeBin = maxTB;
    mNRawHits = nRawHits;
    mNRawHitsZ = nRawHitsZ;
    mNRawHitsRPhi = nRawHitsRPhi;
    mLocalPosition[0] = 0.5*kIstSensorActiveSizeRPhi - (meanRow-0.5)*kIstPadPitchRow;//transfer to the sensor local coordinate (unit: cm)
    mLocalPosition[1] = 0.;
    mLocalPosition[2] = (meanColumn-0.5)*kIstPadPitchColumn - 0.5*kIstSensorActiveSizeZ;//transfer to the sensor local coordiante (unit: cm)
    mDetectorId = kIstId;
}

StIstHit::StIstHit(const StThreeVectorF& p,
                   const StThreeVectorF& e,
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
	if (i<3)
		return mLocalPosition[i];
	else
	        return 0;
}

float StIstHit::localPositionErr(unsigned int i) const
{
        if (i==0) {
	    if(mNRawHitsRPhi == 1)	
                return kIstPosResolutionRPhi;
	    else if(mNRawHitsRPhi > 1)
		return kIstPosResolutionRPhi2;
	    else
		return 0;
	}
	else if (i==2)
		return kIstPosResolutionZ;
        else
        	return 0;
}

ostream& operator<<(ostream& os, const StIstHit& hit)
{
    os << "IST Hit -I- ladder: "<< static_cast<int>(hit.getLadder())
       << " sensor: "<< static_cast<int>(hit.getSensor()) 
       << " localPosition[0]/localPosition[2] : " << hit.localPosition(0)
       << "/" << hit.localPosition(2) 
       << " ADC : " << hit.charge() << "+-" << hit.getChargeErr()
       << " detector: " << hit.detector()
       << endl;
    return os;
}

ClassImp(StIstHit);
