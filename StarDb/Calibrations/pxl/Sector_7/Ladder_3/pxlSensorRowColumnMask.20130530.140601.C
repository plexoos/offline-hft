//Sector:7 Ladder:3
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
	tableSet->AddAt(&row);
	//sensor2
	memset(&row,-1,tableSet->GetRowSize());
	row.columnNum[0]=502;
	row.columnNum[1]=716;
	row.columnNum[2]=874;
	tableSet->AddAt(&row);
	//sensor3
	memset(&row,-1,tableSet->GetRowSize());
	row.columnNum[0]=735;
	tableSet->AddAt(&row);
	//sensor4
	memset(&row,-1,tableSet->GetRowSize());
	row.columnNum[0]=167;
	row.columnNum[1]=435;
	row.columnNum[2]=822;
	tableSet->AddAt(&row);
	//sensor5
	memset(&row,-1,tableSet->GetRowSize());
	row.columnNum[0]=443;
	row.columnNum[1]=471;
	row.columnNum[2]=482;
	row.columnNum[3]=487;
	row.columnNum[4]=499;
	row.columnNum[5]=503;
	row.rowNum[0]=908;
	row.rowNum[1]=909;
	row.rowNum[2]=910;
	row.rowNum[3]=911;
	row.rowNum[4]=913;
	row.rowNum[5]=914;
	row.rowNum[6]=915;
	row.rowNum[7]=916;
	row.rowNum[8]=917;
	row.rowNum[9]=918;
	row.rowNum[10]=919;
	row.rowNum[11]=920;
	row.rowNum[12]=921;
	row.rowNum[13]=922;
	row.rowNum[14]=923;
	row.rowNum[15]=924;
	row.rowNum[16]=925;
	row.rowNum[17]=926;
	row.rowNum[18]=927;
	tableSet->AddAt(&row);
	//sensor6
	memset(&row,-1,tableSet->GetRowSize());
	tableSet->AddAt(&row);
	//sensor7
	memset(&row,-1,tableSet->GetRowSize());
	row.columnNum[0]=74;
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
