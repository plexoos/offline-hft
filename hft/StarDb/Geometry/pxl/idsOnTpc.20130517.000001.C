TDataSet *CreateTable() {
// -----------------------------------------------------------------
// PxlIdsOnTpc Allocated rows: 1  Used rows: 1
//  Table: Survey_st[0]
// ====================================================================
// ------  Test whether this table share library was loaded ------
  if (!gROOT->GetClass("St_Survey")) return 0;
  Survey_st row = {1, 1,0,0, 0,1,0, 0,0,1, 0.0,0.0,0.0, .0001,.0001,.0001,.0001,.0001,.0001,"ideal position"};
  St_Survey *tableSet = new St_Survey("idsOnTpc",1);
  tableSet->AddAt(&row.Id, 0);
  return (TDataSet *)tableSet;
}
