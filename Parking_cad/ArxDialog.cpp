// (C) Copyright 2002-2007 by Autodesk, Inc. 
//
// Permission to use, copy, modify, and distribute this software in
// object code form for any purpose and without fee is hereby granted, 
// provided that the above copyright notice appears in all copies and 
// that both that copyright notice and the limited warranty and
// restricted rights notice below appear in all supporting 
// documentation.
//
// AUTODESK PROVIDES THIS PROGRAM "AS IS" AND WITH ALL FAULTS. 
// AUTODESK SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTY OF
// MERCHANTABILITY OR FITNESS FOR A PARTICULAR USE.  AUTODESK, INC. 
// DOES NOT WARRANT THAT THE OPERATION OF THE PROGRAM WILL BE
// UNINTERRUPTED OR ERROR FREE.
//
// Use, duplication, or disclosure by the U.S. Government is subject to 
// restrictions set forth in FAR 52.227-19 (Commercial Computer
// Software - Restricted Rights) and DFAR 252.227-7013(c)(1)(ii)
// (Rights in Technical Data and Computer Software), as applicable.
//

//-----------------------------------------------------------------------------
//----- ArxDialog.cpp : Implementation of CArxDialog
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "resource.h"
#include "ArxDialog.h"
#include "DBHelper.h"
#include "GeHelper.h"

#include <fstream>
#include <iostream>
#include <json/json.h>
#include "DlgWaiting.h"
#include "Convertor.h"
#include "ModulesManager.h"
#include "OperaParkingSpaceShow.h"

#ifndef _ttof
#ifdef UNICODE
#define _ttof _wtof
#else
#define _ttof atof
#endif
#endif

//-----------------------------------------------------------------------------
IMPLEMENT_DYNAMIC (CArxDialog, CAcUiDialog)

BEGIN_MESSAGE_MAP(CArxDialog, CAcUiDialog)
	ON_MESSAGE(WM_ACAD_KEEPFOCUS, OnAcadKeepFocus)
	ON_BN_CLICKED(IDC_BUTTON_GetRetreatline, &CArxDialog::OnBnClickedButtonGetretreatline)
	ON_BN_CLICKED(IDC_BUTTON_GetStartPoint, &CArxDialog::OnBnClickedButtonGetstartpoint)
	ON_BN_CLICKED(IDOK, &CArxDialog::OnBnClickedOk)
	ON_BN_CLICKED(IDC_RADIO_OtherLength, &CArxDialog::OnBnClickedRadioOtherlength)
	ON_BN_CLICKED(IDC_RADIO_Default, &CArxDialog::OnBnClickedRadioDefault)
	ON_WM_TIMER()
END_MESSAGE_MAP()

//-----------------------------------------------------------------------------
CArxDialog::CArxDialog (CWnd *pParent /*=NULL*/, HINSTANCE hInstance /*=NULL*/) : CAcUiDialog (CArxDialog::IDD, pParent, hInstance) {
}

CArxDialog::~CArxDialog(){

	COperaParkingSpaceShow::ms_dlg = NULL;
}

