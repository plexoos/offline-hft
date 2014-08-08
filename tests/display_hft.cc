#include <cstdlib>
#include <stdint.h>
#include <iomanip>
#include <iostream>
#include <fstream>
#include <string>

#include <boost/program_options/options_description.hpp>
#include <boost/program_options/variables_map.hpp>
#include <boost/program_options/parsers.hpp>

#include "TChain.h"
#include "TFile.h"
#include "TEveTrack.h"
#include "TEveTrackPropagator.h"
#include "TGeoManager.h"
#include "TEveBrowser.h"
#include "TEveManager.h"
#include "TRootBrowser.h"
#include "TEveGeoNode.h"
#include "TGTab.h"
#include "TGButton.h"
#include "TSystem.h"
#include "TApplication.h"
#include "StarVMC/Geometry/Geometry.h"
#include "StarVMC/StarAgmlLib/StarTGeoStacker.h"

#include "StHftPool/EventT/hft_display.h"
#include "StHftPool/EventT/EventT.h"

#include "/afs/rhic.bnl.gov/star/ROOT/5.34.09/.sl64_gcc447/rootdeb/tutorials/eve/MultiView.C"


TChain *myTreeFileChain = 0;
EventT *eventT = 0;

using namespace std;
namespace po = boost::program_options;


//==============================================================================
// Constants.
//------------------------------------------------------------------------------
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

//==============================================================================
// Global variables.
//------------------------------------------------------------------------------

// Implemented in MultiView.C
//class MultiView;

MultiView *gMultiView = 0;

TEveTrackList *gTrackList = 0;
TEvePointSet  *gVtxList = 0;
TEvePointSet  *gHitList = 0;
TEvePointSet  *gTrackHitList = 0;
Int_t NMAX = 0;
TGeoManager *gGeoManager = 0;

//==============================================================================
// Forward decalarations of CINT functions.
//------------------------------------------------------------------------------


//==============================================================================
// Main - hft_display()
//------------------------------------------------------------------------------

void process_options(po::options_description &prgOptions, po::variables_map &prgOptionValues);


int main(int argc, char **argv)
{
   std::string inputRootFile;

   po::options_description prgOptions;
   po::variables_map       prgOptionValues;

   prgOptions.add_options()
      ("help,h",          "Print help message")
      ("inputfile,f",      po::value<string>(&inputRootFile), "Full name including path of input file with ROOT tree")
   ;

   po::store(po::parse_command_line(argc, argv, prgOptions), prgOptionValues);
   po::notify(prgOptionValues);

   process_options(prgOptions, prgOptionValues);

   TApplication *myApp = new TApplication("myApp", &argc, argv);

   hft_display(inputRootFile, -1);

   myApp->Run();
   delete myApp;

   return EXIT_SUCCESS;
}


void process_options(po::options_description &prgOptions, po::variables_map &prgOptionValues)
{
   if (prgOptionValues.count("help"))
   {
      cout << prgOptions << endl;
      exit(EXIT_SUCCESS);
   }

   Info("ProcessOptions", "User provided options:");

   if (prgOptionValues.count("inputfile"))
   {
      std::string inputFile = boost::any_cast<std::string>(prgOptionValues["inputfile"].value());

      cout << "inputRootFile: " << inputFile << endl;
      ifstream tmpFileCheck(inputFile.c_str());
      if (!tmpFileCheck.good()) {
         Error("process_options", "File \"%s\" does not exist", inputFile.c_str());
         exit(EXIT_FAILURE);
      }
   } else {
      Error("process_options", "Input file not set");
      exit(EXIT_FAILURE);
   }
}


void hft_display(std::string fileName, const double B)
{
   myTreeFileChain = new TChain("t");
   myTreeFileChain->AddFile(fileName.c_str());
   NMAX = myTreeFileChain->GetEntries();
   cout << " Total number of events = " << NMAX << endl;
   myEventCounter = 0;

   eventT = new EventT();
   myTreeFileChain->SetBranchAddress("e", &eventT);

   //========================================================================
   // Create views and containers.
   //========================================================================

   make_geometry();

   init(B);

   //========================================================================
   //========================================================================

   hft_make_gui();
   process_event(myEventCounter);
   //
   gEve->Redraw3D(kTRUE);
}


