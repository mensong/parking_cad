#include "stdafx.h"
#include "OperaTest.h"


COperaTest::COperaTest()
{
}


COperaTest::~COperaTest()
{
}

void COperaTest::Start()
{
	AfxMessageBox(_T("mytest"));
}

REG_CMD(COperaTest, BGY, mytest);