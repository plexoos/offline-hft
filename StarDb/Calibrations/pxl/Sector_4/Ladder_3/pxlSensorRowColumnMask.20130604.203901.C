//Sector:4 Ladder:3
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
	row.columnNum[0]=654;
	row.columnNum[1]=850;
	row.rowNum[0]=1;
	row.rowNum[1]=2;
	row.rowNum[2]=3;
	row.rowNum[3]=4;
	row.rowNum[4]=5;
	row.rowNum[5]=6;
	row.rowNum[6]=771;
	tableSet->AddAt(&row);
	//sensor2
	memset(&row,-1,tableSet->GetRowSize());
	row.columnNum[0]=683;
	row.columnNum[1]=704;
	row.rowNum[0]=1;
	row.rowNum[1]=2;
	row.rowNum[2]=3;
	row.rowNum[3]=4;
	row.rowNum[4]=5;
	row.rowNum[5]=6;
	row.rowNum[6]=709;
	tableSet->AddAt(&row);
	//sensor3
	memset(&row,-1,tableSet->GetRowSize());
	row.rowNum[0]=0;
	row.rowNum[1]=1;
	row.rowNum[2]=2;
	row.rowNum[3]=3;
	row.rowNum[4]=4;
	row.rowNum[5]=5;
	row.rowNum[6]=709;
	row.rowNum[7]=771;
	tableSet->AddAt(&row);
	//sensor4
	memset(&row,-1,tableSet->GetRowSize());
	row.rowNum[0]=0;
	row.rowNum[1]=1;
	row.rowNum[2]=2;
	row.rowNum[3]=3;
	row.rowNum[4]=4;
	row.rowNum[5]=709;
	row.rowNum[6]=771;
	tableSet->AddAt(&row);
	//sensor5
	memset(&row,-1,tableSet->GetRowSize());
	row.rowNum[0]=0;
	row.rowNum[1]=1;
	row.rowNum[2]=2;
	row.rowNum[3]=3;
	row.rowNum[4]=4;
	row.rowNum[5]=709;
	row.rowNum[6]=771;
	tableSet->AddAt(&row);
	//sensor6
	memset(&row,-1,tableSet->GetRowSize());
	tableSet->AddAt(&row);
	//sensor7
	memset(&row,-1,tableSet->GetRowSize());
	tableSet->AddAt(&row);
	//sensor8
	memset(&row,-1,tableSet->GetRowSize());
	row.columnNum[0]=459;
	row.columnNum[1]=460;
	row.columnNum[2]=461;
	row.columnNum[3]=480;
	row.columnNum[4]=483;
	row.columnNum[5]=487;
	row.columnNum[6]=559;
	row.columnNum[7]=572;
	row.columnNum[8]=600;
	row.columnNum[9]=605;
	row.columnNum[10]=650;
	row.columnNum[11]=742;
	row.columnNum[12]=814;
	row.columnNum[13]=874;
	row.columnNum[14]=888;
	row.columnNum[15]=914;
	row.columnNum[16]=951;
	row.columnNum[17]=955;
	row.columnNum[18]=958;
	row.columnNum[19]=959;
	tableSet->AddAt(&row);
	//sensor9
	memset(&row,-1,tableSet->GetRowSize());
	row.columnNum[0]=482;
	row.columnNum[1]=493;
	row.rowNum[0]=0;
	row.rowNum[1]=1;
	row.rowNum[2]=2;
	row.rowNum[3]=4;
	row.rowNum[4]=709;
	row.rowNum[5]=771;
	tableSet->AddAt(&row);
	//sensor10
	memset(&row,-1,tableSet->GetRowSize());
	row.columnNum[0]=255;
	row.columnNum[1]=256;
	row.columnNum[2]=507;
	tableSet->AddAt(&row);
	// ----------------- end of code ---------------
	return (TDataSet *)tableSet;
}
