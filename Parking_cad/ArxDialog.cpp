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
//#include "Authenticate\Authenticate.h"
#include "Convertor.h"
#include "EquipmentroomTool.h"
#include "OperaAddFrame.h"
#include "OperaCheck.h"
#include "KV.h"

std::string CArxDialog::ms_posturlPortone;
std::string CArxDialog::ms_posturlPorttwo;


//-----------------------------------------------------------------------------
IMPLEMENT_DYNAMIC(CArxDialog, CAcUiDialog)

BEGIN_MESSAGE_MAP(CArxDialog, CAcUiDialog)
	ON_MESSAGE(WM_ACAD_KEEPFOCUS, OnAcadKeepFocus)
	ON_BN_CLICKED(IDC_BUTTON_GetRetreatline, &CArxDialog::OnBnClickedButtonGetretreatline)
	ON_BN_CLICKED(IDC_BUTTON_GetStartPoint, &CArxDialog::OnBnClickedButtonGetstartpoint)
	ON_BN_CLICKED(IDOK, &CArxDialog::OnBnClickedOk)
	ON_BN_CLICKED(IDC_RADIO_OtherLength, &CArxDialog::OnBnClickedRadioOtherlength)
	ON_BN_CLICKED(IDC_RADIO_Default, &CArxDialog::OnBnClickedRadioDefault)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_CHECK_Partition, &CArxDialog::OnBnClickedCheckPartition)
	ON_EN_KILLFOCUS(IDC_EDIT_NON_CONVEXLEVEL, &CArxDialog::OnEnKillfocusEditNonConvexlevel)
	ON_BN_CLICKED(IDC_BUTTON_V2OK, &CArxDialog::OnBnClickedButtonV2ok)
	ON_BN_CLICKED(IDC_BUTTON_GETENDPOINT, &CArxDialog::OnBnClickedButtonGetendpoint)
	ON_BN_CLICKED(IDC_BUTTON_MANYSHOW, &CArxDialog::OnBnClickedButtonManyshow)
END_MESSAGE_MAP()

//-----------------------------------------------------------------------------
CArxDialog::CArxDialog(CWnd *pParent /*=NULL*/, HINSTANCE hInstance /*=NULL*/) : CAcUiDialog(CArxDialog::IDD, pParent, hInstance) {
}

CArxDialog::~CArxDialog() {

	COperaParkingSpaceShow::ms_dlg = NULL;
}

void CArxDialog::setPostUrlPortone(std::string& posturlPortone)
{
	ms_posturlPortone = posturlPortone;
}

void CArxDialog::setPostUrlPorttwo(std::string& posturlPorttwo)
{
	ms_posturlPorttwo = posturlPorttwo;
}

