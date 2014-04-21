#ifndef StiTestDetectorBuilder_h
#define StiTestDetectorBuilder_h

#include "Sti/StiDetectorBuilder.h"

class StPxlDb;


class StiTestDetectorBuilder : public StiDetectorBuilder
{
public:

   enum ETestGeomType {kDefault, kSimpleBox, kSimpleBox2, kSimpleBox8, kSimplePlane, kSimpleTube, kSimpleTubeSector, kEnclosedTubes};

   StiTestDetectorBuilder(StiTestDetectorBuilder::ETestGeomType geomType);
   virtual void buildDetectors(StMaker &source);

protected:

   StiMaterial *mSiMaterial;
   StiMaterial *mHybridMaterial;

private:

   void buildSimpleBox();
   void buildSimpleBox2();
   void buildSimplePlane();
   void buildSimpleTube();
   void buildSimpleTubeSector();
   void buildEnclosedTubes();

   ETestGeomType mTestGeomType;
};

#endif
