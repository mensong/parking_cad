#include "stdafx.h"
#include "OperaAxisShowOrHide.h"
#include "DBHelper.h"


COperaAxisShowOrHide::COperaAxisShowOrHide()
{
}


COperaAxisShowOrHide::~COperaAxisShowOrHide()
{
}

void COperaAxisShowOrHide::Start()
{
	if (isLayerClosed(_T("axis")))
	{
		setLayerOpen(_T("axis"));
	}
	else
	{
		setLayerClose(_T("axis"));
	}
	if (isLayerClosed(_T("������ע")))
	{
		setLayerOpen(_T("������ע"));
	}
	else
	{
		setLayerClose(_T("������ע"));
	}
}

void COperaAxisShowOrHide::setLayerClose(const CString& layerName)
{
	AcDbLayerTable *pLayerTbl;
	//��ȡ��ǰͼ�β��
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
	//��ȡ��ǰͼ�β��
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
	//��ȡ��ǰͼ�β��
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
REG_CMD(COperaAxisShowOrHide, BGY, AxisShowOrHide);//�������ߺ����Ӧ��עͼ������