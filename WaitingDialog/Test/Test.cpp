// Test.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include <windows.h>
#include "..\WaitingDialog\WaitingDialogDef.h"
#include <string>

void oneProcess(LONG id)
{
	WD::Create(NULL, id);
	WD::Reset(id);

	WD::SetTitle(_T("Test waiting......"), id);
	WD::SetRange(0, 10, id);
	WD::ShowTime(TRUE, id);

	for (int i=0; i<10; ++i)
	{
		Sleep(1000);
		WD::AppendMsg(_T("������־������־������־������־������־������־������־������־������־������־������־������־"), -1, id);
	}
}

int _tmain(int argc, _TCHAR* argv[])
{
	oneProcess(1);

	
	//WD::Close();

	return 0;
}
