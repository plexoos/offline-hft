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
* Revision 1.1  2014/01/23 20:11:29  ypwang
* adding scripts
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

class StIstCalibrationMaker : public StMaker {
public:
    // constructors
    StIstCalibrationMaker( const char* name = "ist_calib" );
    // deconstructor
    virtual ~StIstCalibrationMaker();

    Int_t Init();
    Int_t InitRun(Int_t runNumber);
    Int_t Make();
    Int_t Finish();

    // modifiers
    void setToSavePedToFile( const char* filename );
    void setToSaveCmnToFile( const char* filename );
    void setTimeBinMask( short mask = 0xFF );
    
    void setPedCut(float pedCut = 3.0);
    void setToSaveHistsToFile( const char* rootFilename );
    // Get CVS
    virtual const char *GetCVS() const;

protected:
    // mask for which time bins to save
    Short_t mTimeBinMask;
    Float_t mPedCut, mChanMaxRmsNoiseLevel, mApvMaxCmNoiseLevel;
    std::vector< TH1F* > mHistPedVec;
    std::vector< TH1F* > mHistCmnVec;
   
    Int_t evtIdx; 
    // ped, rms, gain
    struct pedNoiseData_t {
        Int_t n;
        Float_t ped;
        Float_t rms;
        Float_t gain;

        pedNoiseData_t( int nIn=0, float p=0, float r=0, float g=0 ) : n(nIn), ped(p), rms(r), gain(g) { /* */ };
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
   
    // for saving to file
    std::string mPedTable;
    std::string mCmnTable;

    // functions that actually do the saving
    Int_t saveToFile();

    // has finished
    Bool_t mHasFinished;
    
    // for saving to file
    std::string mRootFilename;
    TFile *myRootFile; 			 // file to store histograms
    TH1F *hist_meanPed[kIstNumTimeBins]; // mean pedestal = pedestal histogram -> GetMean()
    TH1F *hist_rmsPed[kIstNumTimeBins];  // standard deveriation = pedestal histogram -> GetRMS()
    TH1F *hist_cmNoise[kIstNumTimeBins]; // common mode noise per APV chip
    TH1F *hist_gain[kIstNumTimeBins];    // occupancy of channels
    static const string sectionLabel[72];
    static const string sectionLabel4APV[72];

private:
    
   ClassDef(StIstCalibrationMaker,1);
}; 

// modifiers
inline void StIstCalibrationMaker::setToSavePedToFile( const char* filename )	{ mPedTable = filename; };
inline void StIstCalibrationMaker::setToSaveCmnToFile( const char* filename )	{ mCmnTable = filename; };
inline void StIstCalibrationMaker::setTimeBinMask( short mask )			{ mTimeBinMask = mask; };
inline void StIstCalibrationMaker::setPedCut(float pedCut)			{ mPedCut = pedCut; };
inline void StIstCalibrationMaker::setToSaveHistsToFile( const char* rootFilename ){ mRootFilename = rootFilename; };

inline const char *StIstCalibrationMaker::GetCVS() const {
   static const char cvs[] = "Tag $Name$ $Id$ built "__DATE__" "__TIME__ ;
   return cvs;
}
#endif
