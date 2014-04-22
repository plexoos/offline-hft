/* $Id$ */

#include <stdio.h>
#include <stdexcept>

#include "TDataSetIter.h"
#include "THashList.h"
#include "TGeoVolume.h"
#include "TGeoMatrix.h"
#include "TVector3.h"

#include "Sti/StiPlanarShape.h"
#include "Sti/StiCylindricalShape.h"
#include "Sti/StiMaterial.h"
#include "Sti/StiPlacement.h"
#include "Sti/StiDetector.h"
#include "Sti/Base/Factory.h"
#include "Sti/StiToolkit.h"
#include "Sti/StiElossCalculator.h"
#include "Sti/StiNeverActiveFunctor.h"
#include "StiTest/StiTestDetectorBuilder.h"
#include "tables/St_HitError_Table.h"
#include "StEvent/StEvent.h"
#include "StEvent/StEventTypes.h"
#include "StPxlDbMaker/StPxlDb.h"
#include "StPxlDbMaker/StPxlDbMaker.h"
#include "StPxlUtil/StPxlConstants.h"
#include "StiMaker/StiDetectorVolume.h"
#include "StBFChain/StBFChain.h"

using namespace std;


StiTestDetectorBuilder::StiTestDetectorBuilder(ETestGeomType geomType) :
   StiDetectorBuilder("testGeom", false, "none"), mSiMaterial(0), mHybridMaterial(0),
   mTestGeomType(geomType)
{ }


/** Build the pixel detector components. */
void StiTestDetectorBuilder::buildDetectors(StMaker &source)
{
   LOG_INFO << "StiTestDetectorBuilder::buildDetectors() -I- Started" << endm;

   // 2 real rows, but we have detector elements and support elements.
   //setNRows(1);

   SetCurrentDetectorBuilder(this);

   // Gas material must be defined. Here we use air properties
   _gasMat = add(new StiMaterial("PixelAir", 7.3, 14.61, 0.001205, 30420.*0.001205, 7.3 * 12.e-9));

   switch (mTestGeomType) {
   case kSimpleBox:
      buildSimpleBox();
      break;
   case kSimpleBox2:
      buildSimpleBox2();
      break;
   case kSimplePlane:
      buildSimplePlane();
      break;
   case kSimpleTube:
      buildSimpleTube();
      break;
   case kSimpleTubeSector:
      buildSimpleTubeSector();
      break;
   case kEnclosedTubes:
      buildEnclosedTubes();
      break;
   default:
      buildSimpleBox();
   }

   // XXX: Check sti geometry by converting it to drawable root objects
   TFile fileTmp("sti2rootgeo_test.root", "RECREATE");
   StiDetectorVolume *stiDetVol = new StiDetectorVolume(*this);
   stiDetVol->Write();
   fileTmp.Close();
   delete stiDetVol;
}


