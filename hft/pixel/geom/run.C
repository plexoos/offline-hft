void run(){
  // gROOT->LoadMacro("MapLoader.C+");
  gROOT->LoadMacro("Sector.C+");
  PlotSector();
  gROOT->LoadMacro("assembly.C+");
  assembly();
}
