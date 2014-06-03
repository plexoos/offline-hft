
#include "StHftPool/EventT/TStiKalmanTrack.h"

#include "Sti/StiKalmanTrackNode.h"


ClassImp(TStiKalmanTrack)


TStiKalmanTrack::TStiKalmanTrack() : TObject(),
   fNodes()
{
}


TStiKalmanTrack::TStiKalmanTrack(const StiKalmanTrack & stiKTrack) : TObject(),
   fNodes()
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
   }
}


void TStiKalmanTrack::Print(Option_t *opt) const
{
   std::set<TStiKalmanTrackNode>::const_iterator iTStiKTrackNode = fNodes.begin();

   for ( ; iTStiKTrackNode != fNodes.end(); ++iTStiKTrackNode) {
      iTStiKTrackNode->Print();
   }
}
