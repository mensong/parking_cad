#include "stdafx.h"
#include "OperaEquipmentRoomSet.h"
#include "DlgEquipmentRoomSet.h"

class CDlgEquipmentRoomSet* COperaEquipmentRoomSet::ms_eqdlg = NULL;

COperaEquipmentRoomSet::COperaEquipmentRoomSet()
{
}


COperaEquipmentRoomSet::~COperaEquipmentRoomSet()
{
}

void COperaEquipmentRoomSet::Start()
{
	// 设置窗口
	CAcModuleResourceOverride resOverride;//资源定位

	ms_eqdlg = new CDlgEquipmentRoomSet;
	ms_eqdlg->Create(CDlgEquipmentRoomSet::IDD, acedGetAcadDwgView());
	ms_eqdlg->ShowWindow(SW_SHOW);
	m_tol.setEqualPoint(200);
}

REG_CMD(COperaEquipmentRoomSet, BGY, EquipmentRoomSet);