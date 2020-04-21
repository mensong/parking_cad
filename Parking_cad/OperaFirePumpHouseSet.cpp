#include "stdafx.h"
#include "OperaFirePumpHouseSet.h"
#include "EquipmentroomTool.h"

#define FirePumpHouse 120000000.0//消防泵房
#define IsDecrease false
COperaFirePumpHouseSet::COperaFirePumpHouseSet(const AcString& group, const AcString& cmd, const AcString& alias, Adesk::Int32 cmdFlag)
	: CIOperaLog(group, cmd, alias, cmdFlag)
	, m_holder(NULL)
{
}


COperaFirePumpHouseSet::~COperaFirePumpHouseSet()
{
}

void COperaFirePumpHouseSet::Start()
{ 
	if (g_dlg)
	{
		m_holder = new HideDialogHolder(g_dlg);
	}
	double dfirePumpHouseArea = FirePumpHouse;
	if (IsDecrease)
	{
		dfirePumpHouseArea = FirePumpHouse - 40000000;
	}
	CEquipmentroomTool::layerSet();
	double FirePumpHouseSideLength = 0;
	double limitLength = 5500;
	double dnewArea = CEquipmentroomTool::areaScale(dfirePumpHouseArea);
	if (dnewArea == 0)
		return;
	AcDbObjectIdArray FirePumpHouseJigUseIds = CEquipmentroomTool::createArea(dnewArea, _T("消防泵房"), FirePumpHouseSideLength, limitLength);
	CEquipmentroomTool::setEntToLayer(FirePumpHouseJigUseIds);
	CEquipmentroomTool::jigShow(FirePumpHouseJigUseIds, FirePumpHouseSideLength);
}

CWnd* COperaFirePumpHouseSet::g_dlg = NULL;

void COperaFirePumpHouseSet::Ended()
{
	if (g_dlg && m_holder)
	{
		delete m_holder;
		m_holder = NULL;
	}

	g_dlg = NULL;
}

REG_CMD_P(COperaFirePumpHouseSet, BGY, FirePumpHouseSet);