void hft_display(const double B, const Int_t runnumber)
{
   //========================================================================
   //loadStatus(runnumber);
   //========================================================================

   //========================================================================
   //========================================================================
   myTreeFileChain = new TChain("T");
   char inname[100];
   sprintf(inname, "output/Event_%d.root", runnumber);
   myTreeFileChain->AddFile(inname);
   NMAX = myTreeFileChain->GetEntries();
   cout << " Total number of events = " << NMAX << endl;
   myEventCounter = 0;

   eventT = new EventT();
   myTreeFileChain->SetBranchAddress("e", &eventT);

   //========================================================================
   // Create views and containers.
   //========================================================================

   make_geometry();

   init(B);

   //========================================================================
   //========================================================================

   hft_make_gui();
   process_event(myEventCounter);
   //
   gEve->Redraw3D(kTRUE);
}


//========================================================================
void loadStatus(const int runnumber)
{
   // Read in the status - maybe moved int othe loop when loading multiple runs
   memset(Status_PXL, 0, sizeof(Status_PXL));
   memset(Status_IST, 0, sizeof(Status_IST));

   char inname[100];
   ifstream inData;
   sprintf(inname, "status/PXL_%d.txt", runnumber);
   inData.open(inname);

   for (int i = 0; i < N_PXL; i++) {
      inData >> Status_PXL[i];
      int id = i + 1;
      int sector, ladder, sensor;
      decodeId(id, &sector, &ladder, &sensor);

      if (Status_PXL[i]) cout << " Bad PXL Sensor # " << sector << "/" << ladder << "/" << sensor << endl;
   }

   inData.close();

   sprintf(inname, "status/IST_%d.txt", runnumber);
   inData.open(inname);

   for (int i = 0; i < N_IST; i++) {
      inData >> Status_IST[i];
      int id = i + 1 + 1000;
      int sector, ladder, sensor;
      decodeId(id, &sector, &ladder, &sensor);

      if (Status_IST[i]) cout << " Bad IST Sensor # " << ladder << "/" << sensor << endl;
   }

   inData.close();
}


//========================================================================
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

//==============================================================================
// intitalize track/hit lists
//------------------------------------------------------------------------------


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

//==============================================================================
// Next event
//------------------------------------------------------------------------------

void process_event(Int_t iEvt)
{
   if (iEvt >= NMAX) {
      cout << " End of the tree! Go backward! " << endl;
   }
   else if (iEvt < 0) {
      cout << " Beginning of the tree! Go forward! " << endl;
   }

   cout << "begin " << myEventCounter << "th entry...." << endl;
   myTreeFileChain->GetEntry(iEvt);
   printf("myTreeFileChain: %p\n", myTreeFileChain);

   gTrackList->DestroyElements();
   gVtxList->Reset();
   gHitList->Reset();
   gTrackHitList->Reset();

   int runId = eventT->fEvtHdr.fRun;
   int evtId = eventT->fEvtHdr.fEvtNum;

   cout << Form("run#%d event#%d", runId, evtId) << endl;

   // Load verteice/hits
   cout << " Event vertex = " << eventT->fVertex[0] << " " << eventT->fVertex[1] << " " << eventT->fVertex[2] << endl;
   //  gVtxList->SetNextPoint(eventT->fVertex[0], eventT->fVertex[1], eventT->fVertex[2]);
   //  gEve->AddElement(gVtxList);

   Int_t nHits = eventT->fHits->GetEntriesFast();

   for (int j = 0; j < nHits; j++) {

      HitT* hitT = (HitT*) eventT->fHits->At(j);

      int id = hitT->Id;

      cout << " Adding a new hit " << id << " " <<  hitT->xG << " " << hitT->yG << " " << hitT->zG << endl;

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

      cout << " Adding a new track " << trackT->fPx << " " << trackT->fPy << " " << trackT->fPz << endl;
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

      cout << " Number of hits on PXL1/PXL2/IST/SSD/TPC = " << nhits_pxl1 << "/" << nhits_pxl2 << "/" << nhits_ist << "/" << nhits_ssd << "/" << nhits_tpc << endl;
   }

   gTrackList->MakeTracks();
   gEve->AddElement(gTrackHitList);

   gEve->SetStatusLine(Form("run#%d event#%d", runId, evtId));

   TEveElement *top = (TEveElement *) gEve->GetCurrentEvent();

   gMultiView->DestroyEventRPhi();
   gMultiView->ImportEventRPhi(top);

   gMultiView->DestroyEventRhoZ();
   gMultiView->ImportEventRhoZ(top);

   gEve->Redraw3D();
}