//-----------------------------------------------------------------------------
void CArxDialog::loadoutlineLayers()
{
	Doc_Locker _locker;	

	AcDbLayerTable* pLT = NULL;
	if (acdbCurDwg()->getLayerTable(pLT, AcDb::kForRead) != Acad::eOk)
		return;

	do
	{
		AcDbLayerTableIterator* pLTIter = NULL;
		if (pLT->newIterator(pLTIter) != Acad::eOk)
			break;

		pLTIter->setSkipHidden(true);
		for (pLTIter->start(); !pLTIter->done(); pLTIter->step())
		{
			AcDbLayerTableRecord* pLTR = NULL;
			if (pLTIter->getRecord(pLTR, AcDb::kForRead) != Acad::eOk)
				continue;

			AcString name;
			pLTR->getName(name);
			m_outlineLayer.AddString(name);

			pLTR->close();
		}

		delete pLTIter;
	} while (0);
	if (pLT)
		pLT->close();

	if (!m_sOutlineLayer.IsEmpty())
	{
		int n = m_outlineLayer.FindStringExact(0, m_sOutlineLayer);
		if (n >= 0)
			m_outlineLayer.SetCurSel(n);
	}
}
void CArxDialog::loadshearwallLayers()
{
	Doc_Locker _locker;

	AcDbLayerTable* pLT = NULL;
	if (acdbCurDwg()->getLayerTable(pLT, AcDb::kForRead) != Acad::eOk)
		return;

	do
	{
		AcDbLayerTableIterator* pLTIter = NULL;
		if (pLT->newIterator(pLTIter) != Acad::eOk)
			break;

		pLTIter->setSkipHidden(true);
		for (pLTIter->start(); !pLTIter->done(); pLTIter->step())
		{
			AcDbLayerTableRecord* pLTR = NULL;
			if (pLTIter->getRecord(pLTR, AcDb::kForRead) != Acad::eOk)
				continue;

			AcString name;
			pLTR->getName(name);
			m_shearwallLayer.AddString(name);

			pLTR->close();
		}

		delete pLTIter;
	} while (0);
	if (pLT)
		pLT->close();

	if (!m_sShearwallLayer.IsEmpty())
	{
		int n = m_shearwallLayer.FindStringExact(0, m_sShearwallLayer);
		if (n >= 0)
			m_shearwallLayer.SetCurSel(n);
	}
}
void CArxDialog::loaddirectionCombo()
{
	int nRow = m_directionCombo.AddString(_T("垂直排布"));
	m_directionCombo.SetItemData(nRow, 1);

	nRow = m_directionCombo.AddString(_T("水平排布"));
	m_directionCombo.SetItemData(nRow, 0);
	m_directionCombo.SetCurSel(nRow);

	if (m_nDirectionCombo != -1)
	{
		for (int i = 0; i < m_directionCombo.GetCount(); ++i)
		{
			if (m_directionCombo.GetItemData(i) == m_nDirectionCombo)
				m_directionCombo.SetCurSel(i);
		}
	}
}


//-----------------------------------------------------------------------------
void CArxDialog::DoDataExchange (CDataExchange *pDX) {
	CAcUiDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_OutlineLayer, m_outlineLayer);
	DDX_Control(pDX, IDC_COMBO_ShearwallLayer, m_shearwallLayer);
	DDX_Control(pDX, IDC_COMBO_Direction, m_directionCombo);
	DDX_Control(pDX, IDC_BUTTON_GetStartPoint, m_btnGetStartPoint);
	DDX_Control(pDX, IDC_BUTTON_GetRetreatline, m_btnGetRetreatLine);
	DDX_Control(pDX, IDC_EDIT_StartPoint, m_editStartPoint);
	DDX_Control(pDX, IDC_EDIT_RetreatLine, m_editRetreatLine);
	DDX_Control(pDX, IDC_EDIT_Length, m_editLength);
	DDX_Control(pDX, IDC_EDIT_WIDTH, m_Width);
	DDX_Control(pDX, IDC_EDIT_LaneWidth, m_LaneWidth);
	DDX_Control(pDX, IDC_EDIT_SquarecolumnLength, m_SquarcolumnLength);
	DDX_Control(pDX, IDC_EDIT_SquareColumnWidth, m_SquareColumnWidth);
	DDX_Control(pDX, IDC_CHECK_Partition, m_checkPartition);
}

void CArxDialog::OnOK()
{
	CAcUiDialog::OnOK();

	this->DestroyWindow();
	delete this;
}

void CArxDialog::OnCancel()
{
	CAcUiDialog::OnOK();

	this->DestroyWindow();
	delete this;
}

//-----------------------------------------------------------------------------
//----- Needed for modeless dialogs to keep focus.
//----- Return FALSE to not keep the focus, return TRUE to keep the focus
LRESULT CArxDialog::OnAcadKeepFocus (WPARAM, LPARAM) {
	return (TRUE) ;
}

