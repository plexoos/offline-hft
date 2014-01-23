/***************************************************************************
*
* $Id$
*
* Author: Yaping Wang, August 2013
****************************************************************************
* Description: 
* See header file.
****************************************************************************
*
* $Log$
* Revision 1.1  2014/01/23 20:11:29  ypwang
* adding scripts
*
*
****************************************************************************
* StIstCalibrationMaker.cxx,v 1.0
* Revision 1.0 2013/11/04 16:20:30 Yaping
* Initial version
****************************************************************************/

#include <string>
#include "StEvent.h"
#include "StIstCalibrationMaker.h"
#include "StRoot/StIstUtil/StIstCollection.h"
#include "StRoot/StIstUtil/StIstRawHitCollection.h"
#include "StRoot/StIstUtil/StIstRawHit.h"
#include "StRoot/StIstUtil/StIstConsts.h"

#include "tables/St_istControl_Table.h"

#include "TMath.h"

const string StIstCalibrationMaker::sectionLabel[72]={"22A","22B","22C","21A","21B","21C","20A","20B","20C","19A","19B","19C",
                                          "18A","18B","18C","17A","17B","17C","16A","16B","16C","15A","15B","15C",
                                          "14A","14B","14C","13A","13B","13C","12A","12B","12C","11A","11B","11C",
                                          "10A","10B","10C","9A","9B","9C","8A","8B","8C","7A","7B","7C",
                                          "6A","6B","6C","5A","5B","5C","4A","4B","4C","3A","3B","3C",
                                          "2A","2B","2C","1A","1B","1C","24A","24B","24C","23A","23B","23C"};

const string StIstCalibrationMaker::sectionLabel4APV[72]={"1C","1B","1A","2C","2B","2A","3C","3B","3A","4C","4B","4A",
                                          "5C","5B","5A","6C","6B","6A","7C","7B","7A","8C","8B","8A",
                                          "9C","9B","9A","10C","10B","10A","11C","11B","11A","12C","12B","12A",
                                          "13C","13B","13A","14C","14B","14A","15C","15B","15A","16C","16B","16A",
                                          "17C","17B","17A","18C","18B","18A","19C","19B","19A","20C","20B","20A",
                                          "21C","21B","21A","22C","22B","22A","23C","23B","23A","24C","24B","24A"};

// constructor
StIstCalibrationMaker::StIstCalibrationMaker( const char* name ) : StMaker( name ), mTimeBinMask(0xFF), evtIdx(0), mPedTable("pedestalNoise_ist.dat"), mCmnTable("cmNoise_ist.dat"), mHasFinished(0), mRootFilename("calibrationQA_ist.root") {
    for(unsigned char iTb=0; iTb<kIstNumTimeBins; iTb++) {
		hist_meanPed[iTb] = NULL;
		hist_rmsPed[iTb]  = NULL;
	        hist_cmNoise[iTb] = NULL;
		hist_gain[iTb]    = NULL;
    }

    mHistPedVec.assign( kIstNumTimeBins*kIstNumElecIds, (TH1F*)0 );
    mHistCmnVec.assign( kIstNumTimeBins*kIstNumApvs, (TH1F*)0 );
    mPedVec.resize( kIstNumTimeBins*kIstNumElecIds );    // set to all zeros
    mCmnVec.resize( kIstNumTimeBins*kIstNumApvs );    // set to all zeros
};

// deconstructor
StIstCalibrationMaker::~StIstCalibrationMaker()
{ 
    mHistPedVec.clear();
    mHistCmnVec.clear();
    mPedVec.clear();
    mCmnVec.clear();
};

