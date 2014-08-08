//Sector:4 Ladder:2
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
	row.columnNum[0]=212;
	row.rowNum[0]=366;
	row.rowNum[1]=434;
	tableSet->AddAt(&row);
	//sensor2
	memset(&row,-1,tableSet->GetRowSize());
	row.columnNum[0]=649;
	row.columnNum[1]=650;
	row.columnNum[2]=651;
	row.rowNum[0]=434;
	row.rowNum[1]=666;
	tableSet->AddAt(&row);
	//sensor3
	memset(&row,-1,tableSet->GetRowSize());
	row.columnNum[0]=89;
	row.rowNum[0]=366;
	tableSet->AddAt(&row);
	//sensor4
	memset(&row,-1,tableSet->GetRowSize());
	tableSet->AddAt(&row);
	//sensor5
	memset(&row,-1,tableSet->GetRowSize());
	tableSet->AddAt(&row);
	//sensor6
	memset(&row,-1,tableSet->GetRowSize());
	tableSet->AddAt(&row);
	//sensor7
	memset(&row,-1,tableSet->GetRowSize());
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