//-----------------------------------------------------------------------------
void CArxDialog::loadoutlineLayers()
{
	Doc_Locker _locker;

	AcDbLayerTable* pLT = NULL;
	if (acdbCurDwg()->getLayerTable(pLT, AcDb::kForRead) != Acad::eOk)
		return;

	static std::vector<AcString> s_Keys;
	if (s_Keys.size() == 0)
	{
		s_Keys.push_back(_T("外轮廓"));
		//添加更多的关键字
	}

	do
	{
		AcDbLayerTableIterator* pLTIter = NULL;
		if (pLT->newIterator(pLTIter) != Acad::eOk)
			break;

		bool bHasSetDef = false;

		pLTIter->setSkipHidden(true);
		for (pLTIter->start(); !pLTIter->done(); pLTIter->step())
		{
			AcDbLayerTableRecord* pLTR = NULL;
			if (pLTIter->getRecord(pLTR, AcDb::kForRead) != Acad::eOk)
				continue;

			AcString name;
			pLTR->getName(name);
			int n = m_outlineLayer.AddString(name);

			if (!bHasSetDef)
			{//自动设置默认图层
				for (int i = 0; i < s_Keys.size(); ++i)
				{
					CString upName = name.constPtr();
					upName.MakeUpper();
					if (upName.Find(s_Keys[i]) > -1)
					{
						m_outlineLayer.SetCurSel(n);
						break;
					}
				}
			}

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

	static std::vector<AcString> s_Keys;
	if (s_Keys.size() == 0)
	{
		s_Keys.push_back(_T("COL"));
		s_Keys.push_back(_T("WALL"));
		s_Keys.push_back(_T("墙"));
		//添加更多的关键字
	}

	do
	{
		AcDbLayerTableIterator* pLTIter = NULL;
		if (pLT->newIterator(pLTIter) != Acad::eOk)
			break;

		bool bHasSetDef = false;

		pLTIter->setSkipHidden(true);
		for (pLTIter->start(); !pLTIter->done(); pLTIter->step())
		{
			AcDbLayerTableRecord* pLTR = NULL;
			if (pLTIter->getRecord(pLTR, AcDb::kForRead) != Acad::eOk)
				continue;

			AcString name;
			pLTR->getName(name);
			int n = m_shearwallLayer.AddString(name);

			if (!bHasSetDef)
			{//自动设置默认图层
				for (int i = 0; i < s_Keys.size(); ++i)
				{
					CString upName = name.constPtr();
					upName.MakeUpper();
					if (upName.Find(s_Keys[i]) > -1)
					{
						m_shearwallLayer.SetCurSel(n);
						break;
					}
				}
			}

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
void CArxDialog::DoDataExchange(CDataExchange *pDX) {
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
	DDX_Control(pDX, IDC_EDIT_PARKINGCOUNT, m_ParkingCount);
	DDX_Control(pDX, IDC_EDIT_NON_CONVEXLEVEL, m_Non_Convexlevel);
	DDX_Control(pDX, IDC_EDIT_PARTITION_LINE, m_PartitionLineEdit);
	DDX_Control(pDX, IDC_EDIT_SHOWENDPOINT, m_EditShowEndPoint);
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
LRESULT CArxDialog::OnAcadKeepFocus(WPARAM, LPARAM) {
	return (TRUE);
}

BOOL CArxDialog::OnInitDialog()
{
	// TODO:  在此添加额外的初始化
	BOOL bRet = CAcUiDialog::OnInitDialog();

	CenterWindow(GetDesktopWindow());//窗口至于屏幕中间

	loadoutlineLayers();
	loadshearwallLayers();
	loaddirectionCombo();

	m_outlineLayer.SetDroppedWidth(200);
	m_shearwallLayer.SetDroppedWidth(200);

	setInitData();
	((CButton*)GetDlgItem(IDC_RADIO_Default))->SetCheck(BST_CHECKED);//程序启动时默认为单选按钮1选中

	GetDlgItem(IDC_EDIT_Length)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_STATIC_m)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_EDIT_PARTITION_LINE)->ShowWindow(SW_HIDE);
	return bRet;
}

void CArxDialog::OnBnClickedButtonGetretreatline()
{
	// TODO: 在此添加控件通知处理程序代码
	HideDialogHolder holder(this);
	Doc_Locker doc_locker;

	ads_name ename; ads_point pt;
	if (acedEntSel(_T("\n请选择地库退线对应多段线:"), ename, pt) != RTNORM)
	{
		return;
	}
	acutPrintf(_T("\n"));
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
				for (int x = 0; x < result.length(); x++)
				{
					AcGePoint2d onArcpoint(result[x].x, result[x].y);
					//acutPrintf(_T("\n该点%d坐标为%.2f,%.2f"), x, onArcpoint.x, onArcpoint.y);
					allPoints.push_back(onArcpoint);
				}
			}
		}
		for (int x = 0; x < allPoints.size(); x++)
		{
			if (compare(GetretreatlinePts, allPoints[x]))
			{
				continue;
			}
			GetretreatlinePts.append(allPoints[x]);
		}

		//检测闭合
		if (GetretreatlinePts.length() > 2 && GetretreatlinePts[GetretreatlinePts.length() - 1] != allPoints[0])
			GetretreatlinePts.append(GetretreatlinePts[0]);
	}
	pEnt->close();

	for (int yy = 0; yy < GetretreatlinePts.length(); yy++)
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
			else if (value.find(_T("排烟")) >= 0)
			{
				types.push_back(0);
			}
			else if (value.find(_T("进风")) >= 0)
			{
				types.push_back(1);
			}
			else if (value == _T("生活泵房"))
			{
				types.push_back(3);
			}
			else if (value == _T("生活水箱"))
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
	m_strLength = _T("5.1");
	m_editLength.SetWindowText(m_strLength);

	m_strWidth = _T("2.4");
	m_Width.SetWindowText(m_strWidth);

	m_StrLaneWidth = _T("5.5");
	m_LaneWidth.SetWindowText(m_StrLaneWidth);

	m_StrSquarcolumnLength = _T("0.6");
	m_SquarcolumnLength.SetWindowText(m_StrSquarcolumnLength);

	m_StrSquareColumnWidth = _T("0.6");
	m_SquareColumnWidth.SetWindowText(m_StrSquareColumnWidth);

	INIT_KV(ModulesManager::Instance().loadModule("KV.dll"));

	m_strUserId = GL::WideByte2Ansi(GetStr(_T("bip_id")));
	//m_strComputerId = GL::WideByte2Ansi(g_computerId.GetString());

	m_sNonConvexLevel = _T("0.2");
	m_Non_Convexlevel.SetWindowText(m_sNonConvexLevel);

	m_sParkingCount = _T("0");
	m_ParkingCount.SetWindowText(m_sParkingCount);
}

int CArxDialog::postToAIApi(const std::string& sData, std::string& sMsg, const bool& useV1)
{
	typedef int(*FN_post)(const char* url, const char*, int, bool, const char*);
	FN_post fn_post = ModulesManager::Instance().func<FN_post>(getHttpModule(), "post");
	if (!fn_post)
	{
		sMsg = getHttpModule() + ":post模块错误。";
		return 1;
	}
	const char * postUrl;
	if (useV1)
	{
		postUrl = ms_posturlPortone.c_str();
	}
	else
	{
		postUrl = ms_posturlPorttwo.c_str();
	}
	//MessageBoxA(NULL, postUrl, "", 0);
	int code = fn_post(postUrl, sData.c_str(), sData.size(), true, "application/json");
	if (code != 200)
	{
		if (useV1)
		{
			sMsg = ms_posturlPortone + ":网络或服务器错误。";
		}
		else
		{
			sMsg = ms_posturlPorttwo + ":网络或服务器错误。";
		}
		return 2;
	}

	typedef const char* (*FN_getBody)(int&);
	FN_getBody fn_getBody = ModulesManager::Instance().func<FN_getBody>(getHttpModule(), "getBody");
	if (!fn_getBody)
	{
		sMsg = getHttpModule() + ":getBody模块错误。";
		return 1;
	}
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
			if (status != 0)
			{
				sMsg = "提交任务出错。";
				return 3;
			}
		}
		else
		{
			sMsg = "没有返回status字段。";
			return 4;
		}
		//std::string messgae = root["messgae"].asString();
		std::string result = root["result"].asString();
		sMsg = result;
		return 0;
	}

	sMsg = "json解析错误";
	return 4;
}

