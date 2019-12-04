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
		if (root["params"]["posturlone"].isNull()|| root["params"]["geturlone"].isNull())
		{
			acedAlert(_T("�����ļ�������[\"params\"][\"geturl\"]�ֶΣ�"));
			return;
		}
		if (root["params"]["posturlone"].isString() && root["params"]["geturlone"].isString())
		{
			std::string postUrlOne = root["params"]["posturlone"].asString();
			std::string getUrlOne = root["params"]["geturlone"].asString();
			std::string postUrlTwo = root["params"]["posturltwo"].asString();
			std::string getUrlTwo = root["params"]["geturltwo"].asString();
			CDlgWaiting::setGetUrlPortOne(getUrlOne);
			CDlgWaiting::setGetUrlPortTwo(getUrlTwo);
			CArxDialog::setPostUrlPortone(postUrlOne);
			CArxDialog::setPostUrlPorttwo(postUrlTwo);
		}
		else
		{
			acedAlert(_T("�����ļ��ֶθ�ʽ��ƥ�䣡"));
			return;
		}
	}
	else
	{
		acedAlert(_T("���������ļ�����"));
		return;
	}

	AcString sTemplateFile = DBHelper::GetArxDir() + _T("template.dwg");
	std::set<AcString> setBlockNames;
	setBlockNames.insert(_T("car_1"));
	if (!DBHelper::ImportBlkDef(sTemplateFile, setBlockNames/*_T("Parking_1")*/))
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


REG_CMD(COperaParkingSpaceShow, BGY, ParkingSpaceShow);//����AI���㷽��