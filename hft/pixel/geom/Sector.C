// $Id$
//
// Can be loaded by .L Sector.C+
// generates geometry file that can be included into
// assembly.C (by include statement)
//
// The little macro run.C executes all the steps.
//
//  Octiober 13 FV
// fixed errors in addTube with named variables (interpreting negative thicknesses.
// Starting adding DoveTail parts.
//

#include "TMath.h"
#include <TVector2.h>
#include "TString.h"
#include <TArc.h>
#include <TLine.h>
#include <TDatime.h>
#include <TMarker.h>
#include "TCanvas.h"
#include "TLatex.h"
#include "TArrow.h"

#include <iostream>
#include <fstream>
#include <iomanip>
#include <map>



int boxColor=kGreen+8;
int markerType= kStar;
int currentColor=kGreen-6;
Bool_t useTGeo=1;
Bool_t useAgml=0;
Bool_t ofOpen=kFALSE;
TString currentMasterVolume = "Sector";
TString geoManager="geom";
Int_t currentVolumeNo=1;
Int_t currentVisibility=1;
Bool_t kMany=kFALSE;
Bool_t debug=kFALSE;

map<string, double> varlist;
map<string, string> volumelist; // mapping of TGeoNames to Geant3 4 characternames used in Agml output.

ofstream of;
ofstream ofvol;
ofstream ofvar;
ofstream ofcontent;

double textSize=0.02;


void setMany(){
  kMany=kTRUE;
}


double dot(TVector2 v1, TVector2 v2){
  return v1.X()*v2.X()+v1.Y()*v2.Y();
}

TLatex* drawText(TVector2 a, string text){
  TLatex *t = new TLatex(a.X(), a.Y(),text.data());
  t->SetTextSize(textSize);
  t->Draw();
  return t;
}

TLatex* drawArrow(TVector2 a, TVector2 b, string text){
  TArrow *ar = new TArrow(a.X(), a.Y(),(a+b).X(), (a+b).Y(),0.02);
  ar->Draw();
  TLatex *t = new TLatex((a+b/2.0).X(), (a+b/2.0).Y(),text.data());
  t->SetTextSize(textSize);
  t->Draw();
  return t;
}

TLine* drawLine(TVector2 a, TVector2 b, double ext=0.0){
//
//
  TLine* t = new TLine(a.X(),a.Y(),a.X()+b.X(),a.Y()+b.Y());
  t->Draw();

  TLine* t1 = new TLine(a.X()+b.X(),a.Y()+b.Y(),a.X()+(2*ext+1)*b.X(),a.Y()+(2*ext+1)*b.Y());
  t1->SetLineStyle(kDashed);
  t1->Draw();

  TLine* t2 = new TLine(a.X(),a.Y(),a.X()-ext*b.X(),a.Y()-ext*b.Y());
  t2->SetLineStyle(kDashed);
  t2->Draw();

return t;
}

//_____________________________________
void showVariables(){
  map<string, double>::iterator it;

 for ( it=varlist.begin() ; it != varlist.end(); it++ )
    cout << (*it).first << " => " << (*it).second << endl;
}

//______________________________________________________________
void defineVolume(string name, string agmlVolume){
  // Have a list of long names that I like and have a stupid max 4 char agmlName.
  //
  volumelist[name] = agmlVolume;
  
}

//_______________________________________________________________________
void findVolume(string searchname, string & name) {
  //
  // If the length of name is <=4 it is likely a valid Geant3 name already.
  //
  int count = volumelist.count(searchname);
  if(count  >0)
    name = volumelist[searchname];
  else {
    name =searchname;
    if(name.size() > 4)
      cout <<"***Error could not find volume name in list : " << name << endl;
  }
}


//_____________________________________
void showVolumeMapping(){
  map<string, string>::iterator it;

 for ( it=volumelist.begin() ; it != volumelist.end(); it++ )
   cout << ((*it).first).data() << " => " << ((*it).second).data() << endl;
}

//______________________________________________
void drawArc(TVector2 C, TVector2 P1, TVector2 P2){
// Define and draw the arc 
// Some of this is to ensure that the proper arc is drawn and not the one with
// the angle spread>180 deg.
// Hopefully the TUBE in gent uses angles just like this. (see statement from TGeoTubes
//
// Here phi1 and phi2 are the starting and ending phi values in degrees. The general phi convention is that the shape 
// ranges from phi1 to phi2 going counterclockwise. The angles can be defined with either negative or positive values.
// They are stored such that phi1 is converted to [0,360] and phi2 > phi1.
//
double rad    = (P1-C).Mod();
double theta1 = (P1-C).Phi()*180.0/TMath::Pi();
double theta2 = (P2-C).Phi()*180.0/TMath::Pi();


if(theta1 > theta2)
  theta2 +=360;


TArc * a = new TArc(C.X(),C.Y(), rad, theta1, theta2);
a->Draw("only");
a->SetFillStyle(4000);
}


void drawBox(TVector2 xc, double width, double thickness, double angle){
double deg2rad = TMath::Pi()/180.0;
 TVector2 dir(1,0), dirn(0,1);
 dir=dir.Rotate(angle*deg2rad);
 dirn=dirn.Rotate(angle*deg2rad);

 TVector2 x00(xc+width/2.0*dir+thickness/2.0*dirn);
 TVector2 x01(xc+width/2.0*dir-thickness/2.0*dirn);
 TVector2 x10(xc-width/2.0*dir+thickness/2.0*dirn);
 TVector2 x11(xc-width/2.0*dir-thickness/2.0*dirn);
 drawLine(x00,x10)->SetLineColor(boxColor);
 drawLine(x01,x11)->SetLineColor(boxColor);
 drawLine(x00,x01)->SetLineColor(boxColor);
 drawLine(x10,x11)->SetLineColor(boxColor);
}

void drawMarker(TVector2 p1){
  TMarker* m = new TMarker(p1.X(),p1.Y(), markerType);
  m->Draw();
}


//_____________________________________________________________
TVector2 Intercept(TVector2 p1, TVector2 v1,
		   TVector2 p2, TVector2 v2, double & det){
  // Find the intersect of two lines;
  // determination ==0 if no solution i.e parallel.
  // The line is determined by a point and a direction vector
  // p1, v1
  //
  double b11=v1.X();
  double b12=v1.Y();
  double b21=v2.X();
  double b22=v2.Y();
  det = -b11*b22+b21*b12;

  if(det !=0){
    double a1 = p1.X()-p2.X();
    double a2 = p1.Y()-p2.Y();
    double t1 = (b22*a1-b21*a2)/det;
    return (p1+t1*v1);
  }

  return p1;
}

//_____________________________________________________________
double GetAngle(TVector2 v1, TVector2 v2){
  double dotv = dot(v1, v2);
  double cost = dotv/(v1.Mod()*v2.Mod());
  return TMath::ACos(cost);
}
    

//______________________________________________________________
void defineVariable(string name, double value, string comment){

  varlist[name]= value;
 if(useTGeo && ofOpen){
   of << "  double " << name.data() << "( " << value << "); //" << comment.data() << endl;
 }
 if(useAgml && ofOpen){
   ofvar << "<varlist type=\"float\" comment=\" " << comment.data() << "\" > " << flush;
   ofvar <<     name.data() << "/"<< flush;
   ofvar <<value << "/</varlist>" << endl;
 }

}



//______________________________________________
void addComment(ofstream &o, string comment){
  if(useAgml && ofOpen){
    o << "<!-- " << comment << " --> \n";
  }
  if(useTGeo && ofOpen){
    o << "// " << comment << endl;
  }
}

//______________________________________________
void addStatement(ofstream &o, string comment){
  if(ofOpen){
    o << comment << endl;
  }
}

//______________________________________________
void SetupArc(TVector2 P, TVector2 v1, TVector2 v2,
	 double radius,
	 TVector2 &C, TVector2 &P1,TVector2& P2)
{
  //
  // A corner with center P1 and two directions (need not to be normalized)
  // defines the region where a 
  //
  double theta = GetAngle(v1, v2)/2;
  double len1  = radius/TMath::Tan(theta);

  P1 = len1*v1/v1.Mod() + P;
  P2 = len1*v2/v2.Mod() + P;
  TVector2 vn(v1.Rotate(TMath::Pi()/2));
  TVector2 perp  = vn/vn.Mod()*radius;
  C=perp+P1;
  if(dot(v2,C-P) < 0) 
	C=P1-perp;
}

//________________________________________________________________________________
void positionVolume(string name, string mothername, double x, double y, double z,
		    double phi=0.0, double theta =0.0){
  //
  // Phi, Theta:  Eulerangles in degrees
  //
  
  if(useTGeo && ofOpen){

    of << "{" << endl;
     of << "TGeoRotation *centerRot = new TGeoRotation();" << endl;
     of << "centerRot->SetAngles(" << phi << " ," << theta << ", 0.0);" << endl;
    of << "TGeoCombiTrans *rotm = new TGeoCombiTrans(" << x << " , " << y << 
      "," << z << " , centerRot);" << endl;
    of << name.data() << " ->SetLineColor(" << currentColor << ");" << endl;
    of << "geom->GetVolume(\"" <<mothername.data() << "\")->AddNode(" << name.data() << ", " << currentVolumeNo <<", rotm);" << endl;
    of << "}" << endl << endl;
  }
  if(useAgml && ofOpen){

    string agmlName;
    findVolume(name, agmlName);

    of << "\n<Create block=\"" << agmlName.data() << "\" />" << endl;
    of << "  <Placement block=\"" << agmlName.data() << "\" x=\"" << x << "\" y=\""  
       << y << "\" z=\"" << z << "\">"  << endl;
    if(phi!=0)
      of << "  <Rotation alphaz=\"" << phi <<"\" />" << endl;
    if(theta!=0)
      of << "  <Rotation alphazy\"" << theta <<"\" />" << endl;
    of << " </Placement>" << endl;
  }
}



//_______________________________________________________________________
void makeBox(string name, double width,
	     double thickness, double zlength, string material="CarbonFiber", string comment=""){
  //
  // Version with data values
  //
  
  if(useTGeo && ofOpen){
    if(kMany){
      of << "TGeoVolume* " << name.data() << " = " << geoManager.Data()<<"->MakeBox(\"" << name.data() << 
	"\"," << material.data() << ", " <<
	width/2.0 <<" , " <<  thickness /2.0 << " , " << zlength/2.0 << " );" << endl;
    }
    else {
           of << "TGeoVolume* " << name.data() << " = " << geoManager.Data()<<"->MakeBox(\"" << name.data() << 
      "\"," << material.data() << ", " <<
      width/2.0 <<" , " <<  thickness /2.0 << " , " << zlength/2.0 << " );" << endl;
    }
    of << name.data() << " ->SetLineColor(" << currentColor << ");" << endl;
  }

  if(useAgml && ofOpen){

    string agmlName;
    findVolume(name, agmlName);

    ofvol << "\n";
    ofvol << "<Volume name=\"" << agmlName.data() << "\" comment=\""<< comment.data() << "\"  >\n" << flush;
    ofvol << "  <Attribute for=\"" << agmlName.data() << "\" seen=\""<< currentVisibility <<"\" colo=\"" << currentColor << "\" />" << endl; 
    ofvol << "  <Material name=\"" << material.data() << "\" />" << endl;
    ofvol << "  <Shape type=\"BOX\" dx=\"" << width/2.0 << "\" dy=\"" << thickness/2.0 << "\" dz=\"" << flush;
    ofvol << zlength/2.0 << "\" />" << endl;
    ofvol << "</Volume>" << endl;
  }
}