void CArxDialog::selectPort(const bool& useV1,bool useManyShow /*= false*/)
{
	// TODO: 在此添加控件通知处理程序代码
	m_editLength.GetWindowText(m_strLength);
	m_Width.GetWindowText(m_strWidth);
	m_LaneWidth.GetWindowText(m_StrLaneWidth);
	m_SquarcolumnLength.GetWindowText(m_StrSquarcolumnLength);
	m_SquareColumnWidth.GetWindowText(m_StrSquareColumnWidth);
	m_ParkingCount.GetWindowText(m_sParkingCount);
	m_Non_Convexlevel.GetWindowText(m_sNonConvexLevel);

	double parkLength = _tstof(m_strLength.GetString());
	double parkWidth = _tstof(m_strWidth.GetString());
	double laneWidth = _tstof(m_StrLaneWidth.GetString());
	double squarcolumnLength = _tstof(m_StrSquarcolumnLength.GetString());
	double squareColumnWidth = _tstof(m_StrSquareColumnWidth.GetString());
	//squareColumnWidth = ((float)((int)((squareColumnWidth + 0.005) * 100))) / 100;//保留精度操作
	int nParkingCount = _ttoi(m_sParkingCount.GetString());
	double dNon_Convexlevel = _tstof(m_sNonConvexLevel.GetString());
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
	if (showDirection == "垂直排布")
	{
		direction = 1;
	}
	else
	{
		direction = 0;
	}
	//if (m_sParkingCount == _T(""))
	//{
	//	acedAlert(_T("没有输入车位总数信息!")); 
	//	return;
	//}
	CString outlineLayer;
	m_outlineLayer.GetWindowText(outlineLayer);
	std::vector<AcGePoint2dArray> outlinePts = getPlinePointForLayer(outlineLayer);
	//传图层名过去
	COperaAddFrame::setOutLineLayerName(outlineLayer);
	if (outlinePts.size() == 0)
	{
		acedAlert(_T("没有选择外轮廓图层信息!"));
		return;
	}
	CString shearwallLaye;
	m_shearwallLayer.GetWindowText(shearwallLaye);
	COperaCheck::setShearWallLayerName(shearwallLaye);
	std::vector<AcGePoint2dArray> shearwallPts = getPlinePointForLayer(shearwallLaye);
	if (shearwallPts.size() == 0)
	{
		acedAlert(_T("没有选择剪力墙图层信息!"));
		return;
	}
	CString sStartPt;
	m_editStartPoint.GetWindowText(sStartPt);
	if (sStartPt == _T(""))
	{
		startPtx = -1.0;
		startPty = -1.0;
	}
	CString strEndPt;
	m_EditShowEndPoint.GetWindowText(strEndPt);
	if (strEndPt == _T(""))
	{
		dEndPtx = -1.0;
		dEndPty = -1.0;
	}
	if (GetretreatlinePts.length() == 0)
	{
		acedAlert(_T("没有选择地库退线信息!"));
		return;
	}
	std::vector<int> types;
	CString zonesLayer(CEquipmentroomTool::getLayerName("equipmentroom").c_str());
	std::vector<AcGePoint2dArray> zonesPts = getPlinePointForLayer(zonesLayer, types);
	CString strCoreWallLayer(CEquipmentroomTool::getLayerName("core_wall").c_str());
	std::vector<AcGePoint2dArray> coreWallPts = getPlinePointForLayer(strCoreWallLayer);

	if (isPartition)
	{
		if (allPartitionPts.size() == 0)
		{
			acedAlert(_T("没有选择分区信息或选择分区用实体类型错误!"));
			return;
		}
	}

	Json::Value root;//根节点
					 //创建子节点
	for (int e = 0; e < zonesPts.size(); e++)
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

	//核心筒数据
	for (int g = 0; g < coreWallPts.size(); g++)
	{
		Json::Value oneCoreWallPline;
		for (int w = 0; w < coreWallPts[g].length(); w++)
		{
			Json::Value corewallPoint;
			corewallPoint.append(coreWallPts[g][w].x);
			corewallPoint.append(coreWallPts[g][w].y);
			oneCoreWallPline.append(corewallPoint);
		}
		root["corewall"].append(oneCoreWallPline);
	}

	if (isPartition)
	{
		for (int a = 0; a < allPartitionPts.size(); a++)
		{
			Json::Value onePartitionPline;
			for (int b = 0; b < allPartitionPts[a].length(); b++)
			{
				Json::Value partitionPoint;
				partitionPoint.append(allPartitionPts[a][b].x);
				partitionPoint.append(allPartitionPts[a][b].y);
				onePartitionPline.append(partitionPoint);
			}
			root["partition"].append(onePartitionPline);
		}
	}
	else
	{
		root["partition"].resize(0);//暂时构造一个空数组
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
	params["parking_count"] = Json::Value(nParkingCount);
	params["non_convexLevel"] = Json::Value(dNon_Convexlevel);
	Json::Value startpoint;
	startpoint.append(startPtx);
	startpoint.append(startPty);
	params["start_point"] = Json::Value(startpoint);
	Json::Value endpoint;
	endpoint.append(dEndPtx);
	endpoint.append(dEndPty);
	params["end_point"] = Json::Value(endpoint);
	//子节点挂到根节点上
	root["params"] = Json::Value(params);
	Json::Value auth;
	//CString strUserId = GL::Ansi2WideByte(m_strUserId.c_str()).c_str();
	//CString strComputerId = GL::Ansi2WideByte(m_strComputerId.c_str()).c_str();
	auth["computer_id"] = GL::Ansi2Utf8(m_strComputerId.c_str());
	auth["user_id"] = GL::Ansi2Utf8(m_strUserId.c_str());
	root["auth"] = auth;
	std::string strData = root.toStyledString();
	if (strData == "")
	{
		acedAlert(_T("生成传输数据失败"));
		return;
	}
	std::string uuid;
	int res = postToAIApi(root.toStyledString(), uuid,useV1);
	if (res != 0)
	{
		CString	sMsg = GL::Ansi2WideByte(uuid.c_str()).c_str();
		acedAlert(sMsg);
		return;
	}
	CDlgWaiting::setUuid(uuid,useV1,useManyShow);

	//CDlgWaiting::Show(true);
	//CDlgWaiting dlg;
	//dlg.DoModal();
	CAcModuleResourceOverride resOverride;//资源定位
	CDlgWaiting* pWaitDlg = new CDlgWaiting;
	pWaitDlg->Create(CDlgWaiting::IDD, acedGetAcadDwgView());
	pWaitDlg->ShowWindow(SW_SHOW);

	////输出到文件
	//Json::StyledWriter sw;
	//std::ofstream os;
	//os.open("parkingConfigtest");
	//if (!os.is_open())
	//{
	//	acedAlert(_T("打开或创建文件失败"));
	//	return;	
	//}
	//os << sw.write(root);
	//os.close();

	CAcUiDialog::OnOK();
}

void CArxDialog::OnBnClickedButtonGetstartpoint()
{
	HideDialogHolder holder(this);
	Doc_Locker doc_locker;
	//提示用户输入一个点
	ads_point pt;
	int re = acedGetPoint(NULL, _T("\n请点选一个点作为车位排布起点："), pt);
	if (re == RTNORM)
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
	else if (re == RTCAN)
	{
		m_sStartPoint = "";
		m_editStartPoint.SetWindowText(m_sStartPoint);
		return;
	}
	else
	{
		m_sStartPoint = "选取了无效的点";
		m_editStartPoint.SetWindowText(m_sStartPoint);
	}
}


void CArxDialog::OnBnClickedOk()
{
	selectPort(true);
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
	m_strLength = _T("5.3");
	m_editLength.SetWindowText(m_strLength);
}


void CArxDialog::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	CAcUiDialog::OnTimer(nIDEvent);
}




