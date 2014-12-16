#include <cstdlib>
#include <stdint.h>
#include <string>
#include <iomanip>
#include <iostream>
#include <fstream>

#include <boost/regex.hpp>

#include "tevedisp.h"

#include "TChain.h"
#include "TFile.h"
#include "TMath.h"
#include "TError.h"
#include "TEveTrack.h"
#include "TEveTrackPropagator.h"
#include "TGeoManager.h"
#include "TGeoNavigator.h"
#include "TEveBrowser.h"
#include "TEveManager.h"
#include "TRootBrowser.h"
#include "TEveGeoNode.h"
#include "TGTab.h"
#include "TGButton.h"
#include "TSystem.h"
#include "TApplication.h"

#include "StHftPool/EventT/EventT.h"
#include "StHftPool/EventT/GuiEventHandler.h"


// Constants
const Double_t kR_MAX = 250.;
const Double_t kZ_MAX = 250.;

const Double_t kR_TPC_min = 46;
const Double_t kR_TPC_max = 200;
const Double_t kZ_TPC_d   = 200;

const Double_t kR_HFT_min = 0;
const Double_t kR_HFT_max = 30;
const Double_t kZ_HFT_d   = 30;

const Double_t kR_BP_min = 2.00;
const Double_t kR_BP_max = 2.076;
const Double_t kZ_BP_d   = 30;

const Int_t NLADDER_PXL = 40;
const Int_t NLADDER_IST = 24;
const Int_t NLADDER_SSD = 20;

const Double_t kR_IST = 14.0;
const Double_t kR_IST_min = 12.0;
const Double_t kR_IST_max = 16.0;
const Double_t kZ_IST_d = 20;

// Solenoid field along z, in Tesla.
const Double_t kMagField = 0.498948;
const uintmax_t kPXL_Cluster_Min =  0; // Min cluster size to be accepted, i.e. nRawHits in cluster >= kPXL_Cluster_Min
const uintmax_t kPXL_Cluster_Max = 20; // Max cluster size to be accepted, i.e. nRawHits in cluster <= kPXL_Cluster_Max

// Draw Options
const Bool_t kPlotHFTSupport = false;
const Color_t  kColors[3] = { kRed, kGreen, kYellow };
const Double_t kTrackHitOffset = 0.5;

// sensor status
const Int_t NSEC_PXL = 10;
const Int_t NLAD_PXL = 4;
const Int_t NSEN_PXL = 10;
const Int_t NLAD_IST = 24;
const Int_t NSEN_IST = 6;

const Int_t N_PXL = NSEC_PXL * NLAD_PXL * NSEN_PXL;
const Int_t N_IST = NLAD_IST * NSEN_IST;

Int_t Status_PXL[N_PXL];
Int_t Status_IST[N_IST];


// Global variables
EventT        *eventT = 0;
TEveTrackList *gTrackList = 0;
TEvePointSet  *gVtxList = 0;
TEvePointSet  *gHitList = 0;
TEvePointSet  *gTrackHitList = 0;
TGeoManager   *gGeoManager = 0;

// These globals are not used but required since we link against TGiant3.o
int    Margc=0;
char** Margv=NULL;


int main(int argc, char **argv)
{
   const std::string hftTreeName = "t";

   PrgOptionProcessor poProc(argc, argv, hftTreeName);
   poProc.ProcessOptions();

   TApplication *myApp = new TApplication("myApp", 0, 0);

   hft_display(poProc, -1);

   myApp->Run();
   delete myApp;

   return EXIT_SUCCESS;
}


void hft_display(PrgOptionProcessor &poProc, const double B)
{
   TChain *myTreeFileChain = poProc.GetHftChain();

   std::cout << "Total number of events = " << myTreeFileChain->GetEntries() << std::endl;

   eventT = new EventT();
   myTreeFileChain->SetBranchAddress("e.", &eventT);

   // Create views and containers
   make_geometry(poProc);

   init(B);

   build_gui(*myTreeFileChain);
   process_event(*myTreeFileChain, gHftGuiEventCounter);

   gEve->Redraw3D(kTRUE);
}


void loadStatus(const int runnumber)
{
   // Read in the status - maybe moved int othe loop when loading multiple runs
   memset(Status_PXL, 0, sizeof(Status_PXL));
   memset(Status_IST, 0, sizeof(Status_IST));

   char inname[100];
   std::ifstream inData;
   sprintf(inname, "status/PXL_%d.txt", runnumber);
   inData.open(inname);

   for (int i = 0; i < N_PXL; i++) {
      inData >> Status_PXL[i];
      int id = i + 1;
      int sector, ladder, sensor;
      decodeId(id, &sector, &ladder, &sensor);

      if (Status_PXL[i]) std::cout << " Bad PXL Sensor # " << sector << "/" << ladder << "/" << sensor << std::endl;
   }

   inData.close();

   sprintf(inname, "status/IST_%d.txt", runnumber);
   inData.open(inname);

   for (int i = 0; i < N_IST; i++) {
      inData >> Status_IST[i];
      int id = i + 1 + 1000;
      int sector, ladder, sensor;
      decodeId(id, &sector, &ladder, &sensor);

      if (Status_IST[i]) std::cout << " Bad IST Sensor # " << ladder << "/" << sensor << std::endl;
   }

   inData.close();
}


