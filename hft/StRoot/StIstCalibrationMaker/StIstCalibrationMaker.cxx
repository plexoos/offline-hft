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
* Revision 1.16  2014/04/16 16:13:57  ypwang
* add rdo/arm/apv/channnel electronics information for produced data files
*
* Revision 1.15  2014/03/25 03:06:52  ypwang
* updates on Db table accessory method
*
* Revision 1.14  2014/03/24 15:55:07  ypwang
* minor updates due to returned const pointers in StIstDbMaker
*
* Revision 1.13  2014/03/17 21:27:03  ypwang
* add 2D histogram set for StIstCalibrationMaker
*
* Revision 1.12  2014/03/11 17:44:20  ypwang
* minor updates to add a flag mRunHist to choose whether run histogram calculation method
*
* Revision 1.11  2014/03/05 16:52:29  ypwang
* mathematical method added for pedestal/RMS calculation and minor updates done for histogram method
*
* Revision 1.3  2014/02/06 12:28:34  ypwang
* updating scripts
*
*
****************************************************************************
* StIstCalibrationMaker.cxx,v 1.0
* Revision 1.0 2013/11/04 16:20:30 Yaping
* Initial version
****************************************************************************/

#include <string>
#include "StEvent.h"
#include "StEvent/StEnumerations.h"
#include "StIOMaker/StIOMaker.h"
#include "StIstCalibrationMaker.h"
#include "StRoot/StIstUtil/StIstCollection.h"
#include "StRoot/StIstUtil/StIstRawHitCollection.h"
#include "StRoot/StIstUtil/StIstRawHit.h"
#include "StRoot/StIstDbMaker/StIstDbMaker.h"
#include "StRoot/StIstUtil/StIstConsts.h"

#include "tables/St_istMapping_Table.h"
#include "tables/St_istControl_Table.h"

#include "TMath.h"

const string StIstCalibrationMaker::sectionLabel[72]={  "1C","1B","1A","2C","2B","2A","3C","3B","3A","4C","4B","4A",
                                          		"5C","5B","5A","6C","6B","6A","7C","7B","7A","8C","8B","8A",
                                          		"9C","9B","9A","10C","10B","10A","11C","11B","11A","12C","12B","12A",
                                          		"13C","13B","13A","14C","14B","14A","15C","15B","15A","16C","16B","16A",
                                          		"17C","17B","17A","18C","18B","18A","19C","19B","19A","20C","20B","20A",
                                          		"21C","21B","21A","22C","22B","22A","23C","23B","23A","24C","24B","24A"};

// constructor
StIstCalibrationMaker::StIstCalibrationMaker( const char* name ) : StMaker( name ), mTimeBinMask(0xFF), mRunHist(true), mDoPedCut(true), evtIdx(0), mHasFinished(0), mIstDbMaker(0) {
    for(unsigned char iTb=0; iTb<kIstNumTimeBins; iTb++) {
	hist_meanPed[iTb] = NULL;
	hist_rmsPed[iTb]  = NULL;
	hist_cmNoise[iTb] = NULL;
	hist_sumPed[iTb] = NULL;
	hist_sumRms[iTb] = NULL;
	hist_sumCmn[iTb] = NULL;
	hist_adcSpectrum[iTb] = NULL;
    }

    mPedVec1stLoop.resize(kIstNumTimeBins*kIstNumElecIds);
    mRmsVec1stLoop.resize(kIstNumTimeBins*kIstNumElecIds);

    mMathPedVec.resize(kIstNumTimeBins*kIstNumElecIds);
    mMathRmsVec.resize(kIstNumTimeBins*kIstNumElecIds);
    mMathCouVec.resize(kIstNumTimeBins*kIstNumElecIds);
   
    if(mRunHist) mHistPedVec.assign( kIstNumTimeBins*kIstNumElecIds, (TH1S*)0 );
    mHistCmnVec.assign( kIstNumTimeBins*kIstNumApvs, (TH1F*)0 );
    mPedVec.resize( kIstNumTimeBins*kIstNumElecIds );    // set to all zeros
    mCmnVec.resize( kIstNumTimeBins*kIstNumApvs );    // set to all zeros
    mMappingVec.resize( kIstNumElecIds );
};

