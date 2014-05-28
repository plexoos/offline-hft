#ifndef TStiKalmanTrackNode_h
#define TStiKalmanTrackNode_h

#include "TObject.h"
#include "Sti/StiKalmanTrack.h"


class TStiKalmanTrackNode : public TObject, private StiKalmanTrackNode
{
protected:

   using StiKalmanTrackNode::mEnergyLosses; //!

   ClassDef(TStiKalmanTrackNode, 1)
};

#endif