//_______________________________________________________________________
void makeTRAP(string name, double zlength, double* param, string material="CarbonFiber", string comment=""){
  //
  // Version with data values
  // The param should have the 10 parameters
  // 
  
  if(useTGeo && ofOpen){
    of << "TGeoVolume* " << name.data() << " = " << geoManager.Data()<<"->MakeTrap(\"" << name.data() << 
      "\"," << material.data() << ", " <<
      zlength/2.0 <<" , " << param[0] << " , " << param[1] << ", " <<
      param[2] <<" , " << param[3] << " , " << param[4] << ", " <<
      param[5] <<" , " << param[6] << " , " << param[7] << ", " <<
      param[8] <<" , " << param[9] << " );" << endl;
    
    of << name.data() << " ->SetLineColor(" << currentColor << ");" << endl;
  }

  if(useAgml && ofOpen){

    string agmlName;
    findVolume(name, agmlName);

    ofvol << "\n";
    ofvol << "<Volume name=\"" << agmlName.data() << "\" comment=\""<< comment.data() << "\"  >\n" << flush;
    ofvol << "  <Attribute for=\"" << agmlName.data() << "\" seen=\""<< currentVisibility <<"\" colo=\"" << currentColor << "\" />" << endl; 
    ofvol << "  <Material name=\"" << material.data() << "\" />" << endl;
    ofvol << "  <Shape type=\"TRAP\" dz=\"" << zlength/2.0 << "\" thet=\""<< param[0] << "\" phi=\""<< param[1] <<"\"" << endl;
    ofvol << "     h1=\""<< param[2] << "\" h2=\""<< param[6] <<"\"" << endl;
    ofvol << "     bl1=\""<< param[3] << "\" bl2=\""<< param[7] <<"\"" << endl;
    ofvol << "     tl1=\""<< param[4] << "\" tl2=\""<< param[8] <<"\"" << endl;
    ofvol << "     alp1=\""<< param[5] << "\" alp2=\""<< param[9] <<"\"" << flush;
    ofvol << " />" << endl;
    ofvol << "</Volume>" << endl;
  }
}

//_______________________________________________________________________
void makeBox(string name, double width,
	     	     string thickness, string zlength, string material="CarbonFiber", string comment=""){
   //
  // Version with data names
  //
  
  if(useTGeo && ofOpen){
    of << "TGeoVolume* " << name.data() << " = " << geoManager.Data()<<"->MakeBox(\"" << name.data() << 
      "\"," << material.data() << ", " 
      << width/2.0 <<" , " <<  thickness << " , " << zlength << " );" << endl;
  }

  if(useAgml && ofOpen){

    string agmlName;
    findVolume(name, agmlName);

    ofvol << "\n";
    ofvol << "<Volume name=\"" << agmlName.data() << "\" comment=\"" << comment.data() << "\"  >\n" << flush;
    ofvol << "  <Attribute for=\"" << agmlName.data() << "\" seen=\"" << currentVisibility <<"\" colo=\"" << currentColor << "\" />" << endl; 
    ofvol << "  <Material name=\"" << material.data() << "\" />" << endl;
    ofvol << "  <Shape type=\"BOX\" dx=\"" << width/2.0 << "\" dy=\"" << thickness << "\" dz=\"" << flush;
    ofvol << zlength << "\" />" << endl;
    ofvol << "</Volume>" << endl;
  }
}

//___________________________________________________________________________________________________
void addTrap(string name, double dz, double z, TVector2 P[4], string material, string comment=""){

  //
  // This piece of code is for development of conversion from 4 Points to
  // TRAP, and checking that there are two parallel vector between the sets of points.
  //

  TVector2 S0 = P[1]-P[0];
  TVector2 S1 = P[2]-P[1];
  TVector2 S2 = P[3]-P[2];
  TVector2 S3 = P[0]-P[3];


  TVector2 C0,C1,C2,C3;
  // Is S0 || S2  or is S1 || S3
  // reorder point in all cases so s0-s1 is parallel to s1-s2
  //
  double del0 = S0.DeltaPhi(S2);
  double del1 = S1.DeltaPhi(S3);

  double eps(1.0e-07);
  if(TMath::Abs(del0) < eps || TMath::Abs(TMath::Abs(del0)-TMath::Pi())< eps ){
    // case 1 ok This implies that P1,P0 is || with P2/3
    //
    C0=P[0]; C1=P[1]; C2=P[2]; C3=P[3];
  }
  else if (TMath::Abs(del1) < eps || TMath::Abs(TMath::Abs(del1)-TMath::Pi())< eps ){
    C0=P[1]; C1 =P[2]; C2=P[3]; C3=P[0];
  }
  else {
    cout << "*** P0-P4 does not consittue a TRPA" << endl;
    return;
  }

  //
  // Calculate the center of the TRAP.
  //
  TVector2 C = 0.25*(C0+C1+C2+C3);
  //
  //Calculate the two length at y=+-dy
  //
   
  double Bh1=(C1-C0).Mod()/2.0;
  double Bl1=(C3-C2).Mod()/2.0;

  // Need two more pieces of information i.e.
  // angle between normal vector between the two parallel lvectors (sides), and the rotation needed.
  //
  TVector2 Ex  =(C1-C0).Unit();
  TVector2 Ey  =Ex.Rotate(TMath::DegToRad()*90);


  TVector2 C0M = 0.5*(C0+C1);
  TVector2 C2M = 0.5*(C2+C3);
 
  double alpha = Ey.DeltaPhi(C0M-C2M)*TMath::RadToDeg();
    
  // rotation is relative to Y-axis in current mothervolume
  //
  TVector2 VR(0.0,1.0);
  double phi = Ey.DeltaPhi(VR)*TMath::RadToDeg();
  double theta(0.0);

  double param[10];
  param[0]=0.0; // fTheta
  param[1]=0.0; // fPhi
  param[2]=TMath::Abs(dot(C-C0M,Ey));
  param[3]=Bl1;
  param[4]=Bh1;
  param[5]=alpha;
  param[6]=param[2];
  param[7]=Bl1;
  param[8]=Bh1;
  param[9]=alpha;
  
  makeTRAP(name, dz , param, material, comment);
  positionVolume(name, currentMasterVolume.Data(), C.X(), C.Y(), z , phi, theta);

}


//_______________________________________________________________________
void addBox(string name, TVector2 p1, TVector2 p2,
	    double zlength, double thickness, double zOffset=0.0, string material="CarbonFiber", string comment=""){
  //
  // add a box along the Z direction
  // with edge P1->P2. The box will be drawn to the positive 90 deg rotation from p2-p1 vector
  // Changed internal coordinate such the width <-> x and thickness <-> y
  //


  TVector2 v = p2-p1;
  double length = v.Mod();
  TVector2 vn = v.Rotate(90*TMath::DegToRad())/v.Mod();
  TVector2 center = p1+0.5*(p2-p1)+vn*thickness/2.0;
  double phi = v.Phi()*TMath::RadToDeg();
  if(thickness < 0)
    thickness=-thickness;
  


  if(useTGeo && ofOpen){

    if(comment !=""){
      of << "// " << comment << endl;
    }
    of << "{" << endl;
    makeBox(name, length, thickness, zlength, material);

    of << "TGeoRotation *centerRot = new TGeoRotation();" << endl;
    of << "centerRot->SetAngles(" << phi  << " ,0,0);" << endl;
    of << "TGeoCombiTrans *rotm = new TGeoCombiTrans(" << center.X() << " , " << center.Y() << 
      "," << zOffset << " , centerRot);" << endl;
    of << name.data() << " ->SetLineColor(" << currentColor << ");" << endl;
    if(kMany){
      of << "geom->GetVolume(\"" << currentMasterVolume.Data() << 
	"\")->AddNodeOverlap(" << name.data() << ", " << 
	currentVolumeNo <<", rotm);" << endl << endl;
      kMany=kFALSE;
    }
    else
      {
	of << "geom->GetVolume(\"" << currentMasterVolume.Data() << 
"\")->AddNode(" << name.data() << ", " << currentVolumeNo <<", rotm);" << endl << endl;;
      }
    of << name.data() << " ->SetLineColor(" << currentColor << ");" << endl;
    of << "}" << endl;
  }
  if(useAgml && ofOpen){

    string agmlName;
    findVolume(name, agmlName);

    makeBox(agmlName, length, thickness, zlength, material, comment); // this write onto ofvol
    if(comment != ""){
      of << "\n<!-- " << comment << " --> \n";
    }

    of << "\n<Create block=\"" << agmlName.data() << "\" />" << endl;
    of << " <Placement block=\"" << agmlName.data() << "\" x=\"" << center.X() << "\" y=\""  
       << center.Y() << "\" z=\"" << zOffset << "\">"  << endl;
    if(phi!= 0)
      of << " <Rotation alphaz=\"" << phi <<"\" />" << endl;
    of << " </Placement>" << endl;
  }
    
}

//_______________________________________________________________________
void addBox(string name, TVector2 p1, TVector2 p2,
	    string zlength, string halfthickness, string zOffset, string material="CarbonFiber", string comment=""){
  //
  // add a box along the Z direction
  // with edge P1->P2. The box will be drawn to the positive 90 deg rotation from p2-p1 vector
  // Changed internal coordinate such the width <-> x and thickness <-> y
  //
  TVector2 v = p2-p1;
  double length = v.Mod();
  TVector2 vn = v.Rotate(90*TMath::DegToRad())/v.Mod();

  double thicknessval = varlist.find(halfthickness)->second;
  TVector2 center = p1+0.5*(p2-p1)+vn*thicknessval;  

  double phi= v.Phi()*TMath::RadToDeg();
  
  if(useTGeo && ofOpen){

    if(comment !=""){
      of << "// " << comment << endl;
    }
    of << "{" << endl;
    makeBox(name, length, halfthickness, zlength, material, comment);

    of << "TGeoRotation *centerRot = new TGeoRotation();" << endl;
    of << "centerRot->SetAngles(" << phi  << " ,0,0);" << endl;
    of << "TGeoCombiTrans *rotm = new TGeoCombiTrans(" << center.X() << " , " << center.Y() << 
      "," << zOffset << " , centerRot);" << endl;
     of << currentMasterVolume.Data() << "->AddNode(" << name.data() << ", " << currentVolumeNo <<", rotm);" << endl << endl;;

    of << "}" << endl;
  }
  if(useAgml && ofOpen){
    
    string agmlName;
    findVolume(name, agmlName);
 
    makeBox(agmlName, length, halfthickness, zlength, material, comment); // this write onto ofvol

    if(comment != ""){
      of << "\n<!-- " << comment << " --> \n";
    }

    of << "\n<Create block=\"" << agmlName.data() << "\" />" << endl;
    of << "  <Placement block=\"" << agmlName.data() << "\" x=\"" << center.X() << "\" y=\""  
       << center.Y() << "\" z=\"" << zOffset << "\">"  << endl;
    if(phi != 0)
      of << "  <Rotation alphaz=\"" << phi <<"\" />" << endl;
    of << " </Placement>" << endl;
  }
    
}


