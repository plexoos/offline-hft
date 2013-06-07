/*!
 * \class  StPxlMonMaker
 * \author Shusu Shi, LBNL
 * \date   May 2013
 *
 *
 *
 * $Id$
 *
 */
#include <iostream>
#include <fstream>
#include "StPxlMonMaker.h"
#include "StEventTypes.h"
#include "StMessMgr.h"
#include "StDcaGeometry.h"
#if ROOT_VERSION_CODE < 334081
#include "TArrayL.h"
#else
#include "TArrayL64.h"
#endif
#include "TClassTable.h"
#include "TH2F.h"
#include "TProfile.h"
#include "TNtuple.h"
#include "StThreeVectorF.hh"
#include "StDetectorName.h"
#include "StPxlUtil/StPxlCluster.h"
#include "StPxlUtil/StPxlClusterCollection.h"
#include "StPxlUtil/StPxlRawHit.h"
#include "StPxlUtil/StPxlRawHitCollection.h"
//#include "StTimer.hh"
#include "StIOMaker/StIOMaker.h"
ClassImp(StPxlMonMaker);

/// The constructor. Initialize you data members here.
StPxlMonMaker::StPxlMonMaker(const Char_t *name) : StMaker(name)
{}

Int_t StPxlMonMaker::Init()
{
	mEventCounter = 0;
	NtupleWrite = 0;
	LOG_INFO<<"StPxlMonMaker::Init()"<<endm;
        //Declareplots();
	return kStOk; 
}
Int_t StPxlMonMaker::Declareplots()
{
	if(NtupleWrite){
		rawHitTree = new TTree("rawHitTree","rawHitTree");
		rawHitTree->Branch("Event", &fillrawHit.eventid, "eventid/I:nrawhits");
		rawHitTree->Branch("sensorid",fillrawHit.sensorid,"sensorid[nrawhits]/S");
		rawHitTree->Branch("column",fillrawHit.column,"column[nrawhits]/S");
		rawHitTree->Branch("row",fillrawHit.row,"row[nrawhits]/S");
                rawHitTree->Branch("idtruth",fillrawHit.idtruth,"idtruth[nrawhits]/I");
                HitTree = new TTree("HitTree","HitTree");
                HitTree->Branch("Event",&fillHit.eventid,"eventid/I:nhits");
                HitTree->Branch("sensorid",fillHit.sensorid,"sensorid[nhits]/S");	
                HitTree->Branch("localx",fillHit.localx,"localx[nhits]/F");
                HitTree->Branch("localy",fillHit.localy,"localy[nhits]/F");
                HitTree->Branch("localz",fillHit.localz,"localz[nhits]/F");
		HitTree->Branch("globalx",fillHit.globalx,"globalx[nhits]/F");
		HitTree->Branch("globaly",fillHit.globaly,"globaly[nhits]/F");
		HitTree->Branch("globalz",fillHit.globalz,"globalz[nhits]/F");
                HitTree->Branch("meanrow",fillHit.meanrow,"meanrow[nhits]/F");
                HitTree->Branch("meancolumn",fillHit.meancolumn,"meancolum[nhits]/F");
                HitTree->Branch("layer",fillHit.layer,"layer[nhits]/S");
                HitTree->Branch("hitnrawhits",fillHit.hitnrawhits,"hitnrawhits[nhits]/S");
	}	
	nRawHits_sensorID = new TH2F("nRawHits_sensorID", "The number of RawHits vs. sensorID", 400, 1, 401, 1200, 0,1200);
	nRawHits_sensorID->GetXaxis()->SetTitle("Sensor ID");
	nRawHits_sensorID->GetYaxis()->SetTitle("nRawHits");

	nHits_sensorID = new TH2F("nHits_sensorID", "The number of Hits vs. sensorID", 400, 1, 401, 1200, 0, 1200);
	nHits_sensorID->GetXaxis()->SetTitle("Sensor ID");
	nHits_sensorID->GetYaxis()->SetTitle("nHits");
	hitnRawHits_sensorID = new TH2F("hitnRawHits_sensorID", "The number of RawHits per hit vs. sensorID", 400, 1, 401, 32, 0, 32);
	hitnRawHits_sensorID->GetXaxis()->SetTitle("Sensor ID");
	hitnRawHits_sensorID->GetYaxis()->SetTitle("RawHits per hit");
	for(int i=0; i< 10; i++)
	{
		if(i==1||i==3||i==6){
			char ename[50];
			char etitle[100];
			sprintf(ename, "nRawHits_eachsector_sensorID_%d", i);
			sprintf(etitle, "RawHits vs. sensorNo: Sector %d", i+1);
			nRawHits_eachsector_sensorID[i] = new TH2F(ename, etitle, 40, 1, 41, 1200, 0, 1200);
			nRawHits_eachsector_sensorID[i]->GetXaxis()->SetTitle("Sensor No.");
			nRawHits_eachsector_sensorID[i]->GetYaxis()->SetTitle("nRawHits");

			sprintf(ename, "nHits_eachsector_sensorID_%d", i);
			sprintf(etitle, "Hits vs. sensorNo: Sector %d", i+1);
			nHits_eachsector_sensorID[i] = new TH2F(ename, etitle, 40, 1, 41, 1200, 0, 1200);
			nHits_eachsector_sensorID[i]->GetXaxis()->SetTitle("Sensor No.");
			nHits_eachsector_sensorID[i]->GetYaxis()->SetTitle("nHits");

			sprintf(ename, "hitnRawHits_eachsector_sensorID_%d", i);
			sprintf(etitle, "RawHits per hit vs. sensorNo: Sector %d", i+1);
			hitnRawHits_eachsector_sensorID[i] = new TH2F(ename, etitle, 40, 1, 41, 32, 0, 32);
			hitnRawHits_eachsector_sensorID[i]->GetXaxis()->SetTitle("Sensor No.");
			hitnRawHits_eachsector_sensorID[i]->GetYaxis()->SetTitle("RawHits per hit");
			
                        sprintf(ename, "innerhits_outerhits_%d", i);
			sprintf(etitle, "Inner hits vs. outer hits: Sector %d", i+1);
			innerhits_outerhits[i] = new TH2F(ename, etitle, 600,0,6000,1800,0,18000);
			innerhits_outerhits[i]->GetXaxis()->SetTitle("Inner hits");
			innerhits_outerhits[i]->GetYaxis()->SetTitle("Outer hits");

			sprintf(ename, "innerrawhits_outerrawhits_%d", i);
			sprintf(etitle, "Inner rawhits vs. outer rawhits: Sector %d", i+1);
			innerrawhits_outerrawhits[i] = new TH2F(ename, etitle,600,0,6000,1800,0,18000);
			innerrawhits_outerrawhits[i]->GetXaxis()->SetTitle("Inner rawhits");
			innerrawhits_outerrawhits[i]->GetYaxis()->SetTitle("Outer rawhits");	

		}
	}

	globalx_y  = new TH2F("globalx_y", "Global X vs. Y", 200, -10, 10, 200, -10, 10);
	globalx_y->GetXaxis()->SetTitle("Global X");
	globalx_y->GetYaxis()->SetTitle("Global Y");
	globalz = new TH1F("globalz", "Global Z", 200, -10, 10);
	globalz->GetXaxis()->SetTitle("Global Z");
	globalz->GetYaxis()->SetTitle("Counts");
	globalphi_z_inner = new TH2F("globalphi_z_inner", "Global #phi vs. global Z, inner", 100, -3.14159, 3.14159, 100, -10, 10);
	globalphi_z_inner->GetXaxis()->SetTitle("Global #phi");
	globalphi_z_inner->GetYaxis()->SetTitle("Global Z");
	globalphi_z_outer = new TH2F("globalphi_z_outer", "Global #phi vs. global Z, outer", 100, -3.14159, 3.14159, 100, -10, 10);
	globalphi_z_outer->GetXaxis()->SetTitle("Global #phi");
	globalphi_z_outer->GetYaxis()->SetTitle("Global Z");
	for(int i=0; i< 10; i++)
	{
		for(int j=0; j<4; j++)
		{
			for(int k=0; k<10; k++)
			{

				if(i==1||i==3||i==6){
					char ename[50];
					char etitle[100];
					sprintf(ename, "rawHit_rowvscolumn_%d", i*40+j*10+k);
					sprintf(etitle, "rawHit column vs. row: Sector %d Ladder %d Sensor %d", i+1, j+1, k+1);
					rawHit_rowvscolumn[i*40+j*10+k] = new TH2F(ename, etitle, 1000, 0, 1000, 1000, 0, 1000);
					rawHit_rowvscolumn[i*40+j*10+k]->GetXaxis()->SetTitle("column");
					rawHit_rowvscolumn[i*40+j*10+k]->GetYaxis()->SetTitle("row");

					sprintf(ename, "nRawHits_EventId_%d", i*40+j*10+k);
					sprintf(etitle, "rawHits vs. EventID: Sector %d Ladder %d Sensor %d", i+1, j+1, k+1);
                                        nRawHits_EventId[i*40+j*10+k] = new TProfile(ename, etitle, 500000, 0, 500000);
                                        nRawHits_EventId[i*40+j*10+k]->GetXaxis()->SetTitle("EventID (Time)");
                                        nRawHits_EventId[i*40+j*10+k]->GetYaxis()->SetTitle("rawhits");					

				}
			}
		}
	}

	return kStOk;
}
Int_t StPxlMonMaker::Finish() {
	f1->Write();
        f1->Close();
	//writeHistograms();
        gMessMgr->Info() << "StPxlMonMaker::Finish() "
		<< "Processed " << mEventCounter << " events." << endm;

	return kStOK;
}

