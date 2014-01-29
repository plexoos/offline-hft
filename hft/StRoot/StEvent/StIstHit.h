/***************************************************************************
*
* $Id$
*
* Author: Yaping Wang, March 2013
****************************************************************************
* Description: 
* Data structure for individual IST hit (a 1D cluster).
****************************************************************************
*
* $Log$
* Revision 1.2  2014/01/29 18:25:00  ypwang
* updating scripts
*
*
****************************************************************************
* StIstHit.h,v 1.0
* Revision 1.0 2013/11/04 15:25:30 Yaping
* Initial version
****************************************************************************/

#ifndef StIstHit_hh
#define StIstHit_hh

#include "StHit.h"
#include "StEnumerations.h"
#include "StRoot/StIstUtil/StIstConsts.h"

class StIstHit : public StHit
{
public:
    StIstHit(unsigned char ladder=-1, unsigned char sensor=-1, float Charge=0., float ChargeErr=0., unsigned char maxTB=0, unsigned char nRawHits=1, unsigned char nRawHitsZ=0, unsigned char nRawHitsRPhi=0, float meanColumn=0., float meanRow=0);
    StIstHit(const StThreeVectorF& position,
             const StThreeVectorF& error,
             unsigned int hwPosition, float charge,
             unsigned char trackRefCount = 0);
    
    ~StIstHit();
    
    virtual StDetectorId detector() const;
    void    setDetectorId(StDetectorId); 
     
    //accessories
    unsigned char     	getLadder() const;
    unsigned char     	getSensor() const;
    float		getMeanColumn() const;
    float		getMeanRow() const;
    unsigned char     	getMaxTimeBin() const;  
    float     		getChargeErr() const;
    unsigned char	getClusteringType() const;
    unsigned char     	getNRawHits() const;
    unsigned char     	getNRawHitsZ() const;
    unsigned char     	getNRawHitsRPhi() const;       
    float     		localPosition(unsigned int ) const;
    float		localPositionErr(unsigned int ) const;

    //modifiers
    void    setMaxTimeBin(unsigned char tb);
    void    setChargeErr(float chargeErr);
    void    setClusteringType(unsigned char clusteringType);
    void    setNRawHits(unsigned char nRawHits);//cluster size
    void    setNRawHitsZ(unsigned char nRawHitsZ);
    void    setNRawHitsRPhi(unsigned char nRawHitsRPhi);  
    void    setLocalPosition(float, float, float);
        
protected:
    UChar_t mMaxTimeBin;        // max charge time bin
    Float_t mChargeErr;         // charge uncertainty
    UChar_t mClusteringType;	// clustering algorithm type
    UChar_t mNRawHits;          // nRawHits: cluster size
    UChar_t mNRawHitsZ;         // cluster size in Z direction
    UChar_t mNRawHitsRPhi;      // cluster size in r-phi direction
    
    //local position of hit inside the sensor 
    Float_t mLocalPosition[3]; //meanColumn, 0. or defined by sensor surface correction, meanRow
        
    StDetectorId mDetectorId;
private:
    ClassDef(StIstHit,1)
};

ostream& operator<<(ostream&, const StIstHit&);

////////////////////////
inline unsigned char StIstHit::getLadder() const          {   return 1 + (mHardwarePosition-1)/kIstNumSensorsPerLadder;         };
inline unsigned char StIstHit::getSensor() const          {   return 1 + (mHardwarePosition-1)%kIstNumSensorsPerLadder;         };
inline float StIstHit::getMeanColumn() const	    	  {   return 0.5 + (0.5*kIstSensorActiveSizeZ    + mLocalPosition[2])/kIstPadPitchColumn;	};
inline float StIstHit::getMeanRow() const	    	  {   return 0.5 + (0.5*kIstSensorActiveSizeRPhi - mLocalPosition[0])/kIstPadPitchRow;	};
inline unsigned char StIstHit::getMaxTimeBin() const      {   return mMaxTimeBin;     };
inline float StIstHit::getChargeErr()    const    	  {   return mChargeErr;      };
inline unsigned char StIstHit::getClusteringType() const  {   return mClusteringType; };
inline unsigned char StIstHit::getNRawHits() const        {   return mNRawHits;       };
inline unsigned char StIstHit::getNRawHitsZ() const       {   return mNRawHitsZ;   };
inline unsigned char StIstHit::getNRawHitsRPhi() const    {   return mNRawHitsRPhi;};

inline void StIstHit::setMaxTimeBin(unsigned char tb)     	     	{   mMaxTimeBin = tb;       };
inline void StIstHit::setChargeErr(float chargeErr)        		{   mChargeErr = chargeErr;         };
inline void StIstHit::setClusteringType(unsigned char clusteringType) 	{   mClusteringType = clusteringType; };
inline void StIstHit::setNRawHits(unsigned char nRawHits)   	     	{   mNRawHits = nRawHits;   };
inline void StIstHit::setNRawHitsZ(unsigned char nRawHitsZ)        	{   mNRawHitsZ = nRawHitsZ;   };
inline void StIstHit::setNRawHitsRPhi(unsigned char nRawHitsRPhi)  	{   mNRawHitsRPhi = nRawHitsRPhi;   };

#endif