BOOL CArxDialog::OnInitDialog()
{
	// TODO:  在此添加额外的初始化
	BOOL bRet = CAcUiDialog::OnInitDialog();

	loadoutlineLayers();
	loadshearwallLayers();
	loaddirectionCombo();

	int nIdxLayer = m_outlineLayer.FindStringExact(0, m_sOutlineLayer);
	if (nIdxLayer > -1)
		m_outlineLayer.SetCurSel(nIdxLayer);
	m_outlineLayer.SetDroppedWidth(200);

	int mIdxLayer = m_shearwallLayer.FindStringExact(0, m_sShearwallLayer);
	if (mIdxLayer > -1)
		m_shearwallLayer.SetCurSel(mIdxLayer);
	m_shearwallLayer.SetDroppedWidth(200);
	
	
	/*m_strLength = "5.3";
	m_editLength.SetWindowText(m_strLength);*/
	setInitData();
	((CButton*)GetDlgItem(IDC_RADIO_Default))->SetCheck(BST_CHECKED);//程序启动时默认为单选按钮1选中

	GetDlgItem(IDC_EDIT_Length)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_STATIC_m)->ShowWindow(SW_HIDE);
	return bRet;
}

void CArxDialog::OnBnClickedButtonGetretreatline()
{
	// TODO: 在此添加控件通知处理程序代码
	HideDialogHolder holder(this);
	Doc_Locker doc_locker;

	ads_name ename; ads_point pt;
	if (acedEntSel(_T("\n请选择选择多段线:"), ename, pt) != RTNORM)
	{
		return;
	}
	AcDbObjectId id;
	acdbGetObjectId(id, ename);
	AcDbEntity *pEnt;
	//下面语句需要判断操作成功与否
	acdbOpenObject(pEnt, id, AcDb::kForRead);
	std::vector<AcGePoint2d> allPoints;//得到的所有点
	if (pEnt->isKindOf(AcDbPolyline::desc()))
	{
		AcDbVoidPtrArray entsTempArray;
		AcDbPolyline *pPline = AcDbPolyline::cast(pEnt);
		AcGeLineSeg2d line;
		AcGeCircArc3d arc;
		int n = pPline->numVerts();
		for (int i = 0; i < n; i++)
		{
			if (pPline->segType(i) == AcDbPolyline::kLine)
			{
				pPline->getLineSegAt(i, line);
				AcGePoint2d startPoint;
				AcGePoint2d endPoint;
				startPoint = line.startPoint();
				endPoint = line.endPoint();
				//acutPrintf(_T("\n该线段起始点坐标为%.2f,%.2f"), startPoint.x, startPoint.y);
				//acutPrintf(_T("\n该线段终止点坐标为%.2f,%.2f"), endPoint.x, endPoint.y);
				allPoints.push_back(startPoint);
				allPoints.push_back(endPoint);
			}
			else if (pPline->segType(i) == AcDbPolyline::kArc)
			{
				pPline->getArcSegAt(i, arc);
				AcGePoint3dArray result = GeHelper::CalcArcFittingPoints(arc, 3);
				for (int x = 0;x<result.length();x++)
				{
					AcGePoint2d onArcpoint(result[x].x, result[x].y);
					//acutPrintf(_T("\n该点%d坐标为%.2f,%.2f"), x, onArcpoint.x, onArcpoint.y);
					allPoints.push_back(onArcpoint);
				}
			}
		}
		
		//acutPrintf(_T("\n该容器长度为%d"), s);
						
		for (int x = 0;x<allPoints.size();x++)
		{
			if (compare(GetretreatlinePts,allPoints[x]))
			{
				continue;
			}
			GetretreatlinePts.append(allPoints[x]);
		}

		//检测闭合
		if (GetretreatlinePts.length() > 2 && GetretreatlinePts[GetretreatlinePts.length() - 1] != allPoints[0])
			GetretreatlinePts.append(GetretreatlinePts[0]);

		//测试代码
		/*int s = GetretreatlinePts.length();
		for (int y = 0; y < GetretreatlinePts.length(); y++)
		{
			acutPrintf(_T("\n第%d点坐标为%.2f,%.2f"), y, GetretreatlinePts[y].x, GetretreatlinePts[y].y);
		}*/
	}
	pEnt->close();

	for (int yy = 0;yy<GetretreatlinePts.length();yy++)
	{
		CString tempStr_X;
		CString tempStr_Y;
		tempStr_X.Format(_T("%.2f"), GetretreatlinePts[yy].x);
		tempStr_Y.Format(_T("%.2f"), GetretreatlinePts[yy].y);

		m_StrRetreatLine += CString(_T("(")) + tempStr_X + _T(",") + tempStr_Y + _T(")");
		m_editRetreatLine.SetWindowText(m_StrRetreatLine);
	}
}


