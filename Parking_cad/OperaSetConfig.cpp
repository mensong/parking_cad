#include "stdafx.h"
#include "OperaSetConfig.h"
#include "DBHelper.h"
#include "EquipmentroomTool.h"

class CDlgSetConfig* COperaSetConfig::ms_SetConfigDlg = NULL;

COperaSetConfig::COperaSetConfig(const AcString& group, const AcString& cmd, const AcString& alias, Adesk::Int32 cmdFlag)
	: CIOperaLog(group, cmd, alias, cmdFlag)
{
}


COperaSetConfig::~COperaSetConfig()
{
}

void COperaSetConfig::Start()
{
	//设置窗口
	CAcModuleResourceOverride resOverride;//资源定位

	ms_SetConfigDlg = new CDlgSetConfig;
	ms_SetConfigDlg->Create(CDlgSetConfig::IDD, acedGetAcadDwgView());
	ms_SetConfigDlg->ShowWindow(SW_SHOW);
	m_tol.setEqualPoint(200);
}

std::vector<CString> COperaSetConfig::getACADLINtag(const AcString& filepath)
{
	/*ACHAR keyvalue[1024];
	memset(keyvalue, 0x00, sizeof(keyvalue));
	DWORD ret = GetPrivateProfileStringW(_T("ACADLIN"), _T("Tag"), NULL, keyvalue, 1024, filepath);
	if (ret == 0)
	{
		return csVector;
	}*/

	std::string keyvalue = CEquipmentroomTool::getJsonInformation("layer_config", "alllinetype", "tag");

	std::vector<CString> csVector;
	CString valuedata(keyvalue.c_str());
	std::vector<std::string> keyvaluevector;
	COperaSetConfig::splitTagValue(valuedata, "|", keyvaluevector);

	for (int i = 0; i < keyvaluevector.size(); ++i)
	{
		CString cstr(keyvaluevector[i].data());
		csVector.push_back(cstr);
	}

	return csVector;
}

void COperaSetConfig::splitTagValue(CString& src, const std::string& separator, std::vector<std::string>& dest)
{
	//参数1：要分割的字符串；参数2：作为分隔符的字符；参数3：存放分割后的字符串的vector向量

	std::string str = CT2A(src.GetBuffer());
	std::string substring;
	std::string::size_type start = 0, index;
	dest.clear();
	index = str.find_first_of(separator, start);
	do
	{
		if (index != std::string::npos)
		{
			substring = str.substr(start, index - start);
			dest.push_back(substring);
			start = index + separator.size();
			index = str.find(separator, start);
			if (start == std::string::npos) break;
		}
	} while (index != std::string::npos);

	//the last part
	substring = str.substr(start);
	dest.push_back(substring);
}

Acad::ErrorStatus COperaSetConfig::getLineTypeId(AcDbObjectId &linetypeid, const AcString& tag, const AcString& acadlinfilepath, AcDbDatabase *pDb /*= acdbCurDwg()*/)
{
	Acad::ErrorStatus es = acdbLoadLineTypeFile(tag, acadlinfilepath, pDb);
	if (es != eOk)
		return es;

	////获取对应id
	//es = COperaSetConfig::getLinetypeIdFromString(tag, linetypeid);
	//if (es != eOk)
	//	return es;

	return es;
}

Acad::ErrorStatus COperaSetConfig::getLinetypeIdFromString(const AcString& tag, AcDbObjectId& id, AcDbDatabase *pDb/* = acdbCurDwg()*/)
{
	Acad::ErrorStatus err;
	// Get the table of currently loaded linetypes. 
	AcDbLinetypeTable *pLinetypeTable;
	err = pDb->getSymbolTable(pLinetypeTable, AcDb::kForRead);
	if (err != Acad::eOk) { return err; }
	// Get the ID of the linetype with the name that 
	// str contains. // 
	err = pLinetypeTable->getAt(tag, id, Adesk::kTrue);

	if (pLinetypeTable)
		pLinetypeTable->close();
	return err;
}


void COperaSetConfig::loadAllLinetype()
{
	AcString filepath = DBHelper::GetArxDir() + _T("getacadlinconfig.ini");
	std::vector<CString> tagvaluevector = COperaSetConfig::getACADLINtag(filepath);

	AcString acadlinfilepath = DBHelper::GetArxDir() + _T("ACAD.LIN");
	for (int i = 0; i < tagvaluevector.size(); ++i)
	{
		AcDbObjectId linetypeid;
		Acad::ErrorStatus es = COperaSetConfig::getLineTypeId(linetypeid, tagvaluevector[i], acadlinfilepath);
	}

}

REG_CMD_P(COperaSetConfig, BGY, SetConfig);//设置配置文件参数