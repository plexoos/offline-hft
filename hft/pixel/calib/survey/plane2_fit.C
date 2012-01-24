
#include "Riostream.h"
#include "TMinuit.h"
#include "TFile.h"
#include "TH1.h"
#include "TH2.h"
#include "TNtuple.h"

//   Example of a program to fit non-equidistant data points
//   The fitting function fcn is a simple chisquare function
//   The data consists of 5 data points (z,y,x) + the errors in errorsz
//   More details on the various functions or parameters for these functions
//   can be obtained in an interactive ROOT session with:
//    Root > TMinuit *minuit = new TMinuit(10);
//    Root > minuit->mnhelp("*",0)  to see the list of possible keywords
//    Root > minuit->mnhelp("SET",0) explains most parameters

Float_t zc[7000],yc[7000],xc[7000],uz[7000],uy[7000],ux[7000],R[7000];
Float_t z[7000],y[7000],x[7000];


Double_t func(float x,float y,float z,Double_t *par) {
  Double_t value=( ( (x-24.4244)*par[0] + (y-50.491)*par[1] + 
		     (z-187.177)*TMath::Sqrt(1.-par[0]*par[0]-par[1]*par[1]) ) );
  //Double_t value=( ( (x-24.3883)*par[0] + (y-50.4952)*par[1] + (z-186.754)*par[2] ) / (sqrt(par[0]*par[0] + par[1]*par[1] + par[2]*par[2])) );
  return value;
}


void fcn(Int_t &npar, Double_t *gin, Double_t &f, Double_t *par, Int_t iflag) {
  const Int_t nbins = 802;
  Int_t i;

  //calculate chisquare
  Double_t chisq = 0;
  Double_t delta;
  for (i=0;i<nbins; i++) {
    delta  = func(x[i],y[i],z[i],par);
    //chisq += delta;
    //chisq += (delta*delta)/(0.00025);
    chisq += delta*delta;
  }
  f = chisq;
}


void Ifit() {
  const Int_t max_files = 802;
  ifstream in;
  // we assume a file basic.dat in the current directory
  // this file has 7 columns of float data
  in.open("test2.txt");

  Float_t val1=0,val2=0,val3=0,val4=0,val5=0,val6=0,val7=0,sumx=0.,sumy=0.,sumz=0.;
  Int_t nlines    = 0;
  Int_t cpt       = 0;
  int tempo;

  while (1) {
    in >> val1 >> val2 >> val3 >> val4 >> val5 >> val6 >> val7;
    if (!in.good()) break;
    if (nlines<max_files) {
      zc[nlines] = val1;
      yc[nlines] = val2;
      xc[nlines] = val3;
      uz[nlines] = val4;
      uy[nlines] = val5;
      ux[nlines] = val6;
      R[nlines] = val7;

      x[nlines] = xc[nlines] + R[nlines]*ux[nlines];
      y[nlines] = yc[nlines] + R[nlines]*uy[nlines];
      z[nlines] = zc[nlines] + R[nlines]*uz[nlines];

      nlines++;
    }
  }

  printf("Found %d points\n",nlines);
  in.close();

  //Calculate the average of the columns
  Float_t sumx=0.,sumy=0.,sumz=0.;
  sumx = 0.;
  sumy = 0.;
  sumz = 0.;

  for (int ii = 0;ii<nlines ;ii++) {
    sumx = sumx + x[ii];
    sumy = sumy + y[ii];
    sumz = sumz + z[ii];
  }

  sumx = sumx/float(nlines);
  sumy = sumy/float(nlines);
  sumz = sumz/float(nlines);
  cout << "num= " << nlines << " x avg = " << sumx << " y avg = " << sumy<< " z avg = " << sumz <<endl;


  TMinuit *gMinuit = new TMinuit(2);  //initialize TMinuit with a maximum of 5 params
  gMinuit->SetFCN(fcn);

  Double_t arglist[10];
  Int_t ierflg = 0;

  arglist[0] = 1;
  gMinuit->mnexcm("SET ERR", arglist ,1,ierflg);

  // Set starting values and step sizes for parameters
  //   static Double_t vstart[4] = {3, 1 , 0.1 , 0.01};
  //   static Double_t step[4] = {0.01 , 0.01 , 0.01 , 0.001};
  //Double_t vstart[3] = {0.1 , 0. , 0.1};
  //Double_t step[3] = {0.001 , 0.001 , 0.001};
  Double_t vstart[2] = {0.1 , 0.5};
  Double_t step[2] = {0.01 , 0.01};
  gMinuit->mnparm(0, "nx", vstart[0], step[0], 0,0,ierflg);
  gMinuit->mnparm(1, "ny", vstart[1], step[1], 0,0,ierflg);
  //gMinuit->mnparm(2, "nz", vstart[2], step[2], 0,0,ierflg);
  //   gMinuit->mnparm(3, "ny", vstart[3], step[3], 0,0,ierflg);
  //  gMinuit->mnparm(4, "nz", vstart[4], step[4], 0,0,ierflg);

  // Now ready for minimization step
  arglist[0] =802;
  arglist[1] = 1.;
  gMinuit->mnexcm("MIGRAD", arglist ,2,ierflg);

  // Print results
  Double_t amin,edm,errdef;
  Int_t nvpar,nparx,icstat;
  gMinuit->mnstat(amin,edm,errdef,nvpar,nparx,icstat);
  gMinuit->mnprin(3,amin);

}

