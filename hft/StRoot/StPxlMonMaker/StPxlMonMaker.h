/*!
 * \class  StPxlMonMaker
 * \author Shusu Shi, LBNL
 * \date   May 2013 
 * \Initial Revision.
 */
/***************************************************************************
 *
 * $Id$
 *
 * Author: Shusu Shi, May 2013
 ***************************************************************************
 *
 * Description:
 * pxl detector QA
 * More information at
 * https://www.star.bnl.gov/protected/heavy/qiuh/HFT/software/PXL_software.pdf
 *
 ***************************************************************************
 *
 * $Log$
 * Revision 1.5  2014/01/23 01:04:57  qiuh
 * *** empty log message ***
 *
 *
 **************************************************************************/


#ifndef StPxlMonMaker_hh     
#define StPxlMonMaker_hh

#include "TString.h"
#include "TNtuple.h"
#include "TH2F.h"
#include "TProfile.h"
#include "StIOMaker/StIOMaker.h"
class StIOMaker;
class StEvent;
class StTrack;
class TNtuple;
class TFile;
class TH2F;
class TProfile;

class StPxlMonMaker : public StMaker {
public:
    
    StPxlMonMaker(const Char_t *name="pxlQa");     // constructor
    ~StPxlMonMaker() {}                               // destructor
    Int_t Init();
    Int_t declareplots();
    void writeHistograms();
    Int_t  Make();                      // invoked for every event
    Int_t  Finish();                    // called once at the end
    void printPixelHits();
    virtual const char *GetCVS() const {
        static const char cvs[]="Tag $Name$ $Id$ built "__DATE__" "__TIME__ ;
        return cvs;
    }
    
    TFile* m_f1;
    TNtuple* m_hitNtuple;
    TNtuple* m_rawHitNtuple;
    TH2F* m_rawHit_rowvscolumn[400];
    TH2F* m_hit_localZ_X[400];
    TProfile* m_nRawHits_EventId[400];
    TH2F* m_globalx_y;
    TH2F* m_hitnRawHits_sensorID;
    TH2F* m_hitnRawHits_eachsector_sensorID[10];
    
    TH2F* m_nRawHits_sensorID;
    TH2F* m_nRawHits_eachsector_sensorID[10];
    
    TH2F* m_nHits_sensorID;
    TH2F* m_nHits_eachsector_sensorID[10];
    
    TH1F* m_globalz;
    TH2F* m_innerhits_outerhits[10];
    TH2F* m_innerrawhits_outerrawhits[10];
    TH2F* m_globalphi_z_inner;
    TH2F* m_globalphi_z_outer;
private:
    Bool_t accept(StEvent*);            // this method serves as an event filter
    Bool_t accept(StTrack*);            // and this is used to select tracks
    
    Int_t        mEventCounter;  //!
    Int_t        mNtupleWrite;
    //
    ClassDef(StPxlMonMaker,0)

};
#endif
