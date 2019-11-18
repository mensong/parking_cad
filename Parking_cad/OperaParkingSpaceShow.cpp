#include "stdafx.h"
#include "OperaParkingSpaceShow.h"
#include "DBHelper.h"
#include "ArxDialog.h"
#include <fstream>
#include <json/json.h>
#include <iosfwd>
#include "DlgWaiting.h"
#include "ArxDialog.h"

class CArxDialog* COperaParkingSpaceShow::ms_dlg = NULL;


COperaParkingSpaceShow::COperaParkingSpaceShow()
{
}


COperaParkingSpaceShow::~COperaParkingSpaceShow()
{
}

void COperaParkingSpaceShow::Start()
{

	Json::Reader reader;
	Json::Value root;

	//从文件中读取
	std::ifstream is;
	AcString sConfigFile = DBHelper::GetArxDir() + _T("ParkingConfig.json");
	is.open(sConfigFile, std::ios::binary);
	if (!is.is_open())
	{
		acedAlert(_T("加载配置文件出错！"));
		return;	
	}
	if (reader.parse(is, root))
	{
		if (root["params"]["posturl"].isNull()|| root["params"]["geturl"].isNull())
		{
			acedAlert(_T("不存在该字段！"));
			is.close();
			return;
		}
		if (root["params"]["posturl"].isString()&& root["params"]["geturl"].isString())
		{
			std::string postUrl = root["params"]["posturl"].asString();
			std::string getUrl = root["params"]["geturl"].asString();
			CDlgWaiting::setGetUrl(getUrl);
			CArxDialog::setPostUrl(postUrl);
		}
		else
		{
			acedAlert(_T("字段格式不匹配！"));
			is.close();
			return;
		}
	}
	is.close();

	AcString sTemplateFile = DBHelper::GetArxDir() + _T("template.dwg");
	if (!DBHelper::ImportBlkDef(sTemplateFile, _T("Parking_1")))
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


REG_CMD(COperaParkingSpaceShow, BGY, ParkingSpaceShow);