// deconstructor
StIstCalibrationMaker::~StIstCalibrationMaker()
{
    mPedVec1stLoop.clear();
    mRmsVec1stLoop.clear();

    mMathPedVec.clear();
    mMathRmsVec.clear();
    mMathCouVec.clear();

    if(mRunHist) 
	while (!mHistPedVec.empty()) delete mHistPedVec.back(), mHistPedVec.pop_back();

    while (!mHistCmnVec.empty()) delete mHistCmnVec.back(), mHistCmnVec.pop_back();

    mPedVec.clear();
    mCmnVec.clear();
    mMappingVec.clear();
};

// initialize
Int_t StIstCalibrationMaker::Init()
{
    Int_t ierr = kStOk;
    	
    char buffer[100];
    for(unsigned char iTB=0; iTB<kIstNumTimeBins; iTB++) {
        sprintf(buffer,"hist_meanPedestal_TimeBin%d",iTB);
        hist_meanPed[iTB] = new TH1F(buffer, buffer, kIstNumElecIds, 1, kIstNumElecIds+1);
	hist_meanPed[iTB]->SetStats(false);
	hist_meanPed[iTB]->GetXaxis()->SetTitle("Channel Geometry Index");
	hist_meanPed[iTB]->GetXaxis()->SetNdivisions(-72,false); //sections
	hist_meanPed[iTB]->GetYaxis()->SetTitle("ADC count");

        sprintf(buffer,"hist_rmsPedestal_TimeBin%d",iTB);
	hist_rmsPed[iTB] = new TH1F(buffer, buffer, kIstNumElecIds, 1, kIstNumElecIds+1);
	hist_rmsPed[iTB]->SetStats(false);
	hist_rmsPed[iTB]->GetXaxis()->SetTitle("Channel Geometry Index");
        hist_rmsPed[iTB]->GetXaxis()->SetNdivisions(-72,false);//sections
        hist_rmsPed[iTB]->GetYaxis()->SetTitle("ADC count");

        sprintf(buffer,"hist_cmNoise_TimeBin%d",iTB);
        hist_cmNoise[iTB] = new TH1F(buffer, buffer, kIstNumApvs, 1, kIstNumApvs+1);
	hist_cmNoise[iTB]->SetStats(false);
	hist_cmNoise[iTB]->GetXaxis()->SetTitle("APV Geometry Index");
        hist_cmNoise[iTB]->GetXaxis()->SetNdivisions(-72,false);//sections
        hist_cmNoise[iTB]->GetYaxis()->SetTitle("ADC count");

	for(int i=0; i<72; i++) {
	    TString binBuffer = "";
	    binBuffer = sectionLabel[i];
	    hist_meanPed[iTB]->GetXaxis()->SetBinLabel(i*1536+768, binBuffer);
	    hist_rmsPed[iTB]->GetXaxis()->SetBinLabel(i*1536+768, binBuffer);
            hist_cmNoise[iTB]->GetXaxis()->SetBinLabel(i*12+6, binBuffer);
	}

	sprintf(buffer,"hist_sumPedestal_TimeBin%d",iTB);
        hist_sumPed[iTB] = new TH1F(buffer, buffer, 128, 0, 4096);
        hist_sumPed[iTB]->SetStats(kTRUE);
        hist_sumPed[iTB]->GetXaxis()->SetTitle("Pedestal [ADC counts]");
        hist_sumPed[iTB]->GetYaxis()->SetTitle("Counts");

	sprintf(buffer,"hist_sumRmsNoise_TimeBin%d",iTB);
        hist_sumRms[iTB] = new TH1F(buffer, buffer, 128, 0, 256);
        hist_sumRms[iTB]->SetStats(kTRUE);
        hist_sumRms[iTB]->GetXaxis()->SetTitle("RMS Noise [ADC counts]");
        hist_sumRms[iTB]->GetYaxis()->SetTitle("Counts");

	sprintf(buffer,"hist_sumCmNoise_TimeBin%d",iTB);
        hist_sumCmn[iTB] = new TH1F(buffer, buffer, 128, 0, 32);
        hist_sumCmn[iTB]->SetStats(kTRUE);
        hist_sumCmn[iTB]->GetXaxis()->SetTitle("CM Noise [ADC counts]");
        hist_sumCmn[iTB]->GetYaxis()->SetTitle("Counts");

	sprintf(buffer,"hist_adcSpectrum_TimeBin%d",iTB);
        hist_adcSpectrum[iTB] = new TH2S(buffer, buffer, kIstNumElecIds, 0, kIstNumElecIds, 512, 0, 4096);
        hist_adcSpectrum[iTB]->SetStats(false);
        hist_adcSpectrum[iTB]->GetXaxis()->SetTitle("Channel Electronics Index");
        hist_adcSpectrum[iTB]->GetYaxis()->SetTitle("ADC counts");
    }  

    //read the pedestal/rms value from 1st loop for signal-like channel excluding in 2nd loop
    if(mDoPedCut) {
	TString infile = "istPedNoiseTable.dat";
    	std::ifstream in(infile);
    	if (!in.is_open()) {
       	    mDoPedCut=false;
            LOG_WARN << "Could not find istPedNoiseTable.dat! Set mDoPedCut to false" << endm;
        } else {
            LOG_INFO << "Read Pedestal and RMS from istPedNoiseTable.dat!"<< endm;
	    int chElecId, rdo, arm, apv, chan, chTimeBin, chCode;
	    float chPed, chRms;
	    while(!in.eof()) {
	        in >> chElecId >> rdo >> arm >> apv >> chan >> chTimeBin >> chPed >> chRms;

	        chCode = kIstNumTimeBins * chElecId + chTimeBin;
	        mPedVec1stLoop[chCode] = chPed;
	        mRmsVec1stLoop[chCode]  = chRms;
           }
	}
	in.close();
    }

    mIstDbMaker = (StIstDbMaker*)GetMaker("istDb");
    if(ierr || !mIstDbMaker) {
        LOG_WARN << "Error getting IST Db maker handler" << endm;
        ierr = kStWarn;
    }

    return ierr;
};

