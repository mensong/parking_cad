#include "stdafx.h"
#include "OperaParkingSpaceShow.h"
#include "DBHelper.h"
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

}

bool COperaParkingSpaceShow::WillStart()
{
	//���ô���
	CAcModuleResourceOverride resOverride;//��Դ��λ

	ms_dlg = new CArxDialog;
	ms_dlg->Create(CArxDialog::IDD, acedGetAcadDwgView());
	ms_dlg->ShowWindow(SW_SHOW);
	m_tol.setEqualPoint(200);
	return true;
}

REG_CMD(COperaParkingSpaceShow, BGY, ParkingSpaceShow);