TDataSet *CreateTable() 
{ 
  /*
    id = 1000 + (ladder - 1)*6 + sensor
    with 1<=ladder<=24
         1<=sensor<=6
    id goes from 1001 to 1144
  */ 
  // Positioning of the IST sensors in the IST ladder coordinate systems
  if (!gROOT->GetClass("St_Survey")) return 0;
  Survey_st row = {0, 1,0,0, 0,1,0, 0,0,1,  0,0,0, .1,.1,.1,.002,.1,.002,"Ideal"};
  const Int_t NLADDER  = 24;
  const Int_t NSENSOR  = 6;
  Float_t ZPOS[NSENSOR] = {-19.3250, -11.5950, -3.8650, 3.8650, 11.5950, 19.3250};
  Int_t id      = 0 ;
  Int_t layer   = 0;
  Int_t counter = 0;
  St_Survey *tableSet = new St_Survey("istSensorOnLadder",NLADDER*NSENSOR);
  for (Int_t ladder = 0; ladder <NLADDER; ladder++) {
      for (Int_t sensor = 0; sensor < NSENSOR; sensor++) {
	row.Id = 1000 + ladder*6 + (sensor+1);
	row.t2 = ZPOS[sensor];
	tableSet->AddAt(&row.Id);
	++counter;
      }
  }
  cout << counter <<" have been added for the IST in the Rotation matrices list "<< endl;
  return (TDataSet *)tableSet;
}
