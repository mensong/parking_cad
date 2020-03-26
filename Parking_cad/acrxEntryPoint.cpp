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
#include "Authenticate\Authenticate.h"
#include "DBHelper.h"
#include "Convertor.h"
#include "ModulesManager.h"
#include "AutoRegCmd.h"
#include "CommonFuntion.h"
#include "LoadCuix.h"
#include "DlgBipLogin.h"

//-----------------------------------------------------------------------------
#define szRDS _RXST("BGY")
#define DES_KEY "#B-G-Y++"
#define TIME_URL "http://licence.orzgod.com:8090/cur_time"

Authenticate g_auth;

//-----------------------------------------------------------------------------
//----- ObjectARX EntryPoint
class CArxProject2App : public AcRxArxApp {

public:
	CArxProject2App () : AcRxArxApp () {}

	virtual AcRx::AppRetCode On_kInitAppMsg (void *pkt) 
	{
		// TODO: Load dependencies here
		ModulesManager::Instance().addDir(DBHelper::GetArxDirA());

		////��Ȩ���
		//g_auth.setDesKey(DES_KEY);

		//DWORD nowTime = 0;
		//if (AcRx::kRetOK != getLicenceServerTime(g_auth, nowTime))
		//	return AcRx::kRetError;

		//if (AcRx::kRetOK != checkLicence(g_auth, nowTime))
		//	return AcRx::kRetError;

		CDlgBipLogin dlgLogin;
		if (dlgLogin.DoModal() != IDOK)
		{
			ModulesManager::Relaese();
			return AcRx::kRetError;
		}
		if (!dlgLogin.loginSuccess)
		{
			ModulesManager::Relaese();
			return AcRx::kRetError;
		}
		acutPrintf(_T("\n��¼�ɹ����û�����%s\n"), dlgLogin.userName.GetString());

		// You *must* call On_kInitAppMsg here
		AcRx::AppRetCode retCode = AcRxArxApp::On_kInitAppMsg (pkt) ;
		
		// TODO: Add your initialization code here
		AUTO_REG_CMD::Init();
		
		AcString filepath = DBHelper::GetArxDir() + _T("parking_cad.cuix");
		LoadCuix::Load(filepath);
		LoadCuix::ShowToolbarAsyn(_T("���ܵؿ�"));
		LoadCuix::SetUnloadOnExit(_T("PARKING_CAD"));

		return (retCode) ;
	}

	virtual AcRx::AppRetCode On_kUnloadAppMsg (void *pkt) 
	{
		AUTO_REG_CMD::Clear();
		ModulesManager::Relaese();

		// You *must* call On_kUnloadAppMsg here
		AcRx::AppRetCode retCode =AcRxArxApp::On_kUnloadAppMsg (pkt) ;		

		return (retCode) ;
	}

	virtual void RegisterServerComponents () {
	}

	AcRx::AppRetCode getLicenceServerTime(Authenticate& auth, DWORD& nowTime)
	{
		typedef int(*FN_get)(const char* url, bool dealRedirect /*= true*/);
		FN_get get = ModulesManager::Instance().func<FN_get>("LibcurlHttp.dll", "get");
		if (!get)
		{
			MessageBox(NULL, AcString(_T("ȱ��LibcurlHttp.dllģ�飡")), _T("ȱ��ģ��"), MB_OK | MB_ICONERROR);
			return AcRx::kRetError;
		}
		typedef const char* (*FN_getBody)(int& len);
		FN_getBody getBody = ModulesManager::Instance().func<FN_getBody>("LibcurlHttp.dll", "getBody");
		if (!getBody)
		{
			MessageBox(NULL, AcString(_T("ȱ��LibcurlHttp.dllģ�飡")), _T("ȱ��ģ��"), MB_OK | MB_ICONERROR);
			return AcRx::kRetError;
		}
		int code = get(TIME_URL, true);
		if (code != 200)
		{
			MessageBox(NULL, AcString(_T("���ӵ�������ʧ�ܣ�")), _T("��·����"), MB_OK | MB_ICONERROR);
			return AcRx::kRetError;
		}
		int nLen = 0;
		const char* pBody = getBody(nLen);
		if (nLen <= 0)
		{
			MessageBox(NULL, AcString(_T("���ӵ�������ʧ�ܣ�")), _T("��·����"), MB_OK | MB_ICONERROR);
			return AcRx::kRetError;
		}
		std::string sNowTime(pBody, nLen);

		nowTime = auth.decodeCurTime(sNowTime);
		if (nowTime == 0)
		{
			MessageBox(NULL, AcString(_T("ʱ�����������")), _T("����������"), MB_OK | MB_ICONERROR);
			return AcRx::kRetError;
		}

		return AcRx::kRetOK;
	}

	AcRx::AppRetCode checkLicence(Authenticate& auth, DWORD nowTime)
	{
		AcString swFileName = DBHelper::GetArxDir() + _T("license.lst");
		auth.loadLicenseFile(GL::WideByte2Ansi(swFileName.constPtr()).c_str());
		int res = auth.check(nowTime);

		std::string user = auth.getCheckedUser();
		std::string serial = auth.getCheckedSerial();
		std::string license = auth.getCheckedLicenceCode();
		DWORD		expireTime = auth.getCheckedExpireTime();
		acutPrintf(_T("\n��Ȩ��Ϣ - ���û���:%s  ����ʱ��:%u��\n"), GL::Ansi2WideByte(user.c_str()).c_str(), expireTime);

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

		return AcRx::kRetOK;
	}
	
} ;

//-----------------------------------------------------------------------------
IMPLEMENT_ARX_ENTRYPOINT(CArxProject2App)
