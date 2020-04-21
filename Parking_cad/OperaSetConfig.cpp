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

std::vector<CString> COperaSetConfig::getACADLINtag()
{
	std::vector<std::string> keyvaluevector;
	CEquipmentroomTool::getJsonInformation("alllinetype", "", "tag", keyvaluevector);
	std::vector<CString> csVector;
	for (int i = 0; i < keyvaluevector.size(); ++i)
	{
		CString cstr(keyvaluevector[i].data());
		csVector.push_back(cstr);
	}

	return csVector;
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
	std::vector<CString> tagvaluevector = COperaSetConfig::getACADLINtag();

	AcString acadlinfilepath = DBHelper::GetArxDir() + _T("ACAD.LIN");
	for (int i = 0; i < tagvaluevector.size(); ++i)
	{
		AcDbObjectId linetypeid;
		Acad::ErrorStatus es = COperaSetConfig::getLineTypeId(linetypeid, tagvaluevector[i], acadlinfilepath);
	}

}

REG_CMD_P(COperaSetConfig, BGY, SetConfig);//设置配置文件参数