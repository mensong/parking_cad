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
	//DBHelper::CallCADCommand(_T("(load \"D:\\自动坡度坡长ssl.lsp\")"));
}

REG_CMD(COperaTest, BGY, mytest);