// initialize
Int_t StIstCalibrationMaker::Init()
{
    Int_t ierr = kStOk;

    if( !mPedTable.empty() ){
        // make sure the file ped/rms/gain table can be opened
        std::ofstream fout_ped( mPedTable.data() );
        if( !fout_ped ){
            LOG_ERROR << "Error opening file '" << mPedTable << "'" << endl;
            ierr = kStFatal;
        }
    }

    if( !mCmnTable.empty() ){
        // make sure the file cm noise table can be opened
        std::ofstream fout_cmn( mCmnTable.data() );
        if( !fout_cmn ){ 
            LOG_ERROR << "Error opening file '" << mCmnTable << "'" << endl;
            ierr = kStFatal;
        }
    }
    
    if( !mRootFilename.empty() ){
    	myRootFile = new TFile(mRootFilename.data(),"RECREATE");
        if( !myRootFile ){
            LOG_ERROR << "Error recreating file '" << mRootFilename << "'" << endl;
            ierr = kStFatal;
        }
        
    	char buffer[100];
    	for(unsigned char iTB=0; iTB<kIstNumTimeBins; iTB++) {
            sprintf(buffer,"hist_meanPedestal_TimeBin%d",iTB);
            hist_meanPed[iTB] = new TH1F(buffer, buffer, kIstNumElecIds, 0, kIstNumElecIds);
	    hist_meanPed[iTB]->SetStats(false);
	    hist_meanPed[iTB]->GetXaxis()->SetTitle("Channel Index");
	    hist_meanPed[iTB]->GetXaxis()->SetNdivisions(72,false); //sections
	    hist_meanPed[iTB]->GetYaxis()->SetTitle("ADC count");
            sprintf(buffer,"hist_rmsPedestal_TimeBin%d",iTB);
            hist_rmsPed[iTB] = new TH1F(buffer, buffer, kIstNumElecIds, 0, kIstNumElecIds);
	    hist_rmsPed[iTB]->SetStats(false);
	    hist_rmsPed[iTB]->GetXaxis()->SetTitle("Channel Index");
            hist_rmsPed[iTB]->GetXaxis()->SetNdivisions(72,false);//sections
            hist_rmsPed[iTB]->GetYaxis()->SetTitle("ADC count");
            sprintf(buffer,"hist_cmNoise_TimeBin%d",iTB);
            hist_cmNoise[iTB] = new TH1F(buffer, buffer, kIstNumApvs, 0, kIstNumApvs);
	    hist_cmNoise[iTB]->SetStats(false);
	    hist_cmNoise[iTB]->GetXaxis()->SetTitle("APV Index");
            hist_cmNoise[iTB]->GetXaxis()->SetNdivisions(72,false);//sections
            hist_cmNoise[iTB]->GetYaxis()->SetTitle("ADC count");
            sprintf(buffer,"hist_gain_TimeBin%d",iTB);
            hist_gain[iTB] = new TH1F(buffer, buffer, kIstNumElecIds, 0, kIstNumElecIds);
	    hist_gain[iTB]->SetStats(false);
	    hist_gain[iTB]->GetXaxis()->SetTitle("Channel Index");
            hist_gain[iTB]->GetXaxis()->SetNdivisions(72,false);//sections
            hist_gain[iTB]->GetYaxis()->SetTitle("Gain");

	    for(int i=0; i<72; i++) {
		TString binBuffer = "";
		binBuffer = sectionLabel[i];
		hist_meanPed[iTB]->GetXaxis()->SetBinLabel(i*1536+768, binBuffer);
		hist_rmsPed[iTB]->GetXaxis()->SetBinLabel(i*1536+768, binBuffer);
		hist_gain[iTB]->GetXaxis()->SetBinLabel(i*1536+768, binBuffer);

		binBuffer = sectionLabel4APV[i];
                hist_cmNoise[iTB]->GetXaxis()->SetBinLabel(i*12+6, binBuffer);
	    }
    	}  
    }

    return ierr;
};

Int_t StIstCalibrationMaker::InitRun(Int_t runnumber)
{
   Int_t ierr = kStOk;

   St_istControl *istControl = (St_istControl *)GetDataBase("Calibrations/ist/istControl");
   if (!istControl)  LOG_WARN << " no istControl table " << endm;

   istControl_st *istControlTable = istControl->GetTable() ;
   mPedCut = istControlTable[0].kIstPedCutDefault;
   mChanMaxRmsNoiseLevel = istControlTable[0].kIstChanMaxRmsNoiseLevel;
   mApvMaxCmNoiseLevel   = istControlTable[0].kIstApvMaxCmNoiseLevel;

   return ierr;
};

