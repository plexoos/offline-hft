void write_istOnPst_survey()
{
    gSystem->Setenv("DB_ACCESS_MODE", "write");
    // Load all required libraries
    gROOT->Macro("LoadLogger.C");
    gSystem->Load("St_base.so");
    gSystem->Load("libStDb_Tables.so");
    gSystem->Load("StDbLib.so");
    
    // Initialize db manager
    StDbManager* mgr = StDbManager::Instance();
    StDbConfigNode* node = mgr->initConfig("Geometry_ist");
    StDbTable* dbtable = node->addDbTable("istOnPst");
    TString storeTime = "2014-01-28 00:00:02"; // beginTime timestamp in MySQL format: "YYYY-MM-DD HH:mm:ss"
    //TString storeTime = "2013-12-15 00:00:00"; //first entry
    mgr->setStoreTime(storeTime.Data());
    
    // Create your c-struct
    //Survey_st table = {1, 1,0,0, 0,1,0, 0,0,1, 0,0,0,.1,.1,.1,.1,.1,.1,"estimation"};
    Survey_st table = {1, 1, 0.00063088, 0.00054519, -0.000629439, 0.999997, -0.00264161, -0.000546865, 0.00264126, 0.999996, 0.0377836, -0.16096, -0.033657, 0.001, 0.001, 0.001, 0.001, 0.001, 0.001, "aligned position"};    

    // Store data to the StDbTable
    dbtable->SetTable((char*)&table, 1);
    
    // uncomment next line to set "sim" flavor. "ofl" flavor is set by default, no need to set it.
    dbtable->setFlavor("ofl");
    
    // Store table to database
    mgr->storeDbTable(dbtable);
}