void selectDaughterVisible(TGeoNode *node, const char *name)
{
   int nDaughters = node->GetVolume()->GetNdaughters();

   for (int i = 0; i < nDaughters; i++) {
      TGeoNode *daughter = node->GetVolume()->GetNode(i);

      if (!daughter) continue;

      if (strstr(daughter->GetName(), name) != 0) {
         daughter->GetVolume()->SetVisibility(1);
      }
      else {
         daughter->GetVolume()->SetVisibility(0);

         selectDaughterVisible(daughter, name);
      }
   }
}


void make_geometry()
{
   const Char_t *path = ".:./StarVMC/Geometry/macros/:$STAR/StarVMC/Geometry/macros/";

   //Char_t *file = gSystem->Which(path, "viewStarGeometry.C", kReadPermission);
   //cout << "Loading macro: " << file << endl;
   //gROOT->LoadMacro(file);

   //TRootBrowser *b = new TRootBrowser(0, "browser", 1200, 800);

   TEveManager::Create();
   std::string myGeomTag("y2014a");
   //cacheGeometry(myGeomTag.c_str(), "");

   TFile *file = new TFile(Form("%s.root", myGeomTag.c_str()));

   if ( file->IsZombie() )
   {
      delete file;

      Error("make_geometry", "No root geometry file found: %s", Form("%s.root", myGeomTag.c_str()));
      exit(EXIT_FAILURE);

      Info("make_geometry", "Creating new geometry root file");

      const Char_t *path  = ".:./StarVMC/Geometry/macros/:$STAR/StarVMC/Geometry/macros/";
      //Char_t *file = gSystem->Which(path,"loadStarGeometry.C",kReadPermission);
      //cout << "Loading macro: " << file << endl;
      //gROOT->ProcessLine(Form(".L %s",file));

      //loadStarGeometry(myGeomTag.c_str());
      
      // ROOT TGeo stacker
      StarTGeoStacker *stacker = new StarTGeoStacker();

      AgBlock::SetStacker( stacker );

      Geometry *build = new Geometry(); 

      Char_t *macroFile = gSystem->Which(path, "StarGeometryDb.C", kReadPermission);
      cout << "Loading macro: " << macroFile << endl;
      gROOT->ProcessLine(Form(".L %s",macroFile));
      gROOT->ProcessLine("StarGeometryDb();");

      Info("make_geometry", "build: %p, gGeoManager: %p", build, gGeoManager);

      //gROOT->ProcessLine(Form(".!mkdir %s", myGeomTag.c_str()));

      //if ( !gGeoManager )
      //   gGeoManager = new TGeoManager(myGeomTag.c_str(), (myGeomTag+" | dyson").c_str());	

      //Info("make_geometry", "build: %p, gGeoManager: %p", build, gGeoManager);

      build->ConstructGeometry ( myGeomTag.c_str() );

      gGeoManager->SetVisLevel(99);        

      // Close the geometry
      gGeoManager->CloseGeometry();

      gGeoManager->Export(Form("%s.root", myGeomTag.c_str()));
   }

   delete file;

   // Register geometry
   gEve->RegisterGeometryAlias("Default", Form("%s.root",myGeomTag.c_str()));
   gGeoManager = gEve->GetDefaultGeometry();

   if (!gGeoManager) {
      Error("make_geometry", "No gGeoManager found");
      exit(EXIT_FAILURE);
   }

   //TGeoVolume* top = gGeoManager->GetTopVolume()->FindNode("CAVE_1")->GetVolume();

   //if (!top) {
   //   Error("make_geometry", "No top node found");
   //   exit(EXIT_FAILURE);
   //}

   //gGeoManager->cd("HALL_1/CAVE_1/TpcRefSys_1/TPCE_1");
   //TGeoNode *tpc_mom = gGeoManager->GetCurrentNode();

   //if (!tpc_mom) {
   //   Error("make_geometry", "No tpc_mom node found");
   //   exit(EXIT_FAILURE);
   //}

   TEveElementList *STAR = new TEveElementList("Geometry");

   //selectDaughterVisible(tpc_mom, "TPCM"); // Central Membrane
   //selectDaughterVisible(tpc_mom, "TSAW");
   //selectDaughterVisible(tpc_mom, "TWMR");
   //selectDaughterVisible(tpc_mom, "TWRB"); // Sector ribs
   //TEveGeoTopNode* tpc = new TEveGeoTopNode(gGeoManager, tpc_mom);
   //tpc->SetMainTransparency(80);
   //tpc->SetVisLevel(5);
   //STAR->AddElement(tpc);


   //gGeoManager->cd("HALL_1/CAVE_1/TpcRefSys_1/IDSM_1/PIPI_1"); // retrieve info of IBSS volume
   //TGeoNode *bp_mom = gGeoManager->GetCurrentNode();

   ////TGeoNode *bp_mom = top->FindNode("IDSM_1")->GetVolume()->FindNode("PIPI_1");
   //selectDaughterVisible(bp_mom, "PBES");
   //TEveGeoTopNode* beampipe = new TEveGeoTopNode(gGeoManager, bp_mom);
   //beampipe->SetMainTransparency(80);
   //beampipe->SetVisLevel(3);
   //STAR->AddElement(beampipe);

   //TGeoNode *ssd_mom = top->FindNode("IDSM_1")->GetVolume()->FindNode("SFMO_1");
   //if(!kPlotHFTSupport) selectDaughterVisible(ssd_mom, "SFSD");
   //TEveGeoTopNode* ssd = new TEveGeoTopNode(gGeoManager, ssd_mom);
   //ssd->SetVisLevel(6);
   //  STAR->AddElement(ssd);

   gGeoManager->cd("HALL_1/CAVE_1/TpcRefSys_1/IDSM_1/IBMO_1");
   TGeoNode *ist_mom = gGeoManager->GetCurrentNode();

   //Path += Form("/IBAM_%d/IBLM_%d/IBSS_1", ladderIdx + 1, sensorIdx + 1);
   if (!kPlotHFTSupport) selectDaughterVisible(ist_mom, "IBSS");

   TEveGeoTopNode *ist = new TEveGeoTopNode(gGeoManager, ist_mom);
   ist->SetVisLevel(6);
   STAR->AddElement(ist);

   gGeoManager->cd("HALL_1/CAVE_1/TpcRefSys_1/IDSM_1/PXMO_1");
   TGeoNode *pxl_mom = gGeoManager->GetCurrentNode();

   if (!kPlotHFTSupport) selectDaughterVisible(pxl_mom, "PLAC");

   TEveGeoTopNode *pxl = new TEveGeoTopNode(gGeoManager, pxl_mom);
   pxl->SetVisLevel(6);
   STAR->AddElement(pxl);

   gEve->AddGlobalElement(STAR);

   gMultiView = new MultiView();
   gMultiView->ImportGeomRPhi(STAR);
   gMultiView->ImportGeomRhoZ(STAR);
}

