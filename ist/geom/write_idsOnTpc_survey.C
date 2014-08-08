void write_idsOnTpc_survey()
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
    StDbTable* dbtable = node->addDbTable("idsOnTpc");
    TString storeTime = "2014-01-28 00:00:02"; // beginTime timestamp in MySQL format: "YYYY-MM-DD HH:mm:ss"
    //TString storeTime = "2013-12-15 00:00:00";//first entry
    mgr->setStoreTime(storeTime.Data());
    
    // Create your c-struct
    //Survey_st table = {1, 1,0,0, 0,1,0, 0,0,1, 0.0,0.0,0.0, .0001,.0001,.0001,.0001,.0001,.0001,"ideal geometry"}; //first entry
    Survey_st table = {1, 1, 0.000354996, -1.34822e-05, -0.00035497, 0.999998, 0.0018345, 1.41334e-05, -0.00183449, 0.9999980, -0.0419401, 0.0192231, 0.208379, 0.001, 0.001, 0.001, 0.001, 0.001, 0.001, "aligned geometry"};

    // Store data to the StDbTable
    dbtable->SetTable((char*)&table, 1);
    
    // uncomment next line to set "sim" flavor. "ofl" flavor is set by default, no need to set it.
    dbtable->setFlavor("ofl");
    
    // Store table to database
    mgr->storeDbTable(dbtable);
}
