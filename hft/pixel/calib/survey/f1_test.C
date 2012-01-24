// Define the fit parameters. 
TF1* life = new TF1("life","[0]+[1]*exp(-x/abs([2]))"); 
life->SetParNames("Background","Normalization","Lifetime"); 
life->SetParameters(0,9000,2500);
// Make sure the fit results are plotted on the graph 
gStyle->SetOptFit(1112);
// Fit the function to the histogram 
//arg1 – the fit function 
//arg2 – An option to make sure errors are calculated 
//arg3 – Ignore this option 
//arg4 – The low bound of the fit range 
//arg5 – the high bound of the fit range 
h->Fit(life,"E","",1500,10000);
// Draw the result with error bars on the points 
h->Draw("E");