void decodeId(int id, int *sector, int *ladder, int *sensor)
{
   if (id > 0 && id <= N_PXL) {
      *sector = (id - 1) / (NSEC_PXL * NLAD_PXL) + 1;
      *ladder = (id - 1) % (NSEC_PXL * NLAD_PXL) / NSEN_PXL + 1;
      *sensor = (id - 1) % NSEN_PXL + 1;
   }
   else if (id > 1000 && id <= 1000 + N_IST) {
      *sector = 0;
      *ladder = (id - 1000 - 1) / NSEN_IST + 1;
      *sensor = (id - 1000 - 1) % NSEN_IST + 1;
   }
}


/** Intitalize track/hit lists */
void init(const double B)
{
   gEve->GetBrowser()->GetTabRight()->SetTab(1);

   gTrackList = new TEveTrackList("Rec Tracks");
   gTrackList->SetMainColor(kYellow);
   gTrackList->SetMarkerColor(kRed);
   gTrackList->SetMarkerStyle(4);
   gTrackList->SetMarkerSize(0.5);
   gEve->AddElement(gTrackList);

   TEveTrackPropagator *trkProp = gTrackList->GetPropagator();
   trkProp->SetMagField(kMagField * B);
   // trkProp->SetMaxR(kR_TPC_max);
   // trkProp->SetMaxZ(kZ_TPC_d);
   trkProp->SetMaxR(kR_MAX);
   trkProp->SetMaxZ(kZ_MAX);

   gVtxList = new TEvePointSet("Primary Vertex");
   gVtxList->SetMainColor(kRed);
   gVtxList->SetMarkerColor(kYellow);
   gVtxList->SetMarkerStyle(20);
   gVtxList->SetMarkerSize(1.0);

   gHitList = new TEvePointSet("HFT Rec Hits");
   gHitList->SetMainColor(kRed);
   gHitList->SetMarkerColor(kWhite);
   gHitList->SetMarkerStyle(20);
   gHitList->SetMarkerSize(1.0);

   gTrackHitList = new TEvePointSet("Track Rec Hits");
   gTrackHitList->SetMainColor(kRed);
   gTrackHitList->SetMarkerColor(kYellow);
   gTrackHitList->SetMarkerStyle(20);
   gTrackHitList->SetMarkerSize(0.8);
}


