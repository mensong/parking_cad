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
//----- DlgEntrance.cpp : Implementation of CDlgEntrance
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "resource.h"
#include "DlgEntrance.h"
#include "DBHelper.h"
#include <json/json.h>
#include "ModulesManager.h"
#include "Convertor.h"
#include "RTreeEx.h"
#include "GeHelper.h"

std::string CDlgEntrance::ms_strEntrancePostUrlPort;
std::string CDlgEntrance::ms_strEntrancePostUrlPortV2;

void CDlgEntrance::deletParkingForEntrance()
{
	AcDbObjectIdArray parkingIds = DBHelper::GetEntitiesByLayerName(_T("parkings"));
	if (parkingIds.length()<=0)
	{
		return;
	}
	//用于删除位于出入口的车位的rtree
	RT::RTreeEx<UINT32, double, 2> rTreeOfParkingFilter;
	for (int a = 0; a < parkingIds.length(); a++)
	{
		//对每个轴线操作
		AcDbEntity* pEntArea = NULL;
		Acad::ErrorStatus es;
		es = acdbOpenAcDbEntity(pEntArea, parkingIds[a], kForRead);
		if (es != eOk)
		{
			acutPrintf(_T("\n获取单个车位实体指针失败！"));
			return;
		}
		if (pEntArea->isKindOf(AcDbBlockReference::desc()))
		{
			AcDbExtents BlkRefExtents;
			DBHelper::GetEntityExtents(BlkRefExtents, pEntArea);
			AcGePoint3d maxPoint = BlkRefExtents.maxPoint();
			AcGePoint3d minPoint = BlkRefExtents.minPoint();
			double positionMin[2];
			double positionMax[2];
			positionMin[0] = minPoint.x;
			positionMin[1] = minPoint.y;
			positionMax[0] = maxPoint.x;
			positionMax[1] = maxPoint.y;
			UINT32 handle = DBHelper::GetObjectIdHandle(parkingIds[a]);
			rTreeOfParkingFilter.Insert(positionMin, positionMax, handle);//装handle
		}
		pEntArea->close();
	}


}

void CDlgEntrance::getParkingIdAndAreaMap(std::map<AcDbObjectId, AcGePoint2dArray>& idAndParkingAreaMap, std::vector<AcGePoint2dArray>& parkingArea)
{
	AcDbObjectIdArray parkingIds = DBHelper::GetEntitiesByLayerName(_T("parkings"));
	if (parkingIds.length()==0)
	{
		acutPrintf(_T("获取不到车位信息！"));
		return;
	}
	for (int i=0; i<parkingIds.length(); i++)
	{
		AcDbEntity* pEntity = NULL;
		if (acdbOpenAcDbEntity(pEntity, parkingIds[i], kForRead) != eOk)//这里只读即可
		{
			acutPrintf(_T("获取单个车位信息操作失败！"));
			continue;
		}
		if (pEntity->isKindOf(AcDbBlockReference::desc()))
		{
			AcDbExtents boundaryOfBlk;
			if (pEntity->bounds(boundaryOfBlk) != TRUE)
			{
				acutPrintf(_T("获取车位的边界信息失败！"));
				continue;
			}
			AcGePoint3d positionOfBlkRefMax = boundaryOfBlk.maxPoint();
			AcGePoint3d positionOfBlkRefMin = boundaryOfBlk.minPoint();
			double positionMin[2];
			double positionMax[2];
			positionMin[0] = positionOfBlkRefMin.x;
			positionMin[1] = positionOfBlkRefMin.y;
			positionMax[0] = positionOfBlkRefMax.x;
			positionMax[1] = positionOfBlkRefMax.y;
			AcGePoint2d positionOfEnt;
			positionOfEnt.x = (positionMax[0] + positionMin[0]) / 2;
			positionOfEnt.y = (positionMax[1] + positionMin[1]) / 2;

			AcGePoint2dArray pts;
			AcGePoint2d ptLeftBottom(positionOfBlkRefMin.x, positionOfBlkRefMin.y);
			AcGePoint2d ptRightBottom(positionOfBlkRefMax.x, positionOfBlkRefMin.y);
			AcGePoint2d ptRightTop(positionOfBlkRefMax.x, positionOfBlkRefMax.y);
			AcGePoint2d ptLeftTop(positionOfBlkRefMin.x, positionOfBlkRefMax.y);
			pts.append(ptLeftBottom);
			pts.append(ptRightBottom);
			pts.append(ptRightTop);
			pts.append(ptLeftTop);
			std::pair<AcDbObjectId, AcGePoint2dArray> value(parkingIds[i], pts);
			idAndParkingAreaMap.insert(value);//插入新元素
			parkingArea.push_back(pts);
		}
		pEntity->close();
	}
}

