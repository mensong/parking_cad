#include "stdafx.h"
#include "OperaLivingWaterTankSet.h"
#include "EquipmentroomTool.h"

COperaLivingWaterTankSet::COperaLivingWaterTankSet(const AcString& group, const AcString& cmd, const AcString& alias, Adesk::Int32 cmdFlag)
	: CIOperaLog(group, cmd, alias, cmdFlag)
	, m_holder(NULL)
{
}


COperaLivingWaterTankSet::~COperaLivingWaterTankSet()
{
}

void COperaLivingWaterTankSet::Start()
{
	//生活水箱
	if (g_dlg)
	{
		m_holder = new HideDialogHolder(g_dlg);
	}
	int iNum;
	int nRetu = acedGetInt(_T("请输入小区户数："), &iNum);
	if (nRetu != RTNORM)
	{
		return;
	}
	double LivingWaterTank = ((iNum * 3.2 * 0.25 * 0.2) * 2.0 / 1.9) * 1000000;
	CEquipmentroomTool::layerSet();
	double LivingWaterTankSideLength = 0;
	double limitLength = 0;
	AcDbObjectIdArray LivingWaterTankJigUseIds = CEquipmentroomTool::createArea(CEquipmentroomTool::areaScale(LivingWaterTank), _T("生活水箱"), LivingWaterTankSideLength, limitLength);
	CEquipmentroomTool::setEntToLayer(LivingWaterTankJigUseIds);
	CEquipmentroomTool::jigShow(LivingWaterTankJigUseIds, LivingWaterTankSideLength);
}

CWnd* COperaLivingWaterTankSet::g_dlg = NULL;

void COperaLivingWaterTankSet::Ended()
{
	if (g_dlg && m_holder)
	{
		delete m_holder;
		m_holder = NULL;
	}

	g_dlg = NULL;
}

REG_CMD_P(COperaLivingWaterTankSet, BGY, LivingWaterTankSet);