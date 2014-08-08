
#include "Riostream.h"
#include "TMinuit.h"
#include "TFile.h"
#include "TH1.h"
#include "TH2.h"
#include "TNtuple.h"

//
//   Example of a program to fit non-equidistant data points
//   =======================================================
//
//   The fitting function fcn is a simple chisquare function
//   The data consists of 5 data points (arrays x,y,z) + the errors in errorsz
//   More details on the various functions or parameters for these functions
//   can be obtained in an interactive ROOT session with:
//    Root > TMinuit *minuit = new TMinuit(10);
//    Root > minuit->mnhelp("*",0)  to see the list of possible keywords
//    Root > minuit->mnhelp("SET",0) explains most parameters
//

Float_t xc[7000],yc[7000],zc[7000],ux[7000],uy[7000],uz[7000],R[7000];
Float_t x[7000],y[7000],z[7000];


//______________________________________________________________________________
Double_t func(float x,float y,float z,Double_t *par)
{
  //const   Double_t s2=0.000025;
  // Double_t value=( (par[0]*par[0])/(x*x)-1)/ ( par[1]+par[2]*y-par[3]*y*y);
 
 // Double_t value=( (x-par[0])*(x-par[0]) +
// 		   (y-par[1])*(y-par[1]) +
// 		   (z-par[2])*(z-par[2])  -
// 		   par[3]*par[3] );
 
 Double_t value = par[2]+sqrt( par[3]*par[3] - 
			       (x-par[0])*(x-par[0]) -
			       (y-par[1])*(y-par[1]) );
 return value;
}

//______________________________________________________________________________
void fcn(Int_t &npar, Double_t *gin, Double_t &f, Double_t *par, Int_t iflag)
{
   const Int_t nbins = 520;
   Int_t i;

//calculate chisquare
   Double_t chisq = 0;
   Double_t delta;
   for (i=0;i<nbins; i++) {
     //     delta  = (z[i]-func(x[i],y[i],par))/errorz[i];
     delta  = (z[i]-func(x[i],y[i],z[i],par));
     //     delta  = func(x[i],y[i],z[i],par);
     chisq += delta*delta;
   }
   f = chisq;
}


//______________________________________________________________________________
void Ifit()
{
// // The z values
//         z[0]=1;
//         z[1]=0.96;
//         z[2]=0.89;
//         z[3]=0.85;
//         z[4]=0.78;
// // // The errors on z values
// //         Float_t error = 0.01;
// //         errorz[0]=error;
// //         errorz[1]=error;
// //         errorz[2]=error;
// //         errorz[3]=error;
// //         errorz[4]=error;
// // the x values
//         x[0]=1.5751;
//         x[1]=1.5825;
//         x[2]=1.6069;
//         x[3]=1.6339;
//         x[4]=1.6706;
// // the y values
//         y[0]=1.0642;
//         y[1]=0.97685;
//         y[2]=1.13168;
//         y[3]=1.128654;
//         y[4]=1.44016;

//  Float_t col1[3000]; 
//  Float_t col2[3000];   
//  Float_t col3[3000]; 
  const Int_t max_files = 7000;
  ifstream in;
  // we assume a file basic.dat in the current directory
  // this file has 7 columns of float data
  //  in.open("mytextfile.txt");
  in.open("Ball_test_8.dat");

Float_t val1=0,val2=0,val3=0,val4=0,val5=0,val6=0,val7=0;
//  Int_t i;
  Int_t nlines    = 0;
  Int_t cpt       = 0;
  int tempo;
  while (1) {
    in >> val1 >> val2 >> val3>>val4>>val5>>val6>>val7;
    if (!in.good()) break;
    if (nlines<max_files)  {
      xc[nlines] = val1;
      yc[nlines] = val2;
      zc[nlines] = val3;
      ux[nlines] = val4;
      uy[nlines] = val5;
      uz[nlines] = val6;
      R[nlines] = val7;

      x[nlines] = xc[nlines] + R[nlines]*ux[nlines];
      y[nlines] = yc[nlines] + R[nlines]*uy[nlines];
      z[nlines] = zc[nlines] + R[nlines]*uz[nlines];

      nlines++;
    }  
  }
  printf(" found %d points\n",nlines);
  in.close();

   for (int ii = 0;ii<5 ;ii++)
     {
       cout << " xc = " << xc[ii] <<" yc = " << yc[ii] << " zc = " << zc[ii] <<  
	 " ux = " << ux[ii] <<" uy = " << uy[ii] << " uz = " << uz[ii]<< " R = " << R[ii]<<endl;
      cout << " x = " << x[ii] <<" y = " << y[ii] << " z = " << z[ii] <<endl;
     } 

   TMinuit *gMinuit = new TMinuit(5);  //initialize TMinuit with a maximum of 5 params
   gMinuit->SetFCN(fcn);

   Double_t arglist[10];
   Int_t ierflg = 0;

   arglist[0] = 1;
   gMinuit->mnexcm("SET ERR", arglist ,1,ierflg);

// Set starting values and step sizes for parameters
//   static Double_t vstart[4] = {3, 1 , 0.1 , 0.01};
//   static Double_t step[4] = {0.01 , 0.01 , 0.01 , 0.001};
 Double_t vstart[4] = {0.01, 0.01 , 0.01 , 3.1};
 Double_t step[4] = {0.001 , 0.001 , 0.001 , 0.001};
   gMinuit->mnparm(0, "xf", vstart[0], step[0], 0,0,ierflg);
   gMinuit->mnparm(1, "yf", vstart[1], step[1], 0,0,ierflg);
   gMinuit->mnparm(2, "zf", vstart[2], step[2], 0,0,ierflg);
   gMinuit->mnparm(3, "Rf", vstart[3], step[3], 0,0,ierflg);

// Now ready for minimization step
   arglist[0] = 525;
   arglist[1] = 1.;
   gMinuit->mnexcm("MIGRAD", arglist ,2,ierflg);

// Print results
   Double_t amin,edm,errdef;
   Int_t nvpar,nparx,icstat;
   gMinuit->mnstat(amin,edm,errdef,nvpar,nparx,icstat);
   gMinuit->mnprin(3,amin);

}

