#include "stdafx.h"
#include "OperaSetEntranceData.h"

class CDlgEntrance* COperaSetEntranceData::ms_EntranceDlg = NULL;
COperaSetEntranceData::COperaSetEntranceData()
{
}


COperaSetEntranceData::~COperaSetEntranceData()
{
}

void COperaSetEntranceData::Start()
{
	//设置窗口
	CAcModuleResourceOverride resOverride;//资源定位

	ms_EntranceDlg = new CDlgEntrance;
	ms_EntranceDlg->Create(CDlgEntrance::IDD, acedGetAcadDwgView());
	ms_EntranceDlg->ShowWindow(SW_SHOW);
	m_tol.setEqualPoint(200);
}

REG_CMD(COperaSetEntranceData, BGY, SetEntranceData);//设置出入口参数