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
 * Revision 1.15  2014/02/07 00:14:32  smirnovd
 * Change histogram type to conserve memory
 *
 * Revision 1.14  2014/02/01 19:03:17  smirnovd
 * Fixed default parameter bug
 *
 * Revision 1.13  2014/02/01 02:37:18  smirnovd
 * This commit is intended to sync with what we had in StRoot/StiRnD
 *
 * Revision 1.7  2014/01/28 19:29:42  qiuh
 * *** empty log message ***
 *
 *
 **************************************************************************/

#ifndef StPxlMonMaker_hh
#define StPxlMonMaker_hh

#include <stdint.h>

#include "TString.h"
#include "TNtuple.h"
#include "TFile.h"
#include "TH2F.h"
#include "TH2S.h"
#include "TProfile.h"
#include "StIOMaker/StIOMaker.h"

class StIOMaker;
class StEvent;
class StTrack;


class StPxlMonMaker : public StMaker
{
public:

   StPxlMonMaker(uint16_t numPixelsPerBin=10, const Char_t *name="pxlQa");
   Int_t  Init();
   Int_t  Make();                      // invoked for every event
   Int_t  Finish();                    // called once at the end
   virtual const char *GetCVS() const {
      static const char cvs[] = "Tag $Name$ $Id$ built "__DATE__" "__TIME__ ;
      return cvs;
   }

   TFile    *m_f1;
   TNtuple  *m_hitNtuple;
   TNtuple  *m_rawHitNtuple;
   TH2S     *m_rawHit_rowvscolumn[400];
   TH2S     *m_hit_localZ_X[400];
   TProfile *m_nRawHits_EventId[400];
   TH2F     *m_globalx_y;
   TH2S     *m_hitnRawHits_sensorID;
   TH2S     *m_hitnRawHits_eachsector_sensorID[10];

   TH2S     *m_nRawHits_sensorID;
   TH2S     *m_nRawHits_eachsector_sensorID[10];

   TH2S     *m_nHits_sensorID;
   TH2S     *m_nHits_eachsector_sensorID[10];

   TH1F     *m_globalz;
   TH2S     *m_innerhits_outerhits[10];
   TH2S     *m_innerrawhits_outerrawhits[10];
   TH2F     *m_globalphi_z_inner;
   TH2F     *m_globalphi_z_outer;

private:

   void   bookHists();
   void   fillHists();
   void   writeHists();

   uint16_t mNumPixelsPerBin;   //!< Number of rows and columns to group in a bin
   Int_t    mEventCounter;      //!
   Int_t    mNtupleWrite;

   ClassDef(StPxlMonMaker, 0)
};

#endif