bool CDlgEntrance::isTwoPolyOverlap(AcGePoint2dArray pts1, AcGePoint2dArray pts2)
{
	/**
	* 算法：判定两多段线是否重合：拿到交叉面积A1，拿到两多段线的其中最小面积A2，如果A1>(A2/3)，则判定为重合
	*/
	std::vector<AcGePoint2dArray> polysInter;
	GeHelper::GetIntersectionOfTwoPolygon(pts1, pts2, polysInter);

	double dAreaOverlap = 0;
	for (int i=0; i<polysInter.size(); ++i)
	{
		dAreaOverlap += GeHelper::CalcPolygonArea(polysInter[i]);
	}
	//如果相交
	if (GeHelper::eq(dAreaOverlap, 0.0))
	{
		return false;
	}
	return true;
//下面部分代码可用于去重判断
	//double dArea1 = 0;
	//double width = 1;
	//int countForPline = pts1.length();
	//AcDbPolyline *pPline1 = new AcDbPolyline(countForPline);
	//for (int j=0; j<countForPline; j++)
	//{
	//	pPline1->addVertexAt(0, pts1.at(j), 0, width, width);
	//}
	//pPline1->setClosed(true);
	//pPline1->getArea(dArea1);
	//double dArea2 = 1;
	//pPline1->getArea(dArea2);
	//double dAreaMin = std::min<>(dArea1, dArea2);
	////!(A1>(A2/3))
	//if (GeHelper::lteq(dAreaOverlap, dAreaMin / 3))//如果有相交部分大于自身1/3
	//	return false;
	//return true;
}

//-----------------------------------------------------------------------------
IMPLEMENT_DYNAMIC (CDlgEntrance, CAcUiDialog)

BEGIN_MESSAGE_MAP(CDlgEntrance, CAcUiDialog)
	ON_MESSAGE(WM_ACAD_KEEPFOCUS, OnAcadKeepFocus)
	ON_BN_CLICKED(IDC_BUTTON_GetStartPoint, &CDlgEntrance::OnBnClickedButtonGetstartpoint)
	ON_BN_CLICKED(IDC_BUTTON_GETENDPOINT, &CDlgEntrance::OnBnClickedButtonGetendpoint)
	ON_BN_CLICKED(IDOK, &CDlgEntrance::OnBnClickedOk)
END_MESSAGE_MAP()

//-----------------------------------------------------------------------------
CDlgEntrance::CDlgEntrance (CWnd *pParent /*=NULL*/, HINSTANCE hInstance /*=NULL*/) : CAcUiDialog (CDlgEntrance::IDD, pParent, hInstance) {
}

//-----------------------------------------------------------------------------
void CDlgEntrance::DoDataExchange (CDataExchange *pDX) {
	CAcUiDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_STARTPOINTSHOW, m_StartPointEdit);
	DDX_Control(pDX, IDC_EDIT_ENDPOINTSHOW, m_EndPointEdit);
	DDX_Control(pDX, IDC_EDIT_LIMITHEIGHT, m_LimitHeight);
	DDX_Control(pDX, IDC_EDIT_ENTRANCEWIDTH, m_EntranceWidth);
	DDX_Control(pDX, IDC_EDIT_ONEGENTLESLOPE, m_OneGentleSlope);
	DDX_Control(pDX, IDC_EDIT_TWOGENTLESLOPE, m_TwoGentleSlope);
	DDX_Control(pDX, IDC_EDIT_THIREGENTLESLOPE, m_ThireGentleSlope);
	DDX_Control(pDX, IDC_EDIT_ONEHORIZONTALDISTANCE, m_OneHorizontalDistance);
	DDX_Control(pDX, IDC_EDIT_TWOHORIZONTALDISTANCE, m_TwoHorizontalDistance);
	DDX_Control(pDX, IDC_EDIT_THIREHORZONTALDISTANCE, m_ThireHorizontalDistance);
}