//_______________________________________________________________________
void positionBox(string name, TVector2 p1, TVector2 p2,
	    double thickness, double zOffset=0.0){
  //
  // position  box volume (already defined) along the Z direction
  // with edge P1->P2. The box will be drawn to the positive 90 deg rotation from p2-p1 vector
  //
  TVector2 v = p2-p1;
  //double length = v.Mod();
  TVector2 vn = v.Rotate(90*TMath::DegToRad())/v.Mod();
  TVector2 center = p1+0.5*(p2-p1)+vn*thickness/2.0;
  double phi = v.Phi()*TMath::RadToDeg();
  
  if(useTGeo && ofOpen){

    of << "{" << endl;
     of << "TGeoRotation *centerRot = new TGeoRotation();" << endl;
    of << "centerRot->SetAngles(" << phi << " ,0,0);" << endl;
    of << "TGeoCombiTrans *rotm = new TGeoCombiTrans(" << center.X() << " , " << center.Y() << 
      "," << zOffset << " , centerRot);" << endl;
    of << name.data() << " ->SetLineColor(" << currentColor << ");" << endl;
    of << "geom->GetVolume(\"" << currentMasterVolume.Data() << "\")->AddNode(" << name.data() << ", " << currentVolumeNo <<", rotm);" << endl;
    of << "}" << endl << endl;
  }
  if(useAgml && ofOpen){

    string agmlName;
    findVolume(name, agmlName);

    of << "\n<Create block=\"" << agmlName.data() << "\" />" << endl;
    of << "  <Placement block=\"" << agmlName.data() << "\" x=\"" << center.X() << "\" y=\""  
       << center.Y() << "\" z=\"" << zOffset << "\">"  << endl;
    if(phi!=0)
      of << "  <Rotation alphaz=\"" << phi <<"\" />" << endl;
    of << " </Placement>" << endl;
  }
}



//____________________________________________________________
void addTube(string name, TVector2 C,double radius,
	     double length, double thickness, double zOffset=0.0, string material="CarbonFiber"){
  //
  // At this point all I do is radial some tubes are per def along Z
  //


  if(useTGeo && ofOpen){
  of << "{" << endl;
    of << "TGeoVolume *" << name.data() << " = " << geoManager.Data() <<
      "->MakeTube(\""<< name.data() << "\"," << 
      material.data() <<  " , " << 
      radius-thickness  << " , " << radius << " , " << length/2.0 << " )" << endl;

    of << name.data() << " ->SetLineColor(" << currentColor << ");" << endl;
    of << "TGeoMatrix *centerPos = new TGeoTranslation(" << C.X() << " , " << C.Y() << " , " << zOffset << " );" << endl;
    of << currentMasterVolume.Data() << "->AddNode(" << name.data() << ", " <<
      currentVolumeNo << ", centerPos);" << endl;
      of << "}" << endl<< endl;
  }
  if(useAgml && ofOpen){
    string agmlName;
    findVolume(name, agmlName);


  } 
}


//_______________________________________________________________________
void addTubeSegment(string name, TVector2 C, TVector2 p1, TVector2 p2,
		    double length, double thickness, double zOffset=0.0, string  material="CarbonFiber", string comment=""){

  //
  // 
  double rad    = (p1-C).Mod();
  double theta1 = (p1-C).Phi()*TMath::RadToDeg();
  double theta2 = (p2-C).Phi()*TMath::RadToDeg();
 
  if(debug) {

    cout << "addTubeSegment " <<endl;
    cout <<"Center " << C.X() << ","<< C.Y() << " P1" << endl;
    cout << "Rad = " << rad << " theta " << theta1 << " " << theta2 << endl;
    cout << "thickness" << thickness << endl;

    
  }
 if(theta1 > theta2)
    theta2+=360;
 if(thickness < 0){
   rad -= thickness;
   thickness = - thickness;
 } 

  double radi=rad-thickness;
  if(debug)cout <<"radi " << radi << endl;
  
  //
  // output the statements needed (in this case for TGeoXXXX in root Geometry
  //
  
  if(useTGeo && ofOpen){
    of << "{" << endl;
    of << "TGeoVolume *" << name.data() << " = geom->MakeTubs(\""<< name.data() << "\"," << 
      material.data() <<  " , " << 
      radi << " , " << rad << " , " << length/2.0 << " , " <<
      theta1 << " , " << theta2 << ");" << endl;
    of << name.data() << " ->SetLineColor(" << currentColor << ");" << endl;
    of << "TGeoMatrix *cornerPos = new TGeoTranslation(" << C.X() << " , " << C.Y() << "," << zOffset << " );" << endl;
    if(kMany){
      of << "geom->GetVolume(\"" << currentMasterVolume.Data() << 
	"\")->AddNodeOverlap(" << name.data() << ", " << 
	currentVolumeNo << ", cornerPos);" << endl ;
      kMany=kFALSE;
    }
    else
      of << currentMasterVolume.Data() << "->AddNode(" << name.data() << ", " << 
	currentVolumeNo << ", cornerPos);" << endl ;
    of << "}" << endl<< endl;
  }
  if(useAgml && ofOpen){
    string agmlName;
    findVolume(name, agmlName);
    
    addComment(of,comment);
    of << "\n<Create block=\"" << agmlName.data() << "\" />" << endl;
    of << " <Placement block=\"" << agmlName.data() << "\" x=\"" << C.X() << "\" y=\""  
       << C.Y() << "\" z=\"" << zOffset << "\">"  << endl;
    of << " </Placement>" << endl;
    
    ofvol << "\n";
    ofvol << "<Volume name=\"" << agmlName.data() << "\" comment=\"" << comment.data() << "\"  >\n" << flush;
    ofvol << "  <Attribute for=\"" << agmlName.data() << "\" seen=\"" << currentVisibility <<"\" colo=\"" << currentColor << "\" />" << endl; 
    ofvol << "  <Material name=\"" << material.data() << "\" />" << endl;
    ofvol << "  <Shape type=\"TUBS\"\" dz=\"" << length << "\" rmin=\"" << radi << "\" rmax=\"" << rad << "\"" << endl;
    ofvol << "   phi1=\"" << theta1 << "\" phi2=\"" << theta2 << "\" />" << endl;
    ofvol << "</Volume>" << endl;
  }  
  


} 


//_____________________________________________________________________-
void addTrapzoid(string name, TVector2 P[4], double length, double zOffset,
		 string  material="CarbonFiber", string comment=""){

  //
  // The trapzoid is assme for this simple case to have same vertecis at +- dz.
  // This is often sufficent.
  //
  //

  double vert[16];
  for(int i=0;i<4; i++){
    vert[i*2]   = P[i].X();
    vert[i*2+1] = P[i].Y();
  }
  
  for (int i=0;i<8;i++){
    vert[8+i]= vert[i];
  }

  if(useTGeo && ofOpen){
    of << "{" << endl;
    of << "// " << comment << endl;
    of << "double vert[16]={" ;
    for (int i=0;i<16;i++){
      of << vert[i] ;
      if(i<15)
	of << ", " << flush;
      else
	of <<"};" << endl;
    }
    
    of << "TGeoVolume *" << name.data() << " = " << geoManager.Data() <<
      "->MakeArb8(\""<< name.data() << "\"," << 
      material.data() <<  " , " << 
      length/2.0  << " , vert);" << endl;
    
    of << name.data() << " ->SetLineColor(" << currentColor << ");" << endl;
    of << "TGeoMatrix *centerPos = new TGeoTranslation(" << 0.0 << " , " << 0.0 << " , " << zOffset << " );" << endl;
    of << "geom->GetVolume(\"" << currentMasterVolume.Data() << 
     "\")->AddNode(" << name.data() << ", " <<
      currentVolumeNo << ", centerPos);" << endl;
    of << "}" << endl<< endl;
  }

  if(useAgml && ofOpen){
    addComment(of,comment);
    string agmlName;
    findVolume(name, agmlName);

 
    of << "\n<Create block=\"" << name.data() << "\" />" << endl;
    of << " <Placement block=\"" << name.data() << "\" x=\"" << 0.0 << "\" y=\""  
       << 0.0 << "\" z=\"" << zOffset << "\">"  << endl;
    of << " </Placement>" << endl;
    
    ofvol << "\n";
    ofvol << "<Volume name=\"" << name.data() << "\" comment=\"" << comment.data() << "\"  >\n" << flush;
    ofvol << "  <Attribute for=\"" << name.data() << "\" seen=\"" << currentVisibility <<"\" colo=\"" << currentColor << "\" />" << endl; 
    ofvol << "  <Material name=\"" << material.data() << "\" />" << endl;
    ofvol << "  <Shape type=\"ARB8\" dz=\"" << length/2.0 << "\" x0=\"" <<  vert[0] << "\" y0=\"" << vert[1] << "\"" << endl;
    ofvol <<"   x1=\"" <<  vert[2] << "\" y1=\"" << vert[3] << "\"" << endl;
    ofvol <<"   x2=\"" <<  vert[4] << "\" y2=\"" << vert[5] << "\"" << endl;
    ofvol <<"   x3=\"" <<  vert[6] << "\" y3=\"" << vert[7] << "\"" << endl;
    ofvol <<"   x4=\"" <<  vert[8] << "\" y4=\"" << vert[9] << "\"" << endl;
    ofvol <<"   x5=\"" <<  vert[10] << "\" y5=\"" << vert[11] << "\"" << endl;
    ofvol <<"   x6=\"" <<  vert[12] << "\" y6=\"" << vert[13] << "\"" << endl;
    ofvol <<"   x7=\"" <<  vert[14] << "\" y7=\"" << vert[15] << "\"" << flush;
    ofvol <<"/>" << endl;
    ofvol << "</Volume>" << endl;

    

  }

} 


//_______________________________________________________________________
void addTubeSegment(string name, TVector2 C, TVector2 p1, TVector2 p2,
		    string length, string thickness, string zOffset, string  material="CarbonFiber", string comment=""){
  //		    double length, double thickness, double zOffset=0.0, 

  //
  // 
  double rad    = (p1-C).Mod();
  double theta1 = (p1-C).Phi()*TMath::RadToDeg();
  double theta2 = (p2-C).Phi()*TMath::RadToDeg();
 
  if(theta1 > theta2)
    theta2+=360;

  double thicknessval= varlist.find(thickness)->second;
  double radi=rad;
  if(thicknessval < 0){
    rad -= thicknessval;
    thicknessval = - thicknessval;
  }
  radi=rad-thicknessval;
  
  //
  // output the statements needed (in this case for TGeoXXXX in root Geometry
  //
  
  if(useTGeo & ofOpen){
    of << "{" << endl;
    of << "TGeoVolume *" << name.data() << " = geom->MakeTubs(\""<< name.data() << "\"," << 
      material.data() <<  " , " << 
      radi << " , " << rad << " , " << length.data() << " , " <<
      theta1 << " , " << theta2 << ");" << endl;
    of << name.data() << " ->SetLineColor(" << currentColor << ");" << endl;
    of << "TGeoMatrix *cornerPos = new TGeoTranslation(" << C.X() << " , " << C.Y() << "," << zOffset.data() << " );" << endl;

     if(kMany){
	of << "geom->GetVolume(\"" << currentMasterVolume.Data() << 
	  "\")->AddNodeOverlap(" << name.data() << ", " << 
	currentVolumeNo << ", cornerPos);" << endl ;
	kMany=kFALSE;
      }
      else
	of << currentMasterVolume.Data() << "->AddNode(" << name.data() << ", " << 
	  currentVolumeNo << ", cornerPos);" << endl ;
    of << "}" << endl<< endl;
  }

  if(useAgml && ofOpen){
    string agmlName;
    findVolume(name, agmlName);

    addComment(of,comment);
    of << "\n<Create block=\"" << agmlName.data() << "\" />" << endl;
    of << " <Placement block=\"" << agmlName.data() << "\" x=\"" << C.X() << "\" y=\""  
       << C.Y() << "\" z=\"" << zOffset << "\">"  << endl;
    of << " </Placement>" << endl;
    
    ofvol << "\n";
    ofvol << "<Volume name=\"" << agmlName.data() << "\" comment=\"" << comment.data() << "\"  >\n" << flush;
    ofvol << "<Attribute for=\"" << agmlName.data() << "\" seen=\"" << currentVisibility <<"\" colo=\"" << currentColor << "\" />" << endl; 
    ofvol << "<Material name=\"" << material.data() << "\" />" << endl;
    ofvol << "<Shape type=\"TUBS\"\" dz=\"" << length << "\" rmin=\"" << radi << "\" rmax=\"" << rad << "\"" << flush;
    ofvol << " phi1=\"" << theta1 << "\" phi2=\"" << theta2 << "\" />" << endl;
    ofvol << "</Volume>" << endl;
  }  
  
} 

