//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Tue Feb 18 17:38:43 2014 by ROOT version 5.30/00
// from TTree T/TTree with HFT hits and tracks
// found on file: output/Event_15049078.root
//////////////////////////////////////////////////////////

#ifndef T_h
#define T_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
const Int_t kMaxfVertices = 1;
const Int_t kMaxfTracks = 50000;
const Int_t kMaxfHits = 500000;
const Int_t kMaxfMatchHits = 2000000;

class T
{
public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain
   Int_t           fCurrent; //!current Tree number in a TChain

   // Declaration of leaf types
   //EventT          *EventT;
   UInt_t          fUniqueID;
   UInt_t          fBits;
   UInt_t          fNPTracks;
   UInt_t          fNvertex;
   UInt_t          fNtrack;
   UInt_t          fNhit;
   UInt_t          fNmatchhit;
   UInt_t          fFlag;
   Float_t         fVzVpd;
   Int_t           fNVpdHits;
   Int_t           fEvtHdr_fEvtNum;
   Int_t           fEvtHdr_fRun;
   Int_t           fEvtHdr_fDate;
   Double32_t      fEvtHdr_fField;
   Double32_t      fVertex[3];
   Double32_t      fCovariantMatrix[6];
   UInt_t          fNPredHFT[4];
   Int_t           fVertices_;
   UInt_t          fVertices_fUniqueID[kMaxfVertices];   //[fVertices_]
   UInt_t          fVertices_fBits[kMaxfVertices];   //[fVertices_]
   Char_t          fVertices_beg[kMaxfVertices];   //[fVertices_]
   Double32_t      fVertices_fVx[kMaxfVertices];   //[fVertices_]
   Double32_t      fVertices_fVy[kMaxfVertices];   //[fVertices_]
   Double32_t      fVertices_fVz[kMaxfVertices];   //[fVertices_]
   Int_t           fVertices_fNtracks[kMaxfVertices];   //[fVertices_]
   Char_t          fVertices_end[kMaxfVertices];   //[fVertices_]
   Int_t           fTracks_;
   UInt_t          fTracks_fUniqueID[kMaxfTracks];   //[fTracks_]
   UInt_t          fTracks_fBits[kMaxfTracks];   //[fTracks_]
   Char_t          fTracks_beg[kMaxfTracks];   //[fTracks_]
   Double32_t      fTracks_fPxDca[kMaxfTracks];   //[fTracks_]
   Double32_t      fTracks_fPyDca[kMaxfTracks];   //[fTracks_]
   Double32_t      fTracks_fPzDca[kMaxfTracks];   //[fTracks_]
   Double32_t      fTracks_fPPx[kMaxfTracks];   //[fTracks_]
   Double32_t      fTracks_fPPy[kMaxfTracks];   //[fTracks_]
   Double32_t      fTracks_fPPz[kMaxfTracks];   //[fTracks_]
   Double32_t      fTracks_fOriginXDca[kMaxfTracks];   //[fTracks_]
   Double32_t      fTracks_fOriginYDca[kMaxfTracks];   //[fTracks_]
   Double32_t      fTracks_fOriginZDca[kMaxfTracks];   //[fTracks_]
   Double32_t      fTracks_fPx[kMaxfTracks];   //[fTracks_]
   Double32_t      fTracks_fPy[kMaxfTracks];   //[fTracks_]
   Double32_t      fTracks_fPz[kMaxfTracks];   //[fTracks_]
   Double32_t      fTracks_fOriginX[kMaxfTracks];   //[fTracks_]
   Double32_t      fTracks_fOriginY[kMaxfTracks];   //[fTracks_]
   Double32_t      fTracks_fOriginZ[kMaxfTracks];   //[fTracks_]
   Int_t           fTracks_fNpoint[kMaxfTracks];   //[fTracks_]
   Double32_t      fTracks_fTpcHitX[kMaxfTracks][45];   //[fTracks_]
   Double32_t      fTracks_fTpcHitY[kMaxfTracks][45];   //[fTracks_]
   Double32_t      fTracks_fTpcHitZ[kMaxfTracks][45];   //[fTracks_]
   Double32_t      fTracks_fNsigmaPi[kMaxfTracks];   //[fTracks_]
   Double32_t      fTracks_fDca2D[kMaxfTracks];   //[fTracks_]
   Double32_t      fTracks_fDca3D[kMaxfTracks];   //[fTracks_]
   UInt_t          fTracks_fPxlHitPattern[kMaxfTracks];   //[fTracks_]
   UInt_t          fTracks_fIstHitPattern[kMaxfTracks];   //[fTracks_]
   UInt_t          fTracks_fSsdHitPattern[kMaxfTracks];   //[fTracks_]
   Double32_t      fTracks_fPxlHitX[kMaxfTracks][3];   //[fTracks_]
   Double32_t      fTracks_fPxlHitY[kMaxfTracks][3];   //[fTracks_]
   Double32_t      fTracks_fPxlHitZ[kMaxfTracks][3];   //[fTracks_]
   Double32_t      fTracks_fIstHitX[kMaxfTracks][2];   //[fTracks_]
   Double32_t      fTracks_fIstHitY[kMaxfTracks][2];   //[fTracks_]
   Double32_t      fTracks_fIstHitZ[kMaxfTracks][2];   //[fTracks_]
   Double32_t      fTracks_fSsdHitX[kMaxfTracks][2];   //[fTracks_]
   Double32_t      fTracks_fSsdHitY[kMaxfTracks][2];   //[fTracks_]
   Double32_t      fTracks_fSsdHitZ[kMaxfTracks][2];   //[fTracks_]
   Char_t          fTracks_end[kMaxfTracks];   //[fTracks_]
   Int_t           fHits_;
   UInt_t          fHits_fUniqueID[kMaxfHits];   //[fHits_]
   UInt_t          fHits_fBits[kMaxfHits];   //[fHits_]
   Char_t          fHits_start[kMaxfHits];   //[fHits_]
   Int_t           fHits_Id[kMaxfHits];   //[fHits_]
   UInt_t          fHits_nRawHits[kMaxfHits];   //[fHits_]
   Double32_t      fHits_xG[kMaxfHits];   //[fHits_]
   Double32_t      fHits_yG[kMaxfHits];   //[fHits_]
   Double32_t      fHits_zG[kMaxfHits];   //[fHits_]
   Double32_t      fHits_xL[kMaxfHits];   //[fHits_]
   Double32_t      fHits_yL[kMaxfHits];   //[fHits_]
   Double32_t      fHits_zL[kMaxfHits];   //[fHits_]
   UInt_t          fHits_index2Track[kMaxfHits];   //[fHits_]
   Char_t          fHits_end[kMaxfHits];   //[fHits_]
   Int_t           fMatchHits_;
   UInt_t          fMatchHits_fUniqueID[kMaxfMatchHits];   //[fMatchHits_]
   UInt_t          fMatchHits_fBits[kMaxfMatchHits];   //[fMatchHits_]
   Char_t          fMatchHits_start[kMaxfMatchHits];   //[fMatchHits_]
   UInt_t          fMatchHits_index2Track[kMaxfMatchHits];   //[fMatchHits_]
   UInt_t          fMatchHits_index2Hit[kMaxfMatchHits];   //[fMatchHits_]
   UInt_t          fMatchHits_detId[kMaxfMatchHits];   //[fMatchHits_]
   UInt_t          fMatchHits_nRawHits[kMaxfMatchHits];   //[fMatchHits_]
   Double32_t      fMatchHits_xGP[kMaxfMatchHits];   //[fMatchHits_]
   Double32_t      fMatchHits_yGP[kMaxfMatchHits];   //[fMatchHits_]
   Double32_t      fMatchHits_zGP[kMaxfMatchHits];   //[fMatchHits_]
   Double32_t      fMatchHits_xLP[kMaxfMatchHits];   //[fMatchHits_]
   Double32_t      fMatchHits_yLP[kMaxfMatchHits];   //[fMatchHits_]
   Double32_t      fMatchHits_zLP[kMaxfMatchHits];   //[fMatchHits_]
   Double32_t      fMatchHits_tuP[kMaxfMatchHits];   //[fMatchHits_]
   Double32_t      fMatchHits_tvP[kMaxfMatchHits];   //[fMatchHits_]
   Double32_t      fMatchHits_xG[kMaxfMatchHits];   //[fMatchHits_]
   Double32_t      fMatchHits_yG[kMaxfMatchHits];   //[fMatchHits_]
   Double32_t      fMatchHits_zG[kMaxfMatchHits];   //[fMatchHits_]
   Double32_t      fMatchHits_xL[kMaxfMatchHits];   //[fMatchHits_]
   Double32_t      fMatchHits_yL[kMaxfMatchHits];   //[fMatchHits_]
   Double32_t      fMatchHits_zL[kMaxfMatchHits];   //[fMatchHits_]
   Double32_t      fMatchHits_pT[kMaxfMatchHits];   //[fMatchHits_]
   Double32_t      fMatchHits_eta[kMaxfMatchHits];   //[fMatchHits_]
   Double32_t      fMatchHits_phi[kMaxfMatchHits];   //[fMatchHits_]
   Double32_t      fMatchHits_ox[kMaxfMatchHits];   //[fMatchHits_]
   Double32_t      fMatchHits_oy[kMaxfMatchHits];   //[fMatchHits_]
   Double32_t      fMatchHits_oz[kMaxfMatchHits];   //[fMatchHits_]
   Int_t           fMatchHits_npoint[kMaxfMatchHits];   //[fMatchHits_]
   Double32_t      fMatchHits_firstPointR[kMaxfMatchHits];   //[fMatchHits_]
   Double32_t      fMatchHits_firstPointZ[kMaxfMatchHits];   //[fMatchHits_]
   Char_t          fMatchHits_end[kMaxfMatchHits];   //[fMatchHits_]
   Bool_t          fIsValid;