//-----------------------------------------------------------------------------
//----- Needed for modeless dialogs to keep focus.
//----- Return FALSE to not keep the focus, return TRUE to keep the focus
LRESULT CDlgEntrance::OnAcadKeepFocus (WPARAM, LPARAM) {
	return (TRUE) ;
}


void CDlgEntrance::OnBnClickedButtonGetstartpoint()
{
	// 拾取出入口起点
	HideDialogHolder holder(this);
	Doc_Locker doc_locker;
	CString strStartPoint;
	//提示用户输入一个点
	ads_point pt;
	if (acedGetPoint(NULL, _T("\n输入一个点："), pt) == RTNORM)
	{
		//如果点有效，继续执行
		CompleteEditorCommand();
		
		CString strXPt;
		CString strYPt;
		strXPt.Format(_T("%.2f"), pt[X]);
		strYPt.Format(_T("%.2f"), pt[Y]);
		strStartPoint = _T("(") + strXPt + _T(",") + strYPt + _T(")");
		//显示点的坐标	 
		m_StartPointEdit.SetWindowText(strStartPoint);
		dStartPtx = pt[X];
		dStartPty = pt[Y];
	}
	else
	{
		strStartPoint = "选取了无效的点";
		m_StartPointEdit.SetWindowText(strStartPoint);
	}
}


void CDlgEntrance::OnBnClickedButtonGetendpoint()
{
	// 拾取出入口终点
	// 拾取出入口起点
	HideDialogHolder holder(this);
	Doc_Locker doc_locker;
	CString strEndPoint;
	//提示用户输入一个点
	ads_point pt;
	if (acedGetPoint(NULL, _T("\n输入一个点："), pt) == RTNORM)
	{
		//如果点有效，继续执行
		CompleteEditorCommand();

		CString strXPt;
		CString strYPt;
		strXPt.Format(_T("%.2f"), pt[X]);
		strYPt.Format(_T("%.2f"), pt[Y]);
		strEndPoint = _T("(") + strXPt + _T(",") + strYPt + _T(")");
		//显示点的坐标	 
		m_EndPointEdit.SetWindowText(strEndPoint);
		dEndPtx = pt[X];
		dEndPty = pt[Y];
	}
	else
	{
		strEndPoint = "选取了无效的点";
		m_EndPointEdit.SetWindowText(strEndPoint);
	}
}

void CDlgEntrance::init()
{
	CString strLimitHeight = _T("2.2");
	m_LimitHeight.SetWindowText(strLimitHeight);

	CString strEntranceWidth = _T("5");
	m_EntranceWidth.SetWindowText(strEntranceWidth);

	CString strOneGentleSlope = _T("7.5");
	m_OneGentleSlope.SetWindowText(strOneGentleSlope);

	CString strTwoGentleSlope = _T("15");
	m_TwoGentleSlope.SetWindowText(strTwoGentleSlope);

	CString strThireGentleSlope = _T("7.5");
	m_ThireGentleSlope.SetWindowText(strThireGentleSlope);

	CString strOneHorizontalDistance = _T("10");
	m_OneHorizontalDistance.SetWindowText(strOneHorizontalDistance);

	CString strTwoHorizontalDistance = _T("10");
	m_TwoHorizontalDistance.SetWindowText(strTwoHorizontalDistance);

	CString strThireHorizontalDistance = _T("10");
	m_ThireHorizontalDistance.SetWindowText(strThireHorizontalDistance);
}


