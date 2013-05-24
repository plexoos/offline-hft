TDataSet *CreateTable() {
// -----------------------------------------------------------------
// PxlHalfOnPxl Allocated rows: 2  Used rows: 2  
//  Table: Survey_st[0]--> Survey_st[1]
// ====================================================================
// ------  Test whether this table share library was loaded ------
  if (!gROOT->GetClass("St_Survey")) return 0;
  Survey_st row[10] = {
    {1, 1 ,0,0, 0, 1,0, 0,0,1, 0,0,0,.0001,.0001,.0001,.0001,.0001,.0001,"top sector"},
    {2, 1, 0,0, 0, 1,0, 0,0,1, 0,0,0,.0001,.0001,.0001,.0001,.0001,.0001,"top sector"}
  };
  St_Survey *tableSet = new St_Survey("pxlHalfOnPxl",2);
  for (Int_t i = 0; i < 2; i++) tableSet->AddAt(&row[i].Id, i);
  // ----------------- end of code ---------------
  return (TDataSet *)tableSet;
}
