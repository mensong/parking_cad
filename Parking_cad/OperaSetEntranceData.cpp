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
	//���ô���
	CAcModuleResourceOverride resOverride;//��Դ��λ

	ms_EntranceDlg = new CDlgEntrance;
	ms_EntranceDlg->Create(CDlgEntrance::IDD, acedGetAcadDwgView());
	ms_EntranceDlg->ShowWindow(SW_SHOW);
	m_tol.setEqualPoint(200);
}

REG_CMD(COperaSetEntranceData, BGY, SetEntranceData);//���ó���ڲ���