/** Creates inactive sti volumes for the pixel support material. */
void StiTestDetectorBuilder::buildSimpleBox()
{
   Info("StiTestDetectorBuilder::buildSimpleBox", "XXX");

   // Build average inactive volumes
   const VolumeMap_t pxlVolumes[] = {
      {"PSAL1", "Long tube in half pixel support", "HALL_1/CAVE_1/TpcRefSys_1/IDSM_1/PSUP_1/PSAL_1", "", ""},
      {"PSAL2", "Long tube in half pixel support", "HALL_1/CAVE_1/TpcRefSys_1/IDSM_1/PSUP_2/PSAL_1", "", ""}
   };

   int nPxlVolumes = sizeof(pxlVolumes) / sizeof(VolumeMap_t);
   LOG_DEBUG << " # of volume(s) : " << nPxlVolumes << endm;

   for (int i = 0; i < nPxlVolumes; i++) {

      TString path(pxlVolumes[i].path);

      bool isAvail = gGeoManager->cd(path);

      if (!isAvail) {
         Error("buildSimpleBox()", "Cannot find node %s. Skipping to next node...", path.Data());
         continue;
      }

      TGeoNode *geoNode = gGeoManager->GetCurrentNode();

      if (!geoNode) continue;

      LOG_DEBUG << "Current node : " << i << "/" << nPxlVolumes << " path is : " << pxlVolumes[i].name << endm;
      LOG_DEBUG << "Number of daughters : " << geoNode->GetNdaughters() << " weight : " << geoNode->GetVolume()->Weight() << endm;

      StiVMCToolKit::LoopOverNodes(geoNode, path, pxlVolumes[i].name, MakeAverageVolume);

      // Access last added volume
      int row = getNRows() - 1;
      int sector = 0;
      StiDetector *stiDetector = getDetector(row, sector);
      stiDetector->setIsOn(true);
      StiMaterial *mat = stiDetector->getMaterial();
      mat->set(mat->getName(), mat->getZ(), mat->getA(), mat->getDensity()*10, mat->getRadLength(), mat->getIonization());

      // Replace the original StiElossCalculator with one based on the modified material
      StiElossCalculator *elossCalculator = stiDetector->getElossCalculator();
      delete elossCalculator;
      stiDetector->setElossCalculator(new StiElossCalculator(mat->getZOverA(), mat->getIonization(), mat->getA(), mat->getZ(), mat->getDensity()));

      // Adjust the volume position by placing it at z=0
      StiPlacement *stiPlacement = stiDetector->getPlacement();
      stiPlacement->setZcenter(0);
   }
}


/** Creates inactive sti volumes for the pixel support material. */
void StiTestDetectorBuilder::buildSimpleBox2()
{
   Info("StiTestDetectorBuilder::buildSimpleBox2", "XXX");

   // Build average inactive volumes
   const VolumeMap_t pxlVolumes[] = {
      {"PSAL1", "Long tube in half pixel support", "HALL_1/CAVE_1/TpcRefSys_1/IDSM_1/PSUP_1/PSAL_1", "", ""},
      {"PSAL2", "Long tube in half pixel support", "HALL_1/CAVE_1/TpcRefSys_1/IDSM_1/PSUP_2/PSAL_1", "", ""}
   };

   gGeoManager->cd("HALL_1/CAVE_1/TpcRefSys_1/IDSM_1/PSUP_1/PSAL_1");
   TGeoNode *geoNode1 = gGeoManager->GetCurrentNode();

   TGeoCombiTrans* myMatrix = (TGeoCombiTrans*) geoNode1->GetMatrix();

   const double *xyz = myMatrix->GetTranslation();
   myMatrix->SetTranslation(xyz[1], xyz[0], 0);

   int nPxlVolumes = sizeof(pxlVolumes) / sizeof(VolumeMap_t);
   LOG_DEBUG << " # of volume(s) : " << nPxlVolumes << endm;

   for (int i = 0; i < nPxlVolumes; i++) {
      TString path(pxlVolumes[i].path);

      bool isAvail = gGeoManager->cd(path);

      if (!isAvail) {
         Error("buildSimpleBox2()", "Cannot find node %s. Skipping to next node...", path.Data());
         continue;
      }

      TGeoNode *geoNode = gGeoManager->GetCurrentNode();

      if (!geoNode) continue;

      LOG_DEBUG << "Current node : " << i << "/" << nPxlVolumes << " path is : " << pxlVolumes[i].name << endm;
      LOG_DEBUG << "Number of daughters : " << geoNode->GetNdaughters() << " weight : " << geoNode->GetVolume()->Weight() << endm;

      StiVMCToolKit::LoopOverNodes(geoNode, path, pxlVolumes[i].name, MakeAverageVolume);

      // Access last added volume
      int row = getNRows() - 1;
      int sector = 0;
      StiDetector *stiDetector = getDetector(row, sector);
      stiDetector->setIsOn(true);
      StiMaterial *mat = stiDetector->getMaterial();
      mat->set(mat->getName(), mat->getZ(), mat->getA(), mat->getDensity()*10, mat->getRadLength(), mat->getIonization());

      // Replace the original StiElossCalculator with one based on the modified material
      StiElossCalculator *elossCalculator = stiDetector->getElossCalculator();
      delete elossCalculator;
      stiDetector->setElossCalculator(new StiElossCalculator(mat->getZOverA(), mat->getIonization(), mat->getA(), mat->getZ(), mat->getDensity()));

      // Adjust the volume position by placing it at z=0
      StiPlacement *stiPlacement = stiDetector->getPlacement();
      stiPlacement->setZcenter(0);
   }
}


