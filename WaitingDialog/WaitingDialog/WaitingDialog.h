
// WaitingDialog.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CWaitingDialogApp:
// �йش����ʵ�֣������ WaitingDialog.cpp
//

class CWaitingDialogApp : public CWinApp
{
public:
	CWaitingDialogApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CWaitingDialogApp theApp;