//_______________________________________________________________________________________________
void DoveTailShape(string basename, TString mate, double zOffset, double ROuter , double RInner,
		   double dz, double dl, double tr, double tl,
		   double tinner, double touter){
  //
  // Approximation to the shape used in the dovetail outside the sectorbeam
  // It consiste of a master containing volume with the given name name
  // It will be approximated by a subset of MANY volume to make proper unions all placed with
  // the conatining volume
  // zOffset      : distance along z axis.
  // ROuter : outer radius
  // RInner : inner radius
  // dz     : length of dovetail part.
  // dl : distance if side from nominal 0-36 deg line of material
  // tr:  thickness of righthand side
  // tl : thickness of lefthand side
  // tinner : thickness of inner part
  // touter : thickness of outer part
  //
  // The actual curver surfaces are quite difficult. Decided to approximate by trapezoids (non overlapping)
  // 

  double theta1 (90.0);
  double theta2(90.0+36.0);
  TVector2 C(0.0,0.0);
  TString material("Air");


  //
  // Place the container Volume
  //
  if(useTGeo && ofOpen){
    of << "{" << endl;
    of << "TGeoVolume *" << basename.data() << " = geom->MakeTubs(\""<< basename.data() << "\"," << 
      material.Data() <<  " , " << 
      RInner << " , " << ROuter << " , " << dz/2.0 << " , " <<
      theta1 << " , " << theta2 << ");" << endl;
    of << basename.data() << " ->SetLineColor(" << currentColor << ");" << endl;
    of << "TGeoMatrix *cornerPos = new TGeoTranslation(" << C.X() << " , " << C.Y() << "," << zOffset << " );" << endl;
    of << currentMasterVolume.Data() << "->AddNode(" << basename.data() << ", " << 
	currentVolumeNo << ", cornerPos);" << endl ;
      of << "}" << endl<< endl;
  }

  TString savedVolume = currentMasterVolume;
  currentMasterVolume = basename;
  
  TVector2 IP(0,0);
  TVector2 VR(0,1.0);

  TVector2 ROO[4], ROI[4], CO[4], CI[4];;
  for(int i=0;i<4; i++){
    double theta = TMath::DegToRad()*(i*12.0);
    ROO[i]= ROuter*VR.Rotate(theta);
    ROI[i]=(ROuter-touter)*VR.Rotate(theta);
  }
  for(int i=1;i<3; i++){
    CO[i] = ROO[i];
    CI[i] = ROI[i];
  }
  TVector2 VR_N(-1.0,0.);
  TVector2 VL   = VR.Rotate(36.0*TMath::DegToRad());
  TVector2 VL_N = VL.Rotate(90.0*TMath::DegToRad());
  //
  // The normal vector points towards positive x
  // The sector structures are at positive rotation, thus at
  // negative X, since things are setup relative to the normal Y-axis
  //
  //
  TVector2 CR = dl*VR_N;
  TVector2 CL = -dl*VL_N;
  // The intercetpts are on the outer radius, and on an inner radius with the thickness
  // of the outer arc.
  //
  double det=0;
  CO[0] = Intercept(CR, VR, ROO[0], ROO[1]-ROO[0], det);
  CI[0] = Intercept(CR, VR, ROI[0], ROI[1]-ROI[0], det);

  CO[3] = Intercept(CL, VL, ROO[3], ROO[2]-ROO[3], det);
  CI[3] = Intercept(CL, VL, ROI[3], ROI[2]-ROI[3], det);

  //
  // This has now defined all the corners of the trapzoid's
  // A correct order to get the clockwise rotation is
  // CO[i], CI[i], CI[i+1], CO[i+1]
  //

  if(0){
    for(int i=0;i<3;i++){
      CO[i].Print(); CO[i+1].Print(); CI[i+1].Print(); CI[i].Print();
    }
  }

  //
  // setup z for the wider part nearest the sector beam and the dove tail.
  //
  // Now I also add part of the dovetailsupport, since when lockedin paled they are really merge together
  // otherwise it would be way too complicated.
  //
  double dzr(0.1);
  double dzm(1.2);
  double dzb(0.4);  // thickness of supportbar+groove.
  double dzl(0.1);
  double zr = dz/2.0-dzr/2.0;
  double zm = dz/2.0-dzr-dzm/2.0;
  double zb = -dz/2.0+dzl+dzb/2.0;
  double zl = -dz/2.0+dzl/2.0;


  TVector2 P[4];
  for(int i=0;i<3;i++){
    P[0]= CO[i]; P[1]= CI[i]; P[2]= CI[i+1]; P[3]=CO[i+1];
    string namer=Form("PTR%1d",i);
    addTrap(namer, dzr, zr, P, "Al", "Approximate arc of first piece of dovetail");
  }
  
  //
  // Define the lower radius cutline
  // 

  TVector2 R25(0, 2.7);
  TVector2 V25(VR.Rotate((90.0-2.604)*TMath::DegToRad()));
  TVector2 V25_N(V25.Rotate(90*TMath::DegToRad()));

  TVector2 RI1  = Intercept(CO[0], VR, R25, V25, det);
  TVector2 RI2  = Intercept(CO[0] + (tr)*VR_N, VR, R25, V25, det);
  TVector2 ROI2 = Intercept(CI[0] + (tr)*VR_N, VR, ROI[0], ROI[1]-ROI[0], det);
  P[0]= RI1; P[3]= CI[0]; P[2]= ROI2; P[1]= RI2;
  addTrap("PTR4", 0.1, zr, P, "Al", "Right hand side dovetail ext");
   
  TVector2 RI4  = Intercept(CO[3], VL, R25, V25, det);
  TVector2 RI3  = Intercept(CO[3] + (-tr)*VL_N, VL, R25, V25, det);
  TVector2 ROI3 = Intercept(CI[3] + (-tr)*VL_N, VL, ROI[3], ROI[3]-ROI[2], det);
  P[0]= RI3; P[3]= ROI3; P[2]= CI[3]; P[1]= RI4;
  addTrap("PTR5", dzr, zr, P, "Al", "Left hand (near piece)");
  



  //
  // The lower pieces that extrudes though all of the dove tail
  //
  TVector2 RI5  = Intercept(CO[0] + (tr)*VR_N, VR, R25-tinner*V25_N, V25, det);
  TVector2 RI6  = Intercept(CO[3] + (-tr)*VL_N, VL, R25-tinner*V25_N, V25, det);
  P[0]= RI2; P[3]= RI5; P[2]= RI6; P[1]= RI3;
  addTrap("PTR6", dzr+dzm, dz/2-(dzr+dzm)/2.0, P, "Al", "Piece near BP");

  //
  // Add the cutout pieces for remaining
  //

  double cut = 0.5;
  CR =  (dl+cut)*VR_N;
  CL = -(dl+cut)*VL_N;

  
  // The intercepts are on the outer radius, and on an inner radius with the thickness
  // of the outer arc.
  // The CO and CI are redefined to macth the cutout.
  CO[0] = Intercept(CR, VR, ROO[0], ROO[1]-ROO[0], det);
  CI[0] = Intercept(CR, VR, ROI[0], ROI[1]-ROI[0], det);

  CO[3] = Intercept(CL, VL, ROO[3], ROO[2]-ROO[3], det);
  CI[3] = Intercept(CL, VL, ROI[3], ROI[2]-ROI[3], det);

  currentColor = kGray;
  for(int i=0;i<3;i++){
    P[0]= CO[i]; P[3]= CO[i+1]; P[2]= CI[i+1]; P[1]=CI[i];
    string name=Form("PTM%1d",i);
    addTrap(name,  dzm, zm ,P, "Al", "Approximate arc of first piece of dovetail");
  }

  TVector2 RIP1  = Intercept(CO[0], VR, R25, V25, det);
  //  TVector2 ROP2  = Intercept(CI[0], VR, ROI[0], ROI[1]-ROI[0], det);
  P[0]= RIP1; P[3]= CI[0]; P[2]= ROI2; P[1]= RI2;
  addTrap("PTM4", dzm, zm, P, "Al", " of first piece of dovetail");
  currentColor=kGray;
  TVector2 RIP4  = Intercept(CO[3],  VL, R25, V25, det);
  P[0]= RI3; P[1]= RIP4; P[2]= CI[3]; P[3]= ROI3;
  addTrap("PTM5", dzm, zm ,P, "Al", "Left middle part ");

  //
  // The left most part (away from IP) extends the full 36 degrees
  // they are part of the doove tail support ring.
  //
  // reset the CO and CI to be the initial values.
  //
  for(int i=0;i<4; i++){
    CO[i] = ROO[i];
    CI[i] = ROI[i];
  }
 
  for(int i=0;i<3;i++){
    P[0]= CO[i]; P[1]= CI[i]; P[2]= CI[i+1]; P[3]=CO[i+1];
    string namel=Form("PTL%1d",i);
    addTrap(namel, dzl, zl, P, "Al", "Dovetail support plate part outer");
  }
 
 

  TVector2 RII[4], RIO[4];
  for(int i=0;i<4; i++){
    double theta = TMath::DegToRad()*(i*12.0);
    RII[i]= RInner*VR.Rotate(theta);
    RIO[i]=(RInner+tinner)*VR.Rotate(theta);
  }

  for(int i=0;i<3;i++){
    P[0]= RIO[i]; P[1]= RII[i]; P[2]= RII[i+1]; P[3]=RIO[i+1];
    string namel=Form("PTL%1d",i+3);
    addTrap(namel,  dzl, zl, P, "Al", "Dovetail support palte part outer");
  }
  //
  // setup the and narrow wide parts i.e from dividing lines to the inner cuts as used previous
  // 
  CR = VR_N*(tl+dl);
  CL = VL_N*(-tl-dl);
  CO[1] = Intercept(CR, VR, ROI[0], ROI[1]-ROI[0], det);
  CI[1] = Intercept(CR, VR, RIO[0], RIO[1]-RIO[0], det);
  CO[2] = Intercept(CL, VL, ROI[3], ROI[2]-ROI[3], det);
  CI[2] = Intercept(CL, VL, RIO[3], RIO[2]-RIO[3], det);

  P[1] = CI[1]; P[0]=RIO[0]; P[2]= CO[1]; P[3]=ROI[0];
  addTrap("PTL6", dzl, zl, P, "Al", "Dovetail support plate part outer");

  P[0] = CI[2]; P[1]=RIO[3]; P[3]= CO[2]; P[2]=ROI[3];
  addTrap("PTL7", dzl, zl, P, "Al", "Dovetail support plate part outer");


  currentColor=kGray;

  CO[1] = Intercept(CR, VR, ROO[0], ROO[1]-ROO[0], det);
  CI[1] = Intercept(CR, VR, RII[0], RII[1]-RII[0], det);
  CO[2] = Intercept(CL, VL, ROO[3], ROO[2]-ROO[3], det);
  CI[2] = Intercept(CL, VL, RII[3], RII[2]-RII[3], det);

  P[1] = CI[1]; P[0]=RII[0]; P[2]= CO[1]; P[3]=ROO[0];
  addTrap("PTL8", dzb, zb, P, "Al", "Dovetail support plate part outer");

  P[0] = CI[2]; P[1]=RII[3]; P[3]= CO[2]; P[2]=ROO[3];
  addTrap("PTL9", dzb, zb, P, "Al", "Dovetail support plate part outer");

  currentMasterVolume= savedVolume;


}   