Int_t StPxlMonMaker::Make() {
	mEventCounter++;  // increase counter
	if(mEventCounter==1)
	{        //Declareplots();
		StIOMaker *ioMaker = (StIOMaker* )GetMaker("inputStream");
		if (!ioMaker){
			gMessMgr->Warning() << "StPxlMonMaker::Init(): No StIOMaker" << endm;
		}

		TString filename = TString(ioMaker->GetFile());
		int found = filename.Last('/');
		if(found >= 0)
			filename.Replace(0, found+1, "");
		filename.ReplaceAll(".daq",".pxlQa.root");

		f1 = new TFile(filename, "RECREATE");

		Declareplots();
	}	

	StEvent* event;
	event = (StEvent *) GetInputDS("StEvent");
	if (!event){
		gMessMgr->Warning() << "StPxlMonMaker::Make : No StEvent" << endm;
		//return kStOK;        // if no event, we're done
	}

	TObjectSet* pxlClusterDataSet = (TObjectSet*)GetDataSet("pxlCluster");
	cout<<"StPxlMonMaker:: pxlClusterDataSet: "<<pxlClusterDataSet<<endl;
	if(pxlClusterDataSet)
	{
		StPxlClusterCollection* pxlClusterCollection = (StPxlClusterCollection*)pxlClusterDataSet->GetObject();
		cout<<"StPxlMonMaker:: pxlClusterCollection: "<<pxlClusterCollection<<endl;
	}
	PrintPixelHits();
        //if(mEventCounter%100==0) {writeHistograms();}
	return kStOK;
}