void CArxDialog::OnBnClickedCheckPartition()
{
	// TODO: 在此添加控件通知处理程序代码
  	if (1 == m_checkPartition.GetCheck())
	{
		HideDialogHolder holder(this);
		Doc_Locker doc_locker;
		GetDlgItem(IDC_EDIT_PARTITION_LINE)->ShowWindow(SW_SHOW);

		std::vector<AcDbEntity*> vctPartitionEnt;
		ads_name ssname;
		ads_name ent;
		//获取选择集
		acedPrompt(_T("\n请选择分区使用的多线段实体："));
		struct resbuf *rb; // 结果缓冲区链表
		rb = acutBuildList(RTDXF0, _T("LWPOLYLINE"), RTNONE);
		// 选择复合要求的实体
		acedSSGet(NULL, NULL, NULL, rb, ssname);
		//获取选择集的长度
		Adesk::Int32 len = 0;
		int nRes = acedSSLength(ssname, &len);
		if (RTNORM == nRes)
		{
			//遍历选择集
			for (int i = 0; i < len; i++)
			{
				//获取实体名
				int nRes = acedSSName(ssname, i, ent);
				if (nRes != RTNORM)
					continue;
				//根据实体名得到ID，然后打开自定义实体
				AcDbObjectId id;
				acdbGetObjectId(id, ent);
				if (!id.isValid())
					continue;
				AcDbEntity *pEnt = NULL;
				acdbOpenObject(pEnt, id, AcDb::kForWrite);
				//判断自定义实体的类型
				if (pEnt == NULL)
					continue;
				vctPartitionEnt.push_back(pEnt);
			}
		}
		//释放选择集和结果缓存区
		acutRelRb(rb);
		acedSSFree(ssname);
		if (vctPartitionEnt.size() < 1)
			return;
		for (int i = 0; i < vctPartitionEnt.size(); i++)
		{
			if (vctPartitionEnt[i]->isKindOf(AcDbPolyline::desc()))
			{
				std::vector<AcGePoint2d> allPoints;//得到的所有点
				AcDbVoidPtrArray entsTempArray;
				AcDbPolyline *pPline = AcDbPolyline::cast(vctPartitionEnt[i]);
				bool bClosed = pPline->isClosed();
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
				allPartitionPts.push_back(onePlinePts);
			}
			vctPartitionEnt[i]->close();
		}
		for (int j=0; j<allPartitionPts.size(); j++)
		{
			CString onePartPline;
			for (int k=0;k<allPartitionPts[j].length(); k++ )
			{
				CString tempStr_X;
				CString tempStr_Y;
				tempStr_X.Format(_T("%.2f"), allPartitionPts[j][k].x);
				tempStr_Y.Format(_T("%.2f"), allPartitionPts[j][k].y);
				onePartPline += CString(_T("(")) + tempStr_X + _T(",") + tempStr_Y + _T(")");				
			}
			m_sPartitionLine += CString(_T("[")) + onePartPline + _T("]");
			m_PartitionLineEdit.SetWindowText(m_sPartitionLine);
		}
	}
	else
	{
		GetDlgItem(IDC_EDIT_PARTITION_LINE)->ShowWindow(SW_HIDE);
		allPartitionPts.clear();
		m_sPartitionLine = _T("");
		m_PartitionLineEdit.SetWindowText(m_sPartitionLine);
	}
}


