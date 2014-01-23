void write_istOnPst()
{
    // Load all required libraries
    gROOT->Macro("LoadLogger.C");
    gSystem->Load("St_base.so");
    gSystem->Load("libStDb_Tables.so");
    gSystem->Load("StDbLib.so");
    
    // Initialize db manager
    StDbManager* mgr = StDbManager::Instance();
    StDbConfigNode* node = mgr->initConfig("Geometry_ist");
    StDbTable* dbtable = node->addDbTable("istOnPst");
    TString storeTime = "2013-06-03 00:00:01"; // beginTime timestamp in MySQL format: "YYYY-MM-DD HH:mm:ss"
    mgr->setStoreTime(storeTime.Data());
    
    // Create your c-struct
    Survey_st table = {1, 1,0,0, 0,1,0, 0,0,1, 0,0,0.3,.1,.1,.1,.1,.1,.1,"estimation"};
    
    // Store data to the StDbTable
    dbtable->SetTable((char*)&table, 1);
    
    // uncomment next line to set "sim" flavor. "ofl" flavor is set by default, no need to set it.
    dbtable->setFlavor("ofl");
    
    // Store table to database
    mgr->storeDbTable(dbtable);
}