/** Process next event */
void process_event(TChain &fhtree, Int_t iEvt)
{
   if (iEvt >= fhtree.GetEntriesFast()) {
      std::cout << "End of the tree! Go backward! " << std::endl;
      return;
   }
   else if (iEvt < 0) {
      std::cout << "Beginning of the tree! Go forward! " << std::endl;
      return;
   }

   std::cout << "Displaying " << gHftGuiEventCounter << "th entry...." << std::endl;
   fhtree.GetEntry(iEvt);

   gTrackList->DestroyElements();
   gVtxList->Reset();
   gHitList->Reset();
   gTrackHitList->Reset();

   int runId = eventT->fEvtHdr.fRun;
   int evtId = eventT->fEvtHdr.fEvtNum;

   std::cout << Form("run#%d event#%d", runId, evtId) << std::endl;

   // Load vertices and hits
   std::cout << " Event vertex = " << eventT->fVertex[0] << " " << eventT->fVertex[1] << " " << eventT->fVertex[2] << std::endl;
   //gVtxList->SetNextPoint(eventT->fVertex[0], eventT->fVertex[1], eventT->fVertex[2]);
   //gEve->AddElement(gVtxList);

   Int_t nHits = eventT->fHits->GetEntriesFast();

   for (int j = 0; j < nHits; j++) {

      HitT* hitT = (HitT*) eventT->fHits->At(j);

      int id = hitT->Id;

      //std::cout << "Adding a new hit " << id << " " <<  hitT->xG << " " << hitT->yG << " " << hitT->zG << std::endl;

      if (id < 1000 && ( hitT->nRawHits < kPXL_Cluster_Min ||  hitT->nRawHits > kPXL_Cluster_Max ) ) continue;
      if (id < 1000 && Status_PXL[id - 1]) continue; // remove noisy channels
      if (id > 1000 && Status_IST[id - 1 - 1000]) continue;

      gHitList->SetNextPoint(hitT->xG, hitT->yG, hitT->zG);
   }

   gEve->AddElement(gHitList);

   // Load tracks
   TEveTrackPropagator *trkProp = gTrackList->GetPropagator();
   Int_t nTracks = eventT->fTracks->GetEntriesFast();

   for (Int_t j = 0; j < nTracks; ++j) {

      TrackT* trackT = (TrackT*) eventT->fTracks->At(j);

      TEveVectorT<double> origin(trackT->fOriginXDca, trackT->fOriginYDca, trackT->fOriginZDca);
      TEveVectorT<double> mom(trackT->fPxDca, trackT->fPyDca, trackT->fPzDca);
      Int_t charge = (trackT->fNpoint > 0) ? +1 : -1;

      TEveRecTrackT<Double_t> tR;
      tR.fIndex = j;
      tR.fP = mom;
      tR.fV = origin;
      tR.fSign = charge;

      TEveTrack *track = new TEveTrack(&tR, trkProp);
      track->SetName(Form("%s [%d]", "rec", j));
      track->SetStdTitle();
      track->SetAttLineAttMarker(gTrackList);

      if (charge == +1)
         track->SetLineColor(kColors[0]);
      else
         track->SetLineColor(kColors[1]);

      //std::cout << "Adding a new track " << trackT->fPx << " " << trackT->fPy << " " << trackT->fPz << std::endl;
      gTrackList->AddElement(track);

      // add track hit

      for (int k = 0; k < 3; k++) {
         if (TMath::Sqrt(TMath::Power(trackT->fPxlHitX[k], 2) + TMath::Power(trackT->fPxlHitY[k], 2)) < 2.) continue;

         gTrackHitList->SetNextPoint(trackT->fPxlHitX[k] + kTrackHitOffset, trackT->fPxlHitY[k] + kTrackHitOffset, trackT->fPxlHitZ[k] + kTrackHitOffset);
      }

      for (int k = 0; k < 2; k++) {
         if (TMath::Sqrt(TMath::Power(trackT->fIstHitX[k], 2) + TMath::Power(trackT->fIstHitY[k], 2)) < 2.) continue;

         gTrackHitList->SetNextPoint(trackT->fIstHitX[k] + kTrackHitOffset, trackT->fIstHitY[k] + kTrackHitOffset, trackT->fIstHitZ[k] + kTrackHitOffset);
      }

      for (int k = 0; k < 2; k++) {
         if (TMath::Sqrt(TMath::Power(trackT->fSsdHitX[k], 2) + TMath::Power(trackT->fSsdHitY[k], 2)) < 2.) continue;

         gTrackHitList->SetNextPoint(trackT->fSsdHitX[k] + kTrackHitOffset, trackT->fSsdHitY[k] + kTrackHitOffset, trackT->fSsdHitZ[k] + kTrackHitOffset);
      }

      for (int k = 0; k < 45; k++) {
         if (TMath::Sqrt(TMath::Power(trackT->fTpcHitX[k], 2) + TMath::Power(trackT->fTpcHitY[k], 2)) < 2.) continue;

         gTrackHitList->SetNextPoint(trackT->fTpcHitX[k], trackT->fTpcHitY[k], trackT->fTpcHitZ[k]);
      }

      int nhits = abs(trackT->fNpoint);
      int nhits_tpc = nhits % 100;
      int nhits_ssd = (nhits % 1000) / 100;
      int nhits_ist = (nhits % 10000) / 1000;
      int nhits_pxl2 = (nhits % 100000) / 10000;
      int nhits_pxl1 = (nhits % 1000000) / 100000;

      std::cout << " Number of hits on PXL1/PXL2/IST/SSD/TPC = " << nhits_pxl1 << "/" << nhits_pxl2 << "/" << nhits_ist << "/" << nhits_ssd << "/" << nhits_tpc << std::endl;
   }

   gTrackList->MakeTracks();
   gEve->AddElement(gTrackHitList);

   gEve->SetStatusLine(Form("run#%d event#%d", runId, evtId));

   //TEveElement *top = (TEveElement *) gEve->GetCurrentEvent();

   gEve->Redraw3D();
}


void make_visible_select_volumes(TGeoNavigator &geoNav, PrgOptionProcessor &poProc)
{
   // First, make all volumes invisible
   make_visible_daughters(geoNav, poProc, false);

   // Then make select volumes visible
   make_visible_daughters(geoNav, poProc, true);
}


