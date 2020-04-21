#include "stdafx.h"
#include "OperaLifePumpHouseSet.h"
#include "EquipmentroomTool.h"

#define LifePumpHouse 80000000.0//生活泵房面积

COperaLifePumpHouseSet::COperaLifePumpHouseSet(const AcString& group, const AcString& cmd, const AcString& alias, Adesk::Int32 cmdFlag)
	: CIOperaLog(group, cmd, alias, cmdFlag)
	, m_holder(NULL)
{
}


COperaLifePumpHouseSet::~COperaLifePumpHouseSet()
{
}

void COperaLifePumpHouseSet::Start()
{
//生活泵房
	if (g_dlg)
	{
		m_holder = new HideDialogHolder(g_dlg);
	}
	CEquipmentroomTool::layerSet();
	double LifepumpAreaSideLength = 0;
	double limitLength = 4000;
	double dnewArea = CEquipmentroomTool::areaScale(LifePumpHouse);
	if (dnewArea == 0)
		return;
	AcDbObjectIdArray LifepumpAreaJigUseIds = CEquipmentroomTool::createArea(dnewArea, _T("生活泵房"), LifepumpAreaSideLength, limitLength);
	CEquipmentroomTool::setEntToLayer(LifepumpAreaJigUseIds);
	CEquipmentroomTool::jigShow(LifepumpAreaJigUseIds, LifepumpAreaSideLength);
}

CWnd* COperaLifePumpHouseSet::g_dlg = NULL;

void COperaLifePumpHouseSet::Ended()
{
	if (g_dlg && m_holder)
	{
		delete m_holder;
		m_holder = NULL;
	}

	g_dlg = NULL;
}

REG_CMD_P(COperaLifePumpHouseSet, BGY, LifePumpHouseSet);