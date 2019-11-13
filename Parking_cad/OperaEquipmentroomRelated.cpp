#include "stdafx.h"
#include "OperaEquipmentroomRelated.h"
#include "EquipmentroomTool.h"

#define VentilationroomArea 45000000.0//ͨ���豸�����


COperaEquipmentroomRelated::COperaEquipmentroomRelated()
{
}


COperaEquipmentroomRelated::~COperaEquipmentroomRelated()
{
}

void COperaEquipmentroomRelated::Start()
{

//ͨ���豸��
	double totalArea = CEquipmentroomTool::getTotalArea(_T("�ؿ������:"));
	if (totalArea == 0)
	{
		acutPrintf(_T("\n�������"));
		return;
	}
	int roomCount = totalArea / 4000;
	if (roomCount == 0)
	{
		acutPrintf(_T("\n���������С��4000��"));
	}
	for (int i = 0; i < roomCount; i++)
	{
		CString count;
		count.Format(_T("%d"), i + 1);
		CString area1Name = _T("����") + count;
		CString area2Name = _T("����") + count;

		CEquipmentroomTool::layerSet();
		double windareaSideLength = 0;
		AcDbObjectIdArray windareaJigUseIds = CEquipmentroomTool::createArea(VentilationroomArea, area1Name, windareaSideLength);
		CEquipmentroomTool::setEntToLayer(windareaJigUseIds);
		CEquipmentroomTool::jigShow(windareaJigUseIds, windareaSideLength);
		AcDbObjectIdArray windarea1JigUseIds = CEquipmentroomTool::createArea(VentilationroomArea, area2Name, windareaSideLength);
		CEquipmentroomTool::setEntToLayer(windarea1JigUseIds);
		CEquipmentroomTool::jigShow(windarea1JigUseIds, windareaSideLength);
		AcDbObjectIdArray windarea2JigUseIds = CEquipmentroomTool::createArea(VentilationroomArea, area2Name, windareaSideLength);
		CEquipmentroomTool::setEntToLayer(windarea2JigUseIds);
		CEquipmentroomTool::jigShow(windarea2JigUseIds, windareaSideLength);
	}
}

REG_CMD(COperaEquipmentroomRelated, BGY, VentilationEquipmentroomSet);