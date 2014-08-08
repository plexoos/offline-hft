//Sector:7 Ladder:2
TDataSet *CreateTable() {
	// ====================================================================
	// number of max entries is 200 
	// for each entry in a ladder, memset is used to clear this array
	// ====================================================================
	if (!TClass::GetClass("St_pxlSensorRowColumnMask")) return 0;
	const int NSENSOR = 10;
	const int MAXSIZE = 200; //to be updated later : 100 may be too big
	pxlSensorRowColumnMask_st row;
	St_pxlSensorRowColumnMask *tableSet = new St_pxlSensorRowColumnMask("pxlSensorRowColumnMask",MAXSIZE);
	//sensor1
	memset(&row,-1,tableSet->GetRowSize());
	row.columnNum[0]=488;
	row.columnNum[1]=490;
	row.columnNum[2]=539;
	row.columnNum[3]=553;
	row.columnNum[4]=661;
	row.columnNum[5]=772;
	row.columnNum[6]=787;
	row.columnNum[7]=843;
	row.columnNum[8]=852;
	row.columnNum[9]=880;
	row.columnNum[10]=885;
	row.columnNum[11]=897;
	row.columnNum[12]=916;
	row.columnNum[13]=934;
	tableSet->AddAt(&row);
	//sensor2
	memset(&row,-1,tableSet->GetRowSize());
	row.columnNum[0]=346;
	row.columnNum[1]=445;
	row.columnNum[2]=527;
	row.columnNum[3]=599;
	row.columnNum[4]=603;
	row.columnNum[5]=605;
	row.columnNum[6]=636;
	row.columnNum[7]=647;
	row.columnNum[8]=655;
	row.columnNum[9]=673;
	row.columnNum[10]=710;
	row.columnNum[11]=755;
	row.columnNum[12]=757;
	row.columnNum[13]=766;
	row.columnNum[14]=767;
	row.columnNum[15]=768;
	row.columnNum[16]=777;
	row.columnNum[17]=824;
	row.columnNum[18]=831;
	row.columnNum[19]=855;
	row.columnNum[20]=890;
	row.columnNum[21]=903;
	row.columnNum[22]=913;
	row.columnNum[23]=942;
	row.columnNum[24]=951;
	row.columnNum[25]=952;
	row.columnNum[26]=954;
	row.columnNum[27]=955;
	row.columnNum[28]=959;
	row.rowNum[0]=6;
	tableSet->AddAt(&row);
	//sensor3
	memset(&row,-1,tableSet->GetRowSize());
	row.columnNum[0]=116;
	row.columnNum[1]=777;
	tableSet->AddAt(&row);
	//sensor4
	memset(&row,-1,tableSet->GetRowSize());
	tableSet->AddAt(&row);
	//sensor5
	memset(&row,-1,tableSet->GetRowSize());
	tableSet->AddAt(&row);
	//sensor6
	memset(&row,-1,tableSet->GetRowSize());
	row.columnNum[0]=286;
	row.columnNum[1]=862;
	tableSet->AddAt(&row);
	//sensor7
	memset(&row,-1,tableSet->GetRowSize());
	row.rowNum[0]=708;
	tableSet->AddAt(&row);
	//sensor8
	memset(&row,-1,tableSet->GetRowSize());
	tableSet->AddAt(&row);
	//sensor9
	memset(&row,-1,tableSet->GetRowSize());
	tableSet->AddAt(&row);
	//sensor10
	memset(&row,-1,tableSet->GetRowSize());
	tableSet->AddAt(&row);
	// ----------------- end of code ---------------
	return (TDataSet *)tableSet;
}
