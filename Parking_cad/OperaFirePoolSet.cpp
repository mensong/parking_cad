#include "stdafx.h"
#include "OperaFirePoolSet.h"
#include "EquipmentroomTool.h"

COperaFirePoolSet::COperaFirePoolSet()
{
}


COperaFirePoolSet::~COperaFirePoolSet()
{
}

void COperaFirePoolSet::Start()
{
	double storeyHeight = CEquipmentroomTool::getTotalArea(_T("���:"));
	if (storeyHeight == 0)
	{
		acutPrintf(_T("\n�������"));
		return;
	}
begin:acedInitGet(0, _T("Yes No"));
	ACHAR Input[256];//���鼴��ָ��
	bool isRegardCharging = false;
	int nRet = acedGetKword(_T("\n�Ƿ��ǳ��׮[����(Y)/������(N)]:"), Input);
	if (nRet == RTNORM)
	{
		CString tempStr = Input;
		//_tcscmp(Input, _T("Yes")) == 0//�ж��Ƿ����
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
	double FirePoolS = 0;
	if (isRegardCharging)
	{
		FirePoolS = ((40 * 3.6 * 2 + 80 * 3.6 * 2 + 30 * 3.6 * 1) / (storeyHeight - 2.3)) * 1000000;
	}
	else
	{
		FirePoolS = ((40 * 3.6 * 2 + 20 * 3.6 * 2 + 30 * 3.6 * 1) / (storeyHeight - 2.3)) * 1000000;
	}
	CEquipmentroomTool::layerSet();
	double FirePoolsideLength = 0;
	AcDbObjectIdArray FirePooljigUseIds = CEquipmentroomTool::createArea(FirePoolS/2,_T("����ˮ��"), FirePoolsideLength);
	CEquipmentroomTool::setEntToLayer(FirePooljigUseIds);
	CEquipmentroomTool::jigShow(FirePooljigUseIds, FirePoolsideLength);
}

REG_CMD(COperaFirePoolSet, BGY, FirePoolSet);