bool CArxDialog::compare(AcGePoint2dArray& targetPts, AcGePoint2d &comparePt)
{
	for (int i = 0; i < targetPts.length(); i++)
	{
		if (targetPts[i] == comparePt)
		{
			return true;
		}
	}
	return false;
}

std::vector<AcGePoint2dArray> CArxDialog::getPlinePointForLayer(CString& layername, bool bClosed/* = true*/)
{
	std::vector<AcGePoint2dArray> outputPoints;
	AcDbObjectIdArray entIds;
	entIds = DBHelper::GetEntitiesByLayerName(layername);
	if (entIds.length() == 0)
	{
		acutPrintf(_T("\n获取该图层实体失败！"));
		return outputPoints;
	}

	for (int i = 0; i < entIds.length(); i++)
	{
		AcDbEntity* pEntity = NULL;
		if (acdbOpenAcDbEntity(pEntity, entIds[i], kForRead) != eOk)							//这里是可读
		{
			//acutPrintf(_T("\n获取单个实体指针失败！"));
			//return outputPoints;
			continue;
		}

		if (pEntity->isKindOf(AcDbPolyline::desc()))
		{
			std::vector<AcGePoint2d> allPoints;//得到的所有点
			AcDbVoidPtrArray entsTempArray;
			AcDbPolyline *pPline = AcDbPolyline::cast(pEntity);
			AcGeLineSeg2d line;
			AcGeCircArc3d arc;
			int n = pPline->numVerts();
			for (int i = 0; i < n; i++)
			{
				if (pPline->segType(i) == AcDbPolyline::kLine)
				{
					pPline->getLineSegAt(i, line);
					AcGePoint2d startPoint;
					AcGePoint2d endPoint;
					startPoint = line.startPoint();
					endPoint = line.endPoint();
					allPoints.push_back(startPoint);
					allPoints.push_back(endPoint);
				}
				else if (pPline->segType(i) == AcDbPolyline::kArc)
				{
					pPline->getArcSegAt(i, arc);
					AcGePoint3dArray result = GeHelper::CalcArcFittingPoints(arc, 16);
					for (int x = 0; x < result.length(); x++)
					{
						AcGePoint2d onArcpoint(result[x].x, result[x].y);
						allPoints.push_back(onArcpoint);
					}
				}
			}

			AcGePoint2dArray onePlinePts;//装取去完重的有效点
			for (int x = 0; x < allPoints.size(); x++)
			{
				if (compare(onePlinePts, allPoints[x]))
				{
					continue;
				}
				onePlinePts.append(allPoints[x]);
			}

			//检查闭合
			if (onePlinePts.length() > 1 && pPline->isClosed() ||
				(bClosed && onePlinePts.length() > 1 && onePlinePts[onePlinePts.length() - 1] != onePlinePts[0]))
			{
				onePlinePts.append(onePlinePts[0]);
			}

			outputPoints.push_back(onePlinePts);
		}
		pEntity->close();
	}
	return outputPoints;
}

