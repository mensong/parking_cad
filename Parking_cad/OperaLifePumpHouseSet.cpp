#include "stdafx.h"
#include "OperaLifePumpHouseSet.h"
#include "EquipmentroomTool.h"

#define LifePumpHouse 80000000.0//����÷����

COperaLifePumpHouseSet::COperaLifePumpHouseSet()
{
}


COperaLifePumpHouseSet::~COperaLifePumpHouseSet()
{
}

void COperaLifePumpHouseSet::Start()
{
//����÷�
	CEquipmentroomTool::layerSet();
	double LifepumpAreaSideLength = 0;
	double limitLength = 4000;
	AcDbObjectIdArray LifepumpAreaJigUseIds = CEquipmentroomTool::createArea(CEquipmentroomTool::areaScale(LifePumpHouse), _T("����÷�"), LifepumpAreaSideLength, limitLength);
	CEquipmentroomTool::setEntToLayer(LifepumpAreaJigUseIds);
	CEquipmentroomTool::jigShow(LifepumpAreaJigUseIds, LifepumpAreaSideLength);
}
REG_CMD(COperaLifePumpHouseSet, BGY, LifePumpHouseSet);