bool StPxlMonMaker::accept(StEvent* event)
{
	if(!event) return kFALSE;
	return event->primaryVertex();
}

bool StPxlMonMaker::accept(StTrack* track)
{
	return track && track->flag() >= 0;
}
//________________________________________________________________________________
void StPxlMonMaker::PrintStEvent(TString opt) {
	// opt = vpg => "v" print vertex, "p" and primary tracks, "g" print global tracks 

	StEvent* pEvent = (StEvent*) StMaker::GetChain()->GetInputDS("StEvent");
	if (!pEvent) return;
	cout << "Event: Run "<< pEvent->runId() << " Event No: " << pEvent->id() << endl;
	UInt_t NpVX = pEvent->numberOfPrimaryVertices();
	if (NpVX) {
		if (opt.Contains("v",TString::kIgnoreCase)) {
			for (UInt_t i = 0; i < NpVX; i++) {
				const StPrimaryVertex *vx = pEvent->primaryVertex(i);
				cout << Form("Vertex: %3i ",i) << *vx << endl;
				if (opt.Contains("p",TString::kIgnoreCase)) {
					UInt_t nDaughters = vx->numberOfDaughters();
					for (UInt_t j = 0; j < nDaughters; j++) {
						StPrimaryTrack* pTrack = (StPrimaryTrack*) vx->daughter(j);
						if (! pTrack) continue;
						cout << *pTrack << endl;
					}
				}
			}
		}
	} else {
		cout << "Event: Vertex Not Found" << endl;
	}
	if (opt.Contains("g",TString::kIgnoreCase)) {
		StSPtrVecTrackNode& trackNode = pEvent->trackNodes();
		UInt_t nTracks = trackNode.size();
		StTrackNode *node = 0;
		cout << " Global tracks " << endl;
		for (UInt_t  i=0; i < nTracks; i++) {
			node = trackNode[i]; if (!node) continue;
			StGlobalTrack* gTrack = static_cast<StGlobalTrack*>(node->track(global));
			cout << *gTrack << endl;
		} 
	}
}
//________________________________________________________________________________
void StPxlMonMaker::PrintVertex(UInt_t ivx) {
	// opt = vpg => "v" print vertex, "p" and primary tracks, "g" print global tracks 
	StEvent* pEvent = (StEvent*) StMaker::GetChain()->GetInputDS("StEvent");
	if (!pEvent) return;
	cout << "Event: Run "<< pEvent->runId() << " Event No: " << pEvent->id() << endl;
	UInt_t NpVX = pEvent->numberOfPrimaryVertices();
	if (NpVX) {
		for (UInt_t i = 0; i < NpVX; i++) {
			if (i != ivx) continue;
			const StPrimaryVertex *vx = pEvent->primaryVertex(i);
			cout << Form("Vertex: %3i ",i) << *vx << endl;
			UInt_t nDaughters = vx->numberOfDaughters();
			for (UInt_t j = 0; j < nDaughters; j++) {
				StPrimaryTrack* pTrack = (StPrimaryTrack*) vx->daughter(j);
				if (! pTrack) continue;
				cout << *pTrack << endl;
				StPtrVecHit hvec = pTrack->detectorInfo()->hits();
				for (UInt_t j=0; j<hvec.size(); j++) {// hit loop
					if (hvec[j]->detector() == kTpcId) {
						StTpcHit *tpcHit = static_cast<StTpcHit *> (hvec[j]);
						if (! tpcHit) continue;
						cout << *tpcHit << endl;
					} else {
						cout << *hvec[j] << endl;
					}
				}
			}
		}
	} else {
		cout << "Event: Vertex Not Found" << endl;
	}
}
//________________________________________________________________________________
void StPxlMonMaker::PrintPixelHits() {

	UInt_t i=0,k=0,l;
	StEvent* pEvent = (StEvent*) StMaker::GetChain()->GetInputDS("StEvent");
	//if (!pEvent) return;
	if(pEvent){
		StPrimaryVertex *primaryVertex = pEvent->primaryVertex();
		if ( primaryVertex) {
			const StThreeVectorF &primXYZ = primaryVertex->position();
			cout << "primaryVertex \t" << primXYZ.x() << "\t" << primXYZ.y() << "\t" << primXYZ.z() << endl;
		}
		else{
			cout<<"no primaryVertex found"<<endl;
		}
		StPxlHitCollection* PxlHitCollection = pEvent->pxlHitCollection();
		if (! PxlHitCollection) { cout << "No Pixel Hit Collection" << endl; return;}
		int counthits = 0;
                for(int i=0; i<PxlHitCollection->numberOfSectors(); i++)
		{
			int hitnumber_inner =0, hitnumber_outer=0;
			StPxlSectorHitCollection* sectorHitCollection = PxlHitCollection->sector(i);
			for(int j=0; j<sectorHitCollection->numberOfLadders(); j++)
			{
				StPxlLadderHitCollection* ladderHitCollection = sectorHitCollection->ladder(j);
				for(int k=0; k<ladderHitCollection->numberOfSensors(); k++)
				{
					if(i==1||i==3||i==6){		
						StPxlSensorHitCollection* sensorHitCollection = ladderHitCollection->sensor(k);
						for(int l=0; l<sensorHitCollection->hits().size(); l++)
						{
							StPxlHit* hit = sensorHitCollection->hits()[l];						
							if (hit) {
								const StThreeVectorF &P = hit->position();
								if(NtupleWrite){
									fillHit.sensorid[counthits] = i*40+j*10+k+1;
									fillHit.localx[counthits] = hit->localPosition(0);
									fillHit.localy[counthits] = hit->localPosition(1);
									fillHit.localz[counthits] = hit->localPosition(2);
									fillHit.globalx[counthits] = P.x();
									fillHit.globaly[counthits] = P.y();
									fillHit.globalz[counthits] = P.z();
									fillHit.meanrow[counthits] = hit->meanRow();
									fillHit.meancolumn[counthits] = hit->meanColumn();
									fillHit.layer[counthits] = hit->layer();
									fillHit.hitnrawhits[counthits] = hit->nRawHits();
                                                                        counthits++;
								}							
								globalx_y->Fill((float)P.x(), (float)P.y());
								globalz->Fill((float)P.z());
								if(j==0) globalphi_z_inner->Fill(P.phi(),(float)P.z());
								if(j==1||j==2||j==3) globalphi_z_outer->Fill(P.phi(),(float)P.z());
								//hit_localZ_X[i*40+j*10+k]->Fill((float)hit->localPosition(2), (float)hit->localPosition(0));
								hitnRawHits_sensorID->Fill(i*40+j*10+k+1, (int)hit->nRawHits());
								hitnRawHits_eachsector_sensorID[i]->Fill(j*10+k+1, (int)hit->nRawHits());
							}
						}
						nHits_sensorID->Fill(i*40+j*10+k+1, sensorHitCollection->hits().size());
						nHits_eachsector_sensorID[i]->Fill(j*10+k+1, sensorHitCollection->hits().size());
						if(j==0) hitnumber_inner+=sensorHitCollection->hits().size();
						if(j==1||j==2||j==3) hitnumber_outer+=sensorHitCollection->hits().size();
					}			
				}
			}
			if(i==1||i==3||i==6) innerhits_outerhits[i]->Fill(hitnumber_inner, hitnumber_outer);
		}
		if(NtupleWrite){
			fillHit.eventid = pEvent->id();
			fillHit.nhits = counthits;
			HitTree->Fill();
		}
	}

	TObjectSet* pxlRawHitDataSet = (TObjectSet*)GetDataSet("pxlRawHit");
	if (! pxlRawHitDataSet) {
		LOG_WARN << "StPxlClusterMaker::Make there is no pxlRawHitDataSet " << endm;
		return;
	}

	StPxlRawHitCollection* pxlRawHitCollection = (StPxlRawHitCollection*)pxlRawHitDataSet->GetObject();
	if(!pxlRawHitCollection) {
		LOG_WARN << "StPxlClusterMaker::Make() no pxlRawHitCollection."<<endm;
		return;
	}
	int countrawhits = 0;
	for (int i=0; i<nPxlSectors; i++)
	{
		int rawhitnumber_inner =0, rawhitnumber_outer=0;
		for(int j=0; j<nPxlLaddersPerSector; j++)
		{
			for(int k=0; k<nPxlSensorsPerLadder; k++)
			{

				if(i==1||i==3||i==6){
					for(int l=0; l<pxlRawHitCollection->pxlRawHitVec[i][j][k].size(); l++)
					{
						StPxlRawHit* rawHit = pxlRawHitCollection->pxlRawHitVec[i][j][k][l];
						if(NtupleWrite){
							fillrawHit.sensorid[countrawhits] = i*40+j*10+k+1;
							fillrawHit.column[countrawhits]   = rawHit->column();
							fillrawHit.row[countrawhits]   = rawHit->row();
                                                        fillrawHit.idtruth[countrawhits] = rawHit->idTruth();
							countrawhits++;

						}
						rawHit_rowvscolumn[i*40+j*10+k]->Fill(rawHit->column(), rawHit->row());
					}
					nRawHits_sensorID->Fill(i*40+j*10+k+1, pxlRawHitCollection->pxlRawHitVec[i][j][k].size());
					nRawHits_eachsector_sensorID[i]->Fill(j*10+k+1, pxlRawHitCollection->pxlRawHitVec[i][j][k].size());
					nRawHits_EventId[i*40+j*10+k]->Fill((int)pEvent->id()+1,pxlRawHitCollection->pxlRawHitVec[i][j][k].size());					
					if(j==0) rawhitnumber_inner+=pxlRawHitCollection->pxlRawHitVec[i][j][k].size();
					if(j==1||j==2||j==3) rawhitnumber_outer+=pxlRawHitCollection->pxlRawHitVec[i][j][k].size();
				}		
			}
		}
		if(i==1||i==3||i==6) innerrawhits_outerrawhits[i]->Fill(rawhitnumber_inner, rawhitnumber_outer);
	}
	if(NtupleWrite){
		fillrawHit.eventid = pEvent->id();
		fillrawHit.nrawhits = countrawhits;
		rawHitTree->Fill();
	}
}

