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

   // Extra root libraries
   gSystem->Load("$ROOTSYS/lib/libEve");
   gSystem->Load("$ROOTSYS/lib/libGeom");
   gSystem->Load("$ROOTSYS/lib/libTable");

   // temp
   gSystem->Load("$OPTSTAR/lib/libboost_system");
   gSystem->Load("$OPTSTAR/lib/libboost_program_options");
   gSystem->Load("$OPTSTAR/lib/libboost_regex");
   gSystem->Load("$OFFLINE_HFT_DIR/.$STAR_HOST_SYS/lib/libStIstDbMaker");
   gSystem->Load("$OFFLINE_HFT_DIR/.$STAR_HOST_SYS/lib/libStHftPoolEventT");
   gSystem->Load("$OFFLINE_HFT_DIR/.$STAR_HOST_SYS/lib/libStHftPoolHftMatchedTree");
   gSystem->Load("$OFFLINE_HFT_DIR/.$STAR_HOST_SYS/lib/libStiScan");
}
