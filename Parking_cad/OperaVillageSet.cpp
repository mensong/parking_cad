#include "stdafx.h"
#include "OperaVillageSet.h"
#include "EquipmentroomTool.h"

COperaVillageSet::COperaVillageSet(const AcString& group, const AcString& cmd, const AcString& alias, Adesk::Int32 cmdFlag)
	: CIOperaLog(group, cmd, alias, cmdFlag)
{

}


COperaVillageSet::~COperaVillageSet()
{
}

void COperaVillageSet::Start()
{
	//�õ��豸��

	double villageArea = 0;
	CEquipmentroomTool::getTotalArea(_T("С�������:"), villageArea);
	if (villageArea == 0)
	{
		acutPrintf(_T("\n�������"));
		return;
	}
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

REG_CMD_P(COperaVillageSet, BGY, VillageSet);