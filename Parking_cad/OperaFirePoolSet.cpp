#include "stdafx.h"
#include "OperaFirePoolSet.h"
#include "EquipmentroomTool.h"
#define IsDecrease false

COperaFirePoolSet::COperaFirePoolSet(const AcString& group, const AcString& cmd, const AcString& alias, Adesk::Int32 cmdFlag)
	: CIOperaLog(group, cmd, alias, cmdFlag)
	, m_holder(NULL)
{
}


COperaFirePoolSet::~COperaFirePoolSet()
{
}

void COperaFirePoolSet::Start()
{
	if (g_dlg)
	{
		m_holder = new HideDialogHolder(g_dlg);
	}
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
		FirePoolS = ((40 * 3.6 * 2 + 20 * 3.6 * 2 + 80 * 3.6 * 1.5) / (storeyHeight - 2.3)) * 1000000;
	}
	else
	{
		FirePoolS = ((40 * 3.6 * 2 + 20 * 3.6 * 2 + 30 * 3.6 * 1) / (storeyHeight - 2.3)) * 1000000;
	}
	if (IsDecrease)
	{
		FirePoolS = FirePoolS - 288000000;
	}
	CEquipmentroomTool::layerSet();
	double FirePoolsideLength = 0;
	double limitLength = 0;
	AcDbObjectIdArray FirePooljigUseIds = CEquipmentroomTool::createArea(CEquipmentroomTool::areaScale(FirePoolS),_T("����ˮ��"), FirePoolsideLength, limitLength);
	CEquipmentroomTool::setEntToLayer(FirePooljigUseIds);
	CEquipmentroomTool::jigShow(FirePooljigUseIds, FirePoolsideLength);
}

CWnd* COperaFirePoolSet::g_dlg = NULL;

void COperaFirePoolSet::Ended()
{
	if (g_dlg && m_holder)
	{
		delete m_holder;
		m_holder = NULL;
	}

	g_dlg = NULL;
}

REG_CMD_P(COperaFirePoolSet, BGY, FirePoolSet);