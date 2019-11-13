#include "stdafx.h"
#include "OperaFirePumpHouseSet.h"
#include "EquipmentroomTool.h"

#define FirePumpHouse 120000000.0//消防泵房

COperaFirePumpHouseSet::COperaFirePumpHouseSet()
{
}


COperaFirePumpHouseSet::~COperaFirePumpHouseSet()
{
}

void COperaFirePumpHouseSet::Start()
{
	CEquipmentroomTool::layerSet();
	double FirePumpHouseSideLength = 0;
	AcDbObjectIdArray FirePumpHouseJigUseIds = CEquipmentroomTool::createArea(FirePumpHouse, _T("消防泵房"), FirePumpHouseSideLength);
	CEquipmentroomTool::setEntToLayer(FirePumpHouseJigUseIds);
	CEquipmentroomTool::jigShow(FirePumpHouseJigUseIds, FirePumpHouseSideLength);
}

REG_CMD(COperaFirePumpHouseSet, BGY, FirePumpHouseSet);