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
	MessageCenter::sendMessage("toolbar", false, true);
}

REG_CMD(COperaTest, BGY, mytest);