   // List of branches
   TBranch        *b_EventT_fUniqueID;   //!
   TBranch        *b_EventT_fBits;   //!
   TBranch        *b_EventT_fNPTracks;   //!
   TBranch        *b_EventT_fNvertex;   //!
   TBranch        *b_EventT_fNtrack;   //!
   TBranch        *b_EventT_fNhit;   //!
   TBranch        *b_EventT_fNmatchhit;   //!
   TBranch        *b_EventT_fFlag;   //!
   TBranch        *b_EventT_fVzVpd;   //!
   TBranch        *b_EventT_fNVpdHits;   //!
   TBranch        *b_EventT_fEvtHdr_fEvtNum;   //!
   TBranch        *b_EventT_fEvtHdr_fRun;   //!
   TBranch        *b_EventT_fEvtHdr_fDate;   //!
   TBranch        *b_EventT_fEvtHdr_fField;   //!
   TBranch        *b_EventT_fVertex;   //!
   TBranch        *b_EventT_fCovariantMatrix;   //!
   TBranch        *b_EventT_fNPredHFT;   //!
   TBranch        *b_EventT_fVertices_;   //!
   TBranch        *b_fVertices_fUniqueID;   //!
   TBranch        *b_fVertices_fBits;   //!
   TBranch        *b_fVertices_beg;   //!
   TBranch        *b_fVertices_fVx;   //!
   TBranch        *b_fVertices_fVy;   //!
   TBranch        *b_fVertices_fVz;   //!
   TBranch        *b_fVertices_fNtracks;   //!
   TBranch        *b_fVertices_end;   //!
   TBranch        *b_EventT_fTracks_;   //!
   TBranch        *b_fTracks_fUniqueID;   //!
   TBranch        *b_fTracks_fBits;   //!
   TBranch        *b_fTracks_beg;   //!
   TBranch        *b_fTracks_fPxDca;   //!
   TBranch        *b_fTracks_fPyDca;   //!
   TBranch        *b_fTracks_fPzDca;   //!
   TBranch        *b_fTracks_fPPx;   //!
   TBranch        *b_fTracks_fPPy;   //!
   TBranch        *b_fTracks_fPPz;   //!
   TBranch        *b_fTracks_fOriginXDca;   //!
   TBranch        *b_fTracks_fOriginYDca;   //!
   TBranch        *b_fTracks_fOriginZDca;   //!
   TBranch        *b_fTracks_fPx;   //!
   TBranch        *b_fTracks_fPy;   //!
   TBranch        *b_fTracks_fPz;   //!
   TBranch        *b_fTracks_fOriginX;   //!
   TBranch        *b_fTracks_fOriginY;   //!
   TBranch        *b_fTracks_fOriginZ;   //!
   TBranch        *b_fTracks_fNpoint;   //!
   TBranch        *b_fTracks_fTpcHitX;   //!
   TBranch        *b_fTracks_fTpcHitY;   //!
   TBranch        *b_fTracks_fTpcHitZ;   //!
   TBranch        *b_fTracks_fNsigmaPi;   //!
   TBranch        *b_fTracks_fDca2D;   //!
   TBranch        *b_fTracks_fDca3D;   //!
   TBranch        *b_fTracks_fPxlHitPattern;   //!
   TBranch        *b_fTracks_fIstHitPattern;   //!
   TBranch        *b_fTracks_fSsdHitPattern;   //!
   TBranch        *b_fTracks_fPxlHitX;   //!
   TBranch        *b_fTracks_fPxlHitY;   //!
   TBranch        *b_fTracks_fPxlHitZ;   //!
   TBranch        *b_fTracks_fIstHitX;   //!
   TBranch        *b_fTracks_fIstHitY;   //!
   TBranch        *b_fTracks_fIstHitZ;   //!
   TBranch        *b_fTracks_fSsdHitX;   //!
   TBranch        *b_fTracks_fSsdHitY;   //!
   TBranch        *b_fTracks_fSsdHitZ;   //!
   TBranch        *b_fTracks_end;   //!
   TBranch        *b_EventT_fHits_;   //!
   TBranch        *b_fHits_fUniqueID;   //!
   TBranch        *b_fHits_fBits;   //!
   TBranch        *b_fHits_start;   //!
   TBranch        *b_fHits_Id;   //!
   TBranch        *b_fHits_nRawHits;   //!
   TBranch        *b_fHits_xG;   //!
   TBranch        *b_fHits_yG;   //!
   TBranch        *b_fHits_zG;   //!
   TBranch        *b_fHits_xL;   //!
   TBranch        *b_fHits_yL;   //!
   TBranch        *b_fHits_zL;   //!
   TBranch        *b_fHits_index2Track;   //!
   TBranch        *b_fHits_end;   //!
   TBranch        *b_EventT_fMatchHits_;   //!
   TBranch        *b_fMatchHits_fUniqueID;   //!
   TBranch        *b_fMatchHits_fBits;   //!
   TBranch        *b_fMatchHits_start;   //!
   TBranch        *b_fMatchHits_index2Track;   //!
   TBranch        *b_fMatchHits_index2Hit;   //!
   TBranch        *b_fMatchHits_detId;   //!
   TBranch        *b_fMatchHits_nRawHits;   //!
   TBranch        *b_fMatchHits_xGP;   //!
   TBranch        *b_fMatchHits_yGP;   //!
   TBranch        *b_fMatchHits_zGP;   //!
   TBranch        *b_fMatchHits_xLP;   //!
   TBranch        *b_fMatchHits_yLP;   //!
   TBranch        *b_fMatchHits_zLP;   //!
   TBranch        *b_fMatchHits_tuP;   //!
   TBranch        *b_fMatchHits_tvP;   //!
   TBranch        *b_fMatchHits_xG;   //!
   TBranch        *b_fMatchHits_yG;   //!
   TBranch        *b_fMatchHits_zG;   //!
   TBranch        *b_fMatchHits_xL;   //!
   TBranch        *b_fMatchHits_yL;   //!
   TBranch        *b_fMatchHits_zL;   //!
   TBranch        *b_fMatchHits_pT;   //!
   TBranch        *b_fMatchHits_eta;   //!
   TBranch        *b_fMatchHits_phi;   //!
   TBranch        *b_fMatchHits_ox;   //!
   TBranch        *b_fMatchHits_oy;   //!
   TBranch        *b_fMatchHits_oz;   //!
   TBranch        *b_fMatchHits_npoint;   //!
   TBranch        *b_fMatchHits_firstPointR;   //!
   TBranch        *b_fMatchHits_firstPointZ;   //!
   TBranch        *b_fMatchHits_end;   //!
   TBranch        *b_EventT_fIsValid;   //!