/** Creates inactive sti volumes for the pixel support material. */
void StiTestDetectorBuilder::buildSimplePlane()
{
   Info("StiTestDetectorBuilder::buildSimplePlane", "XXX");

   // Build average inactive volumes
   const VolumeMap_t pxlVolumes[] = {
      {"PSCL1", "Long plane in half pixel support", "HALL_1/CAVE_1/TpcRefSys_1/IDSM_1/PSUP_1/PSCL_1", "", ""},
      {"PSCL2", "Long plane in half pixel support", "HALL_1/CAVE_1/TpcRefSys_1/IDSM_1/PSUP_2/PSCL_1", "", ""}
   };

   int nPxlVolumes = sizeof(pxlVolumes) / sizeof(VolumeMap_t);
   LOG_DEBUG << " # of volume(s) : " << nPxlVolumes << endm;

   for (int i = 0; i < nPxlVolumes; i++) {

      TString path(pxlVolumes[i].path);

      bool isAvail = gGeoManager->cd(path);

      if (!isAvail) {
         Error("buildSimplePlane()", "Cannot find node %s. Skipping to next node...", path.Data());
         continue;
      }

      TGeoNode *geoNode = gGeoManager->GetCurrentNode();

      if (!geoNode) continue;

      LOG_DEBUG << "Current node : " << i << "/" << nPxlVolumes << " path is : " << pxlVolumes[i].name << endm;
      LOG_DEBUG << "Number of daughters : " << geoNode->GetNdaughters() << " weight : " << geoNode->GetVolume()->Weight() << endm;

      StiVMCToolKit::LoopOverNodes(geoNode, path, pxlVolumes[i].name, MakeAverageVolume);

      // The created Sti detectors share the same shape and material
      // Access last added volume
      int row = getNRows() - 1;
      int sector = 0;
      StiDetector *stiDetector = getDetector(row, sector);
      stiDetector->setIsOn(true);

      StiMaterial *mat = stiDetector->getMaterial();
      // Set density to 2.7 g/cm^3
      mat->set(mat->getName(), mat->getZ(), mat->getA(), 2.7, mat->getRadLength(), mat->getIonization());

      // Replace the original StiElossCalculator with one based on the modified material
      StiElossCalculator *elossCalculator = stiDetector->getElossCalculator();
      delete elossCalculator;
      stiDetector->setElossCalculator(new StiElossCalculator(mat->getZOverA(), mat->getIonization(), mat->getA(), mat->getZ(), mat->getDensity()));

      // Adjust the volume position by placing it at z=0
      StiPlacement *stiPlacement = stiDetector->getPlacement();
      stiPlacement->setZcenter(0);

      StiPlanarShape *stiShape = (StiPlanarShape*) stiDetector->getShape();
      stiShape->setThickness(2); // set thickness to 2 cm
   }
}


