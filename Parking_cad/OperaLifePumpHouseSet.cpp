#include "stdafx.h"
#include "OperaLifePumpHouseSet.h"
#include "EquipmentroomTool.h"

#define LifePumpHouse 100000000.0//����÷����

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
	AcDbObjectIdArray LifepumpAreaJigUseIds = CEquipmentroomTool::createArea(LifePumpHouse, _T("����÷�"), LifepumpAreaSideLength);
	CEquipmentroomTool::setEntToLayer(LifepumpAreaJigUseIds);
	CEquipmentroomTool::jigShow(LifepumpAreaJigUseIds, LifepumpAreaSideLength);
}
REG_CMD(COperaLifePumpHouseSet, BGY, LifePumpHouseSet);