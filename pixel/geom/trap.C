// $Id$
//
// play script for incarnation of geometry for STAR HFT
// This make a tube - some issues
// Test of ARb8
// Compare with incarnation of TRAP instead.


#include <TSystem.h>

#include <TGeoManager.h>
#include <TGeoMatrix.h>
#include <TMath.h>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <TH1.h>

void Geom2(){


  TGeoManager* geom= new TGeoManager("world","geom1");
  
  TGeoElementTable *table = gGeoManager->GetElementTable();

  TGeoElement *elBe = table->FindElement("BE");
  TGeoElement *elAl = table->FindElement("AL");

  TGeoMaterial *mateVacuum = new TGeoMaterial("vacuum",0,0,0);
  TGeoMaterial *mateBe     = new TGeoMaterial("be",elBe,1.848 );
  TGeoMedium   *medVacuum  = new TGeoMedium("vacuum",1,mateVacuum);
  TGeoMedium   *medBe      = new TGeoMedium("be",1,mateBe);
  TGeoMaterial *matAl = new TGeoMaterial("Al", 26.98,13,  2.7);
  TGeoMedium *Al     = new TGeoMedium("Aluminium",2, matAl);

  TGeoMaterial *mat5 = new TGeoMaterial("air",14.61,7.3,0.1205000E-02);
  mat5->SetUniqueID(   5);
  TGeoMedium *medAir = new TGeoMedium("air",5, 5,0,0,0,20,0.1000000E+11,0.2488534,0.5000000E-01,1.150555);
  TGeoVolume *CAVE = gGeoManager->MakeBox("CAVE",medAir, 60, 60, 400);
  CAVE->SetVisibility(0);



  gGeoManager->SetTopVolume(CAVE);
  
  #include "trap.txt"

  
  




  gGeoManager->CloseGeometry();
  
  gGeoManager->SetVisLevel(4);
  CAVE->Draw("ogle");
}
  
