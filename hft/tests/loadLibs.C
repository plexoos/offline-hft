{
   gROOT->Reset();
   gROOT->Clear();

   gROOT->LoadMacro("$STAR/StRoot/StMuDSTMaker/COMMON/macros/loadSharedLibraries.C");
   loadSharedLibraries();

   gSystem->Load("$STAR_LIB/libSti");
   gSystem->Load("$STAR_LIB/libStiUtilities");
   gSystem->Load("$STAR_LIB/libStTpcDb");
   gSystem->Load("$STAR_LIB/libStSsdDbMaker");
   gSystem->Load("$STAR_LIB/libStDbUtilities");
   gSystem->Load("$STAR_LIB/libStDetectorDbMaker");
   gSystem->Load("$STAR_LIB/libStSvtDbMaker");
   gSystem->Load("$STAR_LIB/libStiMaker");
   gSystem->Load("$STAR_LIB/libStiPxl");
}
