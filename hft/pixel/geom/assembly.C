// $Id$
//
//Geometry detector assembly example
//Author: Andrei Gheata
// Modified with include for pxl geometry
//
// Is compiled
//
#include <TSystem.h>

#include <TGeoManager.h>
#include <TGeoMatrix.h>
#include <TMath.h>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <TH1.h>


TGeoManager *geom;
Double_t Rmax=3.3;


void FindRad(Double_t x, Double_t y, Double_t z,Double_t theta, Double_t phi, Int_t &nbound, Float_t &length, Float_t &safe, Float_t &rad, Bool_t verbose) {
   Double_t xp  = TMath::Sin(theta)*TMath::Cos(phi);
   Double_t yp  = TMath::Sin(theta)*TMath::Sin(phi);
   Double_t zp  = TMath::Cos(theta);
   Double_t snext;
   char path[256];
   Double_t pt[3];
   Double_t loc[3];
   Double_t epsil = 1.E-3;
   Double_t lastrad = 0.;
   Int_t ismall = 0;
   nbound = 0;
   length = 0.;
   safe   = 0.;
   rad    = 0.;
   TGeoMedium *med;
   TGeoShape *shape;
   geom->InitTrack(x,y,z,xp,yp,zp);
   if (verbose) {
      printf("Track: (%15.10f,%15.10f,%15.10f,%15.10f,%15.10f,%15.10f)\n",
                       x,y,z,xp,yp,zp);
      sprintf(path, "%s", geom->GetPath());
   }                    
   TGeoNode *nextnode = geom->GetCurrentNode();
   safe = geom->Safety();
   while (nextnode) {
      med = 0;
      if (nextnode) med = nextnode->GetVolume()->GetMedium();
      else return;      
      shape = nextnode->GetVolume()->GetShape();
      nextnode = geom->FindNextBoundaryAndStep();
      snext  = geom->GetStep();
      if (snext<1.e-8) {
         ismall++;
         if (ismall > 3) {
            printf("ERROR: Small steps in: %s shape=%s\n",geom->GetPath(), shape->ClassName());
            return;
         }   
         memcpy(pt,geom->GetCurrentPoint(),3*sizeof(Double_t));
         const Double_t *dir = geom->GetCurrentDirection();
         for (Int_t i=0;i<3;i++) pt[i] += epsil*dir[i];
         snext = epsil;
	   
	   
         length += snext;
         rad += lastrad*snext;
         geom->CdTop();
         nextnode = geom->FindNode(pt[0],pt[1],pt[2]);
         if (geom->IsOutside()) return;
         TGeoMatrix *mat = geom->GetCurrentMatrix();
         mat->MasterToLocal(pt,loc);
         if (!geom->GetCurrentVolume()->Contains(loc)) {
//            printf("Woops - out\n");
            geom->CdUp();
            nextnode = geom->GetCurrentNode();
         }   
         continue;
      } else {
	ismall = 0;
      }      
      nbound++;
      Bool_t endTrack=kFALSE;
      if(length+snext> Rmax) {
	snext=Rmax-length;
	endTrack=kTRUE;
      }
      length += snext;
      if (med) {
	Double_t radlen = med->GetMaterial()->GetRadLen();
	if (radlen>1.e-5 && radlen<1.e10) {
	  //	  lastrad = med->GetMaterial()->GetDensity()/radlen;
	  rad += snext/radlen;
	} else {
	  lastrad = 0.;
	}      
	if (verbose) {
	  printf(" STEP #%d: %s\n",nbound, path);
	  Double_t radlen = med->GetMaterial()->GetRadLen();
	  if (radlen>1.e-5 && radlen<1.e10) {
	    
	    printf("    step=%g  length=%g  radlen= %g rad=%g %s summa(rad) %g\n", snext,length,radlen,
		   snext/med->GetMaterial()->GetRadLen(),med->GetName(), rad);
	  }
	    sprintf(path, "%s", geom->GetPath());
	} 
	if(endTrack) return;
      }
   }   
}





   
void assembly()
{
//--- Definition of a geometry for PXL sectors.
// 
   gSystem->Load("libGeom");
   geom = new TGeoManager("Assemblies", 
      "Geometry using assemblies");
   Int_t i;
   //--- define some materials

   TGeoElementTable *table = geom->GetElementTable();
   TGeoElement *elO = table->FindElement("Oxygen");
   TGeoElement *elH = table->FindElement("Hydrogen");
   TGeoElement *elN = table->FindElement("Nitrogen");
   TGeoElement *elC = table->FindElement("Carbon");
   TGeoMixture *matAdhesive = new TGeoMixture("adhesive", 3, 1.2);
   matAdhesive->AddElement(elO, 0.164);
   matAdhesive->AddElement(elC, 0.763);
   matAdhesive->AddElement(elH, 0.073);
   // not correct
   TGeoMixture *matSiAdhesive = new TGeoMixture("SiAdhesive", 3, 1.2);
   matSiAdhesive->AddElement(elO, 0.164);
   matSiAdhesive->AddElement(elC, 0.763);
   matSiAdhesive->AddElement(elH, 0.073);
   matSiAdhesive->SetRadLen(-23,0);



   TGeoMaterial *matVacuum = new TGeoMaterial("Vacuum", 0,0,0);
   TGeoMaterial *matBe = new TGeoMaterial("Be", 9.01, 4, 1.848);
   TGeoMaterial *matAl = new TGeoMaterial("Al", 26.98,13,  2.7);
   TGeoMaterial *matSi = new TGeoMaterial("Si", 27.98, 14, 2.329);
   TGeoMaterial *matAir = new TGeoMaterial("air",14.61,7.3,0.1205000E-02);

   TGeoMedium *Vacuum = new TGeoMedium("Vacuum",1, matVacuum);
   TGeoMedium *Al     = new TGeoMedium("Aluminium",2, matAl);

   

   //
   //   TGeoMaterial *matCarbonFiber   = new TGeoMaterial("CarbonFiber", 12, 6, 1.3, -23.9);
   //   TGeoMaterial *matCarbonFiber   = new TGeoMaterial("CarbonFiber", 12, 6, 1.5);


   TGeoMixture *matCarbonFiber = new TGeoMixture("CarbonFiber", 2, 1.6);
   matCarbonFiber->AddElement(elC, 0.80);
   matCarbonFiber->AddElement(matAdhesive, 0.20);
   //   cout <<    matCarbonFiber->GetRadLen() << endl;
   // cout <<    matAdhesive->GetRadLen() << endl;
   
   TGeoMixture *matKapton = new TGeoMixture("Kapton", 4, 1.42);
   matKapton->AddElement(elH, 0.026362);
   matKapton->AddElement(elC, 0.69113);
   matKapton->AddElement(elN, 0.073270);
   matKapton->AddElement(elO, 0.209235);
   //

   TGeoMaterial *matCarbonBacking = new TGeoMaterial("CarbonFiberBacking", 12, 6, 1.3, -34.0);
   TGeoMedium *CarbonFiber        = new TGeoMedium("CarbonFiber", 3, matCarbonFiber);
   TGeoMedium *CFRPMix            = new TGeoMedium("CFRPMix", 3, matCarbonFiber);
   TGeoMedium *CarbonFiberBacking = new TGeoMedium("CarbonFiberBacking", 4, matCarbonBacking);
   TGeoMedium *Silicon            = new TGeoMedium("Silicon", 5, matSi);
   TGeoMedium *ActiveSilicon      = new TGeoMedium("ActiveSilicon", 6, matSi);


   TGeoMaterial *matAlCable       = new TGeoMaterial("AlCable", 26.98, 13, 2.7, -23.7);
   TGeoMedium *medAlCable         = new TGeoMedium("AlCable", 7, matAlCable);
   TGeoMedium *Be                 = new TGeoMedium("Be", 8, matBe);

   TGeoMedium *Air                = new TGeoMedium("Air", 9, matAir);
   TGeoMedium *medAdhesive        = new TGeoMedium("Adhesive", 10, matAdhesive);
   TGeoMedium *medSiAdhesive      = new TGeoMedium("SiAdhesive", 11, matSiAdhesive);
   TGeoMedium *medFR4     = new TGeoMedium("FR4Board", 12, matKapton);

//--- make the top container volume
 
   TGeoVolume *top = geom->MakeBox("TOP", Air, 20., 20., 35.);
   geom->SetTopVolume(top);
 
   TGeoVolume *BeamPipe = geom->MakeTube("BeamPipe", Be , 1.92 , 2 , 30 );
   {
   BeamPipe ->SetLineColor(1);
   TGeoMatrix *centerPos = new TGeoTranslation(0 , 0 , 0 );
   top->AddNode(BeamPipe, 1 , centerPos);
   }
   
// Make the elementary assembly of the whole structure
   TGeoVolume *tplate = new TGeoVolumeAssembly("PXL");


   Double_t rmin = 2.5;
   Double_t rmax = 8.0;

   TGeoVolume *Sector = new TGeoVolumeAssembly("Sector");


   TGeoVolume *currentTGeo;
   //
   // Include updated sector.txt here
   // 


#include "sector.txt"  

   //
   // create 10 copies of the assembly
   //


   TGeoRotation *rot1 = new TGeoRotation();
   rot1->RotateX(0);
   TGeoRotation *rot;

   for (i=0; i<5; i++) {
      Double_t phi =  36.*i;
      Double_t phirad = phi*TMath::DegToRad();
      rot = new TGeoRotation(*rot1);
      rot->RotateZ(phi);
      // This offset should be the sectorOffset !!
      //
      tplate->AddNode( Sector,i+1,new TGeoCombiTrans(0, 0, 0.0,  rot));

  }   

   top->AddNode(tplate, 1, 0); 
 

  
   //--- close the geometry
   geom->CloseGeometry();
      
   //geom->SetTopVisible();
   geom->SetVisLevel(5);
   geom->SetVisOption(1);
   geom->GetVolume("LADR")->SetLineStyle(kDotted);
   geom->GetVolume("Sensor")->SetLineStyle(kDotted);
   top->Draw("");

   
   //
   // test radlength
   //


   TH1F* hphi2 = new TH1F("PhiRad2","radlenght vs phi (z=0, R=2.2)", 180, 0., 90.);
   TH1F* hphi3 = new TH1F("PhiRad3","radlenght vs phi (z=0, R=3.3)", 180, 0., 90.);
   TH1F* hphi7 = new TH1F("PhiRad7","radlenght vs phi (z=0, R=7.5)", 180, 0., 90.);
   TH1F* hphi9 = new TH1F("PhiRad9","radlenght vs phi (z=0, R=9.0)", 180, 0., 90.);

   double  theta;
   int nbound(0);
   double x(0.0), y(0.0), z(0.0);

   z=0;

  for(float phil=90; phil< 95; phil+=1.0){
     float length(0), safe(0), rad(0);
     double p = phil*TMath::DegToRad();
     double t = 90.0*TMath::DegToRad();

     Rmax=10.0;
     FindRad(x, y, z, t , p,
	     nbound, length, safe, rad, kTRUE);
     hphi2->Fill(phil, rad);
 
  }
  return;

   for(float phil=0.0; phil< 90.0; phil+=0.5){
     float length(0), safe(0), rad(0);
     double p = phil*TMath::DegToRad();
     double t = 90.0*TMath::DegToRad();


     Rmax=2.2;
     FindRad(x, y, z, t , p,
	     nbound, length, safe, rad, kFALSE);
     hphi2->Fill(phil, rad);
     Rmax=3.2;
     FindRad(x, y, z, t , p,
	     nbound, length, safe, rad, kFALSE);
     hphi3->Fill(phil, rad);
     Rmax=7.5;
     FindRad(x, y, z, t , p,
	     nbound, length, safe, rad, kFALSE);
     hphi7->Fill(phil, rad);
     Rmax=9.0;
     FindRad(x, y, z, t , p,
	     nbound, length, safe, rad, kFALSE);
     hphi9->Fill(phil, rad);

   }
   


   /*
   hphi2->SetMinimum(0.0);
   hphi2->SetMinimum(0.015);
   hphi2->Draw();
   hphi3->Draw("same");
   hphi7->Draw("same");
   hphi9->Draw("same");
   */

   z=-23;


  
}   
   
