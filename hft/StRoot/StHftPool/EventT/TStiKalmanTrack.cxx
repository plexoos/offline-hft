
#include "StHftPool/EventT/TStiKalmanTrack.h"

#include "Sti/StiKalmanTrackNode.h"


ClassImp(TStiKalmanTrack)


TStiKalmanTrack::TStiKalmanTrack() : TObject(),
   fNodes(), fEnergyLosses(0)
{
}


TStiKalmanTrack::TStiKalmanTrack(const StiKalmanTrack & stiKTrack) : TObject(),
   fNodes(), fEnergyLosses(0)
{
   Info("TStiKalmanTrack", "Convert StiKalmanTrack to TStiKalmanTrack");

   StiKTNIterator it = stiKTrack.begin();

   // Loop over track nodes
   for ( ; it != stiKTrack.end(); ++it)
   {
      StiKalmanTrackNode *stiNode = &(*it);

      if ( !stiNode ) {
         Info("TStiKalmanTrack", "Invalid kalman node. Skipping to next one...");
         continue;
      }

      fNodes.insert( TStiKalmanTrackNode(*stiNode) );
      fEnergyLosses += stiNode->getEnergyLosses();
   }
}


const TStiKalmanTrackNode& TStiKalmanTrack::GetDcaNode() const { return *fNodes.begin(); }

double TStiKalmanTrack::GetEnergyLosses() const { return fEnergyLosses; }


void TStiKalmanTrack::Print(Option_t *opt) const
{
   printf("fEnergyLosses: %f\n", fEnergyLosses);

   std::set<TStiKalmanTrackNode>::const_iterator iTStiKTrackNode = fNodes.begin();

   for ( ; iTStiKTrackNode != fNodes.end(); ++iTStiKTrackNode) {
      iTStiKTrackNode->Print();
   }
}