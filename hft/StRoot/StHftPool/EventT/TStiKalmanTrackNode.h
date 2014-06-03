#ifndef TStiKalmanTrackNode_h
#define TStiKalmanTrackNode_h

#include "TObject.h"
#include "Sti/StiKalmanTrackNode.h"


class TStiKalmanTrackNode : public TObject
{
public:

   TStiKalmanTrackNode();
   TStiKalmanTrackNode(const StiKalmanTrackNode &stiKTN);
   TStiKalmanTrackNode & operator=(const StiKalmanTrackNode &stiKTN);
   virtual void Print(Option_t *opt = "") const;

   friend bool operator< (const TStiKalmanTrackNode& lhs, const TStiKalmanTrackNode& rhs);

protected:

   double      fEnergyLosses;
   double      fLayerRadius;
   std::string fStiDetectorName;

   ClassDef(TStiKalmanTrackNode, 1)
};


bool operator< (const TStiKalmanTrackNode& lhs, const TStiKalmanTrackNode& rhs);


#endif