void make_visible_daughters(TGeoNavigator &geoNav, PrgOptionProcessor &poProc, bool usePattern)
{
   TGeoNode* currNode = geoNav.GetCurrentNode();

   if (!currNode) {
      Warning("make_visible_daughters", "Invalid TGeoNode provided as input. Skipping...");
      return;
   }

   // Keep track of current level with respect to the original node
   static int level = 0;

   if (!usePattern) {
      currNode->SetVisibility(false);
      currNode->VisibleDaughters(false);
   }

   std::string currentPath( geoNav.GetPath() );
   //Info("make_visible_daughters", "gmc path b: %s: level: %d", currentPath.c_str(), level);

   int nDaughters = currNode->GetVolume()->GetNdaughters();

   if (nDaughters) {

      TGeoNode* motherNode = currNode;
      for (int iDaughter = 0; iDaughter < nDaughters; iDaughter++)
      {
         TGeoNode *daughter = motherNode->GetVolume()->GetNode(iDaughter);

         geoNav.CdDown(daughter);
         level++;

         make_visible_daughters(geoNav, poProc, usePattern);
      }

   } else { // We are here if this node is a leaf, i.e. no daughters

      if (usePattern && poProc.MatchedVolName(currentPath) )
      {
         Info("make_visible_daughters", "Current path [%s] matched pattern", currentPath.c_str());
         currNode->SetVisibility(true);
         currNode->VisibleDaughters(true);

         make_visible_mothers(geoNav, level);

         geoNav.cd(currentPath.c_str());
      }
   }

   if (level > 0) {
      geoNav.CdUp();
      level--;
   }
}


void make_visible_mothers(TGeoNavigator &geoNav, int levelsUp)
{
   if (levelsUp < 0) return;

   std::string currentPath( geoNav.GetPath() );
   //Info("MPV", "curr path: %s level: %d", currentPath.c_str(), levelsUp);

   unsigned slash_last_pos = currentPath.find_last_of("/");
   currentPath.erase(slash_last_pos);
   levelsUp--;
   geoNav.cd(currentPath.c_str());

   TGeoNode *currNode = geoNav.GetCurrentNode();
   currNode->SetVisibility(true);
   currNode->VisibleDaughters(true);

   if (levelsUp >= 0) {
      make_visible_mothers(geoNav, levelsUp);
   }
}


void make_geometry(PrgOptionProcessor &poProc)
{
   TEveManager::Create();
   std::string myGeomTag("y2014a");

   TGeoManager::Import( Form("%s.root",myGeomTag.c_str()) );

   if (!gGeoManager) {
      Error("make_geometry", "No gGeoManager found");
      exit(EXIT_FAILURE);
   }

   TEveElementList *STAR = new TEveElementList("Geometry");

   gGeoManager->cd("HALL_1/CAVE_1");

   TGeoNode *myGeoNode = gGeoManager->GetCurrentNode();

   TGeoNavigator* geoNav = gGeoManager->GetCurrentNavigator();
   assert(geoNav);

   make_visible_select_volumes(*geoNav, poProc);

   TEveGeoTopNode *myTopNode = new TEveGeoTopNode(gGeoManager, myGeoNode);
   myTopNode->SetVisLevel(10);

   STAR->AddElement(myTopNode);

   gEve->AddGlobalElement(STAR);
}


/** GUI stuff */
void build_gui(TChain &hftree)
{
   // Create minimal GUI for event navigation.

   TEveBrowser *browser = gEve->GetBrowser();
   browser->StartEmbedding(TRootBrowser::kLeft);

   TGMainFrame *frmMain = new TGMainFrame(gClient->GetRoot(), 400, 400);
   frmMain->SetWindowName("XX GUI");
   frmMain->SetCleanup(kDeepCleanup);

   TGHorizontalFrame *hf = new TGHorizontalFrame(frmMain);

   TString icondir( Form("%s/icons/", gSystem->Getenv("ROOTSYS")) );
   TGPictureButton *b = 0;
   GuiEventHandler *fh = new GuiEventHandler(hftree);

   b = new TGPictureButton(hf, gClient->GetPicture(icondir + "GoBack.gif"));
   //b->SetEnabled(kFALSE);
   b->SetToolTipText("Go to previous event - not supported.");
   hf->AddFrame(b);
   b->Connect("Clicked()", "GuiEventHandler", fh, "Bck()");

   b = new TGPictureButton(hf, gClient->GetPicture(icondir + "GoForward.gif"));
   b->SetToolTipText("Generate new event.");
   hf->AddFrame(b);
   b->Connect("Clicked()", "GuiEventHandler", fh, "Fwd()");

   frmMain->AddFrame(hf);

   frmMain->MapSubwindows();
   frmMain->Resize();
   frmMain->MapWindow();

   browser->StopEmbedding();
   browser->SetTabTitle("Event Control", 0);
}