Int_t StIstCalibrationMaker::Make()
{
    Int_t ierr = kStOk;
    //obtain raw hit information from temporary dataset
    TObjectSet* istDataSet = (TObjectSet*)GetDataSet("istRawHitAndCluster");
    if (! istDataSet) {
        LOG_WARN << "Make() - there is no istDataSet (raw hits container) " << endm;
        ierr = kStWarn;
    }

    StIstCollection* istCollectionPtr = (StIstCollection*)istDataSet->GetObject();
    if(!istCollectionPtr) {
        LOG_WARN << "Make() - no istCollection."<<endm;
        ierr = kStWarn;
    }

    if( !ierr ){
	if( evtIdx%10 == 0 )
		cout << "event index: " << evtIdx << endl;

        std::stringstream ss, sc;
	int sumAdcPerEvent[kIstNumApvs][kIstNumTimeBins];
	int channelCountsPerEvent[kIstNumApvs][kIstNumTimeBins];
        float cmNoisePerEvent[kIstNumApvs][kIstNumTimeBins];
	for(int i=0; i<kIstNumApvs; i++)	{
	    for(int j=0; j<kIstNumTimeBins; j++)  {
		sumAdcPerEvent[i][j] = 0;
		channelCountsPerEvent[i][j] = 0;
		cmNoisePerEvent[i][j] = 0.;
	    }
	}

        for( unsigned char ladderIdx=0; ladderIdx<istCollectionPtr->getNumLadders() && ladderIdx<kIstNumLadders; ++ladderIdx ){
            StIstRawHitCollection *rawHitCollectionPtr = istCollectionPtr->getRawHitCollection( ladderIdx );

            if( rawHitCollectionPtr ){
                std::vector<StIstRawHit*>& rawHitVec = rawHitCollectionPtr->getRawHitVec();
                std::vector< StIstRawHit* >::iterator rawHitIter;

                for( rawHitIter = rawHitVec.begin(); rawHitIter != rawHitVec.end(); ++rawHitIter ){
		    int elecId = (*rawHitIter)->getChannelId();
		    int geoId  = (*rawHitIter)->getGeoId();
                    int apvId  = (geoId - 1)/kIstNumApvChannels;  // APV geometry ID
		    if(apvId >= kIstNumApvs)	continue;

                    for( unsigned char timeBin = 0; timeBin < kIstNumTimeBins; ++timeBin ){
			Int_t adc = (*rawHitIter)->getCharge( timeBin );
			if(adc)	{
                            int t = (int)timeBin;
                            if(mTimeBinMask==0) t=0;

			    sumAdcPerEvent[apvId][t] += adc;
			    channelCountsPerEvent[apvId][t] ++;

                            int code = kIstNumTimeBins * elecId + t;                               
			    TH1F* histPed = mHistPedVec[ code ];
                                
                            if( !histPed ){
                             	ss.str("");
                             	ss.clear();
                             	ss << "hist_Pedestal_Ch" << code / kIstNumTimeBins << "_TB" << code % kIstNumTimeBins;
                             	histPed = new TH1F( ss.str().data(), "", 128, 0, kIstMaxAdc );
                             	mHistPedVec[ code ] = histPed;
                            }
                            histPed->Fill( (float)adc );
			}//adc cut
                    }//time bin loop
                }//raw hits loop
            }//raw hit collection cut
        }//ladderIdx loop

	//common mode calculation per event
	for(int i=0; i<kIstNumApvs; i++) {
            for(int j=0; j<kIstNumTimeBins; j++)  {
		if( sumAdcPerEvent[i][j] )	{
		    cmNoisePerEvent[i][j] = (float)sumAdcPerEvent[i][j] / (float)channelCountsPerEvent[i][j];
		    int code = kIstNumTimeBins * i + j;
		    TH1F* histCmn = mHistCmnVec[ code ];

		    if( !histCmn ){
		        sc.str("");
                        sc.clear();
                        sc << "hist_CMNoise_APV" << code / kIstNumTimeBins << "_TB" << code % kIstNumTimeBins;
                        histCmn = new TH1F( sc.str().data(), "", 128, 0, kIstMaxAdc);
                        mHistCmnVec[ code ] = histCmn;
		    }
		    histCmn->Fill( cmNoisePerEvent[i][j] );
		}
	    }
	}
    }//ierr cut
    evtIdx++;

    return ierr;
};