//_________________________________________________
void Sector(double theta, Bool_t guides=kFALSE){
//
// This is a pretty good description of one sector and ladder
// The volume is called Sector  


  of << endl;
  double deg2rad = TMath::DegToRad();
  double Ro(8.0);
  
  // These are the basis for any setup
  // by rotating 36 deg I believe this can be done properly
  //
  TVector2 IP(0.0,0.0);
  TVector2 R8_0(0.0, Ro);
  TVector2 V1(0,1);
  TVector2 R25_0(0,2.5);
  TVector2 XBR(-0.1,3.0);
  TVector2 XBL(+0.1,3.0);
   
   
  V1=V1.Rotate(theta*deg2rad);
  R8_0=R8_0.Rotate(theta*deg2rad);
  R25_0=R25_0.Rotate(theta*deg2rad);
  XBR = XBR.Rotate(theta*deg2rad);
  XBL = XBL.Rotate(theta*deg2rad);
  TVector2 VBR = V1;
  
  TVector2 R8_36 = R8_0.Rotate(+36.0*deg2rad);
  if(guides){
    TLine *t = drawLine(IP, R8_0);
     t->SetLineColor(kGreen);
     t->SetLineStyle(kDashed);
     TLine *t36 = drawLine(IP, R8_36);
     t36->SetLineColor(kGreen);
     t36->SetLineStyle(kDashed);
   
     TArc *bpr = new TArc(0,0, 8.0, 90+theta,90+theta+36);
     bpr->SetLineWidth(1.0);
     bpr->SetLineColor(kGreen);
     bpr->SetLineStyle(kDotted);
     bpr->SetFillStyle(4000);
     bpr->Draw();
   
   }
   //
   // Setup corners and active area. of active Si
   //
   boxColor=kBlue;

   double angleOffset      =  1.26*deg2rad;
   double innerAngleOffset = 2.604*deg2rad;
   double tiltAngle        = +(90-10.037)*deg2rad; // 10.037 from SW model
   double	deltaAngle = 12.0*deg2rad;
   double ladderThickness  = 0.064; 
   double ladderNominal    = 0.088; 
   double siWidth          = 1.92;
   double siThickness      = 0.005;
   double siFrameOffset    = 0.0232;

   double siOuterFrameWidth(0.338);
   double siSensorCableOffset(0.31);
   double siLadderOffset(0.0);  
   double cableWidth       = 2.4605;  
   double cableThickness   = 0.022; 
   double cableExtrusion   = 0.0;  // 0.0250;
   double cableLength      = 30.65;  // as per latest SW model. Add driver board seperately.

   double sensorLength     = 2.0250;
   double sensorWidth      = 2.2740;
   double gap =0.0010;  // From Joe measurements.
   double siLength         = 10*sensorLength +9*gap;
   double activeSensorLength = 1.9872;
   double activeSensorWidth  = 1.921; 
   double activeSensorYOffset = 0.0;
   double activeSensorXOffset = (siFrameOffset+activeSensorWidth/2.0-sensorWidth/2.0);
   
   double sectorEndToSensor = 3.3+siSensorCableOffset;

   double backingThickness = 0.0175; 
   double backingWidth     = 2.49;
   double backingLength    = 29.9;

   double driverBoardLength  = 9.1; 
   double driverBoardWidth   = 2.4605;  
   double driverBoardThickness  = 0.05; 
   double driverBoardOffset  = -cableLength/2. + 0.186 + driverBoardLength/2.0;


   double ladderLength     = cableLength;
   double ladderWidth      = backingWidth;
  

   double sectorThickness  = 0.024;    // 
   double sectorLength     = 35.1;  // was 36.52 
   double sectorZOffset    = -sectorLength/2.0+siLength/2.0+sectorEndToSensor;
   double ladderZOffset     = sectorLength/2.0 -(3.3 + ladderLength/2.0)+sectorZOffset; // check and redo but close
   cout << "sectorZOffset " << sectorZOffset << endl;
   cout << "ladderZOffset " << ladderZOffset << endl;

   double backingZOffset   = (ladderLength-backingLength)/2.0-siSensorCableOffset; 
   double cableZOffset    =  0.0;  
   cout << "cableZOffset " << cableZOffset << endl;

   double glue1Thickness   = 0.005;
   double glue2Thickness   = 0.005;
   double glue3Thickness   = 0.010;
   double glue1Width       = siWidth+siOuterFrameWidth+siFrameOffset;
   double glue3Width       = 1.44 ;

   double ladderVolumeThickness  =  driverBoardThickness + cableThickness + 
     backingThickness+glue2Thickness+glue3Thickness;
   cout << "ladderVolumeThickness " <<  ladderVolumeThickness <<endl;
   cout << "Total Thickkness "  << siThickness+glue1Thickness+cableThickness+ glue2Thickness+backingThickness + glue3Thickness<< endl;
   ladderThickness = siThickness+glue1Thickness+cableThickness+ glue2Thickness+backingThickness + glue3Thickness ;
   defineVariable("LadderThickness", siThickness+glue1Thickness+cableThickness+ glue2Thickness+backingThickness + glue3Thickness ,
		  "Thickness of low mass sensor part");

   currentColor = kGreen -6;
   currentMasterVolume="Sector";
   currentVolumeNo=1;


   double DoveTailLength    = 1.0;
   double DoveTailExtrusionLength = 0.1;
   double DoveTailThickness = 0.1;
   double DoveTailZOffset    = -sectorLength/2.0+DoveTailLength/2.0+sectorZOffset;


   TVector2 R8_1(R8_0.Rotate(angleOffset));
   
   TVector2 V8_1(V1.Rotate(tiltAngle));
   TVector2 V8_1N(V8_1.Rotate(+90*deg2rad));
   TVector2 P8_1 = R8_1+V8_1N*ladderNominal;
   
   TVector2 R8_2(R8_0.Rotate(angleOffset+deltaAngle));
   TVector2 V2(V1.Rotate(deltaAngle));
   TVector2 V8_2(V2.Rotate(tiltAngle));
   TVector2 V8_2N(V8_2.Rotate(+90*deg2rad));
   TVector2 P8_2 = R8_2+V8_2N*ladderNominal;
   
   TVector2 R8_3(R8_0.Rotate(angleOffset+2*deltaAngle));
   TVector2 V3(V1.Rotate(2*deltaAngle));
   TVector2 V8_3(V3.Rotate(tiltAngle));
   TVector2 V8_3N(V8_3.Rotate(+90*deg2rad));
   TVector2 P8_3 = R8_3+V8_3N*ladderNominal;
   
   
   TVector2 R25_1(R25_0.Rotate(-innerAngleOffset));
   TVector2 V25_1(V1.Rotate(90*deg2rad-innerAngleOffset));
   TVector2 V25_1N(V25_1.Rotate(+90*deg2rad));
   TVector2 P25_1 = R25_1-V25_1N*ladderNominal;
   
   XBL = XBL.Rotate(36*deg2rad);
   TVector2 VBL(VBR.Rotate(36*deg2rad));

   TVector2 VBR_N(VBR.Rotate(+90*deg2rad));
   TVector2 VBL_N(VBL.Rotate(+90*deg2rad));

   //
   //
   //

   
   //
   // Short sectors.
   // line are at 36-12.46
   // and 36-12.46-12 degrees.
   //

   TVector2 OB1 = R8_0.Rotate((+36.0-12.46)*deg2rad);
   TVector2 OV1(VBR.Rotate((36-12.46)*deg2rad));
   TVector2 OV1_N(OV1.Rotate(+90*deg2rad));
   TVector2 OB2 = R8_0.Rotate((+36.0-12.46-12)*deg2rad);
   TVector2 OV2(VBR.Rotate((36-12.46-12)*deg2rad));
   TVector2 OV2_N(OV2.Rotate(+90*deg2rad));
   //
   // Calculate the 8 intersections point between the defining surfaces
   //
   double det=0;
   TVector2 XP1 = Intercept(P25_1, V25_1, XBR, VBR,  det);
   TVector2 XP2 = Intercept(XBR, VBR, P8_1, V8_1, det);
   drawArrow(P8_1, V8_1, "v8_1");
   TVector2 XP3 = Intercept(OB2, OV2, P8_1, V8_1, det);
   drawArrow(P8_2, V8_2, "v8_2");
   drawArrow(OB2, OV2, "ob2");
   TVector2 XP4 = Intercept(OB2, OV2, P8_2, V8_2, det);
   TVector2 XP5 = Intercept(OB1, OV1, P8_2, V8_2, det);
   TVector2 XP6 = Intercept(OB1, OV1, P8_3, V8_3, det);
   drawArrow(OB1, OV1, "ob1");
   drawArrow(P8_3, V8_3, "v8_3");
   TVector2 XP7 = Intercept(XBL, VBL, P8_3, V8_3, det);
   TVector2 XP8 = Intercept(XBL, VBL, P25_1, V25_1, det);
   drawArrow(P25_1, V25_1, "v25");
  //
  // Setup the Arc's
  //
  
  double radius=0.15;
  TVector2 CP11, CP12, C1;

  SetupArc(XP1, V25_1, VBR, radius,
	   C1, CP11, CP12); 
   
  TVector2 CP21, CP22, C2;
  SetupArc(XP2, -1.0*VBR,V8_1,  radius,
	   C2, CP21, CP22);
  
  TVector2 C3, CP31, CP32;
  SetupArc(XP3, -1.0*V8_1, -1.0*OV2, radius,
	   C3, CP31, CP32);
  
  TVector2 C4, CP41, CP42;
  SetupArc(XP4, V8_2, OV2, radius,
	   C4, CP41, CP42);
  
  TVector2 C5, CP51, CP52;
  SetupArc(XP5, -1.0*V8_2, -1.0*OV1, radius,
	   C5, CP51, CP52);
 
  TVector2 C6, CP61, CP62;
  SetupArc(XP6,V8_3, OV1, radius,
	   C6, CP61, CP62);
  
  TVector2 CP71, CP72, C7;
  SetupArc(XP7, -1.0*V8_3, -1.0*VBL, radius,
	   C7, CP71, CP72);
  
  TVector2 CP81, CP82, C8;
  SetupArc(XP8, VBL, -1.0*V25_1, 0.2,
	   C8, CP81, CP82);
  
  drawArc(C1, CP11, CP12);
  drawArc(C2, CP21, CP22);
  drawText(C2, "C2");
  drawLine(CP12, CP21-CP12);
  drawArc(C3, CP31, CP32);
  drawLine(CP22, CP31-CP22);
  drawArc(C4, CP41, CP42);
  drawLine(CP32, CP42-CP32);
  drawArc(C5, CP51, CP52);
  drawLine(CP41, CP51-CP41);
  drawArc(C6, CP61, CP62);
  drawLine(CP52, CP62-CP52);
  drawArc(C7, CP71, CP72);
  drawLine(CP71, CP61-CP71);
  drawArc(C8, CP81, CP82);
  drawLine(CP81, CP72-CP81);
  drawLine(CP82, CP11-CP82);
  drawText(C1, "C1");

 if(ofOpen && useAgml){
   of.close();
   ofvar.open("variable.xml");
  
   addComment(ofvar," File variable.xml");
   TDatime time;
   addComment(ofvar,Form(" Created on : %s",time.AsString()));
 }

 defineVariable("sectorLength",    sectorLength/2.0,    "Half length of Carbon Fibre sector");
 defineVariable("sectorThickness", sectorThickness,     "Thickness of Carbon Fibre sector");
 defineVariable("sectorThicknessNeg", -sectorThickness,     "negative Thickness of Carbon Fibre sector");
 defineVariable("sectorHalfThickness", sectorThickness/2.0, "HalfThickness of Carbon Fibre sector");
 defineVariable("sectorZOffset",   sectorZOffset, "Z offset for Sector");

 defineVariable("backingLength",    backingLength/2.0," Half Length of Ladder CF backing");
 defineVariable("backingWidth",     backingWidth/2.0, " Half Width of Ladder CF backing");
 defineVariable("backingThickness", backingThickness/2.0," Half Thickness of Ladder CF backing");
 defineVariable("backingZOffset",   backingZOffset, "Z Offset of CF backing");

 defineVariable("DoveTailExtrusionLength",    DoveTailExtrusionLength/2.0,"length of short Extrusion");
 defineVariable("DoveTailExtrusionThickness", DoveTailThickness+sectorThickness,"Thickness of lip");
 defineVariable("DoveTailExtrusionThicknessBox", (DoveTailThickness+0.4+sectorThickness)/2.0,"Thickness of lip Boxes");
 defineVariable("DoveTailExtrusionThicknessNeg",-(DoveTailThickness+sectorThickness),"Thickness of lip");
 defineVariable("DoveTailExtrusionHalfThickness", (DoveTailThickness+sectorThickness)/2.0,"Thickness of lip");
 double DoveTailExtrusionZOffset = DoveTailZOffset-DoveTailLength/2.0-DoveTailExtrusionLength/2.0; 
 defineVariable("DoveTailExtrusionZOffset",   DoveTailExtrusionZOffset,"Extrusion is position just outside the end of the sector beam");

 defineVariable("DoveTailLength",    DoveTailLength/2.0,"length of Dovetail inside sector beam");
 defineVariable("DoveTailThickness", DoveTailThickness,"Thickness of Dovetail");
 defineVariable("DoveTailHalfThickness", DoveTailThickness/2.0,"Half Thickness of Dovetail");
 defineVariable("DoveTailZOffset",   DoveTailZOffset,"Dove tail is just inside end of sector beam");



 if(ofOpen && useAgml){
    of.open("sector.xml",ios::app);
 }

 addComment(of," Placement of arcs in sector beam");

 addTubeSegment("PXCA",C1, CP11, CP12, "sectorLength", "sectorThickness", "sectorZOffset","CFRPMix","Arc bottom right");
 addTubeSegment("PXCC",C2, CP21, CP22, "sectorLength", "sectorThickness", "sectorZOffset","CFRPMix","Arc topright"); 
 addTubeSegment("PXCD",C3, CP31, CP32, "sectorLength", "sectorThickness", "sectorZOffset","CFRPMix","Arc top 23"); 
 addTubeSegment("PXCE",C4, CP41, CP42, "sectorLength", "sectorThicknessNeg", "sectorZOffset","CFRPMix","Arc bottom 23");
 addTubeSegment("PXCF",C5, CP51, CP52, "sectorLength", "sectorThickness", "sectorZOffset","CFRPMix","Arc top 12"); 
 addTubeSegment("PXCG",C6, CP61, CP62, "sectorLength", "sectorThicknessNeg", "sectorZOffset","CFRPMix","Arc bottom 12");
 addTubeSegment("PXCH",C7, CP71, CP72, "sectorLength", "sectorThickness", "sectorZOffset","CFRPMix","arc top left"); 
 addTubeSegment("PXCB",C8, CP81, CP82, "sectorLength", "sectorThickness", "sectorZOffset","CFRPMix","arc bottom left"); 

 addComment(of," Placement of Planes ");

 addBox("PXRB", CP12, CP21, "sectorLength", "sectorHalfThickness", "sectorZOffset", "CFRPMix","Sector Right side");
 addBox("PXTR", CP22, CP31, "sectorLength", "sectorHalfThickness", "sectorZOffset","CFRPMix");
 addBox("PXTM", CP41, CP51, "sectorLength", "sectorHalfThickness", "sectorZOffset","CFRPMix");
 addBox("PXTL", CP61, CP71, "sectorLength", "sectorHalfThickness", "sectorZOffset","CFRPMix");
 addBox("PXTJ", CP52, CP62, "sectorLength", "sectorHalfThickness", "sectorZOffset","CFRPMix");
 addBox("PXTI", CP32, CP42, "sectorLength", "sectorHalfThickness", "sectorZOffset","CFRPMix");
 addBox("PXLB", CP72, CP81,  "sectorLength", "sectorHalfThickness", "sectorZOffset","CFRPMix","Sector Left side");
 addBox("PXIB", CP82, CP11,  "sectorLength", "sectorHalfThickness", "sectorZOffset","CFRPMix","Sector Inner side");
 
 
 
 //
 // Create the DoveTail
 //
 // Move the ARC points by the normal vector and the sectorThickness
 // The first part is the inside part
 // The order or corners are as for the sector pieces
 addComment(of," Placement of Dovetail inside sector "); 

 currentColor = kGray;
 string dovetailmaterial = "Al";
 
 TVector2 CP11_D = CP11-sectorThickness*V25_1N;
 TVector2 CP12_D = CP12+sectorThickness*VBR_N;
 
 defineVolume("DoveTailCorner1","DTCA");
 defineVolume("DoveTailCorner2","DTCB");
 defineVolume("DoveTailCorner3","DTCC");
 defineVolume("DoveTailCorner4","DTCD");
 defineVolume("DoveTailCorner5","DTCE");
 defineVolume("DoveTailCorner6","DTCF");
 defineVolume("DoveTailCorner7","DTCG");
 defineVolume("DoveTailCorner8","DTCH");

 addTubeSegment("DoveTailCorner1",C1, CP11_D, CP12_D, 
		"DoveTailLength", "DoveTailThickness", "DoveTailZOffset", dovetailmaterial); 

  TVector2 CP21_D = CP21+sectorThickness*VBR_N;

 TVector2 CP22_D = CP22+sectorThickness*V8_1N;
  addTubeSegment("DoveTailCorner2",C2, CP21_D, CP22_D, 
		"DoveTailLength", "DoveTailThickness", "DoveTailZOffset", dovetailmaterial); 
 
 TVector2 CP31_D = CP31+sectorThickness*V8_1N;
 TVector2 CP32_D = CP32-sectorThickness*OV2_N;
 
 addTubeSegment("DoveTailCorner3",C3, CP31_D, CP32_D, 
		"DoveTailLength", "DoveTailThickness", "DoveTailZOffset", dovetailmaterial); 
 
 TVector2 CP42_D = CP42-(DoveTailThickness+sectorThickness)*OV2_N;
 TVector2 CP41_D = CP41+(sectorThickness+DoveTailThickness)*V8_2N;

 
 addTubeSegment("DoveTailCorner4",C4, CP41_D, CP42_D, 
		"DoveTailLength", "DoveTailThickness", "DoveTailZOffset", dovetailmaterial); 
 
 TVector2 CP51_D = CP51+sectorThickness*V8_2N;
 TVector2 CP52_D = CP52-sectorThickness*OV1_N;
 
 addTubeSegment("DoveTailCorner5",C5, CP51_D, CP52_D, 
		"DoveTailLength", "DoveTailThickness", "DoveTailZOffset", dovetailmaterial); 
 
 
 //Since the dovetial is on inside of sector redefine ARC..

 TVector2 CP62_D = CP62-(DoveTailThickness+sectorThickness)*OV1_N;
 TVector2 CP61_D = CP61+(sectorThickness+DoveTailThickness)*V8_3N;

 addTubeSegment("DoveTailCorner6",C6, CP61_D, CP62_D, 
		"DoveTailLength", "DoveTailThickness", "DoveTailZOffset", dovetailmaterial); 
 
 TVector2 CP71_D = CP71+sectorThickness*V8_3N;
 TVector2 CP72_D = CP72-sectorThickness*VBL_N;
 
 addTubeSegment("DoveTailCorner7",C7, CP71_D, CP72_D, 
		"DoveTailLength", "DoveTailThickness", "DoveTailZOffset", dovetailmaterial); 
 TVector2 CP81_D = CP81-sectorThickness*VBL_N;
 TVector2 CP82_D = CP82+sectorThickness*V25_1N;
 addTubeSegment("DoveTailCorner8",C8, CP81_D, CP82_D, 
		"DoveTailLength", "DoveTailThickness", "DoveTailZOffset", dovetailmaterial); 
  
 
 //
 // Must recalculate the Displaced corners since for the corner 4 and 6 they had the added witdh.
 //

 defineVolume("DoveTailSide1","DTSA");
 defineVolume("DoveTailSide2","DTSB");
 defineVolume("DoveTailSide3","DTSC");
 defineVolume("DoveTailSide4","DTSD");
 defineVolume("DoveTailSide5","DTSE");
 defineVolume("DoveTailSide6","DTSF");
 defineVolume("DoveTailSide7","DTSG");
 defineVolume("DoveTailSide8","DTSH");

CP42_D = CP42-(sectorThickness)*OV2_N;
CP41_D = CP41+(sectorThickness)*V8_2N;
CP62_D = CP62-(sectorThickness)*OV1_N;
CP61_D = CP61+(sectorThickness)*V8_3N;

  addBox("DoveTailSide1", CP12_D, CP21_D, "DoveTailLength","DoveTailHalfThickness","DoveTailZOffset", dovetailmaterial, "DoveTail Sector Right side");


  addBox("DoveTailSide2", CP22_D, CP31_D, "DoveTailLength","DoveTailHalfThickness","DoveTailZOffset", dovetailmaterial, "DoveTail  Sector Right upper side");
 addBox("DoveTailSide3", CP32_D, CP42_D, "DoveTailLength","DoveTailHalfThickness","DoveTailZOffset", dovetailmaterial, "DoveTail Sector Right upper side");
 addBox("DoveTailSide4", CP41_D, CP51_D, "DoveTailLength","DoveTailHalfThickness","DoveTailZOffset", dovetailmaterial, "DoveTail  Sector Right upper side");
 addBox("DoveTailSide5", CP52_D, CP62_D, "DoveTailLength","DoveTailHalfThickness","DoveTailZOffset", dovetailmaterial, "DoveTail  Sector Right upper side");
 addBox("DoveTailSide6", CP61_D, CP71_D, "DoveTailLength","DoveTailHalfThickness","DoveTailZOffset", dovetailmaterial, "DoveTail  Sector Right upper side");
 addBox("DoveTailSide7", CP72_D, CP81_D, "DoveTailLength","DoveTailHalfThickness","DoveTailZOffset", dovetailmaterial, "DoveTail  Sector Right upper side");
 addBox("DoveTailSide8", CP82_D, CP11_D, "DoveTailLength","DoveTailHalfThickness","DoveTailZOffset", dovetailmaterial, "DoveTail  Sector Right upper side");


 //
 // DoveTailExtrusion 
 // It probably also needs to be in a containervolume¾¾  
 //


defineVolume("DoveTailExtrusionCorner1","DTEA");
defineVolume("DoveTailExtrusionCorner2","DTEB");
defineVolume("DoveTailExtrusionCorner3","DTEC");
defineVolume("DoveTailExtrusionCorner4","DTED");
defineVolume("DoveTailExtrusionCorner5","DTEE");
defineVolume("DoveTailExtrusionCorner6","DTEF");
defineVolume("DoveTailExtrusionCorner7","DTEG");
defineVolume("DoveTailExtrusionCorner8","DTEH");

 addComment(of," Placement of Dovetail Extrusion"); 
  

 addTubeSegment("DoveTailExtrusionCorner1",C1, CP11, CP12, 
		"DoveTailExtrusionLength", "DoveTailExtrusionThickness","DoveTailExtrusionZOffset", dovetailmaterial);
 addTubeSegment("DoveTailExtrusionCorner2",C2, CP21, CP22, 
		"DoveTailExtrusionLength", "DoveTailExtrusionThickness","DoveTailExtrusionZOffset", dovetailmaterial);
 addTubeSegment("DoveTailExtrusionCorner3",C3, CP31, CP32, 
		"DoveTailExtrusionLength", "DoveTailExtrusionThickness","DoveTailExtrusionZOffset", dovetailmaterial);
 addTubeSegment("DoveTailExtrusionCorner4",C4, CP41_D, CP42_D, 
		"DoveTailExtrusionLength", "DoveTailExtrusionThickness","DoveTailExtrusionZOffset", dovetailmaterial);
 addTubeSegment("DoveTailExtrusionCorner5",C5, CP51, CP52, 
		"DoveTailExtrusionLength", "DoveTailExtrusionThickness","DoveTailExtrusionZOffset", dovetailmaterial);
 addTubeSegment("DoveTailExtrusionCorner6",C6, CP61_D, CP62_D, 
		"DoveTailExtrusionLength", "DoveTailExtrusionThickness","DoveTailExtrusionZOffset", dovetailmaterial);
 addTubeSegment("DoveTailExtrusionCorner7",C7, CP71, CP72, 
		"DoveTailExtrusionLength", "DoveTailExtrusionThickness","DoveTailExtrusionZOffset", dovetailmaterial);
 

addTubeSegment("DoveTailExtrusionCorner8",C8, CP81, CP82, 
		"DoveTailExtrusionLength", "DoveTailExtrusionThickness","DoveTailExtrusionZOffset", dovetailmaterial);
 
 defineVolume("DoveTailExtrusionSide1","DESA");
 defineVolume("DoveTailExtrusionSide2","DESB");
 defineVolume("DoveTailExtrusionSide3","DESC");
 defineVolume("DoveTailExtrusionSide4","DESD");
 defineVolume("DoveTailExtrusionSide5","DESE");
 defineVolume("DoveTailExtrusionSide6","DESF");
 defineVolume("DoveTailExtrusionSide7","DESG");
 defineVolume("DoveTailExtrusionSide8","DESH");

addBox("DoveTailExtrusionSide1", CP12, CP21, "DoveTailExtrusionLength","DoveTailExtrusionHalfThickness","DoveTailExtrusionZOffset", dovetailmaterial, "DoveTail Extrusion Sector Right side");

addBox("DoveTailExtrusionSide2", CP22, CP31, "DoveTailExtrusionLength","DoveTailExtrusionThicknessBox","DoveTailExtrusionZOffset", dovetailmaterial, "DoveTailExtrusion Extrusion Sector Right upper side");
 addBox("DoveTailExtrusionSide3", CP32, CP42, "DoveTailExtrusionLength","DoveTailExtrusionThicknessBox","DoveTailExtrusionZOffset", dovetailmaterial, "DoveTailExtrusion Extrusion Sector Right upper side");
 addBox("DoveTailExtrusionSide4", CP41, CP51, "DoveTailExtrusionLength","DoveTailExtrusionThicknessBox","DoveTailExtrusionZOffset", dovetailmaterial, "DoveTailExtrusion Extrusion Sector Right upper side");
 addBox("DoveTailExtrusionSide5", CP52, CP62, "DoveTailExtrusionLength","DoveTailExtrusionThicknessBox","DoveTailExtrusionZOffset", dovetailmaterial, "DoveTailExtrusion Extrusion Sector Right upper side");
 addBox("DoveTailExtrusionSide6", CP61, CP71, "DoveTailExtrusionLength","DoveTailExtrusionThicknessBox","DoveTailExtrusionZOffset", dovetailmaterial, "DoveTailExtrusion Extrusion Sector Right upper side");
 addBox("DoveTailExtrusionSide7", CP72, CP81, "DoveTailExtrusionLength","DoveTailExtrusionThicknessBox","DoveTailExtrusionZOffset", dovetailmaterial, "DoveTailExtrusion Extrusion Sector Right upper side");
 addBox("DoveTailExtrusionSide8", CP82, CP11, "DoveTailExtrusionLength","DoveTailExtrusionThicknessBox","DoveTailExtrusionZOffset", dovetailmaterial, "DoveTail Extrusion Sector Right upper side");
 
 //
 //
 double dz = 0.1+1.2+0.4+0.1;
 double zoff = DoveTailZOffset-DoveTailLength/2.0-DoveTailExtrusionLength - dz/2.0;
 defineVariable("DoveTailShapeOffset",DoveTailZOffset-DoveTailLength/2.0-DoveTailExtrusionLength - dz/2.0,"offset for final pieces");
 
 defineVolume("DoveTailExt","DTEX");
 DoveTailShape("DoveTailExt", "Al", zoff, 7.6, 2.7, 
	       dz, 0.15, 0.6, 0.6, 0.8, 0.4);
 //
 // build a ladder element, that will contain the active silicon, and
 // place 4 identical ladders with different number in each sector volume.
 //
 //
 if(useTGeo&ofOpen){
   of << "//" << endl;
   of << "// Define the basic ladder container" << endl;
   of << "//" << endl;
 }

   addComment(ofvol," BOX for silicon active sensors, cables etc ");
   makeBox("LADR", ladderWidth, ladderVolumeThickness, ladderLength, "Air","Mothervolume for ladder");


   // Here I have som issue since I add another layer. so the
   // Volume of LADR needs to include the Create for the subsequent ladder stuff.
   // the closing bracket for the ofvol of ladr has to be pushed back behind 
   // 
   //
   // Create the content of the ladder (si backing....)
   //
   
   

   if(useTGeo&ofOpen){
     of << "//" << endl;
     of << "// Add the intrinsic components of the ladder" << endl;
     of << "//" << endl;
   }
   if(useAgml & ofOpen){
     of.close();
     of.open("ladder.xml");
     addComment(of, " File ladder.xml");
     TDatime time;
     addComment(of,Form(" Created on : %s",time.AsString()));
     
     addComment(of,"Ladder definition");
     ofvol.close();
     
     ofvol.open("ladderVol.xml");
     addComment(ofvol, " File ladderVol.xml");
     addComment(ofvol,Form(" Created on : %s",time.AsString()));
     
     addComment(ofvol," Ladder Volume defs"); 
   }
   //
   currentMasterVolume="LADR";
   currentColor=kBlue;
   
   TVector2 RC(-ladderWidth/2.0, ladderVolumeThickness/2.0-ladderThickness);
   TVector2 V(1.0,0.0);
   TVector2 VN(0.0, 1.0);
   TVector2 RCP = RC;
   
   //
   // Note the z-offsets has to be considered carefully, to account properly for the ladder container
   //
   
   if(useTGeo&ofOpen){
     of << "//" << endl;
     of << "// Position The siLayers " << endl;
     of << "//" << endl;
   }
   


#if 0
   currentColor=kBlue-5;
   
   siLadderOffset = ladderLength/2.0 - siSensorCableOffset - siLength/2.0;
   addBox("SIFL", RC+cableExtrusion*V, RC+(cableExtrusion+siFrameOffset)*V, siLength, siThickness, siLadderOffset, "Silicon");
   currentColor=kBlue;
   RC = RC+(siFrameOffset+cableExtrusion)*V;
   RC.Print();
   addBox("PLAC", RC, RC+siWidth*V, siLength, siThickness, siLadderOffset, "ActiveSilicon");
   RC = RC+siWidth*V;
   currentColor=kBlue;
   addBox("SIFR", RC, RC+siOuterFrameWidth*V, siLength, siThickness, siLadderOffset, "Silicon");
   
#else
   siLadderOffset = ladderLength/2.0 - siSensorCableOffset - siLength/2.0;
   cout << "siLadderOffset " << siLadderOffset << endl;
   //
   // Create a sensor
   //   
   currentColor=kBlue-6;
   defineVolume("Sensor","PXSI");

   makeBox("Sensor", sensorWidth, siThickness, sensorLength, "Silicon", "MotherVolume for sensor");
   currentColor=kBlue;
   makeBox("PLAC", activeSensorWidth, siThickness, activeSensorLength,"ActiveSilicon","Active silicon ");
   positionVolume("PLAC", "Sensor", activeSensorXOffset, activeSensorYOffset, 0.0);
   //
   // Place 10 copies in LADR
   //

   RC = RC+(siFrameOffset+cableExtrusion)*V;
  
   for(int i=0;i<10;i++){
     currentVolumeNo=i+1;
     double siSensorOffset = ladderLength/2.0 - siSensorCableOffset - siLength/2.0 -
       (sensorLength/2.0+gap/2.0) + (i-4)*(sensorLength+gap);
     positionBox("Sensor", RC, RC+sensorWidth*V,  siThickness, siSensorOffset);
   }
   currentVolumeNo=1;
#endif
   
   currentColor = kOrange;
   RC = RCP + VN*(siThickness)+V*cableExtrusion;
   addBox("GLUA", RC, RC + (siFrameOffset+siWidth+siOuterFrameWidth)*V, siLength, glue1Thickness, siLadderOffset, "medAdhesive",
	  "Glue for CMOS sensors");
   
   
   if(useTGeo&ofOpen){
     of << "//" << endl;
     of << "// Position the AlCable " << endl;
     of << "//" << endl;
   }
   
   currentColor = kMagenta;
   RC = RCP + VN*(siThickness+glue1Thickness);

   defineVariable("cableLength",cableLength/2.0,"Cable Half Length");
   defineVariable("cableThickness",cableThickness/2.0,"Cable Half Length");
   defineVariable("cableZOffset", cableZOffset,"Al cable Offset");

   addBox("AlCa", RC, RC + cableWidth*V, "cableLength", "cableThickness", "cableZOffset", "medAlCable");
   
   
   
   defineVolume("CarbonBacking","CFBK");
   if(useTGeo&ofOpen){
     of << "//" << endl;
     of << "// Position the glue and CarbonBacking " << endl;
     of << "//" << endl;
   }
   currentColor = kRed-3;
   RC = RCP + VN*(siThickness+glue1Thickness+cableThickness)+V*cableExtrusion;
   addBox("GLUB", RC, RC + backingWidth*V, backingLength, glue2Thickness, backingZOffset, "medAdhesive","Glue for Al/Cu Cable");
   
   
   currentColor = kBlack;
   RC = RCP + VN*(siThickness+glue1Thickness+cableThickness+glue2Thickness)+V*cableExtrusion;;
   addBox("CarbonBacking", RC, RC + backingWidth*V, "backingLength", "backingThickness", "backingZOffset", "CarbonFiberBacking","Carbon Fiber backing on ladder");
   
   currentColor = kRed-3;
   RC = RCP + VN*(siThickness+glue1Thickness+cableThickness + glue2Thickness+backingThickness)+V*cableExtrusion;;
   addBox("GLUC", RC, RC + glue3Width*V, backingLength, glue3Thickness, backingZOffset, "medSiAdhesive","Glue layer backing to sector beam");
   
   //
   // Define the driver boards
   //
   
   RC = RCP + VN*(siThickness+glue1Thickness);
   addBox("DRIV", RC, RC + driverBoardWidth*V, driverBoardLength, -driverBoardThickness, driverBoardOffset, "medFR4","Driverboard PCB");
   
   


   
   
   //
   // Reopen ...sector with append for useAgml
   // 
   if(useAgml&&ofOpen){
     of.close();
     of.open("sector.xml", ios::app);
     of << "<!-- placement of silicon volume -->\n" << endl;
   }
   
    //
   // position the ladder in the sector
   // 
   if(useTGeo&&ofOpen){
     of << "//" << endl;
     of << "// Position 4 copies of the ladder" << endl;
     of << "//" << endl;
   }
   
     
   currentColor=kRed;
   currentMasterVolume="Sector";
   currentVolumeNo= 3;
   //
   // suffix for position, since we need to reuse R8_1 for the driver board positioning.
   //
   TVector2 R8_1P = R8_1- (siFrameOffset+cableExtrusion)*V8_1+V8_1N*(ladderNominal-ladderVolumeThickness);
   positionBox("LADR",   R8_1P,  R8_1P + V8_1*ladderWidth, ladderVolumeThickness, ladderZOffset);
   currentVolumeNo= 2;
   TVector2 R8_2P = R8_2- (siFrameOffset+cableExtrusion)*V8_2 +V8_2N*(ladderNominal-ladderVolumeThickness);
   positionBox("LADR",   R8_2P,  R8_2P + V8_2*ladderWidth, ladderVolumeThickness, ladderZOffset);
   currentVolumeNo= 1;
   TVector2 R8_3P = R8_3- (siFrameOffset+cableExtrusion)*V8_3 +V8_3N*(ladderNominal-ladderVolumeThickness);
   positionBox("LADR",   R8_3P,  R8_3P + V8_3*ladderWidth, ladderVolumeThickness, ladderZOffset);
   currentVolumeNo= 4;
   TVector2 R25_1P = R25_1 + V25_1*(siWidth+siFrameOffset+cableExtrusion)- ladderNominal*V25_1N;
   V25_1 = -1.0*V25_1;
   V25_1N = -1.0*V25_1N;

   positionBox("LADR",   R25_1P,  R25_1P + V25_1*ladderWidth, -ladderVolumeThickness, ladderZOffset);
 


   if(useAgml){
     addComment(of," Close PXLA volume ");
     addStatement(of,"</Volume>");
   }

   // Si outer layers
   // 
   drawLine(R8_1, V8_1*1.92)->SetLineColor(kBlue);
   drawLine(R8_2, V8_2*1.92)->SetLineColor(kBlue);
   drawLine(R8_3, V8_3*1.92)->SetLineColor(kBlue);
   drawLine(R25_1,V25_1*1.92)->SetLineColor(kBlue);
   
   


if(guides){
 if(guides){
     drawMarker(XP1);
     drawMarker(XP2);
     drawMarker(XP3);
     drawMarker(XP4);
     drawMarker(XP5);
     drawMarker(XP6);
     drawMarker(XP7);
     drawMarker(XP8);   
   }

  if(guides){
   drawMarker( C2 );
   drawMarker( CP21);
   drawMarker( CP22);
  }

 drawLine(P8_1, V8_1, 1.0)->SetLineColor(kRed);
 drawLine(P8_2, V8_2, 1.0)->SetLineColor(kRed);
 drawLine(P8_3, V8_3, 1.0)->SetLineColor(kRed);
 drawLine(P25_1, V25_1, 1.0)->SetLineColor(kRed);

 drawLine(OB1, 0.2*OV1, 2.0);
 drawLine(OB2, 0.2*OV2, 2.0);

 drawLine(XBR, VBR, 3.0);
 drawLine(XBL, VBL, 3.0);
}
 


}


