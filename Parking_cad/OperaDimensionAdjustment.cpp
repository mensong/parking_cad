#include "stdafx.h"
#include "OperaDimensionAdjustment.h"

REG_CMD_EX(COperaDimensionAdjustment, BGY, DimensionAdjustment, CODADJIUST, -1);

CDlgDimensionAdjustment* COperaDimensionAdjustment::m_dimadjustDlg = NULL;

COperaDimensionAdjustment::COperaDimensionAdjustment()
{
}


COperaDimensionAdjustment::~COperaDimensionAdjustment()
{
}

void COperaDimensionAdjustment::Start()
{
	if (!m_dimadjustDlg)
	{
		CAcModuleResourceOverride resOverride;//资源定位
		m_dimadjustDlg = new CDlgDimensionAdjustment(acedGetAcadDwgView());
		m_dimadjustDlg->Create(CDlgDimensionAdjustment::IDD, acedGetAcadDwgView());
	}
	m_dimadjustDlg->ShowWindow(SW_SHOW);
}