// save as needed
Int_t StIstCalibrationMaker::Finish()
{
    Int_t ierr = kStOk;

    if( !mHasFinished ){
        mHasFinished = 1;
        cout << "StIstCalibrationMaker::Finish()" << endl;

        std::vector< TH1F* >::iterator mHistPedVecIter;
        std::vector< TH1F* >::iterator mHistCmnVecIter;
        int elecIdx = 0, chipIdx = 0;

        for( mHistPedVecIter = mHistPedVec.begin(); mHistPedVecIter != mHistPedVec.end(); ++mHistPedVecIter ){
            TH1F *histPed = *mHistPedVecIter;
            elecIdx = std::distance( mHistPedVec.begin(), mHistPedVecIter );

            short timebin = elecIdx % kIstNumTimeBins;
            int chanIdx   = elecIdx / kIstNumTimeBins;
            
            if(histPed)  {
                int   entries = histPed->GetEntries();
                float meanPed = histPed->GetMean();
		float rmsPed  = histPed->GetRMS(); 
		float gain    = 1.0; // to be defined later
                float meanPedErr=0., rmsPedErr=0.;

                if( entries == 0 || rmsPed > mChanMaxRmsNoiseLevel)   {
		    meanPed = 0.;
                    rmsPed  = 1000.;
		}
                else    {
                    histPed->GetXaxis()->SetRangeUser(meanPed-mPedCut*rmsPed, meanPed+mPedCut*rmsPed);
                    meanPed    = histPed->GetMean();
                    meanPedErr = histPed->GetMeanError();
                    rmsPed     = histPed->GetRMS();
                    rmsPedErr  = histPed->GetRMSError();
                }

                pedNoiseData_t &data = mPedVec[ elecIdx ];
		data.n    = entries;
                data.ped  = meanPed;
                data.rms  = rmsPed;
                data.gain = gain;
                
                hist_meanPed[timebin]->SetBinContent(chanIdx+1, meanPed);
                hist_meanPed[timebin]->SetBinError(chanIdx+1, meanPedErr);
                hist_rmsPed[timebin]->SetBinContent(chanIdx+1, rmsPed);
                hist_rmsPed[timebin]->SetBinError(chanIdx+1, rmsPedErr);
                hist_gain[timebin]->SetBinContent(chanIdx+1, gain);
            }
        }

	for( mHistCmnVecIter = mHistCmnVec.begin(); mHistCmnVecIter != mHistCmnVec.end(); ++mHistCmnVecIter ){
            TH1F *histCmn = *mHistCmnVecIter;
	    chipIdx = std::distance( mHistCmnVec.begin(), mHistCmnVecIter );

            short timebin = chipIdx % kIstNumTimeBins;
            int   apvIdx  = chipIdx / kIstNumTimeBins;

	    if(histCmn)  {
		int   entries = histCmn->GetEntries();
		float cmNoise = histCmn->GetRMS();
		float cmNoiseErr = 0.;

		if(entries == 0 || cmNoise > mApvMaxCmNoiseLevel)
		    cmNoise = 1000.;
		else	{
		    cmNoise    = histCmn->GetRMS();
		    cmNoiseErr = histCmn->GetRMSError();
		}

		cmNoiseData_t &data = mCmnVec[ chipIdx ];
                data.n    = entries;
                data.cmn  = cmNoise;

		hist_cmNoise[timebin]->SetBinContent(apvIdx+1, cmNoise);
                hist_cmNoise[timebin]->SetBinError(apvIdx+1, cmNoiseErr);
	    }
	}
        //saving histograms
        myRootFile->Write();
        myRootFile->Close();
        
        ierr = saveToFile();
   }

   return ierr;
};

// functions that actually do the saving
Int_t StIstCalibrationMaker::saveToFile()
{
    Int_t ierr = kStOk;

    if( !mPedTable.empty() ){
        std::ofstream fout_ped( mPedTable.data(), std::ios_base::out & std::ios_base::trunc );

        if( !fout_ped ){
            LOG_ERROR << "Error opening file '" << mPedTable << "'" << endm;
            ierr = kStFatal;
        }

        pedNoiseDataVec_t::iterator pedDataVecIter; 
        int idx = 0;
        for( pedDataVecIter = mPedVec.begin(); pedDataVecIter != mPedVec.end() && !ierr; ++pedDataVecIter, ++idx ){
            short timebin = idx % kIstNumTimeBins;
            int elecId  = idx / kIstNumTimeBins;

            fout_ped << elecId << ' ' << timebin << ' ' << pedDataVecIter->ped << ' ' << pedDataVecIter->rms << ' ' << pedDataVecIter->gain << endl;
        }
    };

    if( !mCmnTable.empty() ){
        std::ofstream fout_cmn( mCmnTable.data(), std::ios_base::out & std::ios_base::trunc );

        if( !fout_cmn ){
            LOG_ERROR << "Error opening file '" << mCmnTable << "'" << endm;
            ierr = kStFatal;
        }

        cmNoiseDataVec_t::iterator cmnDataVecIter;
        int idx = 0;
        for( cmnDataVecIter = mCmnVec.begin(); cmnDataVecIter != mCmnVec.end() && !ierr; ++cmnDataVecIter, ++idx ){
            short timebin = idx % kIstNumTimeBins;
            int apvId  = idx / kIstNumTimeBins;

            fout_cmn << apvId << ' ' << timebin << ' ' << cmnDataVecIter->cmn << endl;
        }
    };

   return ierr;
};

ClassImp( StIstCalibrationMaker );
