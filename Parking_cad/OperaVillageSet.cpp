#include "stdafx.h"
#include "OperaVillageSet.h"
#include "EquipmentroomTool.h"

COperaVillageSet::COperaVillageSet()
{

}


COperaVillageSet::~COperaVillageSet()
{
}

void COperaVillageSet::Start()
{
	//用电设备房

	double villageArea = CEquipmentroomTool::getTotalArea(_T("小区总面积:"));
begin:acedInitGet(0, _T("Yes No"));
	ACHAR Input[256];
	bool isRegardCharging = false;
	int nRet = acedGetKword(_T("\n是否考虑充电桩[考虑(Y)/不考虑(N)]:"), Input);
	if (nRet == RTNORM)
	{
		CString tempStr = Input;
		if (tempStr == _T("Yes"))
		{
			isRegardCharging = true;
		}
	}
	else if (nRet == RTCAN)
	{
		acutPrintf(_T("\n用户已取消，默认不考虑！"));
	}
	else if (nRet == RTNONE)
	{
		acutPrintf(_T("\n输入为空字符串，请再输入一次！"));
		goto begin;
	}
	double distributionRoomS = 0;
	if (isRegardCharging)
	{
		distributionRoomS = (villageArea / 100000) * 550000000;
	}
	else
	{
		distributionRoomS = (villageArea / 100000) * 450000000;
	}
	CEquipmentroomTool::layerSet();
	double sideLength = 0;
	double limitLength = 250000;
	AcDbObjectIdArray jigUseIds = CEquipmentroomTool::createArea(CEquipmentroomTool::areaScale(distributionRoomS), _T("配电房"), sideLength, limitLength, false);
	CEquipmentroomTool::setEntToLayer(jigUseIds);
	CEquipmentroomTool::jigShow(jigUseIds, sideLength);
}

REG_CMD(COperaVillageSet, BGY, VillageSet);