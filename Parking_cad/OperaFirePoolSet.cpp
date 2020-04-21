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
	double storeyHeight = CEquipmentroomTool::getTotalArea(_T("层高:"));
	if (storeyHeight == 0)
	{
		acutPrintf(_T("\n输入错误！"));
		return;
	}
begin:acedInitGet(0, _T("Yes No"));
	ACHAR Input[256];//数组即是指针
	bool isRegardCharging = false;
	int nRet = acedGetKword(_T("\n是否考虑充电桩[考虑(Y)/不考虑(N)]:"), Input);
	if (nRet == RTNORM)
	{
		CString tempStr = Input;
		//_tcscmp(Input, _T("Yes")) == 0//判断是否相等
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
	AcDbObjectIdArray FirePooljigUseIds = CEquipmentroomTool::createArea(CEquipmentroomTool::areaScale(FirePoolS),_T("消防水池"), FirePoolsideLength, limitLength);
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