//==============================================================================
// GUI stuff
//------------------------------------------------------------------------------

//______________________________________________________________________________
void hft_make_gui()
{
   // Create minimal GUI for event navigation.

   TEveBrowser *browser = gEve->GetBrowser();
   browser->StartEmbedding(TRootBrowser::kLeft);

   TGMainFrame *frmMain = new TGMainFrame(gClient->GetRoot(), 400, 400);
   frmMain->SetWindowName("XX GUI");
   frmMain->SetCleanup(kDeepCleanup);

   TGHorizontalFrame *hf = new TGHorizontalFrame(frmMain);
   {
      TString icondir( Form("%s/icons/", gSystem->Getenv("ROOTSYS")) );
      TGPictureButton *b = 0;
      EvNavHandler    *fh = new EvNavHandler;

      b = new TGPictureButton(hf, gClient->GetPicture(icondir + "GoBack.gif"));
      //      b->SetEnabled(kFALSE);
      b->SetToolTipText("Go to previous event - not supported.");
      hf->AddFrame(b);
      b->Connect("Clicked()", "EvNavHandler", fh, "Bck()");

      b = new TGPictureButton(hf, gClient->GetPicture(icondir + "GoForward.gif"));
      b->SetToolTipText("Generate new event.");
      hf->AddFrame(b);
      b->Connect("Clicked()", "EvNavHandler", fh, "Fwd()");

   }
   frmMain->AddFrame(hf);

   frmMain->MapSubwindows();
   frmMain->Resize();
   frmMain->MapWindow();

   browser->StopEmbedding();
   browser->SetTabTitle("Event Control", 0);
}
