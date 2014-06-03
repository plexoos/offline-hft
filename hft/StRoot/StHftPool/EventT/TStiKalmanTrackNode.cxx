
#include "StHftPool/EventT/TStiKalmanTrackNode.h"

#include "Sti/StiPlacement.h"


ClassImp(TStiKalmanTrackNode)


TStiKalmanTrackNode::TStiKalmanTrackNode() : TObject(),
   fEnergyLosses(-1), fNodeRadius(0), fStiDetectorName()
{
}


TStiKalmanTrackNode::TStiKalmanTrackNode(const StiKalmanTrackNode &stiKTN) : TObject(),
   fEnergyLosses(-1), fNodeRadius(0), fStiDetectorName()
{
   Info("TStiKalmanTrackNode", "Convert StiKalmanTrackNode to TStiKalmanTrackNode");

   fEnergyLosses = stiKTN.getEnergyLosses();

   if (stiKTN.getDetector()) {
      Info("TStiKalmanTrackNode", "detector found: %s", stiKTN.getDetector()->getName().c_str());
      fStiDetectorName = stiKTN.getDetector()->getName();
      Info("TStiKalmanTrackNode", "detector found: %s", fStiDetectorName.c_str());

      StiPlacement* stiPlacement = stiKTN.getDetector()->getPlacement();
      assert(stiPlacement);
      fNodeRadius = stiPlacement->getLayerRadius();

   } else {
      Info("TStiKalmanTrackNode", "detector not found:" );
   }
}


TStiKalmanTrackNode & TStiKalmanTrackNode::operator=(const StiKalmanTrackNode &stiKTN)
{
   TStiKalmanTrackNode tmpNode(stiKTN);

   fStiDetectorName = tmpNode.fStiDetectorName;

   return *this;
}


void TStiKalmanTrackNode::Print(Option_t *opt) const
{
   printf("fEnergyLosses:    %f\n", fEnergyLosses);
   printf("fNodeRadius:      %f\n", fNodeRadius);
   printf("fStiDetectorName: %s\n", fStiDetectorName.c_str());
}


bool operator< (const TStiKalmanTrackNode& lhs, const TStiKalmanTrackNode& rhs)
{
   return lhs.fNodeRadius < rhs.fNodeRadius;
}
