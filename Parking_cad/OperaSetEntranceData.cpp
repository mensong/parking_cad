#include "stdafx.h"
#include "OperaSetEntranceData.h"
#include "DBHelper.h"


class CDlgEntrance* COperaSetEntranceData::ms_EntranceDlg = NULL;
COperaSetEntranceData::COperaSetEntranceData(const AcString& group, const AcString& cmd, const AcString& alias, Adesk::Int32 cmdFlag)
	: CIOperaLog(group, cmd, alias, cmdFlag)
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


REG_CMD_P(COperaSetEntranceData, BGY, SetEntranceData);//���ó���ڲ���
