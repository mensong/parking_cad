// Test.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <windows.h>
#include "..\WaitingDialog\WaitingDialogDef.h"
#include <string>

void oneProcess(LONG id)
{
	RECT rc;
	rc.top = 0;
	rc.left = 0;
	rc.right = -1;
	rc.bottom = -1;
	WD::Create(NULL, id, &rc);
	WD::Reset(id);

	WD::SetTitle(_T("Test waiting......"), id);
	WD::SetRange(0, 10, id);
	WD::ShowTime(TRUE, id);

	for (int i=0; i<10; ++i)
	{
		Sleep(1000);
		WD::AppendMsg(_T("这是日志这是日志这是日志这是日志这是日志这是日志这是日志这是日志这是日志这是日志这是日志这是日志"), -1, id);
	}
}

int _tmain(int argc, _TCHAR* argv[])
{
	oneProcess(1);

	
	//WD::Close();

	return 0;
}

