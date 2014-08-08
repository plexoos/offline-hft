#include <stdexcept>

#include "StEvent/StEvent.h"
#include "StiTest/StiTestDetectorGroup.h"
#include "StiTest/StiTestDetectorBuilder.h"
#include "Sti/StiHitLoader.h"


StiTestDetectorGroup::StiTestDetectorGroup(StiTestDetectorBuilder::ETestGeomType geomType)
   : StiDetectorGroup<StEvent>("TestGeom", 0, new StiTestDetectorBuilder(geomType), 0)
{}

StiTestDetectorGroup::~StiTestDetectorGroup()
{}

