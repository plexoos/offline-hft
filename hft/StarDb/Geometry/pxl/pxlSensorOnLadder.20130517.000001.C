TDataSet *CreateTable() 
{ 
  /*
    id = (sector - 1)*40 + (ladder - 1)*10 + sensor
    with 1<=sector<=10
         1<=ladder<=4
         1<=sensor<=10
    id goes from 1 to 400
  */ 
  // Positioning of the PXL wafers in the PXL ladder coordinate systems
  if (!gROOT->GetClass("St_Survey")) return 0;
  Survey_st row = {0, 1,0,0, 0,1,0, 0,0,1,  0,0,0, .1,.1,.1,.002,.1,.002,"Ideal"};
  const Int_t NSECTOR = 10;
  const Int_t NLADDER = 4;
  const Int_t NSENSOR  = 10;
  Float_t ZPOS[NSENSOR] = {-9.1170, -7.0910, -5.0650, -3.0390, -1.0130, 1.0130, 3.0390, 5.0650, 7.0910, 9.1170};
  Int_t id      = 0 ;
  Int_t layer   = 0;
  Int_t counter = 0;
  St_Survey *tableSet = new St_Survey("pxlSensorOnLadder",NSECTOR*NLADDER*NSENSOR);
  for (Int_t sector = 0; sector <NSECTOR; sector++) {
    for (Int_t ladder = 0; ladder <NLADDER; ladder++) {
      for (Int_t sensor = 0; sensor < NSENSOR; sensor++) {
	row.Id = sector*40 + ladder*10 + (sensor+1);
	row.t2 = ZPOS[sensor];
	tableSet->AddAt(&row.Id);
	++counter;
      }
    }
  }
  cout << counter <<" have been added for the PXL in the Rotation matrices list "<< endl;
  return (TDataSet *)tableSet;
}
