/***************************************************************************
*
* $Id$
*
* Author: Yaping Wang, August 2013
****************************************************************************
* Description: 
* Calculates mean pedestal, rms noise and cm noise by histogram methods,
* and produce two data files and several QA plots.
****************************************************************************
*
* $Log$
* Revision 1.3  2014/02/06 12:28:34  ypwang
* updating scripts
*
*
****************************************************************************
* StIstCalibrationMaker.h,v 1.0
* Revision 1.0 2013/11/04 16:20:30 Yaping
* Initial version
****************************************************************************/

#ifndef StIstCalibrationMaker_h
#define StIstCalibrationMaker_h

#include <string>
#include "StMaker.h"
#include "StRoot/StIstUtil/StIstConsts.h"

class StIstDbMaker;
class St_istMapping;

class StIstCalibrationMaker : public StMaker {
public:
    // constructors
    StIstCalibrationMaker( const char* name = "ist_calib" );
    // deconstructor
    ~StIstCalibrationMaker();

    Int_t Init();
    Int_t InitRun(Int_t runNumber);
    Int_t Make();
    Int_t Finish();

    // modifiers
    void setTimeBinMask( short mask = 0xFF );
    void setPedCut(float pedCut = 3.0);
    // Get CVS
    virtual const char *GetCVS() const;

protected:
    // mask for which time bins to save
    Short_t mTimeBinMask;
    Float_t mPedCut;
    std::vector< TH1F* > mHistPedVec;
    std::vector< TH1F* > mHistCmnVec;
   
    Int_t evtIdx; 
    // ped, rms, gain
    struct pedNoiseData_t {
        Int_t n;
        Float_t ped;
        Float_t rms;

        pedNoiseData_t( int nIn=0, float p=0, float r=0 ) : n(nIn), ped(p), rms(r) { /* */ };
    };
    typedef std::vector< pedNoiseData_t > pedNoiseDataVec_t;
    pedNoiseDataVec_t mPedVec;
    // cmn
    struct cmNoiseData_t {
        Int_t n;
        Float_t cmn;

        cmNoiseData_t( int nIn=0, float c=0 ) : n(nIn), cmn(c) { /* */ };
    };
    typedef std::vector< cmNoiseData_t > cmNoiseDataVec_t;
    cmNoiseDataVec_t mCmnVec; 
   
    // functions that actually do the saving
    Int_t saveToFile();

    // has finished
    Bool_t mHasFinished;

    // Db
    StIstDbMaker *mIstDbMaker;
    // mapping
    typedef std::vector< int > MappingVec_t;//Channel elec. index, geometry ID
    MappingVec_t mMappingVec;
    
    // for saving to file
    TFile *myRootFile; 			 // file to store histograms
    TH1F *hist_meanPed[kIstNumTimeBins]; // mean pedestal = pedestal histogram -> GetMean()
    TH1F *hist_rmsPed[kIstNumTimeBins];  // standard deveriation = pedestal histogram -> GetRMS()
    TH1F *hist_cmNoise[kIstNumTimeBins]; // common mode noise per APV chip
    TH1F *hist_sumPed[kIstNumTimeBins];  // summary pedestal over all channels
    TH1F *hist_sumRms[kIstNumTimeBins]; // summary rms noise over all channels
    TH1F *hist_sumCmn[kIstNumTimeBins]; // summary common mode noise over all channels
    static const string sectionLabel[72];

private:
    
   ClassDef(StIstCalibrationMaker,1);
}; 

// modifiers
inline void StIstCalibrationMaker::setTimeBinMask( short mask )			{ mTimeBinMask = mask; };
inline void StIstCalibrationMaker::setPedCut(float pedCut)			{ mPedCut = pedCut; };
inline const char *StIstCalibrationMaker::GetCVS() const {
   static const char cvs[] = "Tag $Name$ $Id$ built "__DATE__" "__TIME__ ;
   return cvs;
}
#endif
