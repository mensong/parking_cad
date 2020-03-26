#include "stdafx.h"
#include "OperaLivingWaterTankSet.h"
#include "EquipmentroomTool.h"

COperaLivingWaterTankSet::COperaLivingWaterTankSet(const AcString& group, const AcString& cmd, const AcString& alias, Adesk::Int32 cmdFlag)
	: CIOperaLog(group, cmd, alias, cmdFlag)
{
}


COperaLivingWaterTankSet::~COperaLivingWaterTankSet()
{
}

void COperaLivingWaterTankSet::Start()
{
	//生活水箱
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

REG_CMD_P(COperaLivingWaterTankSet, BGY, LivingWaterTankSet);