std::vector<AcGePoint2dArray> CArxDialog::getPlinePointForLayer(CString& layername, std::vector<int>& types)
{
	std::vector<AcGePoint2dArray> outputPoints;
	AcDbObjectIdArray entIds;
	entIds = DBHelper::GetEntitiesByLayerName(layername);
	if (entIds.length() == 0)
	{
		return outputPoints;
	}

	for (int i = 0; i < entIds.length(); i++)
	{
		AcDbEntity* pEntity = NULL;
		if (acdbOpenAcDbEntity(pEntity, entIds[i], kForRead) != eOk)							//这里是可读
		{
			continue;
		}

		if (pEntity->isKindOf(AcDbPolyline::desc()))
		{
			
			std::vector<AcGePoint2d> allPoints;//得到的所有点
			AcDbVoidPtrArray entsTempArray;
			AcDbPolyline *pPline = AcDbPolyline::cast(pEntity);

			AcString value;
			DBHelper::GetXData(pPline, _T("设备房类型"), value);
			if (value == _T("配电房"))
			{
				types.push_back(2);
			}
			else if(value.find(_T("排烟"))>=0)
			{
				types.push_back(0);
			}
			else if(value.find(_T("进风"))>=0)
			{
				types.push_back(1);
			}
			else if(value == _T("生活泵房"))
			{
				types.push_back(3);
			}
			else if(value == _T("生活水箱"))
			{
				types.push_back(4);
			}
			else if (value == _T("消防泵房"))
			{
				types.push_back(5);
			}
			else if (value == _T("消防水池"))
			{
				types.push_back(6);
			}
			else
			{
				pEntity->close();
				continue;
			}

			AcGeLineSeg2d line;
			AcGeCircArc3d arc;
			int n = pPline->numVerts();
			for (int i = 0; i < n; i++)
			{
				if (pPline->segType(i) == AcDbPolyline::kLine)
				{
					pPline->getLineSegAt(i, line);
					AcGePoint2d startPoint;
					AcGePoint2d endPoint;
					startPoint = line.startPoint();
					endPoint = line.endPoint();
					allPoints.push_back(startPoint);
					allPoints.push_back(endPoint);
				}
				else if (pPline->segType(i) == AcDbPolyline::kArc)
				{
					pPline->getArcSegAt(i, arc);
					AcGePoint3dArray result = GeHelper::CalcArcFittingPoints(arc, 3);
					for (int x = 0; x < result.length(); x++)
					{
						AcGePoint2d onArcpoint(result[x].x, result[x].y);
						allPoints.push_back(onArcpoint);
					}
				}
			}
		
			AcGePoint2dArray onePlinePts;//装取去完重的有效点
			for (int x = 0; x < allPoints.size(); x++)
			{
				if (compare(onePlinePts, allPoints[x]))
				{
					continue;
				}
				onePlinePts.append(allPoints[x]);
			}

			//检查闭合
			if (onePlinePts.length() > 2 && onePlinePts[onePlinePts.length() - 1] != onePlinePts[0])
			{
				onePlinePts.append(onePlinePts[0]);
			}

			outputPoints.push_back(onePlinePts);			
		}

		pEntity->close();
	}
	return outputPoints;
}

void CArxDialog::setInitData()
{
	m_strLength = "5.3";
	m_editLength.SetWindowText(m_strLength);

	m_strWidth = "2.4";
	m_Width.SetWindowText(m_strWidth);

	m_StrLaneWidth = "5.5";
	m_LaneWidth.SetWindowText(m_StrLaneWidth);

	m_StrSquarcolumnLength = "0.6";
	m_SquarcolumnLength.SetWindowText(m_StrSquarcolumnLength);

	m_StrSquareColumnWidth = "0.6";
	m_SquareColumnWidth.SetWindowText(m_StrSquareColumnWidth);
}

