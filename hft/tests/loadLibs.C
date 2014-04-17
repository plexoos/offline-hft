
{
   gROOT->Reset();
   gROOT->Clear();

   gSystem->AddIncludePath("-I$HOME/bnl_me/recostar_cvs/StRoot");

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

   gSystem->Load("$HOME/bnl_me/recostar_cvs/.$STAR_HOST_SYS/lib/libStiPxl");


   //gROOT->LoadMacro("test_pxl_sti.C+");
}