/*void StPxlMonMaker::writeHistograms()
{
        StIOMaker *ioMaker = (StIOMaker* )GetMaker("inputStream");
        if (!ioMaker){
                gMessMgr->Warning() << "StPxlMonMaker::Init(): No StIOMaker" << endm;
        }

        TString filename = TString(ioMaker->GetFile());
        int found = filename.Last('/');
        if(found >= 0)
            filename.Replace(0, found+1, "");
        filename.ReplaceAll(".daq",".pxlQa.root");

        f1 = new TFile(filename, "RECREATE");
        if(NtupleWrite){    f1->WriteTObject(HitTree); f1->WriteTObject(rawHitTree);}
        f1->WriteTObject(nRawHits_sensorID);
        for(int i=0; i< 10; i++)
        {
            if(i==1||i==3||i==6)f1->WriteTObject(nRawHits_eachsector_sensorID[i]);
        }

        f1->WriteTObject(nHits_sensorID);
        for(int i=0; i< 10; i++)
        {
            if(i==1||i==3||i==6)f1->WriteTObject(nHits_eachsector_sensorID[i]);
        }

        f1->WriteTObject(hitnRawHits_sensorID);
        for(int i=0; i< 10; i++)
        {
            if(i==1||i==3||i==6)f1->WriteTObject(hitnRawHits_eachsector_sensorID[i]);
        }

        f1->WriteTObject(globalx_y);
        f1->WriteTObject(globalz);
        f1->WriteTObject(globalphi_z_inner);
        f1->WriteTObject(globalphi_z_outer);
        for(int i=0; i< 10; i++)
        {
                if(i==1||i==3||i==6){
                    f1->WriteTObject(innerhits_outerhits[i]);
                    f1->WriteTObject(innerrawhits_outerrawhits[i]);
                }
        }
        for(int i=0; i< 10; i++)
        {
                for(int j=0; j<4; j++)
                {
                        for(int k=0; k<10; k++)
			{
				if(i==1||i==3||i==6){
					f1->WriteTObject(rawHit_rowvscolumn[i*40+j*10+k]);
					f1->WriteTObject(nRawHits_EventId[i*40+j*10+k]);
					//hit_localZ_X[i*40+j*10+k]->Write();
				}
			}
		}
	}
	//f1->Close();

}
*/
