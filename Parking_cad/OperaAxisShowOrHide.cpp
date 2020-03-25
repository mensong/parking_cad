#include "stdafx.h"
#include "OperaAxisShowOrHide.h"
#include "DBHelper.h"
#include "EquipmentroomTool.h"


COperaAxisShowOrHide::COperaAxisShowOrHide(const AcString& group, const AcString& cmd, const AcString& alias, Adesk::Int32 cmdFlag)
	: CIOperaLog(group, cmd, alias, cmdFlag)
{
}


COperaAxisShowOrHide::~COperaAxisShowOrHide()
{
}

void COperaAxisShowOrHide::Start()
{
	CString sAxisLayerName(CEquipmentroomTool::getLayerName("parking_axis").c_str());
	if (sAxisLayerName == _T(""))
	{
		acedAlert(_T("获取轴线图层失败！"));
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
	CString sAxisDimLayerName(CEquipmentroomTool::getLayerName("axis_dimensions").c_str());
	if (sAxisDimLayerName == _T(""))
	{
		acedAlert(_T("获取轴网标注图层失败！"));
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

REG_CMD_P(COperaAxisShowOrHide, BGY, AxisShowOrHide);//控制轴线和其对应标注图层显隐