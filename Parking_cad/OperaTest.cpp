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
	WD::Create((DBHelper::GetArxDirA() + "WaitingDialog.exe").c_str());
	WD::SetRange(0, 100);
	WD::AppendMsg(_T("test"));
	int npos = WD::GetPos();
	int mi, ma;
	WD::GetRange(mi, ma);
}

REG_CMD(COperaTest, BGY, mytest);