/** Creates a single aluminum tube. */
void StiTestDetectorBuilder::buildSimpleTube()
{
   Info("StiTestDetectorBuilder::buildSimpleTube", "XXX");

   // aluminum
   double z = 13;
   double a_mass = 26.98;
   double density = 2.7;
   double radLength = 24;
   double ionization = 100e-9; // XXX:ds: Need correct value. Victor claims this value is not used anywhere

   // z, A, density, radLength, ionization
   StiMaterial *stiMaterial = new StiMaterial("aluminum", z, a_mass, density, radLength, ionization);

   add(stiMaterial);

   // First tube
   StiElossCalculator *elossCalculator = new StiElossCalculator(z/a_mass, ionization * ionization, a_mass, z, density);

   double halfDepth = 20;
   double thickness = 1;
   double outerRadius = 13;

   StiShape *stiShape = new StiCylindricalShape("cylone", halfDepth, thickness, outerRadius, 2*M_PI);

   add(stiShape);

   StiPlacement *stiPlacement = new StiPlacement();

   stiPlacement->setZcenter(0);
   stiPlacement->setLayerRadius(outerRadius-thickness/2);
   stiPlacement->setLayerAngle(0);
   stiPlacement->setRegion(StiPlacement::kMidRapidity);
   stiPlacement->setNormalRep(0, outerRadius-thickness/2, 0);

   StiDetector *stiDetector = getDetectorFactory()->getInstance();

   stiDetector->setName("tubeone");
   stiDetector->setIsOn(true);
   stiDetector->setIsActive(new StiNeverActiveFunctor());
   stiDetector->setIsContinuousMedium(false); // true for gases
   stiDetector->setIsDiscreteScatterer(true); // true for anything other than gas
   stiDetector->setGroupId(kTestGeomId);
   stiDetector->setShape(stiShape);
   stiDetector->setPlacement(stiPlacement);
   stiDetector->setGas(getGasMat()); // XXX:ds: Not sure what this does
   stiDetector->setMaterial(stiMaterial);
   stiDetector->setElossCalculator(elossCalculator);

   int layer = getNRows();
   add(layer, 0, stiDetector);
   cout << "StiTestDetectorBuilder::buildEnclosedTubes build detector " << stiDetector->getName() << " at layer " << layer << endl;
}


/** Creates inactive sti volumes for the pixel support material. */
void StiTestDetectorBuilder::buildSimpleTubeSector()
{
   Info("StiTestDetectorBuilder::buildSimpleTubeSector", "XXX");

   // Build average inactive volumes
   const VolumeMap_t pxlVolumes[] = {
      {"DTUH1", "Dtube part of pixel support",  "HALL_1/CAVE_1/TpcRefSys_1/IDSM_1/DTUH_1", "", ""},
      {"DTUH2", "Dtube part of pixel support",  "HALL_1/CAVE_1/TpcRefSys_1/IDSM_1/DTUH_2", "", ""}
   };

   int nPxlVolumes = sizeof(pxlVolumes) / sizeof(VolumeMap_t);
   LOG_DEBUG << " # of volume(s) : " << nPxlVolumes << endm;

   for (int i = 0; i < nPxlVolumes; i++) {
      TString path(pxlVolumes[i].path);

      bool isAvail = gGeoManager->cd(path);

      if (!isAvail) {
         Error("buildSimpleTubeSector()", "Cannot find node %s. Skipping to next node...", path.Data());
         continue;
      }

      TGeoNode *geoNode = gGeoManager->GetCurrentNode();

      if (!geoNode) continue;

      LOG_DEBUG << "Current node : " << i << "/" << nPxlVolumes << " path is : " << pxlVolumes[i].name << endm;
      LOG_DEBUG << "Number of daughters : " << geoNode->GetNdaughters() << " weight : " << geoNode->GetVolume()->Weight() << endm;

      StiVMCToolKit::LoopOverNodes(geoNode, path, pxlVolumes[i].name, MakeAverageVolume);

      // The created Sti detectors share the same shape and material
      // Access last added volume
      int row = getNRows() - 1;
      int sector = 0;
      StiDetector *stiDetector = getDetector(row, sector);
      stiDetector->setIsOn(true);
      StiMaterial *mat = stiDetector->getMaterial();
      mat->set(mat->getName(), mat->getZ(), mat->getA(), mat->getDensity()*10, mat->getRadLength(), mat->getIonization());

      // Replace the original StiElossCalculator with one based on the modified material
      StiElossCalculator *elossCalculator = stiDetector->getElossCalculator();
      delete elossCalculator;
      stiDetector->setElossCalculator(new StiElossCalculator(mat->getZOverA(), mat->getIonization(), mat->getA(), mat->getZ(), mat->getDensity()));

      // Adjust the volume position by placing it at z=0
      StiPlacement *stiPlacement = stiDetector->getPlacement();
      stiPlacement->setZcenter(0);

      StiCylindricalShape *stiShape = (StiCylindricalShape*) stiDetector->getShape();
      stiShape->setOpeningAngle(0.5);
   }
}


