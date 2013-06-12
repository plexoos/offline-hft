//
//  This is a STAR typical comment header. You should modify
//  it to reflect your changes.
//  As a minimum it should contain the name of the author, the
//  date it was written/modified, and a short description of what
//  the class is meant to do. The cvs strings $X$ (where X=Id, Log)
//  are not needed when you do not intend to put the file under
//  cvs control. Remove them.
//  
/*!
 * \class  StPxlMonMaker
 * \author Shusu Shi, LBNL
 * \date   May 2013 
 */

//
// Every header file should have these macros to protect
// from being included multiple times in the same scope.
// If you change the name of the class change the name
// of the macro.
//
#ifndef StPxlMonMaker_hh     
#define StPxlMonMaker_hh

//
//  Include files. StMaker.h is needed since your maker
//  inherits from StMaker. 
//#include "StMaker.h"
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
//
//  The class declaration. Every maker has to
//  inherit from StMaker.
//
class StPxlMonMaker : public StMaker {
	public:

		StPxlMonMaker(const Char_t *name="pxlQa");     // constructor
		~StPxlMonMaker() {}                               // destructor
                virtual Int_t Init();
                Int_t Declareplots();
                void writeHistograms();
		Int_t  Make();                      // invoked for every event
		Int_t  Finish();                    // called once at the end
		static void PrintStEvent(TString opt="vpg");
		static void PrintVertex(UInt_t ivx = 0);
		void PrintPixelHits();
		virtual const char *GetCVS() const {
			static const char cvs[]="Tag $Name$ $Id$ built "__DATE__" "__TIME__ ; 
			return cvs;
		}
		//TString filename;
		TFile* f1;
		TNtuple* hitNtuple;
		TNtuple* rawHitNtuple;
		TH2F* rawHit_rowvscolumn[400];
                TH2F* hit_localZ_X[400];
                TProfile* nRawHits_EventId[400];
		TH2F* globalx_y;
		TH2F* hitnRawHits_sensorID;
                TH2F* hitnRawHits_eachsector_sensorID[10];
 
		TH2F* nRawHits_sensorID;
                TH2F* nRawHits_eachsector_sensorID[10];
 
		TH2F* nHits_sensorID;
                TH2F* nHits_eachsector_sensorID[10];

		TH1F* globalz;
		TH2F* innerhits_outerhits[10];
		TH2F* innerrawhits_outerrawhits[10];
		TH2F* globalphi_z_inner;
		TH2F* globalphi_z_outer;
	private:
		Bool_t accept(StEvent*);            // this method serves as an event filter
		Bool_t accept(StTrack*);            // and this is used to select tracks

		//
		//  Data members
		//  Note, that it is recommended to start all member names with
		//  an 'm'. This makes it easier to read the code later.
		//
		Int_t        mEventCounter;  //!
                Int_t        NtupleWrite;
                 //
		ClassDef(StPxlMonMaker,0)
};
#endif
