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
#include "DBHelper.h"
#include "Convertor.h"
#include "ModulesManager.h"
#include "AutoRegCmd.h"
#include "CommonFuntion.h"
#include "LoadCuix.h"
#include "DlgBipLogin.h"
#include "Authenticate\HardDiskSerial.h"
#include "KV.h"
#include "ParkingLog.h"

//-----------------------------------------------------------------------------
#define szRDS _RXST("BGY")

//-----------------------------------------------------------------------------
//----- ObjectARX EntryPoint
class CArxProject2App : public AcRxArxApp {

public:
	CArxProject2App () : AcRxArxApp () {}

	virtual AcRx::AppRetCode On_kInitAppMsg (void *pkt) 
	{
		//��ӻ�Ŀ¼
		ModulesManager::Instance().addDir(DBHelper::GetArxDirA());
		//Ԥ����dll����ֹ���ش����dll
		HMODULE h1 = ModulesManager::Instance().loadModule("KV.dll");
		HMODULE h2 = ModulesManager::Instance().loadModule("L09.dll");
		HMODULE h3 = ModulesManager::Instance().loadModule("LibcurlHttp.dll");
		HMODULE h4 = ModulesManager::Instance().loadModule("lib_bipsignin.dll");
#if 0
		TCHAR szModuleName[MAX_PATH];
		HMODULE h = LoadLibraryA("KV.dll");
		::GetModuleFileName(h, szModuleName, MAX_PATH);
		FreeLibrary(h);
		acutPrintf(_T("%s\n"), szModuleName);
		h = LoadLibraryA("L09.dll");
		::GetModuleFileName(h, szModuleName, MAX_PATH);
		FreeLibrary(h);
		acutPrintf(_T("%s\n"), szModuleName);
		h = LoadLibraryA("LibcurlHttp.dll");
		::GetModuleFileName(h, szModuleName, MAX_PATH);
		FreeLibrary(h);
		acutPrintf(_T("%s\n"), szModuleName);
		h = LoadLibraryA("lib_bipsignin.dll");
		::GetModuleFileName(h, szModuleName, MAX_PATH);
		FreeLibrary(h);
		acutPrintf(_T("%s\n"), szModuleName);
#endif
		
		//��ʼ����־
		CParkingLog::Init();

		char serial[MAX_PATH];
		HardDiskSerial::GetSerial(serial, MAX_PATH, 0);
		std::string session = serial;
		KV::Ins().SetStrA("mac", serial);

		CAcModuleResourceOverride resOverride;//��Դ��λ
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

		KV::Ins().SetStrW(_T("bip_id"), dlgLogin.bipId.GetString());//ʹ������ж��Ƿ��������ģ��
		KV::Ins().SetStrW(_T("user_name"), dlgLogin.userName.GetString());
		KV::Ins().SetStrW(_T("group_udid"), dlgLogin.groupUdid.GetString());
		CString sAllow;
		sAllow.Format(_T("%d"), dlgLogin.allow);
		KV::Ins().SetStrW(_T("allow"), sAllow.GetString());
		KV::Ins().SetStrW(_T("descr"), dlgLogin.descr.GetString());
		KV::Ins().SetStrW(_T("reg_time"), dlgLogin.regTime.GetString());
		KV::Ins().SetStrW(_T("last_signin_time"), dlgLogin.lastSigninTime.GetString());
		CString sSigninCount;
		sSigninCount.Format(_T("%d"), dlgLogin.signinCount);
		KV::Ins().SetStrW(_T("signin_count"), sSigninCount.GetString());

		acutPrintf(_T("\n��¼�ɹ����û�����%s\n"), dlgLogin.userName.GetString());

		// You *must* call On_kInitAppMsg here
		AcRx::AppRetCode retCode = AcRxArxApp::On_kInitAppMsg (pkt) ;
		
		AUTO_REG_CMD::Init();
		
		//���ع�����
		AcString filepath = (GetUserDir() + _T("parking_cad.cuix")).GetString();
		LoadCuix::Load(filepath);
		std::string sCheckToobar = GetUserDirA() + "toolbar.shown";
		if (!FileHelper::IsFileExitstA(sCheckToobar))
		{//ֻ����һ��
			LoadCuix::ShowToolbarAsyn(_T("���ܵؿ�"));
			LoadCuix::SetUnloadOnExit(_T("PARKING_CAD"));
			FileHelper::WriteFile(sCheckToobar.c_str(), "1", 1);//���һ���Ѿ����ع�ͼ��
		}
		
		//���ز����		
		DBHelper::CallCADCommandEx(_T("aipaknav"));

		//����cad���ڱ���
		CMDIFrameWnd *pCadWin = acedGetAcadFrame();
		CString sTitle;
		sTitle.Format(_T("���ܵؿ����ϵͳ ��¼:%s"), dlgLogin.userName);
		pCadWin->SetWindowText(sTitle);
		pCadWin->UpdateWindow();
				
		return (retCode) ;
	}

	virtual AcRx::AppRetCode On_kUnloadAppMsg (void *pkt) 
	{
		KV::Ins().DelStrW(_T("bip_id"));
		KV::Ins().DelStrW(_T("user_name"));
		KV::Ins().DelStrW(_T("group_udid"));
		KV::Ins().DelStrW(_T("allow"));
		KV::Ins().DelStrW(_T("descr"));
		KV::Ins().DelStrW(_T("reg_time"));
		KV::Ins().DelStrW(_T("last_signin_time"));
		KV::Ins().DelStrW(_T("signin_count"));

		AUTO_REG_CMD::Clear();
		ModulesManager::Relaese();

		// You *must* call On_kUnloadAppMsg here
		AcRx::AppRetCode retCode =AcRxArxApp::On_kUnloadAppMsg (pkt) ;		

		return (retCode) ;
	}

	virtual void RegisterServerComponents () {
	}	
} ;

//-----------------------------------------------------------------------------
IMPLEMENT_ARX_ENTRYPOINT(CArxProject2App)
