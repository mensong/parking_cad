#include "stdafx.h"
#include "OperaAxisShowOrHide.h"
#include "DBHelper.h"
#include "EquipmentroomTool.h"


COperaAxisShowOrHide::COperaAxisShowOrHide()
{
}


COperaAxisShowOrHide::~COperaAxisShowOrHide()
{
}

void COperaAxisShowOrHide::Start()
{
	CString sAxisLayerName(CEquipmentroomTool::getLayerName("axislayer").c_str());
	if (sAxisLayerName == _T(""))
	{
		acedAlert(_T("��ȡ����ͼ��ʧ�ܣ�"));
		return;
	}
	if (CEquipmentroomTool::isLayerClosed(sAxisLayerName))
	{
		CEquipmentroomTool::setLayerOpen(sAxisLayerName);

	}
	else
	{
		CEquipmentroomTool::setLayerClose(sAxisLayerName);
	}
	CString sAxisDimLayerName(CEquipmentroomTool::getLayerName("axisdimlayer").c_str());
	if (sAxisDimLayerName == _T(""))
	{
		acedAlert(_T("��ȡ������עͼ��ʧ�ܣ�"));
		return;
	}
	if (CEquipmentroomTool::isLayerClosed(sAxisDimLayerName))
	{
		CEquipmentroomTool::setLayerOpen(sAxisDimLayerName);
	}
	else
	{
		CEquipmentroomTool::setLayerClose(sAxisDimLayerName);
	}
}

REG_CMD(COperaAxisShowOrHide, BGY, AxisShowOrHide);//�������ߺ����Ӧ��עͼ������