//________________________________________________
void PlotSector(){

 TCanvas *c3 = new TCanvas("c3","",10,10,800,800);
 c3->Range(-10,-10,10,10);
 c3->Draw();



//
// Beam Pipe
//

 



 TArc *bp = new TArc(0,0,2.0);
 bp->SetLineWidth(1.5);
 bp->SetFillStyle(4000);
 bp->Draw();




//
// Out sector boundaries
// Candidate for sectorModule, so it can be repeated.
// This routines will first calculate all proper information
//
 
//
// This piece has not been updated.
//
 if(useTGeo){
   

   of.open("material.txt");
   ofOpen=kTRUE;
   currentColor=1;

   of << "TGeoMaterial *matCarbonFiber = new TGeoMaterial(\"CarbonFiber\", 12, 6, 1.3);" << endl;
   of << "TGeoMedium *CarbonFiber = new TGeoMedium(\"CarbonFiber\", 3, matCarbonFiber);" << endl;
   of << "TGeoMaterial *matSilicon = new TGeoMaterial(\"Silicon\", 28, 14, 2.9);" << endl;
   of << "TGeoMaterial *matAlCable = new TGeoMaterial(\"AlCable\", 27, 13, 2.8);" << endl;

   TVector2 C(0.0,0.0);

   addTube("BeamPipe", C, 2.0, 60.0, 0.08);


   of.close();


 }
 
if(useTGeo){
   of.open("sector.txt");
   ofOpen=kTRUE;
 }

 if(useAgml){
   of.open("sector.xml");   ofvol.open("sectorVol.xml");
   ofOpen=kTRUE;
 }

 Sector(0,0);

 of.close();
 ofOpen=kFALSE;
 
 return;

 Sector(36,0);
 
 Sector(72,0);
 Sector(108,0);
 Sector(144,0);
 Sector(-36,0);
 Sector(-72,0);
 Sector(-108,0);
 Sector(-144,0);
 Sector(180,0);
 


}




