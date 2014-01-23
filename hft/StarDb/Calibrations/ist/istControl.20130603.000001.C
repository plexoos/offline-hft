TDataSet *CreateTable() {
  // ====================================================================
  if (!gROOT->GetClass("St_istControl")) return 0;
  istControl_st row;
  St_istControl *tableSet = new St_istControl("istControl",1);
  memset(&row,0,tableSet->GetRowSize());
  row.kIstChanMaxRmsNoiseLevel    = 25.0;
  row.kIstChanMinRmsNoiseLevel    = 12.0;
  row.kIstApvMaxCmNoiseLevel      = 15.0;
  row.kIstPedCutDefault           = 3.0;
  row.kIstHitCutDefault           = 5.0;
  row.kIstCMNCutDefault           = 3.0;
  row.kIstMinNumOfRawHits         = 1;
  row.kIstMaxNumOfRawHits         = 460;
  row.kIstAlldata                 = 0;
  row.kIstADCdata                 = 1;
  row.kIstZSdata                  = 2;
  row.kIstDefaultTimeBin          = 3;
  tableSet->AddAt(&row,0);
  return (TDataSet *)tableSet;
}
