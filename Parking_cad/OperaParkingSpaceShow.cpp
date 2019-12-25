#include "stdafx.h"
#include "OperaParkingSpaceShow.h"
#include "DBHelper.h"
#include "ArxDialog.h"
#include <fstream>
#include <json/json.h>
#include "DlgWaiting.h"
#include "ArxDialog.h"
#include "FileHelper.h"
#include "DlgEntrance.h"
#include "Convertor.h"
#include "../DES_CBC_5/DesHelper.h"
#include "../base64/base64.h"
#include "EquipmentroomTool.h"

class CArxDialog* COperaParkingSpaceShow::ms_dlg = NULL;


COperaParkingSpaceShow::COperaParkingSpaceShow()
{
}


COperaParkingSpaceShow::~COperaParkingSpaceShow()
{
}

bool COperaParkingSpaceShow::setDesKey(const std::string& key)
{
	if (m_desKey.size() > 8)
		return false;
	m_desKey = key;
	return true;
}

std::string COperaParkingSpaceShow::decrypt(const std::string& code)
{
	char sDeBase64[2048];
	int nDeBase64Len = 0;
	base64_decode(code.c_str(), code.size(), (unsigned char*)sDeBase64, nDeBase64Len);
	char sDeDes[1024];
	int nDeDesLen = 0;
	GL::DES_cbc_decrypt(sDeBase64, nDeBase64Len, sDeDes, nDeDesLen,
		m_desKey.c_str(), m_desKey.size(), m_desKey.c_str(), m_desKey.size());
	sDeDes[nDeDesLen] = '\0';
	std::string strResultUrl(sDeDes, sDeDes+strlen(sDeDes));
	return strResultUrl;
}

std::string COperaParkingSpaceShow::encrypt(const std::string& sData)
{
	char mmCode[1024];
	int nOutLen = 0;
	GL::DES_cbc_encrypt(sData.c_str(), sData.size(), mmCode, nOutLen, m_desKey.c_str(), m_desKey.size(), m_desKey.c_str(), m_desKey.size());
	char szBase64[1024 * 2];
	int nBase64 = 0;
	base64_encode((const unsigned char*)mmCode, nOutLen, szBase64, nBase64);
	szBase64[nBase64] = '\0';
	std::string code(szBase64, szBase64 + strlen(szBase64));
	return code;
}

void COperaParkingSpaceShow::Start()
{
	setDesKey("#B-G-Y++");
	//std::string sData = "http://10.8.202.199:80/entrance";
	//std::string code = encrypt(sData);
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
			std::string postUrlOne = root["params"]["posturl"].asString();
			std::string getUrlOne = root["params"]["geturl"].asString();
			std::string entrancePostUrl = root["params"]["entrance_posturl"].asString();
			std::string strPostUrl = decrypt(postUrlOne);
			std::string strGetUrl = decrypt(getUrlOne);
			std::string strEntrancePostUrl = decrypt(entrancePostUrl);
			CArxDialog::setPostUrlPortone(strPostUrl);
			CDlgWaiting::setGetUrlPortOne(strGetUrl);
			CDlgEntrance::setEntrancePostUrl(strEntrancePostUrl);
			//std::string postUrlTwo = root["params"]["posturlv2"].asString();
			//std::string getUrlTwo = root["params"]["geturlv2"].asString();
			//std::string strEntrancePostUrlV2 = root["params"]["entrance_posturlv2"].asString();
			//CDlgWaiting::setGetUrlPortTwo(getUrlTwo);
			//CArxDialog::setPostUrlPorttwo(postUrlTwo);	
			//CDlgEntrance::setEntrancePostUrlV2(strEntrancePostUrlV2);
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
	ObjectCollector oc;
	oc.start(acdbCurDwg());
	if (!DBHelper::ImportBlkDef(sTemplateFile, setBlockNames/*_T("Parking_1")*/))
	{
		acedAlert(_T("加载模板文件出错！"));
		return;
	}
	if (oc.m_objsAppended.length() > 0)
	{
		CString sParkingsLayer(CEquipmentroomTool::getLayerName("parkingslayer").c_str());
		CEquipmentroomTool::layerSet(sParkingsLayer, 7);

		for (int i = 0; i < oc.m_objsAppended.length(); i++)
		{
			CEquipmentroomTool::setEntToLayer(oc.m_objsAppended[i], sParkingsLayer);
		}
	}
	//设置窗口
	CAcModuleResourceOverride resOverride;//资源定位

	ms_dlg = new CArxDialog;
	ms_dlg->Create(CArxDialog::IDD, acedGetAcadDwgView());
	ms_dlg->ShowWindow(SW_SHOW);
	m_tol.setEqualPoint(200);
}


REG_CMD(COperaParkingSpaceShow, BGY, ParkingSpaceShow);//调用AI计算方案