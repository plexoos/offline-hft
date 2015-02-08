TDataSet *CreateTable() {
  if (!gROOT->GetClass("St_ssdHotChips")) return 0;
  ssdHotChips_st row;
  St_ssdHotChips *tableSet = new St_ssdHotChips("ssdHotChips",1);
  //row.hotChip[0]=1;   //L0W0C1P
  //row.hotChip[1]=6;   //L0W0C6P
  //row.hotChip[2]=7;   //L0W1C1P
  //row.hotChip[3]=96;  //L0W15C6P  
  //row.hotChip[4]=97;  //L1W0C1P  
  //row.hotChip[5]=2037; //L0W6C1N  
  // tempo
  for(int i=0;i<3840;++i) row.hotChip[i] = 0;
  // ----------------- end of code ---------------
  tableSet->AddAt(&row,0);
  return (TDataSet *)tableSet;
}