void CheckIntercept(){
  TVector2 p1(0,0), v1(1,0);
  TVector2 p2(2,1), v2(1,1);
  double det(-1.0);

  TVector2 res= Intercept(p1, v1, p2, v2, det);
  res.Print();
 }
  
void checkVolumeName(){
  //
  // test for volume names and findinh
  //
  defineVolume("MyLongName1","MLN1");
  defineVolume("MyLongName2","MLN2");

  
  showVolumeMapping();

  string name;
  findVolume("MyLongName1", name);
  cout << name.data() << endl;

  findVolume("MyLongName3", name);
  cout << name << endl;
}



void checkArb8Trap(double angle=36){
  //
  // Used to develop code that takes 4 points as used in Arb8 and converts to a TRPP.
  // Not quite I assume boths end are identical
  // It also assumes that all point P0..P3 are in the x-y plane and that only rotations are in the
  // 

  of.open("trap.txt");
  ofOpen=kTRUE;

  TVector2 P[4];
  double ri(7.0), ro(8.0);
  TVector2 VR(0.,1.);
  TVector2 P0 = ri*VR;
  TVector2 P1 = ro*VR;
  TVector2 P2 = P0.Rotate(TMath::DegToRad()*angle);
  TVector2 P3 = P1.Rotate(TMath::DegToRad()*angle);

  
  P[0] = P0; P[1]=P2; P[2]=P3; P[3]=P1;

  currentMasterVolume="CAVE";
  addTrapzoid("TRAP_ARB8", P, 1.0, 0.0, "Al", "Trapzoid from ARB8");


  //
  // This piece of code is for development of conversion from 4 Points to
  // TRAP, and checking that there are two parallel vector between the sets of points.
  //

  TVector2 S0 = P[1]-P[0];
  TVector2 S1 = P[2]-P[1];
  TVector2 S2 = P[3]-P[2];
  TVector2 S3 = P[0]-P[3];


  TVector2 C0,C1,C2,C3;
  // Is S0 || S2  or is S1 || S3
  // reorder point in all cases so s0-s1 is parallel to s1-s2
  //
  double del0 = S0.DeltaPhi(S2);
  double del1 = S1.DeltaPhi(S3);

  double eps(1.0e-07);
  if(TMath::Abs(del0) < eps || TMath::Abs(TMath::Abs(del0)-TMath::Pi())< eps ){
    // case 1 ok This implies that P1,P0 is || with P2/3
    //
    C0=P[0]; C1=P[1]; C2=P[2]; C3=P[3];
  }
  else if (TMath::Abs(del1) < eps || TMath::Abs(TMath::Abs(del1)-TMath::Pi())< eps ){
    C0=P[1]; C1 =P[2]; C2=P[3]; C3=P[0];
  }
  else {
    cout << "*** P0-P4 does not consittue a TRPA" << endl;
    return;
  }

  //
  // Calculate the center of the TRAP.
  //
  TVector2 C = 0.25*(C0+C1+C2+C3);
  //
  //Calculate the two length at y=+-dy
  //
   
  double Bh1=(C1-C0).Mod()/2.0;
  double Bl1=(C3-C2).Mod()/2.0;

  // Need two more pieces of information i.e.
  // angle between normal vector between the two parallel lvectors (sides), and the rotation needed.
  //
  TVector2 Ex  =(C1-C0).Unit();
   TVector2 Ey  =Ex.Rotate(TMath::DegToRad()*90);


  TVector2 C0M = 0.5*(C0+C1);
  TVector2 C2M = 0.5*(C2+C3);
 
  double alpha = Ey.DeltaPhi(C0M-C2M);
  currentColor=kRed;
  
  double phi = Ey.DeltaPhi(VR)*TMath::RadToDeg();
  double theta(0.0);

  double param[10];
  param[0]=0.0; // fTheta
  param[1]=0.0; // fPhi
  param[2]=TMath::Abs(dot(C-C0M,Ey));
  param[3]=Bl1;
  param[4]=Bh1;
  param[5]=alpha;
  param[6]=param[2];
  param[7]=Bl1;
  param[8]=Bh1;
  param[9]=alpha;
  
  makeTRAP("TRAP", 1.0, param, "Al","test setup of TRAP");
  positionVolume("TRAP","CAVE", C.X(), C.Y(), 0.0, phi, theta);

      

  


  of.close();
}
  


