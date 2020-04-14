#include "stdafx.h"
#include "OperaFirePumpHouseSet.h"
#include "EquipmentroomTool.h"

#define FirePumpHouse 120000000.0//消防泵房
#define IsDecrease false
COperaFirePumpHouseSet::COperaFirePumpHouseSet(const AcString& group, const AcString& cmd, const AcString& alias, Adesk::Int32 cmdFlag)
	: CIOperaLog(group, cmd, alias, cmdFlag)
{
}


COperaFirePumpHouseSet::~COperaFirePumpHouseSet()
{
}

void COperaFirePumpHouseSet::Start()
{ 
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

REG_CMD_P(COperaFirePumpHouseSet, BGY, FirePumpHouseSet);