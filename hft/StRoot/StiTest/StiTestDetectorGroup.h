#ifndef StiTestDetectorGroup_h
#define StiTestDetectorGroup_h

#include "Sti/StiDetectorGroup.h"
#include "StiTest/StiTestDetectorBuilder.h"

class StEvent;


/** Class to create test volumes for Sti tests. */
class StiTestDetectorGroup : public StiDetectorGroup<StEvent>
{
public:
   StiTestDetectorGroup(StiTestDetectorBuilder::ETestGeomType geomType);
   ~StiTestDetectorGroup();
};

#endif