   T(TTree *tree = 0);
   virtual ~T();
   virtual Int_t    Cut(Long64_t entry);
   virtual Int_t    GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);
   virtual void     Init(TTree *tree);
   virtual void     Loop();
   virtual Bool_t   Notify();
   virtual void     Show(Long64_t entry = -1);
};

#endif

//#ifdef T_cxx
T::T(TTree *tree)
{
   // if parameter tree is not specified (or zero), connect the file
   // used to generate this class and read the Tree.
   if (tree == 0) {
      TFile *f = (TFile *)gROOT->GetListOfFiles()->FindObject("output/Event_15049078.root");

      if (!f || !f->IsOpen()) {
         f = new TFile("output/Event_15049078.root");
      }

      f->GetObject("T", tree);

   }

   Init(tree);
}

T::~T()
{
   if (!fChain) return;

   delete fChain->GetCurrentFile();
}

Int_t T::GetEntry(Long64_t entry)
{
   // Read contents of entry.
   if (!fChain) return 0;

   return fChain->GetEntry(entry);
}
Long64_t T::LoadTree(Long64_t entry)
{
   // Set the environment to read one entry
   if (!fChain) return -5;

   Long64_t centry = fChain->LoadTree(entry);

   if (centry < 0) return centry;

   if (fChain->GetTreeNumber() != fCurrent) {
      fCurrent = fChain->GetTreeNumber();
      Notify();
   }

   return centry;
}

