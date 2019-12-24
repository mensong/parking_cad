#include "stdafx.h"
#include "OperaSetConfig.h"

class CDlgSetConfig* COperaSetConfig::ms_SetConfigDlg = NULL;
COperaSetConfig::COperaSetConfig()
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

REG_CMD(COperaSetConfig, BGY, SetConfig);//设置配置文件参数