Int_t StIstCalibrationMaker::InitRun(Int_t runnumber)
{
   Int_t ierr = kStOk;

   //control parameter
   const TDataSet *dbControl = mIstDbMaker->GetControl();
   St_istControl *istControl = 0;
   istControl = (St_istControl *)dbControl->Find("istControl");
   if(!istControl) {
	LOG_ERROR << "Dataset does not contain IST control table!" << endm;
	ierr = kStErr;
   }
   else {
   	istControl_st *istControlTable = istControl->GetTable() ;
   	if (!istControlTable)  {
	    LOG_ERROR << "Pointer to IST control table is null" << endm;
	    ierr = kStErr;
	}
	else
   	    mPedCut = istControlTable[0].kIstPedCutDefault;
   }

   //channel mapping
   const TDataSet *dbMapping = mIstDbMaker->GetMapping();
   St_istMapping *istMapping = 0;
   istMapping = (St_istMapping *)dbMapping->Find("istMapping");
   if(!istMapping) {
	LOG_ERROR << "Dataset does not contain IST mapping table!" << endm;
        ierr = kStErr;
   }
   else {
   	istMapping_st *gM = istMapping->GetTable();
   	if( !gM ) {
       	    LOG_ERROR << "Pointer to IST mapping table is null" << endm;
       	    ierr = kStErr;
   	}
	else {
   	    for(int i=0; i<kIstNumElecIds; i++) {
       		LOG_DEBUG<<Form(" Print entry %d : geoId=%d ",i,gM[0].mapping[i])<<endm;
       		mMappingVec[i] = gM[0].mapping[i];
   	    }
	}
   }

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
        double cmNoisePerEvent[kIstNumApvs][kIstNumTimeBins];
	for(int i=0; i<kIstNumApvs; i++)	{
	    for(int j=0; j<kIstNumTimeBins; j++)  {
		sumAdcPerEvent[i][j] = 0;
		channelCountsPerEvent[i][j] = 0;
		cmNoisePerEvent[i][j] = 0.;
	    }
	}

        for( unsigned char ladderIdx=0; ladderIdx<kIstNumLadders; ++ladderIdx ){
            StIstRawHitCollection *rawHitCollectionPtr = istCollectionPtr->getRawHitCollection( ladderIdx );

            if( rawHitCollectionPtr ){
                std::vector<StIstRawHit*>& rawHitVec = rawHitCollectionPtr->getRawHitVec();
                std::vector< StIstRawHit* >::iterator rawHitIter;

                for( rawHitIter = rawHitVec.begin(); rawHitIter != rawHitVec.end(); ++rawHitIter ){
		    int elecId = (*rawHitIter)->getChannelId();
		    int geoId  = (*rawHitIter)->getGeoId();		//channel geometry Id, counting from 1 to 110592
                    int apvId  = 1 + (geoId - 1)/kIstNumApvChannels;  	//APV chip geometry Id, counting from 1 to 864
		    if(elecId >= kIstNumElecIds ||  geoId > kIstNumElecIds || apvId > kIstNumApvs)	
			continue;

                    for( unsigned char timeBin = 0; timeBin < kIstNumTimeBins; ++timeBin ){
			Int_t adc = (*rawHitIter)->getCharge( timeBin );			
			if(adc)	{
                            int t = (int)timeBin;
                            if(mTimeBinMask==0) t=0;
			    int code = kIstNumTimeBins * elecId + t; 

			    //exclude signal-like channels time bin by time bin in current event
			    bool pass = kTRUE;
			    if(mDoPedCut) {
				pass = (adc > (mPedVec1stLoop[code]-mPedCut*mRmsVec1stLoop[code])) && (adc < (mPedVec1stLoop[code]+mPedCut*mRmsVec1stLoop[code]));
				hist_adcSpectrum[t]->Fill(elecId, adc);
			    }
			    if(!pass) continue;

			    sumAdcPerEvent[apvId-1][t] += adc;
			    channelCountsPerEvent[apvId-1][t] ++;

			    mMathPedVec[code] += adc;
                            mMathRmsVec[code] += adc*adc;
                            mMathCouVec[code] ++;

                            if(mRunHist) {
			        TH1S* histPed = mHistPedVec[ code ];
                                if( !histPed ){
                             	    ss.str("");
                             	    ss.clear();
                             	    ss << "hist_Pedestal_Ch" << code / kIstNumTimeBins << "_TB" << code % kIstNumTimeBins;
                             	    histPed = new TH1S( ss.str().data(), "", 512, 0, kIstMaxAdc );
                             	    mHistPedVec[ code ] = histPed;
                                }
                                histPed->Fill( adc );
                            }
			}//adc cut
                    }//time bin loop
                }//raw hits loop
		//clear raw hit objects in vector
		while (!rawHitVec.empty()) delete rawHitVec.back(), rawHitVec.pop_back();
            }//raw hit collection cut
        }//ladderIdx loop

	//common mode calculation per event
	for(int i=0; i<kIstNumApvs; i++) {
            for(int j=0; j<kIstNumTimeBins; j++)  {
		if( sumAdcPerEvent[i][j] )	{
		    cmNoisePerEvent[i][j] = (double)sumAdcPerEvent[i][j] / channelCountsPerEvent[i][j];
		    int code = kIstNumTimeBins * i + j;
		    TH1F* histCmn = mHistCmnVec[ code ];

		    if( !histCmn ){
		        sc.str("");
                        sc.clear();
                        sc << "hist_CMNoise_APV" << code / kIstNumTimeBins << "_TB" << code % kIstNumTimeBins;
                        histCmn = new TH1F( sc.str().data(), "", 512, 0, kIstMaxAdc);
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

	//calculate pedestal/RMS with mathematical method
	for(int i=0; i<kIstNumTimeBins*kIstNumElecIds; i++) {
	    double mathPed, mathRms;
	    if(mMathCouVec[i] < 1) {
		mathPed = 0.;
		mathRms = 100.;
	    }
	    else {
	    	mathPed = mMathPedVec[i]/mMathCouVec[i];
	    	mathRms = mMathRmsVec[i]/mMathCouVec[i];
	    }

	    double variance = mathRms - mathPed*mathPed;
	    if(variance > 0)
	    	mathRms = sqrt( variance );
	    else
		mathRms = 100.;

	    mMathPedVec[i] = mathPed;
	    mMathRmsVec[i] = mathRms;
	}

	//calculate pedestal/RMS with histogram method
        if(mRunHist) {
            std::vector< TH1S* >::iterator mHistPedVecIter;
            int elecIdx = 0;

            for( mHistPedVecIter = mHistPedVec.begin(); mHistPedVecIter != mHistPedVec.end(); ++mHistPedVecIter ){
                TH1S *histPed = *mHistPedVecIter;
                elecIdx = std::distance( mHistPedVec.begin(), mHistPedVecIter );
                short timebin = elecIdx % kIstNumTimeBins;
                int chanIdx   = elecIdx / kIstNumTimeBins;
                if(histPed)  {
                    int   entries = histPed->GetEntries();
                    float meanPed = histPed->GetMean();
		    float rmsPed  = histPed->GetRMS(); 

		    if( entries == 0 ) {
		        meanPed = 0.;
                        rmsPed  = 100.; //marked as dead channel
		    }

                    pedNoiseData_t &data = mPedVec[ elecIdx ];
		    data.n    = entries;
                    data.ped  = meanPed;
                    data.rms  = rmsPed;

		    if(mDoPedCut) {//only fill histograms in the 2nd loop
			int geomIdx  = mMappingVec[chanIdx];
                        hist_meanPed[timebin]->SetBinContent(geomIdx, meanPed);
                        hist_meanPed[timebin]->SetBinError(geomIdx, 0.);
                        hist_rmsPed[timebin]->SetBinContent(geomIdx, rmsPed);
                        hist_rmsPed[timebin]->SetBinError(geomIdx, 0.);
                        hist_sumPed[timebin]->Fill( meanPed );
                        hist_sumRms[timebin]->Fill( rmsPed );
		    }
                }
            }
        }

        std::vector< TH1F* >::iterator mHistCmnVecIter;
        int chipIdx = 0;
	for( mHistCmnVecIter = mHistCmnVec.begin(); mHistCmnVecIter != mHistCmnVec.end(); ++mHistCmnVecIter ){
            TH1F *histCmn = *mHistCmnVecIter;
	    chipIdx = std::distance( mHistCmnVec.begin(), mHistCmnVecIter );

            short timebin = chipIdx % kIstNumTimeBins;
            int   apvIdx  = chipIdx / kIstNumTimeBins;

	    if(histCmn)  {
		int   entries = histCmn->GetEntries();
		float cmNoise = histCmn->GetRMS();

		if(entries == 0) {
		    cmNoise = 100.; //marked as dead chip
		}

		cmNoiseData_t &data = mCmnVec[ chipIdx ];
                data.n    = entries;
                data.cmn  = cmNoise;

		if(mDoPedCut) {
		    hist_cmNoise[timebin]->SetBinContent(apvIdx+1, cmNoise);
		    hist_cmNoise[timebin]->SetBinError(apvIdx+1, 0.);
		    hist_sumCmn[timebin]->Fill(cmNoise);
		}
	    }
	}
        
        ierr = saveToFile();
   }

   return ierr;
};

// functions that actually do the saving
Int_t StIstCalibrationMaker::saveToFile()
{
    Int_t ierr = kStOk;

    //create output file
    StIOMaker *ioMaker = (StIOMaker * )GetMaker("inputStream");
    if (!ioMaker) {
        gMessMgr->Warning() << "StIstCalibrationMaker::Init(): No StIOMaker" << endm;
    }

    TString filename = TString(ioMaker->GetFile());
    int found = filename.Last('/');
    if(found >= 0){
        filename.Replace(0, found + 1, "");
    }
    found = filename.First(".");
    if(found == 0) found = filename.Length();

    TString mRootFilename = filename.Data();
    TString mPedNoiseFilename_hist = filename.Data();
    TString mCmNoiseFilename = filename.Data();    
    TString mPedNoiseFilename_math = filename.Data();

    mRootFilename.Replace(found, mRootFilename.Length() - found, ".istCaliQa.root");
    LOG_INFO << "IST Calibration QA File Name: " << mRootFilename << endm;

    //create QA file
    myRootFile = new TFile(mRootFilename.Data(),"RECREATE");
    if( !myRootFile ) {
        LOG_WARN << "Error recreating file '" << mRootFilename << "'" << endl;
        ierr = kStWarn;
    }
    //save calibration QA histograms
    for(unsigned char iTB=0; iTB<kIstNumTimeBins; iTB++) {
    	myRootFile->WriteTObject(hist_meanPed[iTB]);
    	myRootFile->WriteTObject(hist_rmsPed[iTB]);
    	myRootFile->WriteTObject(hist_cmNoise[iTB]);
	myRootFile->WriteTObject(hist_sumPed[iTB]);
	myRootFile->WriteTObject(hist_sumRms[iTB]);
	myRootFile->WriteTObject(hist_sumCmn[iTB]);
	myRootFile->WriteTObject(hist_adcSpectrum[iTB]);
    }
    myRootFile->Close();

    //save pedestal and rms noise with mathematical method
    mPedNoiseFilename_math.Replace(found, mPedNoiseFilename_math.Length() - found, ".istPedNoise_math.dat");
    LOG_INFO << "IST Pedestal and RMS File Name using math method: " << mPedNoiseFilename_math << endm;

    std::ofstream fout_ped_math( mPedNoiseFilename_math.Data(), std::ios_base::out & std::ios_base::trunc );
    if( !fout_ped_math ){
        LOG_ERROR << "Error opening file '" << mPedNoiseFilename_math << "'" << endm;
        ierr = kStFatal;
    }
    fout_ped_math.setf(std::ios::fixed, std::ios::floatfield);
    fout_ped_math.precision(5);

    for(int i=0; i<kIstNumTimeBins*kIstNumElecIds; i++) {
	short timebin = i % kIstNumTimeBins;
        int elecId  = i / kIstNumTimeBins;
	//obtain rdo/arm/apv/chan
	int rdo = 0, arm = -1, apv = -1, chan = -1;
	rdo = 1 + elecId/(kIstNumArmsPerRdo*kIstNumChanPerArm);
	arm = (elecId%(kIstNumArmsPerRdo*kIstNumChanPerArm))/kIstNumChanPerArm;
	apv = ((elecId%(kIstNumArmsPerRdo*kIstNumChanPerArm))%kIstNumChanPerArm)/kIstNumApvChannels;
	chan = ((elecId%(kIstNumArmsPerRdo*kIstNumChanPerArm))%kIstNumChanPerArm)%kIstNumApvChannels;
	fout_ped_math << elecId << ' ' << rdo << ' ' << arm << ' ' << apv << ' ' << chan << ' ' << timebin << ' ' << mMathPedVec[i] << ' ' << mMathRmsVec[i] << endl;
    }
    fout_ped_math.close();

    //create a link as istPedNoiseTable.dat in 1st loop
    if(!mDoPedCut) {
        char cmd[128];
        sprintf(cmd, "/bin/ln -f -s %s istPedNoiseTable.dat", mPedNoiseFilename_math.Data());
        system(cmd);
    }

    //save pedestal and rms noise with histogram method
    if(mRunHist) {
        mPedNoiseFilename_hist.Replace(found, mPedNoiseFilename_hist.Length() - found, ".istPedNoise_hist.dat");
        LOG_INFO << "IST Pedestal and RMS File Name using histogram method: " << mPedNoiseFilename_hist << endm;

        std::ofstream fout_ped_hist( mPedNoiseFilename_hist.Data(), std::ios_base::out & std::ios_base::trunc );
        if( !fout_ped_hist ){
            LOG_ERROR << "Error opening file '" << mPedNoiseFilename_hist << "'" << endm;
            ierr = kStFatal;
        }
        fout_ped_hist.setf(std::ios::fixed, std::ios::floatfield);
        fout_ped_hist.precision(5);

        pedNoiseDataVec_t::iterator pedDataVecIter;
        int idx = 0;
        for( pedDataVecIter = mPedVec.begin(); pedDataVecIter != mPedVec.end() && !ierr; ++pedDataVecIter, ++idx ){
            short timebin = idx % kIstNumTimeBins;
            int elecId  = idx / kIstNumTimeBins;
	    //obtain rdo/arm/apv/chan
	    int rdo = 0, arm = -1, apv = -1, chan = -1;
            rdo = 1 + elecId/(kIstNumArmsPerRdo*kIstNumChanPerArm);
            arm = (elecId%(kIstNumArmsPerRdo*kIstNumChanPerArm))/kIstNumChanPerArm;
            apv = ((elecId%(kIstNumArmsPerRdo*kIstNumChanPerArm))%kIstNumChanPerArm)/kIstNumApvChannels;
            chan = ((elecId%(kIstNumArmsPerRdo*kIstNumChanPerArm))%kIstNumChanPerArm)%kIstNumApvChannels;

            fout_ped_hist << elecId << ' ' << rdo << ' ' << arm << ' ' << apv << ' ' << chan << ' ' << timebin << ' ' << pedDataVecIter->ped << ' ' << pedDataVecIter->rms << endl;
        }
        fout_ped_hist.close();
    }

    //save common mode noise
    mCmNoiseFilename.Replace(found, mCmNoiseFilename.Length() - found, ".istCmNoise.dat");
    LOG_INFO << "IST Common Mode Noise File Name: " << mCmNoiseFilename << endm;

    std::ofstream fout_cmn( mCmNoiseFilename.Data(), std::ios_base::out & std::ios_base::trunc );
    if( !fout_cmn ){
        LOG_ERROR << "Error opening file '" << mCmNoiseFilename << "'" << endm;
        ierr = kStFatal;
    }
    fout_cmn.setf(std::ios::fixed, std::ios::floatfield);
    fout_cmn.precision(5);

    cmNoiseDataVec_t::iterator cmnDataVecIter;
    int idx = 0;
    for( cmnDataVecIter = mCmnVec.begin(); cmnDataVecIter != mCmnVec.end() && !ierr; ++cmnDataVecIter, ++idx ){
        short timebin = idx % kIstNumTimeBins;
        int apvId  = idx / kIstNumTimeBins;
	int ladderGeomId = 1 + apvId/kIstApvsPerLadder;
	int ladderElecId = -1;
	if(ladderGeomId<23)	ladderElecId = 22 - ladderGeomId;
	else			ladderElecId = 46 - ladderGeomId;
	int apvElecIdxOnLadder = 35 - apvId%kIstApvsPerLadder;
	int apvElecId = ladderElecId*kIstApvsPerLadder + apvElecIdxOnLadder;
	int rdo = 1 + apvElecId/(kIstNumArmsPerRdo*kIstNumApvsPerArm);
	int arm = (apvElecId%(kIstNumArmsPerRdo*kIstNumApvsPerArm))/kIstNumApvsPerArm;
	int apv = (apvElecId%(kIstNumArmsPerRdo*kIstNumApvsPerArm))%kIstNumApvsPerArm;	

        fout_cmn << apvId << ' ' << rdo << ' ' << arm << ' '<< apv << ' ' << timebin << ' ' << cmnDataVecIter->cmn << endl;
    }
    fout_cmn.close();

   return ierr;
};

ClassImp( StIstCalibrationMaker );