/** Creates inactive sti volumes for the pixel support material. */
void StiTestDetectorBuilder::buildEnclosedTubes()
{
   Info("StiTestDetectorBuilder::buildEnclosedTubes", "XXX");

   // aluminum
   double z = 13;
   double a_mass = 26.98;
   double density = 2.7;
   double radLength = 24;
   double ionization = 100e-9; // XXX:ds: Need correct value

   StiMaterial *stiMaterial = new StiMaterial("aluminum", z, a_mass, density, radLength, ionization);

   add(stiMaterial);

   // First tube
   StiElossCalculator *elossCalculator = new StiElossCalculator(z/a_mass, ionization * ionization, a_mass, z, density);

   double halfDepth = 20;
   double thickness = 1;
   double outerRadius = 13;

   StiShape *stiShape = new StiCylindricalShape("cylone", halfDepth, thickness, outerRadius, 2*M_PI);

   add(stiShape);

   StiPlacement *stiPlacement = new StiPlacement();

   stiPlacement->setZcenter(0);
   stiPlacement->setLayerRadius(outerRadius-thickness/2);
   stiPlacement->setLayerAngle(0);
   stiPlacement->setRegion(StiPlacement::kMidRapidity);
   stiPlacement->setNormalRep(0, outerRadius-thickness/2, 0);

   StiDetector *stiDetector = getDetectorFactory()->getInstance();

   stiDetector->setName("tubeone");
   stiDetector->setIsOn(true);
   stiDetector->setIsActive(new StiNeverActiveFunctor());
   stiDetector->setIsContinuousMedium(false); // true for gases
   stiDetector->setIsDiscreteScatterer(true); // true for anything other than gas
   stiDetector->setGroupId(kTestGeomId);
   stiDetector->setShape(stiShape);
   stiDetector->setPlacement(stiPlacement);
   stiDetector->setGas(getGasMat()); // XXX:ds: Not sure what this does
   stiDetector->setMaterial(stiMaterial);
   stiDetector->setElossCalculator(elossCalculator);

   int layer = getNRows();
   add(layer, 0, stiDetector);
   cout << "StiTestDetectorBuilder::buildEnclosedTubes build detector " << stiDetector->getName() << " at layer " << layer << endl;

   // Second tube
   elossCalculator = new StiElossCalculator(_gasMat->getZ()/_gasMat->getA(), _gasMat->getIonization()*_gasMat->getIonization(), _gasMat->getA(), _gasMat->getZ(), _gasMat->getDensity());

   halfDepth = 20;
   thickness = 5;
   outerRadius = 15;

   stiShape = new StiCylindricalShape("cyltwo", halfDepth, thickness, outerRadius, 2*M_PI);

   add(stiShape);

   stiPlacement = new StiPlacement();

   stiPlacement->setZcenter(0);
   stiPlacement->setLayerRadius(outerRadius-thickness/2);
   stiPlacement->setLayerAngle(0);
   stiPlacement->setRegion(StiPlacement::kMidRapidity);
   stiPlacement->setNormalRep(0, outerRadius-thickness/2, 0);

   stiDetector = getDetectorFactory()->getInstance();

   stiDetector->setName("tubetwo");
   stiDetector->setIsOn(true);
   stiDetector->setIsActive(new StiNeverActiveFunctor());
   stiDetector->setIsContinuousMedium(false); // true for gases
   stiDetector->setIsDiscreteScatterer(true); // true for anything other than gas
   stiDetector->setGroupId(kTestGeomId);
   stiDetector->setShape(stiShape);
   stiDetector->setPlacement(stiPlacement);
   stiDetector->setGas(getGasMat()); // XXX:ds: Not sure what this does
   stiDetector->setMaterial(_gasMat);
   stiDetector->setElossCalculator(elossCalculator);

   layer = getNRows();
   add(layer, 0, stiDetector);
   cout << "StiTestDetectorBuilder::buildEnclosedTubes build detector " << stiDetector->getName() << " at layer " << layer << endl;
}