BOOL CDlgEntrance::OnInitDialog()
{
	CAcUiDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
	init();

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


void CDlgEntrance::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	CString strLimitHeight;
	m_LimitHeight.GetWindowText(strLimitHeight);
	CString strEntranceWidth;
	m_EntranceWidth.GetWindowText(strEntranceWidth);
	CString strOneGentleSlope;
	m_OneGentleSlope.GetWindowText(strOneGentleSlope);
	CString strTwoGentleSlope;
	m_TwoGentleSlope.GetWindowText(strTwoGentleSlope);
	CString strThireGentleSlope;
	m_ThireGentleSlope.GetWindowText(strThireGentleSlope);
	CString strOneHorizontalDistance;
	m_OneHorizontalDistance.GetWindowText(strOneHorizontalDistance);
	CString strTwoHorizontalDistance;
	m_TwoHorizontalDistance.GetWindowText(strTwoHorizontalDistance);
	CString strThireHorizontalDistance;
	m_ThireHorizontalDistance.GetWindowText(strThireHorizontalDistance);

	double dLimitHeight = _tstof(strLimitHeight.GetString());
	double dEntranceWidth = _tstof(strEntranceWidth.GetString());
	double dOneGentleSlope = _tstof(strOneGentleSlope.GetString());
	double dTwoGentleSlope = _tstof(strTwoGentleSlope.GetString());
	double dThireGentleSlope = _tstof(strThireGentleSlope.GetString());
	double dOneHorizontalDistance = _tstof(strOneHorizontalDistance.GetString());
	double dTwoHorizontalDistance = _tstof(strTwoHorizontalDistance.GetString());
	double dThireHorizontalDistance = _tstof(strThireHorizontalDistance.GetString());

	Json::Value root;//根节点
	//创建子节点
	Json::Value params;
	//字节点属性
	params["limit_height"] = Json::Value(dLimitHeight);
	params["entrance_width"] = Json::Value(dEntranceWidth);
	params["one_gentle_slope"] = Json::Value(dOneGentleSlope);
	params["two_gentle_slope"] = Json::Value(dTwoGentleSlope);
	params["three_gentle_slope"] = Json::Value(dThireGentleSlope);
	params["one_horizontal_distance"] = Json::Value(dOneHorizontalDistance);
	params["two_horizontal_distance"] = Json::Value(dTwoHorizontalDistance);
	params["three_horizontal_distance"] = Json::Value(dThireHorizontalDistance);
	Json::Value startpoint;
	startpoint.append(dStartPtx);
	startpoint.append(dStartPty);
	params["start_point"] = Json::Value(startpoint);
	Json::Value endpoint;
	endpoint.append(dEndPtx);
	endpoint.append(dEndPty);
	params["end_point"] = Json::Value(endpoint);
	//子节点挂到根节点上
	root["params"] = Json::Value(params);
	std::string strData = root.toStyledString();
	std::string uuid;
	int res = postToAIApi(strData, uuid,true);
	if (res != 0)
	{
		CString	sMsg = GL::Ansi2WideByte(uuid.c_str()).c_str();
		acedAlert(sMsg);
		return;
	}
	CAcUiDialog::OnOK();
}

void CDlgEntrance::setEntrancePostUrl(std::string& strEntrancePostUrl)
{
	ms_strEntrancePostUrlPort = strEntrancePostUrl;
}

void CDlgEntrance::setEntrancePostUrlV2(std::string& strEntrancePostUrlV2)
{
	ms_strEntrancePostUrlPortV2 = strEntrancePostUrlV2;
}

int CDlgEntrance::postToAIApi(const std::string& sData, std::string& sMsg, const bool& useV1)
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
		postUrl = ms_strEntrancePostUrlPort.c_str();
	}
	else
	{
		postUrl = ms_strEntrancePostUrlPortV2.c_str();
	}
	//MessageBoxA(NULL, postUrl, "", 0);
	int code = fn_post(postUrl, sData.c_str(), sData.size(), true, "application/json");
	if (code != 200)
	{
		if (useV1)
		{
			sMsg = ms_strEntrancePostUrlPort + ":网络或服务器错误。";
		}
		else
		{
			sMsg = ms_strEntrancePostUrlPortV2 + ":网络或服务器错误。";
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