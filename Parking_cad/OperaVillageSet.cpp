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
	//�õ��豸��

	double villageArea = CEquipmentroomTool::getTotalArea(_T("С�������:"));
begin:acedInitGet(0, _T("Yes No"));
	ACHAR Input[256];
	bool isRegardCharging = false;
	int nRet = acedGetKword(_T("\n�Ƿ��ǳ��׮[����(Y)/������(N)]:"), Input);
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
		acutPrintf(_T("\n�û���ȡ����Ĭ�ϲ����ǣ�"));
	}
	else if (nRet == RTNONE)
	{
		acutPrintf(_T("\n����Ϊ���ַ�������������һ�Σ�"));
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
	AcDbObjectIdArray jigUseIds = CEquipmentroomTool::createArea(CEquipmentroomTool::areaScale(distributionRoomS), _T("��緿"), sideLength, limitLength, false);
	CEquipmentroomTool::setEntToLayer(jigUseIds);
	CEquipmentroomTool::jigShow(jigUseIds, sideLength);
}

REG_CMD(COperaVillageSet, BGY, VillageSet);