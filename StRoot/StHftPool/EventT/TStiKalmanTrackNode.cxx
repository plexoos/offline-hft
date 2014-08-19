#include "StHftPool/EventT/TStiKalmanTrackNode.h"

#include <boost/regex.hpp>

#include "StarClassLibrary/StThreeVector.hh"
#include "Sti/StiPlacement.h"


ClassImp(TStiKalmanTrackNode)


TStiKalmanTrackNode::TStiKalmanTrackNode() : TObject(),
   fPosition(), fTrackP(), fEnergyLosses(-1), fNodeRadius(0), fNodeCenterRefAngle(0), fVolumeName()
{
}


TStiKalmanTrackNode::TStiKalmanTrackNode(const StiKalmanTrackNode &stiKTN) : TObject(),
   fPosition(), fTrackP(), fEnergyLosses(-1), fNodeRadius(0), fNodeCenterRefAngle(0), fVolumeName()
{
   // Access node parameters
   fPosition.SetXYZ(stiKTN.x_g(), stiKTN.y_g(), stiKTN.z_g());

   StThreeVector<double> p3 = stiKTN.getGlobalMomentum();
   fTrackP.SetXYZ( p3.x(), p3.y(),  p3.z() );

   fEnergyLosses = stiKTN.getEnergyLosses();

   if (stiKTN.getDetector()) {
      fVolumeName = stiKTN.getDetector()->getName();
      StiPlacement* stiPlacement = stiKTN.getDetector()->getPlacement();
      assert(stiPlacement);
      fNodeRadius = stiPlacement->getLayerRadius();
      fNodeCenterRefAngle = stiPlacement->getCenterRefAngle();
   }
}


TStiKalmanTrackNode & TStiKalmanTrackNode::operator=(const StiKalmanTrackNode &stiKTN)
{
   TStiKalmanTrackNode tmpNode(stiKTN);

   fVolumeName = tmpNode.fVolumeName;

   return *this;
}


bool TStiKalmanTrackNode::MatchedVolName(const std::string & pattern) const
{
   if (fVolumeName.empty()) return true;

   boost::regex r(pattern);
   bool matched = boost::regex_match(fVolumeName, r);

   //if (matched)
   //   Info("MatchedVolName", "Volume [%s] matched pattern [%s]", fVolumeName.c_str(), pattern.c_str());

   return matched;
}


bool TStiKalmanTrackNode::MatchedVolName(const std::set<std::string> & patterns) const
{
   if (fVolumeName.empty() || patterns.empty())
      return true;

   std::set<std::string>::const_iterator iPattern = patterns.begin();

   for( ; iPattern != patterns.end(); ++iPattern )
   {
      //Info("MatchedVolName", "Looking for pattern [%s] in fVolumeName [%s]", (*iPattern).c_str(), fVolumeName.c_str());
      if ( MatchedVolName(*iPattern) )
         return true;
   }

   return false;
}


void TStiKalmanTrackNode::Print(Option_t *opt) const
{
   fPosition.Print();
   fTrackP.Print();
   printf("fEnergyLosses:    %f\n", fEnergyLosses);
   printf("fNodeRadius:      %f\n", fNodeRadius);
   printf("fVolumeName:      %s\n", fVolumeName.c_str());
}


bool operator< (const TStiKalmanTrackNode& lhs, const TStiKalmanTrackNode& rhs)
{
   return lhs.fNodeRadius < rhs.fNodeRadius;
}