#include "stdafx.h"
#include "OperaResetEntityLayer.h"

REG_CMD_EX(COperaResetEntityLayer, BGY, ResetEntityLayer, RSENTL, -1);

CDlgResetEntityLayer* COperaResetEntityLayer::ms_pResetLayerDlg = NULL;

COperaResetEntityLayer::COperaResetEntityLayer()
{
}


COperaResetEntityLayer::~COperaResetEntityLayer()
{
}

void COperaResetEntityLayer::Start()
{
	CAcModuleResourceOverride resOverride;

	if (ms_pResetLayerDlg == NULL)
	{
		ms_pResetLayerDlg = new CDlgResetEntityLayer;
		ms_pResetLayerDlg->Create(CDlgResetEntityLayer::IDD, acedGetAcadDwgView());
	}
	ms_pResetLayerDlg->ShowWindow(SW_SHOW);
}
