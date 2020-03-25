#include "stdafx.h"
#include "OperaLifePumpHouseSet.h"
#include "EquipmentroomTool.h"

#define LifePumpHouse 80000000.0//生活泵房面积

COperaLifePumpHouseSet::COperaLifePumpHouseSet(const AcString& group, const AcString& cmd, const AcString& alias, Adesk::Int32 cmdFlag)
	: CIOperaLog(group, cmd, alias, cmdFlag)
{
}


COperaLifePumpHouseSet::~COperaLifePumpHouseSet()
{
}

void COperaLifePumpHouseSet::Start()
{
//生活泵房
	CEquipmentroomTool::layerSet();
	double LifepumpAreaSideLength = 0;
	double limitLength = 4000;
	AcDbObjectIdArray LifepumpAreaJigUseIds = CEquipmentroomTool::createArea(CEquipmentroomTool::areaScale(LifePumpHouse), _T("生活泵房"), LifepumpAreaSideLength, limitLength);
	CEquipmentroomTool::setEntToLayer(LifepumpAreaJigUseIds);
	CEquipmentroomTool::jigShow(LifepumpAreaJigUseIds, LifepumpAreaSideLength);
}
REG_CMD_P(COperaLifePumpHouseSet, BGY, LifePumpHouseSet);