#ifndef EventT_h
#define EventT_h

#include <string.h>
#include "TObject.h"
#include "HitT.h"
#include "TrackT.h"
#include "VertexT.h"
#include "HitMatchT.h"
#include "TClonesArray.h"
#include "TRefArray.h"
#include "TRef.h"
#include "StHftPool/EventT/TStiKalmanTrack.h"
#include "Sti/StiTrackContainer.h"

class HftMatchedTree;
class StEvent;
class StPxlDb;
class StIstDb;


class EventTHeader
{
public:
   Int_t      fEvtNum;
   Int_t      fRun;
   Int_t      fDate;
   Double32_t fField;
public:
   EventTHeader() : fEvtNum(0), fRun(0), fDate(0), fField(0) { }
   virtual ~EventTHeader() { }
   void   Set(Int_t i, Int_t r, Int_t d, Double32_t Field = 0)
   { fEvtNum = i; fRun = r; fDate = d; fField = Field;}
   Int_t  GetEvtNum() const { return fEvtNum; }
   Int_t  GetRun() const { return fRun; }
   Int_t  GetDate() const { return fDate; }
   Double32_t GetField() const {return fField;}

   ClassDef(EventTHeader, 1) //EventT Header
};


class EventT : public TObject
{

public:
   UInt_t         fNPTracks;
   UInt_t         fNvertex;           //Number of vertex
   UInt_t         fNtrack;            //Number of tracks
   UInt_t         fNhit;              //Number of hits
   UInt_t         fNmatchhit;         //
   UInt_t         fFlag;
   Float_t        fVzVpd;
   Int_t          fNVpdHits;           // nVpdHitsEast * 100 + nVpdHitsWest
   EventTHeader   fEvtHdr;
   Double32_t     fVertex[3];         //
   Double32_t     fCovariantMatrix[6];//
   UInt_t         fNPredHFT[4];        //
   TClonesArray  *fVertices;          //->array of primary vertices
   TClonesArray  *fTracks;            //->array with all tracks
   TClonesArray  *fHits;              //->array with all hits
   TClonesArray  *fMatchHits;         //->array with hit-track match
   Bool_t         fIsValid;           //
   std::vector<TStiKalmanTrack> fTStiKalmanTracks;

   EventT();
   virtual ~EventT();
   Int_t             Build(StEvent *pEventT, UInt_t minNoHits = 2, Double_t pCut = 0.2);
   Int_t             Fill(StiTrackContainer &stiTrackContainer);
   void              Clear(Option_t *option = "");
   Bool_t            IsValid() const { return fIsValid; }
   void              SetNtrack(UInt_t n) { fNtrack = n; }
   void              SetNhit(UInt_t n) { fNhit = n; }
   void              SetNvertex(UInt_t n) { fNvertex = n; }
   void              SetNmatchhit(UInt_t n) { fNmatchhit = n; }
   void              SetFlag(UInt_t f) { fFlag = f; }
   void              SetNPredHFT(UInt_t *val) { for (int i = 0; i < 4; i++) fNPredHFT[i] = val[i]; }
   void              SetDbDatasets(const StPxlDb *pxlDb, const StIstDb *istDb) { fPxlDb = pxlDb; fIstDb = istDb; }
   void              SetHeader(Int_t i, Int_t run, Int_t date, Double32_t field);
   TrackT           *AddTrackT();
   HitT             *AddHitT();
   VertexT          *AddVertexT();
   HitMatchT        *AddHitMatchT();
   Double32_t        GetVertex(UInt_t i = 0) {return (i < 3) ? fVertex[i] : 0;}
   UInt_t            GetTotalNoTracks() const {return fNPTracks;}
   UInt_t            GetNtrack() const { return fNtrack; }
   UInt_t            GetNhit() const { return fNhit; }
   UInt_t            GetNmatchhit() const { return fNmatchhit; }
   UInt_t            GetNvertex() const { return fNvertex; }
   UInt_t            GetFlag() const { return fFlag; }
   EventTHeader     *GetHeader() { return &fEvtHdr; }
   const Double32_t *GetVertex() const {return fVertex;}
   const Double32_t *GetCovMatrix() const {return fCovariantMatrix;}
   TClonesArray     *GetTracks() const {return fTracks;}
   TClonesArray     *GetHits() const {return fHits;}
   TClonesArray     *GetVertices() const {return fVertices;}
   TClonesArray     *GetMatchHits() const {return fMatchHits;}
   TrackT           *GetTrackT(UInt_t i = 0) const {return fTracks && i < fNtrack ? (TrackT *) fTracks->At(i) : 0;}
   HitT             *GetHitT(UInt_t i = 0) const {return fHits && i < fNhit ? (HitT *) fHits->At(i) : 0;}
   VertexT          *GetVertexT(UInt_t i = 0) const {return fVertices && i < fNvertex ? (VertexT *) fVertices->At(i) : 0;}
   HitMatchT        *GetHitMatchT(UInt_t i = 0) const {return fMatchHits && i < fNmatchhit ? (HitMatchT *) fMatchHits->At(i) : 0; }
   Int_t             GetIndexOfTrackT(const TrackT *obj) const {return fTracks->IndexOf(obj);}
   Int_t             GetIndexOfHitT(const HitT *obj) const {return fHits->IndexOf(obj);}
   Int_t             GetIndexOfVertexT(const VertexT *obj) const {return fVertices->IndexOf(obj);}
   Int_t             GetIndexOfHitMatchT(const HitMatchT *obj) const {return fMatchHits->IndexOf(obj);}
   const StPxlDb*    GetPxlDb() const { return fPxlDb; }
   const StIstDb*    GetIstDb() const { return fIstDb; }
   virtual void      Print(Option_t *opt = "") const;

private:

   const StPxlDb*   fPxlDb;   //!< Pointer to data with spatial transformations for PXL
   const StIstDb*   fIstDb;   //!< Pointer to data with spatial transformations for IST

   ClassDef(EventT, 1)
};

#endif