void CheckSetupArc(double rotate=0.0){
//
//  
  TVector2 v2(0,1), v1(-1,0.1), P(0,0);
  TVector2 P1, P2, C;
  double radius=1.0;
  v2=1.0*v2;

  v1=v1.Rotate(rotate*TMath::Pi()/180.0);
  v2=v2.Rotate(rotate*TMath::Pi()/180.0);

  v1.Print();v2.Print();
  SetupArc(P, v1, v2, radius,
	   C, P1, P2);

  C.Print(); P1.Print(); P2.Print();
  TCanvas *cc = new TCanvas("cc","",10,10,400,400);
  cc->Range(-4,-4, 4, 4);
  cc->Draw();
  drawLine(P, v1)->SetLineStyle(kDashed);
  drawLine(P, v2)->SetLineStyle(kDashed);
  drawMarker( C );

  drawArc(C, P1, P2);



 	
}
//**********************************************************
//
// $Log$
// Revision 1.1.1.1  2013/03/28 19:27:12  videbaks
// These are the macro files used to generate both TGeo and xml skeleton for the PXL geometry
// Originally developed under my provate accountw
//
//
// Revision 1.11  2013/01/27 19:01:46  videbaek
// Remove debug output
//
// Revision 1.10  2013/01/25 20:52:53  videbaek
// Fix small typo. Aded some vol comments
//
// Revision 1.9  2013/01/25 20:50:38  videbaek
// Update geomerty to finalize for Y2013
// Added DRIVER board for ladders.
//
// Revision 1.8  2013/01/12 19:36:43  videbaek
// Fixed the conversion to TRAP in Sector.C
//
// Revision 1.7  2013/01/12 17:51:29  videbaek
// Change the volumename sector to Sector, so GetViolume can be used consistently
// Asdded new method addTrap.
// Stored because it works ahead of changing addTrapziod to AddTrap..
//
// Revision 1.6  2013/01/11 21:49:41  videbaek
// Fixed the points to TRAP.
//
// Revision 1.5  2012/12/31 12:52:59  videbaek
// Fix typo in ARB8 and added the missing "/>"
//
// Revision 1.4  2012/12/30 17:52:24  videbaek
// Update with agml output for ARB8 shape.
//
// Revision 1.3  2012/12/30 15:51:51  videbaek
// Update  to have agmlVolume names dealt with.
// Version saved ahead of midifying ladder sector positions.
//
// Revision 1.2  2012/12/27 18:38:38  videbaek
// Add cvs commands
// Save Sector before making modifications for the DovePlate that holds the
// sector though the DoveTails and DoveTail extension.
//
