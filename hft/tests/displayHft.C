{
   gROOT->Reset();
   gROOT->Clear();

   gROOT->LoadMacro("$STAR/StRoot/StMuDSTMaker/COMMON/macros/loadSharedLibraries.C");
   loadSharedLibraries();

   gSystem->Load("$STAR_LIB/libStTpcDb");
   gSystem->Load("$STAR_LIB/libStIstDbMaker");

   gSystem->Load("$ROOTSYS/lib/libEve");

   gSystem->Load(".sl64_gcc447/lib/libStHftPoolEventT.so");

   const Char_t *path = ".:./StarVMC/Geometry/macros/:$STAR/StarVMC/Geometry/macros/";

   Char_t *file = gSystem->Which(path, "viewStarGeometry.C", kReadPermission);
   cout << "Loading macro: " << file << endl;
   gROOT->LoadMacro(file);

   TEveManager::Create();
   std::string myGeomTag("y2014a");
   cacheGeometry(myGeomTag.c_str(), "");

   // Register geometry
   gEve->RegisterGeometryAlias("Default", Form("%s.root",myGeomTag.c_str()));
   gGeoManager = gEve->GetDefaultGeometry();

   gROOT->LoadMacro("$ROOTSYS/tutorials/eve/MultiView.C+");

   //hft_display(0,15040023);
}
