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
		acedAlert(_T("获取轴线图层失败！"));
		return;
	}
	if (isLayerClosed(sAxisLayerName))
	{
		setLayerOpen(sAxisLayerName);

	}
	else
	{
		setLayerClose(sAxisLayerName);
	}
	CString sAxisDimLayerName(CEquipmentroomTool::getLayerName("axisdimlayer").c_str());
	if (sAxisDimLayerName == _T(""))
	{
		acedAlert(_T("获取轴网标注图层失败！"));
		return;
	}
	if (isLayerClosed(sAxisDimLayerName))
	{
		setLayerOpen(sAxisDimLayerName);
	}
	else
	{
		setLayerClose(sAxisDimLayerName);
	}
}

void COperaAxisShowOrHide::setLayerClose(const CString& layerName)
{
	AcDbLayerTable *pLayerTbl;
	//获取当前图形层表
	Acad::ErrorStatus es;
	es = acdbCurDwg()->getLayerTable(pLayerTbl, AcDb::kForWrite);
	if (es != eOk)
	{
		return;
	}
	if (pLayerTbl->has(layerName))
	{
		AcDbLayerTableRecord *pLTR = NULL;
		es = pLayerTbl->getAt(layerName, pLTR, AcDb::kForWrite);
		if (es != eOk)
		{
			pLayerTbl->close();
			return;
		}
		DBHelper::SetLayerIsOff(pLTR);
		pLTR->close();
	}
	pLayerTbl->close();
}

bool COperaAxisShowOrHide::isLayerClosed(const CString& strLayerName)
{
	AcDbLayerTable *pLayerTbl;
	bool result = false;
	//获取当前图形层表
	Acad::ErrorStatus es;
	es = acdbCurDwg()->getLayerTable(pLayerTbl, AcDb::kForWrite);
	if (es != eOk)
	{
		return result;
	}
	if (pLayerTbl->has(strLayerName))
	{
		AcDbLayerTableRecord *pLTR = NULL;
		es = pLayerTbl->getAt(strLayerName, pLTR, AcDb::kForRead);
		if (es != eOk)
		{
			pLayerTbl->close();
			return result;
		}
		result = pLTR->isOff();
		pLTR->close();
	}
	pLayerTbl->close();
	return result;
}

void COperaAxisShowOrHide::setLayerOpen(const CString& strLayerName)
{
	AcDbLayerTable *pLayerTbl;
	//获取当前图形层表
	Acad::ErrorStatus es;
	es = acdbCurDwg()->getLayerTable(pLayerTbl, AcDb::kForWrite);
	if (es != eOk)
	{
		return;
	}
	if (pLayerTbl->has(strLayerName))
	{
		AcDbLayerTableRecord *pLTR = NULL;
		es = pLayerTbl->getAt(strLayerName, pLTR, AcDb::kForWrite);
		if (es != eOk)
		{
			pLayerTbl->close();
			return;
		}
		es = pLTR->upgradeOpen();
		pLTR->setIsOff(false);
		es = pLTR->downgradeOpen();
		pLTR->close();
	}
	pLayerTbl->close();
}
REG_CMD(COperaAxisShowOrHide, BGY, AxisShowOrHide);//控制轴线和其对应标注图层显隐