void T::Init(TTree *tree)
{
   // The Init() function is called when the selector needs to initialize
   // a new tree or chain. Typically here the branch addresses and branch
   // pointers of the tree will be set.
   // It is normally not necessary to make changes to the generated
   // code, but the routine can be extended by the user if needed.
   // Init() will be called many times when running on PROOF
   // (once per file to be processed).

   // Set branch addresses and branch pointers
   if (!tree) return;

   fChain = tree;
   fCurrent = -1;
   fChain->SetMakeClass(1);

   fChain->SetBranchAddress("fUniqueID", &fUniqueID, &b_EventT_fUniqueID);
   fChain->SetBranchAddress("fBits", &fBits, &b_EventT_fBits);
   fChain->SetBranchAddress("fNPTracks", &fNPTracks, &b_EventT_fNPTracks);
   fChain->SetBranchAddress("fNvertex", &fNvertex, &b_EventT_fNvertex);
   fChain->SetBranchAddress("fNtrack", &fNtrack, &b_EventT_fNtrack);
   fChain->SetBranchAddress("fNhit", &fNhit, &b_EventT_fNhit);
   fChain->SetBranchAddress("fNmatchhit", &fNmatchhit, &b_EventT_fNmatchhit);
   fChain->SetBranchAddress("fFlag", &fFlag, &b_EventT_fFlag);
   fChain->SetBranchAddress("fVzVpd", &fVzVpd, &b_EventT_fVzVpd);
   fChain->SetBranchAddress("fNVpdHits", &fNVpdHits, &b_EventT_fNVpdHits);
   fChain->SetBranchAddress("fEvtHdr.fEvtNum", &fEvtHdr_fEvtNum, &b_EventT_fEvtHdr_fEvtNum);
   fChain->SetBranchAddress("fEvtHdr.fRun", &fEvtHdr_fRun, &b_EventT_fEvtHdr_fRun);
   fChain->SetBranchAddress("fEvtHdr.fDate", &fEvtHdr_fDate, &b_EventT_fEvtHdr_fDate);
   fChain->SetBranchAddress("fEvtHdr.fField", &fEvtHdr_fField, &b_EventT_fEvtHdr_fField);
   fChain->SetBranchAddress("fVertex[3]", fVertex, &b_EventT_fVertex);
   fChain->SetBranchAddress("fCovariantMatrix[6]", fCovariantMatrix, &b_EventT_fCovariantMatrix);
   fChain->SetBranchAddress("fNPredHFT[4]", fNPredHFT, &b_EventT_fNPredHFT);
   fChain->SetBranchAddress("fVertices", &fVertices_, &b_EventT_fVertices_);
   fChain->SetBranchAddress("fVertices.fUniqueID", &fVertices_fUniqueID, &b_fVertices_fUniqueID);
   fChain->SetBranchAddress("fVertices.fBits", &fVertices_fBits, &b_fVertices_fBits);
   fChain->SetBranchAddress("fVertices.beg", &fVertices_beg, &b_fVertices_beg);
   fChain->SetBranchAddress("fVertices.fVx", &fVertices_fVx, &b_fVertices_fVx);
   fChain->SetBranchAddress("fVertices.fVy", &fVertices_fVy, &b_fVertices_fVy);
   fChain->SetBranchAddress("fVertices.fVz", &fVertices_fVz, &b_fVertices_fVz);
   fChain->SetBranchAddress("fVertices.fNtracks", &fVertices_fNtracks, &b_fVertices_fNtracks);
   fChain->SetBranchAddress("fVertices.end", &fVertices_end, &b_fVertices_end);
   fChain->SetBranchAddress("fTracks", &fTracks_, &b_EventT_fTracks_);
   fChain->SetBranchAddress("fTracks.fUniqueID", fTracks_fUniqueID, &b_fTracks_fUniqueID);
   fChain->SetBranchAddress("fTracks.fBits", fTracks_fBits, &b_fTracks_fBits);
   fChain->SetBranchAddress("fTracks.beg", fTracks_beg, &b_fTracks_beg);
   fChain->SetBranchAddress("fTracks.fPxDca", fTracks_fPxDca, &b_fTracks_fPxDca);
   fChain->SetBranchAddress("fTracks.fPyDca", fTracks_fPyDca, &b_fTracks_fPyDca);
   fChain->SetBranchAddress("fTracks.fPzDca", fTracks_fPzDca, &b_fTracks_fPzDca);
   fChain->SetBranchAddress("fTracks.fPPx", fTracks_fPPx, &b_fTracks_fPPx);
   fChain->SetBranchAddress("fTracks.fPPy", fTracks_fPPy, &b_fTracks_fPPy);
   fChain->SetBranchAddress("fTracks.fPPz", fTracks_fPPz, &b_fTracks_fPPz);
   fChain->SetBranchAddress("fTracks.fOriginXDca", fTracks_fOriginXDca, &b_fTracks_fOriginXDca);
   fChain->SetBranchAddress("fTracks.fOriginYDca", fTracks_fOriginYDca, &b_fTracks_fOriginYDca);
   fChain->SetBranchAddress("fTracks.fOriginZDca", fTracks_fOriginZDca, &b_fTracks_fOriginZDca);
   fChain->SetBranchAddress("fTracks.fPx", fTracks_fPx, &b_fTracks_fPx);
   fChain->SetBranchAddress("fTracks.fPy", fTracks_fPy, &b_fTracks_fPy);
   fChain->SetBranchAddress("fTracks.fPz", fTracks_fPz, &b_fTracks_fPz);
   fChain->SetBranchAddress("fTracks.fOriginX", fTracks_fOriginX, &b_fTracks_fOriginX);
   fChain->SetBranchAddress("fTracks.fOriginY", fTracks_fOriginY, &b_fTracks_fOriginY);
   fChain->SetBranchAddress("fTracks.fOriginZ", fTracks_fOriginZ, &b_fTracks_fOriginZ);
   fChain->SetBranchAddress("fTracks.fNpoint", fTracks_fNpoint, &b_fTracks_fNpoint);
   fChain->SetBranchAddress("fTracks.fTpcHitX[45]", fTracks_fTpcHitX, &b_fTracks_fTpcHitX);
   fChain->SetBranchAddress("fTracks.fTpcHitY[45]", fTracks_fTpcHitY, &b_fTracks_fTpcHitY);
   fChain->SetBranchAddress("fTracks.fTpcHitZ[45]", fTracks_fTpcHitZ, &b_fTracks_fTpcHitZ);
   fChain->SetBranchAddress("fTracks.fNsigmaPi", fTracks_fNsigmaPi, &b_fTracks_fNsigmaPi);
   fChain->SetBranchAddress("fTracks.fDca2D", fTracks_fDca2D, &b_fTracks_fDca2D);
   fChain->SetBranchAddress("fTracks.fDca3D", fTracks_fDca3D, &b_fTracks_fDca3D);
   fChain->SetBranchAddress("fTracks.fPxlHitPattern", fTracks_fPxlHitPattern, &b_fTracks_fPxlHitPattern);
   fChain->SetBranchAddress("fTracks.fIstHitPattern", fTracks_fIstHitPattern, &b_fTracks_fIstHitPattern);
   fChain->SetBranchAddress("fTracks.fSsdHitPattern", fTracks_fSsdHitPattern, &b_fTracks_fSsdHitPattern);
   fChain->SetBranchAddress("fTracks.fPxlHitX[3]", fTracks_fPxlHitX, &b_fTracks_fPxlHitX);
   fChain->SetBranchAddress("fTracks.fPxlHitY[3]", fTracks_fPxlHitY, &b_fTracks_fPxlHitY);
   fChain->SetBranchAddress("fTracks.fPxlHitZ[3]", fTracks_fPxlHitZ, &b_fTracks_fPxlHitZ);
   fChain->SetBranchAddress("fTracks.fIstHitX[2]", fTracks_fIstHitX, &b_fTracks_fIstHitX);
   fChain->SetBranchAddress("fTracks.fIstHitY[2]", fTracks_fIstHitY, &b_fTracks_fIstHitY);
   fChain->SetBranchAddress("fTracks.fIstHitZ[2]", fTracks_fIstHitZ, &b_fTracks_fIstHitZ);
   fChain->SetBranchAddress("fTracks.fSsdHitX[2]", fTracks_fSsdHitX, &b_fTracks_fSsdHitX);
   fChain->SetBranchAddress("fTracks.fSsdHitY[2]", fTracks_fSsdHitY, &b_fTracks_fSsdHitY);
   fChain->SetBranchAddress("fTracks.fSsdHitZ[2]", fTracks_fSsdHitZ, &b_fTracks_fSsdHitZ);
   fChain->SetBranchAddress("fTracks.end", fTracks_end, &b_fTracks_end);
   fChain->SetBranchAddress("fHits", &fHits_, &b_EventT_fHits_);
   fChain->SetBranchAddress("fHits.fUniqueID", fHits_fUniqueID, &b_fHits_fUniqueID);
   fChain->SetBranchAddress("fHits.fBits", fHits_fBits, &b_fHits_fBits);
   fChain->SetBranchAddress("fHits.start", fHits_start, &b_fHits_start);
   fChain->SetBranchAddress("fHits.Id", fHits_Id, &b_fHits_Id);
   fChain->SetBranchAddress("fHits.nRawHits", fHits_nRawHits, &b_fHits_nRawHits);
   fChain->SetBranchAddress("fHits.xG", fHits_xG, &b_fHits_xG);
   fChain->SetBranchAddress("fHits.yG", fHits_yG, &b_fHits_yG);
   fChain->SetBranchAddress("fHits.zG", fHits_zG, &b_fHits_zG);
   fChain->SetBranchAddress("fHits.xL", fHits_xL, &b_fHits_xL);
   fChain->SetBranchAddress("fHits.yL", fHits_yL, &b_fHits_yL);
   fChain->SetBranchAddress("fHits.zL", fHits_zL, &b_fHits_zL);
   fChain->SetBranchAddress("fHits.index2Track", fHits_index2Track, &b_fHits_index2Track);
   fChain->SetBranchAddress("fHits.end", fHits_end, &b_fHits_end);
   fChain->SetBranchAddress("fMatchHits", &fMatchHits_, &b_EventT_fMatchHits_);
   fChain->SetBranchAddress("fMatchHits.fUniqueID", fMatchHits_fUniqueID, &b_fMatchHits_fUniqueID);
   fChain->SetBranchAddress("fMatchHits.fBits", fMatchHits_fBits, &b_fMatchHits_fBits);
   fChain->SetBranchAddress("fMatchHits.start", fMatchHits_start, &b_fMatchHits_start);
   fChain->SetBranchAddress("fMatchHits.index2Track", fMatchHits_index2Track, &b_fMatchHits_index2Track);
   fChain->SetBranchAddress("fMatchHits.index2Hit", fMatchHits_index2Hit, &b_fMatchHits_index2Hit);
   fChain->SetBranchAddress("fMatchHits.detId", fMatchHits_detId, &b_fMatchHits_detId);
   fChain->SetBranchAddress("fMatchHits.nRawHits", fMatchHits_nRawHits, &b_fMatchHits_nRawHits);
   fChain->SetBranchAddress("fMatchHits.xGP", fMatchHits_xGP, &b_fMatchHits_xGP);
   fChain->SetBranchAddress("fMatchHits.yGP", fMatchHits_yGP, &b_fMatchHits_yGP);
   fChain->SetBranchAddress("fMatchHits.zGP", fMatchHits_zGP, &b_fMatchHits_zGP);
   fChain->SetBranchAddress("fMatchHits.xLP", fMatchHits_xLP, &b_fMatchHits_xLP);
   fChain->SetBranchAddress("fMatchHits.yLP", fMatchHits_yLP, &b_fMatchHits_yLP);
   fChain->SetBranchAddress("fMatchHits.zLP", fMatchHits_zLP, &b_fMatchHits_zLP);
   fChain->SetBranchAddress("fMatchHits.tuP", fMatchHits_tuP, &b_fMatchHits_tuP);
   fChain->SetBranchAddress("fMatchHits.tvP", fMatchHits_tvP, &b_fMatchHits_tvP);
   fChain->SetBranchAddress("fMatchHits.xG", fMatchHits_xG, &b_fMatchHits_xG);
   fChain->SetBranchAddress("fMatchHits.yG", fMatchHits_yG, &b_fMatchHits_yG);
   fChain->SetBranchAddress("fMatchHits.zG", fMatchHits_zG, &b_fMatchHits_zG);
   fChain->SetBranchAddress("fMatchHits.xL", fMatchHits_xL, &b_fMatchHits_xL);
   fChain->SetBranchAddress("fMatchHits.yL", fMatchHits_yL, &b_fMatchHits_yL);
   fChain->SetBranchAddress("fMatchHits.zL", fMatchHits_zL, &b_fMatchHits_zL);
   fChain->SetBranchAddress("fMatchHits.pT", fMatchHits_pT, &b_fMatchHits_pT);
   fChain->SetBranchAddress("fMatchHits.eta", fMatchHits_eta, &b_fMatchHits_eta);
   fChain->SetBranchAddress("fMatchHits.phi", fMatchHits_phi, &b_fMatchHits_phi);
   fChain->SetBranchAddress("fMatchHits.ox", fMatchHits_ox, &b_fMatchHits_ox);
   fChain->SetBranchAddress("fMatchHits.oy", fMatchHits_oy, &b_fMatchHits_oy);
   fChain->SetBranchAddress("fMatchHits.oz", fMatchHits_oz, &b_fMatchHits_oz);
   fChain->SetBranchAddress("fMatchHits.npoint", fMatchHits_npoint, &b_fMatchHits_npoint);
   fChain->SetBranchAddress("fMatchHits.firstPointR", fMatchHits_firstPointR, &b_fMatchHits_firstPointR);
   fChain->SetBranchAddress("fMatchHits.firstPointZ", fMatchHits_firstPointZ, &b_fMatchHits_firstPointZ);
   fChain->SetBranchAddress("fMatchHits.end", fMatchHits_end, &b_fMatchHits_end);
   fChain->SetBranchAddress("fIsValid", &fIsValid, &b_EventT_fIsValid);
   Notify();
}

Bool_t T::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

void T::Show(Long64_t entry)
{
   // Print contents of entry.
   // If entry is not specified, print current entry
   if (!fChain) return;

   fChain->Show(entry);
}
Int_t T::Cut(Long64_t entry)
{
   // This function may be called from Loop.
   // returns  1 if entry is accepted.
   // returns -1 otherwise.
   return 1;
}
void T::Loop()
{}
//#endif // #ifdef T_cxx
