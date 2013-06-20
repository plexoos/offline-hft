//Sector:7 Ladder:4
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
	row.columnNum[0]=71;
	tableSet->AddAt(&row);
	//sensor2
	memset(&row,-1,tableSet->GetRowSize());
	row.columnNum[0]=354;
	row.rowNum[0]=1;
	row.rowNum[1]=2;
	row.rowNum[2]=3;
	row.rowNum[3]=4;
	row.rowNum[4]=5;
	row.rowNum[5]=6;
	row.rowNum[6]=7;
	row.rowNum[7]=8;
	tableSet->AddAt(&row);
	//sensor3
	memset(&row,-1,tableSet->GetRowSize());
	tableSet->AddAt(&row);
	//sensor4
	memset(&row,-1,tableSet->GetRowSize());
	row.rowNum[0]=0;
	row.rowNum[1]=1;
	row.rowNum[2]=2;
	row.rowNum[3]=3;
	row.rowNum[4]=4;
	row.rowNum[5]=5;
	row.rowNum[6]=6;
	row.rowNum[7]=7;
	tableSet->AddAt(&row);
	//sensor5
	memset(&row,-1,tableSet->GetRowSize());
	row.rowNum[0]=0;
	row.rowNum[1]=1;
	row.rowNum[2]=2;
	row.rowNum[3]=3;
	row.rowNum[4]=4;
	row.rowNum[5]=5;
	row.rowNum[6]=6;
	row.rowNum[7]=7;
	tableSet->AddAt(&row);
	//sensor6
	memset(&row,-1,tableSet->GetRowSize());
	row.rowNum[0]=0;
	row.rowNum[1]=1;
	row.rowNum[2]=2;
	row.rowNum[3]=3;
	row.rowNum[4]=4;
	row.rowNum[5]=5;
	row.rowNum[6]=6;
	tableSet->AddAt(&row);
	//sensor7
	memset(&row,-1,tableSet->GetRowSize());
	row.columnNum[0]=501;
	row.columnNum[1]=551;
	row.columnNum[2]=553;
	row.columnNum[3]=584;
	row.columnNum[4]=613;
	row.columnNum[5]=662;
	row.columnNum[6]=664;
	row.columnNum[7]=665;
	row.columnNum[8]=697;
	row.columnNum[9]=715;
	row.columnNum[10]=719;
	row.columnNum[11]=721;
	row.columnNum[12]=742;
	row.columnNum[13]=846;
	row.columnNum[14]=865;
	row.columnNum[15]=890;
	row.columnNum[16]=959;
	row.rowNum[0]=1;
	row.rowNum[1]=2;
	row.rowNum[2]=3;
	row.rowNum[3]=4;
	row.rowNum[4]=5;
	row.rowNum[5]=6;
	row.rowNum[6]=7;
	row.rowNum[7]=8;
	tableSet->AddAt(&row);
	//sensor8
	memset(&row,-1,tableSet->GetRowSize());
	row.columnNum[0]=679;
	row.rowNum[0]=1;
	row.rowNum[1]=2;
	row.rowNum[2]=3;
	row.rowNum[3]=4;
	row.rowNum[4]=5;
	row.rowNum[5]=6;
	row.rowNum[6]=7;
	tableSet->AddAt(&row);
	//sensor9
	memset(&row,-1,tableSet->GetRowSize());
	row.rowNum[0]=1;
	row.rowNum[1]=2;
	row.rowNum[2]=3;
	row.rowNum[3]=4;
	row.rowNum[4]=5;
	row.rowNum[5]=6;
	row.rowNum[6]=7;
	tableSet->AddAt(&row);
	//sensor10
	memset(&row,-1,tableSet->GetRowSize());
	row.rowNum[0]=722;
	tableSet->AddAt(&row);
	// ----------------- end of code ---------------
	return (TDataSet *)tableSet;
}
