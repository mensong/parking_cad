#include "stdafx.h"
#include "OperaTest.h"
#include "DBHelper.h"


COperaTest::COperaTest()
{
}


COperaTest::~COperaTest()
{
}

void COperaTest::Start()
{
	//DBHelper::CallCADCommand(_T("(load \"D:\\�Զ��¶��³�ssl.lsp\")"));
}

REG_CMD(COperaTest, BGY, mytest);