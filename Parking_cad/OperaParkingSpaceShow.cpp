#include "stdafx.h"
#include "OperaParkingSpaceShow.h"
#include "DBHelper.h"
#include "ArxDialog.h"
#include <fstream>
#include <json/json.h>
#include "DlgWaiting.h"
#include "ArxDialog.h"
#include "FileHelper.h"

class CArxDialog* COperaParkingSpaceShow::ms_dlg = NULL;


COperaParkingSpaceShow::COperaParkingSpaceShow()
{
}


COperaParkingSpaceShow::~COperaParkingSpaceShow()
{
}

void COperaParkingSpaceShow::Start()
{
	//���ļ��ж�ȡ
	std::string sConfigFile = DBHelper::GetArxDirA() + "ParkingConfig.json";
	std::string sConfigStr = FileHelper::ReadText(sConfigFile.c_str());
	Json::Reader reader;
	Json::Value root;
	if (reader.parse(sConfigStr, root))
	{
		std::string postUrl = root["params"]["posturl"].asString();
		std::string getUrl = root["params"]["geturl"].asString();
		CDlgWaiting::setGetUrl(getUrl);
		CArxDialog::setPostUrl(postUrl);
	}
	else
	{
		acedAlert(_T("���������ļ�����"));
		return;
	}

	AcString sTemplateFile = DBHelper::GetArxDir() + _T("template.dwg");
	if (!DBHelper::ImportBlkDef(sTemplateFile, _T("Parking_1")))
	{
		acedAlert(_T("����ģ���ļ�����"));
		return;
	}

	//���ô���
	CAcModuleResourceOverride resOverride;//��Դ��λ

	ms_dlg = new CArxDialog;
	ms_dlg->Create(CArxDialog::IDD, acedGetAcadDwgView());
	ms_dlg->ShowWindow(SW_SHOW);
	m_tol.setEqualPoint(200);
}


REG_CMD(COperaParkingSpaceShow, BGY, ParkingSpaceShow);