std::string CArxDialog::postToAIApi(const std::string& sData)
{
	typedef int(*FN_post)(const char* url, const char*, int, bool, const char*);
	FN_post fn_post = ModulesManager::Instance().func<FN_post>(getHttpModule(), "post");
	if (!fn_post)
		return "";
	int code = fn_post("http://10.8.212.187/park", sData.c_str(), sData.size(), true, "application/json");
	if (code!=200)
	{
		return "";
	}

	typedef const char* (*FN_getBody)(int&);
	FN_getBody fn_getBody = ModulesManager::Instance().func<FN_getBody>(getHttpModule(), "getBody");
	if (!fn_getBody)
		return "";
	int len = 0;
	std::string json = fn_getBody(len);

	Json::Reader reader;
	Json::Value root;
	//从字符串中读取数据
	if (reader.parse(json, root))
	{
		
		if (root["status"].isInt())
		{
			int status = root["status"].asInt();
			if (status!=0)
			{
				return "";
			}
		}
		else
		{
			return "";
		}
		//std::string messgae = root["messgae"].asString();
		std::string result = root["result"].asString();	
		return result;
	}

	return "";
}

void CArxDialog::OnBnClickedButtonGetstartpoint()
{
	HideDialogHolder holder(this);
	Doc_Locker doc_locker;
	//提示用户输入一个点
	ads_point pt;
	if (acedGetPoint(NULL, _T("\n输入一个点："), pt) == RTNORM)
	{
		//如果点有效，继续执行
		CompleteEditorCommand();
		m_strXPt.Format(_T("%.2f"), pt[X]);
		m_strYPt.Format(_T("%.2f"), pt[Y]);
		m_sStartPoint = _T("(") + m_strXPt + _T(",") + m_strYPt + _T(")");
		//显示点的坐标	 
		m_editStartPoint.SetWindowText(m_sStartPoint);
		startPtx = pt[X];
		startPty = pt[Y];
	}
	else
	{
		m_sStartPoint = "选取了无效的点";
		m_editStartPoint.SetWindowText(m_sStartPoint);
	}
}


