// (C) Copyright 2002-2012 by Autodesk, Inc. 
//
// Permission to use, copy, modify, and distribute this software in
// object code form for any purpose and without fee is hereby granted, 
// provided that the above copyright notice appears in all copies and 
// that both that copyright notice and the limited warranty and
// restricted rights notice below appear in all supporting 
// documentation.
//
// AUTODESK PROVIDES THIS PROGRAM "AS IS" AND WITH ALL FAULTS. 
// AUTODESK SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTY OF
// MERCHANTABILITY OR FITNESS FOR A PARTICULAR USE.  AUTODESK, INC. 
// DOES NOT WARRANT THAT THE OPERATION OF THE PROGRAM WILL BE
// UNINTERRUPTED OR ERROR FREE.
//
// Use, duplication, or disclosure by the U.S. Government is subject to 
// restrictions set forth in FAR 52.227-19 (Commercial Computer
// Software - Restricted Rights) and DFAR 252.227-7013(c)(1)(ii)
// (Rights in Technical Data and Computer Software), as applicable.
//

//-----------------------------------------------------------------------------
//----- acrxEntryPoint.cpp
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "resource.h"
#include "DlgWaiting.h"
#include "Authenticate.h"
#include "DBHelper.h"
#include "Convertor.h"
#include "ModulesManager.h"
#include "AutoRegCmd.h"

//-----------------------------------------------------------------------------
#define szRDS _RXST("BGY")

Authenticate g_auth;

//-----------------------------------------------------------------------------
//----- ObjectARX EntryPoint
class CArxProject2App : public AcRxArxApp {

public:
	CArxProject2App () : AcRxArxApp () {}

	virtual AcRx::AppRetCode On_kInitAppMsg (void *pkt) {
		// TODO: Load dependencies here

		//DWORD dwIDESerial;
		//CString str;
		//GetVolumeInformation(_T("E:\\"), NULL, NULL,&dwIDESerial,NULL,NULL,NULL,NULL);
		//str.Format( _T("Ӳ�����к�: %X - %X"),HIWORD(dwIDESerial),LOWORD(dwIDESerial));
		//AfxMessageBox(str);

		//��Ȩ���
		g_auth.setDesKey("#B-G-Y++");
		
		//std::string s = g_auth.localEncode(20191201, "���");
		//Authenticate::LICENSE_INFO li;
		//g_auth.decode(li, "z/6ftPSRnF/a0YVdN0o8SAmROFe2jH9RAbsjKwrDcDo=");

		AcString swFileName = DBHelper::GetArxDir() + _T("license.lst");
		g_auth.loadLicenseFile(GL::WideByte2Ansi(swFileName.constPtr()).c_str());
		int res = g_auth.check();
		if (res == 1)
		{
			MessageBox(NULL, AcString(_T("��ʼ��ʧ�ܣ�")), _T("��ʼ��ʧ��"), MB_OK | MB_ICONERROR);
			return AcRx::kRetError;
		}
		else if (res == 2)
		{
			MessageBox(NULL, AcString(_T("δ��Ȩ�նˣ�����ϵ����Ա��")), _T("δ��Ȩ"), MB_OK | MB_ICONERROR);
			return AcRx::kRetError;
		}
		else if (res == 3)
		{
			MessageBox(NULL, AcString(_T("��Ȩ�ѹ��ڣ�����ϵ����Ա��")), _T("��Ȩ����"), MB_OK | MB_ICONERROR);
			return AcRx::kRetError;
		}
		else if (res == 4)
		{
			MessageBox(NULL, AcString(_T("�û���Ϣ��������ϵ����Ա��")), _T("�û���Ϣ����"), MB_OK | MB_ICONERROR);
			return AcRx::kRetError;
		}

		std::string user = g_auth.getCheckedUser();
		std::string serial = g_auth.getCheckedSerial();
		std::string license = g_auth.getCheckedLicenceCode();
		DWORD		expireTime = g_auth.getCheckedExpireTime();
		acutPrintf(_T("\n��Ȩ��Ϣ - ���û���:%s  ����ʱ��:%u��\n"), GL::Ansi2WideByte(user.c_str()).c_str(), expireTime);

		// You *must* call On_kInitAppMsg here
		AcRx::AppRetCode retCode = AcRxArxApp::On_kInitAppMsg (pkt) ;
		
		// TODO: Add your initialization code here
		AUTO_REG_CMD::Init();

		return (retCode) ;
	}

	virtual AcRx::AppRetCode On_kUnloadAppMsg (void *pkt) {
		// TODO: Add your code here

		// You *must* call On_kUnloadAppMsg here
		AcRx::AppRetCode retCode =AcRxArxApp::On_kUnloadAppMsg (pkt) ;

		// TODO: Unload dependencies here
		CDlgWaiting::Destroy();
		ModulesManager::Relaese();
		AUTO_REG_CMD::Clear();

		return (retCode) ;
	}

	virtual void RegisterServerComponents () {
	}
	
} ;

//-----------------------------------------------------------------------------
IMPLEMENT_ARX_ENTRYPOINT(CArxProject2App)