void CArxDialog::OnEnKillfocusEditNonConvexlevel()
{
	// TODO: 在此添加控件通知处理程序代码
	CString sTempNonLev;
	m_Non_Convexlevel.GetWindowText(sTempNonLev);
	double dNon_Convexlevel = _tstof(sTempNonLev.GetString());
	if ((dNon_Convexlevel > 0.6) || (dNon_Convexlevel < 0.1))
	{	
		acedAlert(_T("请输入0.1到0.6之间数值!"));
		m_Non_Convexlevel.SetWindowText(_T("0.2"));
	}
}


void CArxDialog::OnBnClickedButtonV2ok()
{
	// TODO: 在此添加控件通知处理程序代码
	selectPort(false);
}


void CArxDialog::OnBnClickedButtonGetendpoint()
{
	// TODO: 在此添加控件通知处理程序代码
	HideDialogHolder holder(this);
	Doc_Locker doc_locker;
	//提示用户输入一个点
	ads_point pt;
	int re = acedGetPoint(NULL, _T("\n请点选一个点作为车位排布终点："), pt);
	if (re == RTNORM)
	{
		//如果点有效，继续执行
		CompleteEditorCommand();
		m_strEndXPt.Format(_T("%.2f"), pt[X]);
		m_strEndYPt.Format(_T("%.2f"), pt[Y]);
		m_strEndPoint = _T("(") + m_strEndXPt + _T(",") + m_strEndYPt + _T(")");
		//显示点的坐标	 
		m_EditShowEndPoint.SetWindowText(m_strEndPoint);
		dEndPtx = pt[X];
		dEndPty = pt[Y];
	}
	else if (re == RTCAN)
	{
		m_strEndPoint = "";
		m_EditShowEndPoint.SetWindowText(m_strEndPoint);
		return;
	}
	else
	{
		m_strEndPoint = "选取了无效的点";
		m_EditShowEndPoint.SetWindowText(m_strEndPoint);
	}
}


void CArxDialog::OnBnClickedButtonManyshow()
{
	// TODO: 在此添加控件通知处理程序代码
	selectPort(true,true);
}