void CArxDialog::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	m_editLength.GetWindowText(m_strLength);
	m_Width.GetWindowText(m_strWidth);
	m_LaneWidth.GetWindowText(m_StrLaneWidth);
	m_SquarcolumnLength.GetWindowText(m_StrSquarcolumnLength);
	m_SquareColumnWidth.GetWindowText(m_StrSquareColumnWidth);

	/*acutPrintf(_T("\n单车位不靠墙长度：%s"), m_strLength);
	acutPrintf(_T("\n单车位宽度：%s"), m_strWidth);
	acutPrintf(_T("\n车道宽度：%s"), m_StrLaneWidth);
	acutPrintf(_T("\n方柱长度：%s"), m_StrSquarcolumnLength);
	acutPrintf(_T("\n方柱宽度：%s"), m_StrSquareColumnWidth);*/

	double parkLength = _ttof(m_strLength.GetString());
	//parkLength = ((float)((int)((parkLength + 0.005) * 100))) / 100;
	double parkWidth = _ttof(m_strWidth.GetString());
	//parkWidth = ((float)((int)((parkWidth + 0.005) * 100))) / 100;
	double laneWidth = _ttof(m_StrLaneWidth.GetString());
	//laneWidth = ((float)((int)((laneWidth + 0.005) * 100))) / 100;
	double squarcolumnLength = _ttof(m_StrSquarcolumnLength.GetString());
	//squarcolumnLength = ((float)((int)((squarcolumnLength + 0.005) * 100))) / 100;
	double squareColumnWidth = _ttof(m_StrSquareColumnWidth.GetString());
	//squareColumnWidth = ((float)((int)((squareColumnWidth + 0.005) * 100))) / 100;//保留精度操作


	if (1 == m_checkPartition.GetCheck())
	{
		isPartition = true;
	}
	else
	{
		isPartition = false;
	}
	int direction = 0;
	CString showDirection;
	m_directionCombo.GetWindowText(showDirection);
	if (showDirection=="垂直排布")
	{
		direction = 1;
	}
	else
	{
		direction = 0;
	}
	CString outlineLayer;
	m_outlineLayer.GetWindowText(outlineLayer);
	std::vector<AcGePoint2dArray> outlinePts = getPlinePointForLayer(outlineLayer);
	CString shearwallLaye;
	m_shearwallLayer.GetWindowText(shearwallLaye);
	std::vector<AcGePoint2dArray> shearwallPts = getPlinePointForLayer(shearwallLaye);

	std::vector<int> types;
	CString zonesLayer = _T("设备房");
	std::vector<AcGePoint2dArray> zonesPts = getPlinePointForLayer(zonesLayer, types);

	Json::Value root;//根节点
					 
	//创建子节点
	//Json::Value childnode;
	//Json::Value oneZoesPts;
	for (int e = 0;e < zonesPts.size();e++)
	{
		Json::Value oneZoesPts;
		for (int f = 0; f < zonesPts[e].length(); f++)
		{
			Json::Value zonePoint;
			zonePoint.append(zonesPts[e][f].x);//[]
			zonePoint.append(zonesPts[e][f].y);//[]
			oneZoesPts["outer"].append(zonePoint);//[]
			int typeValue = types[e];
			oneZoesPts["type"] = Json::Value(typeValue);
		}
		root["zones"].append(oneZoesPts);//[]
	}



	for (int m = 0; m < shearwallPts.size(); m++)
	{
		Json::Value oneShearwallPline;
		for (int n = 0; n < shearwallPts[m].length(); n++)
		{
			Json::Value shearwallPoint;
			shearwallPoint.append(shearwallPts[m][n].x);
			shearwallPoint.append(shearwallPts[m][n].y);
			oneShearwallPline.append(shearwallPoint);
		}
		root["column"].append(oneShearwallPline);
	}

	for (int x = 0; x < outlinePts.size(); x++)
	{
		Json::Value oneOutlinePline;
		for (int y = 0; y < outlinePts[x].length(); y++)
		{
			Json::Value outlinePoint;
			outlinePoint.append(outlinePts[x][y].x);
			outlinePoint.append(outlinePts[x][y].y);
			oneOutlinePline.append(outlinePoint);
		}
		root["building"].append(oneOutlinePline);
	}

	root["partition"].resize(0);//暂时构造一个空数组
	//数组形式
	int count = GetretreatlinePts.length();
	for (int i = 0; i < count; i++)
	{
		Json::Value point;
		point.append(GetretreatlinePts[i].x);
		point.append(GetretreatlinePts[i].y);
		root["outer"].append(point);
	}

	//子节点
	Json::Value params;
	//字节点属性
	params["direction"] = Json::Value(direction);
	params["park_length"] = Json::Value(parkLength);
	params["park_width"] = Json::Value(parkWidth);
	params["lane_width"] = Json::Value(laneWidth);
	params["column_length"] = Json::Value(squarcolumnLength);
	params["column_width"] = Json::Value(squareColumnWidth);
	Json::Value startpoint;
	startpoint.append(startPtx);
	startpoint.append(startPty);
	params["start_point"] = Json::Value(startpoint);
	//子节点挂到根节点上
	root["params"] = Json::Value(params);
	
	std::string uuid = postToAIApi(root.toStyledString());
	CDlgWaiting::setUuid(uuid);
	//CDlgWaiting::Show(true);
	CDlgWaiting dlg;
	dlg.DoModal();

	////输出到文件
	//Json::StyledWriter sw;
	//std::ofstream os;
	//os.open("ParkingConfig");
	//if (!os.is_open())
	//{
	//	acedAlert(_T("打开或创建文件失败"));
	//	return;	
	//}
	//os << sw.write(root);
	//os.close();

	CAcUiDialog::OnOK();
}



void CArxDialog::OnBnClickedRadioOtherlength()
{
	// TODO: 在此添加控件通知处理程序代码
	GetDlgItem(IDC_EDIT_Length)->ShowWindow(SW_SHOW);
	GetDlgItem(IDC_STATIC_m)->ShowWindow(SW_SHOW);
}


void CArxDialog::OnBnClickedRadioDefault()
{
	// TODO: 在此添加控件通知处理程序代码
	GetDlgItem(IDC_EDIT_Length)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_STATIC_m)->ShowWindow(SW_HIDE);
	m_strLength = "5.3";
	m_editLength.SetWindowText(m_strLength);
}


void CArxDialog::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	CAcUiDialog::OnTimer(nIDEvent);
}


