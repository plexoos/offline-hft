
#include "StHftPool/EventT/TStiKalmanTrackNode.h"

#include "StarClassLibrary/StThreeVector.hh"
#include "Sti/StiPlacement.h"


ClassImp(TStiKalmanTrackNode)


TStiKalmanTrackNode::TStiKalmanTrackNode() : TObject(),
   fPosition(), fTrackP(), fEnergyLosses(-1), fNodeRadius(0), fStiDetectorName()
{
}


TStiKalmanTrackNode::TStiKalmanTrackNode(const StiKalmanTrackNode &stiKTN) : TObject(),
   fPosition(), fTrackP(), fEnergyLosses(-1), fNodeRadius(0), fStiDetectorName()
{
   Info("TStiKalmanTrackNode", "Convert StiKalmanTrackNode to TStiKalmanTrackNode");

   // Access node parameters
   fPosition.SetXYZ(stiKTN.x_g(), stiKTN.y_g(), stiKTN.z_g());

   StThreeVector<double> p3 = stiKTN.getGlobalMomentum();
   fTrackP.SetXYZ( p3.x(), p3.y(),  p3.z() );

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
   fPosition.Print();
   fTrackP.Print();
   printf("fEnergyLosses:    %f\n", fEnergyLosses);
   printf("fNodeRadius:      %f\n", fNodeRadius);
   printf("fStiDetectorName: %s\n", fStiDetectorName.c_str());
}


bool operator< (const TStiKalmanTrackNode& lhs, const TStiKalmanTrackNode& rhs)
{
   return lhs.fNodeRadius < rhs.fNodeRadius;
}
