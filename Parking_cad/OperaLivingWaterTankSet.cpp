#include "stdafx.h"
#include "OperaLivingWaterTankSet.h"
#include "EquipmentroomTool.h"

COperaLivingWaterTankSet::COperaLivingWaterTankSet()
{
}


COperaLivingWaterTankSet::~COperaLivingWaterTankSet()
{
}

void COperaLivingWaterTankSet::Start()
{
	//����ˮ��
	int iNum;
	int nRetu = acedGetInt(_T("������С��������"), &iNum);
	if (nRetu != RTNORM)
	{
		return;
	}
	double LivingWaterTank = ((iNum * 3.2 * 0.25 * 0.2) * 1.2 / 1.9) * 1000000;
	CEquipmentroomTool::layerSet();
	double LivingWaterTankSideLength = 0;
	AcDbObjectIdArray LivingWaterTankJigUseIds = CEquipmentroomTool::createArea(LivingWaterTank, _T("����ˮ��"), LivingWaterTankSideLength);
	CEquipmentroomTool::setEntToLayer(LivingWaterTankJigUseIds);
	CEquipmentroomTool::jigShow(LivingWaterTankJigUseIds, LivingWaterTankSideLength);
}

REG_CMD(COperaLivingWaterTankSet, BGY, LivingWaterTankSet);