//Sector:4 Ladder:4
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
	row.rowNum[0]=1;
	tableSet->AddAt(&row);
	//sensor2
	memset(&row,-1,tableSet->GetRowSize());
	row.rowNum[0]=1;
	tableSet->AddAt(&row);
	//sensor3
	memset(&row,-1,tableSet->GetRowSize());
	row.rowNum[0]=0;
	row.rowNum[1]=639;
	row.rowNum[2]=703;
	tableSet->AddAt(&row);
	//sensor4
	memset(&row,-1,tableSet->GetRowSize());
	row.rowNum[0]=0;
	row.rowNum[1]=1;
	tableSet->AddAt(&row);
	//sensor5
	memset(&row,-1,tableSet->GetRowSize());
	row.columnNum[0]=0;
	row.columnNum[1]=1;
	row.columnNum[2]=2;
	row.columnNum[3]=3;
	row.columnNum[4]=4;
	row.columnNum[5]=5;
	row.columnNum[6]=6;
	row.columnNum[7]=7;
	row.columnNum[8]=8;
	row.columnNum[9]=9;
	row.columnNum[10]=10;
	row.columnNum[11]=11;
	row.columnNum[12]=12;
	row.columnNum[13]=13;
	row.columnNum[14]=14;
	row.columnNum[15]=15;
	row.columnNum[16]=16;
	row.columnNum[17]=17;
	row.columnNum[18]=18;
	row.columnNum[19]=19;
	row.columnNum[20]=20;
	row.columnNum[21]=21;
	row.columnNum[22]=22;
	row.columnNum[23]=23;
	row.columnNum[24]=64;
	row.columnNum[25]=65;
	row.columnNum[26]=66;
	row.columnNum[27]=67;
	row.columnNum[28]=68;
	row.columnNum[29]=69;
	row.columnNum[30]=70;
	row.columnNum[31]=71;
	row.columnNum[32]=72;
	row.columnNum[33]=73;
	row.columnNum[34]=74;
	row.columnNum[35]=75;
	tableSet->AddAt(&row);
	//sensor6
	memset(&row,-1,tableSet->GetRowSize());
	row.rowNum[0]=1;
	tableSet->AddAt(&row);
	//sensor7
	memset(&row,-1,tableSet->GetRowSize());
	row.rowNum[0]=1;
	tableSet->AddAt(&row);
	//sensor8
	memset(&row,-1,tableSet->GetRowSize());
	row.rowNum[0]=1;
	tableSet->AddAt(&row);
	//sensor9
	memset(&row,-1,tableSet->GetRowSize());
	row.rowNum[0]=1;
	tableSet->AddAt(&row);
	//sensor10
	memset(&row,-1,tableSet->GetRowSize());
	row.rowNum[0]=1;
	tableSet->AddAt(&row);
	// ----------------- end of code ---------------
	return (TDataSet *)tableSet;
}
