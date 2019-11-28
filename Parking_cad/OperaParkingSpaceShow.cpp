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
	//从文件中读取
	std::string sConfigFile = DBHelper::GetArxDirA() + "ParkingConfig.json";
	std::string sConfigStr = FileHelper::ReadText(sConfigFile.c_str());
	Json::Reader reader;
	Json::Value root;
	if (reader.parse(sConfigStr, root))
	{
		if (root["params"]["posturl"].isNull()|| root["params"]["geturl"].isNull())
		{
			acedAlert(_T("配置文件不存在[\"params\"][\"geturl\"]字段！"));
			return;
		}
		if (root["params"]["posturl"].isString() && root["params"]["geturl"].isString())
		{
			std::string postUrl = root["params"]["posturl"].asString();
			std::string getUrl = root["params"]["geturl"].asString();
			CDlgWaiting::setGetUrl(getUrl);
			CArxDialog::setPostUrl(postUrl);
		}
		else
		{
			acedAlert(_T("配置文件字段格式不匹配！"));
			return;
		}
	}
	else
	{
		acedAlert(_T("加载配置文件出错！"));
		return;
	}

	AcString sTemplateFile = DBHelper::GetArxDir() + _T("template.dwg");
	std::set<AcString> setBlockNames;
	setBlockNames.insert(_T("car_1"));
	if (!DBHelper::ImportBlkDef(sTemplateFile, setBlockNames/*_T("Parking_1")*/))
	{
		acedAlert(_T("加载模板文件出错！"));
		return;
	}

	//设置窗口
	CAcModuleResourceOverride resOverride;//资源定位

	ms_dlg = new CArxDialog;
	ms_dlg->Create(CArxDialog::IDD, acedGetAcadDwgView());
	ms_dlg->ShowWindow(SW_SHOW);
	m_tol.setEqualPoint(200);
}


REG_CMD(COperaParkingSpaceShow, BGY, ParkingSpaceShow);//调用AI计算方案