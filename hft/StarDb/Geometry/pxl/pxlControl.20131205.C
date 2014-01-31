TDataSet *CreateTable() {
  // ====================================================================
  // ------  Test whether this table share library was loaded ------
  if (!gROOT->GetClass("St_pxlControl")) return 0;
  pxlControl_st row;
  St_pxlControl *tableSet = new St_pxlControl("pxlControl",1);
  //
  memset(&row,0,tableSet->GetRowSize());
  row.headerLength           = 16;
  row.hardwareIdPosition     = 6;
  row.headerToken            = 0xaaaaaaaa;
  row.separatorToken         = 0xcccccccc;
  row.endToken               = 0xbbbbbbbb;
  row.chipIdStartBit         = 13;
  row.chipIdEndBit           = 16;
  row.chipIdPow              = 8;
  row.overflowBit            = 1;
  row.rowOrColumnFlagBit     = 12;
  row.codingStartBit         = 0;
  row.codingEndBit           = 2;
  row.dataStartBit           = 2;
  row.dataEndBit             = 12; 
  row.dummyState             = 1023;
  row.pixelSize              = 20.7e-4;    
  row.centerOfDiodeZ         = 5.5e-4;
  row.centerOfDiodeX         = 15.1e-4;
  row.sensorGoodStatusMin    = 1;
  row.sensorGoodStatusMax    = 10;
  row.rowColumnGoodStatus    = 1;
  tableSet->AddAt(&row,0);
  // ----------------- end of code ---------------
  return (TDataSet *)tableSet;
}

