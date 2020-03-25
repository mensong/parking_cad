#include "stdafx.h"
#include "OperaEquipmentroomRelated.h"
#include "EquipmentroomTool.h"

#define VentilationroomArea 15000000.0//通风设备房面积


COperaEquipmentroomRelated::COperaEquipmentroomRelated()
{
}


COperaEquipmentroomRelated::~COperaEquipmentroomRelated()
{
}

void COperaEquipmentroomRelated::Start()
{

//通风设备房
	double totalArea = CEquipmentroomTool::getTotalArea(_T("地库总面积:"));
	if (totalArea == 0)
	{
		acutPrintf(_T("\n输入错误！"));
		return;
	}
	int roomCount = totalArea / 4000;
	if (roomCount == 0)
	{
		acutPrintf(_T("\n输入总面积小于4000！"));
	}
	double scaleArea = CEquipmentroomTool::areaScale(VentilationroomArea);
	for (int i = 0; i < roomCount; i++)
	{
		CString count;
		count.Format(_T("%d"), i + 1);
		CString area1Name = _T("进风") + count;
		CString area2Name = _T("排烟") + count;

		CEquipmentroomTool::layerSet();
		double windareaSideLength = 0;
		double limitLength = 3500;
		AcDbObjectIdArray windareaJigUseIds = CEquipmentroomTool::createArea(scaleArea, area1Name, windareaSideLength, limitLength);
		CEquipmentroomTool::setEntToLayer(windareaJigUseIds);
 		if (!CEquipmentroomTool::jigShow(windareaJigUseIds, windareaSideLength))
 		{
			break;
 		}
		AcDbObjectIdArray windarea1JigUseIds = CEquipmentroomTool::createArea(scaleArea, area2Name, windareaSideLength, limitLength);
		CEquipmentroomTool::setEntToLayer(windarea1JigUseIds);
		if (!CEquipmentroomTool::jigShow(windarea1JigUseIds, windareaSideLength))
		{
			break;
		}
		AcDbObjectIdArray windarea2JigUseIds = CEquipmentroomTool::createArea(scaleArea, area2Name, windareaSideLength, limitLength);
		CEquipmentroomTool::setEntToLayer(windarea2JigUseIds);
		if (!CEquipmentroomTool::jigShow(windarea2JigUseIds, windareaSideLength))
		{
			break;
		}
	}
}

REG_CMD(COperaEquipmentroomRelated, BGY, VentilationEquipmentroomSet);