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
#include "EquipmentroomTool.h"


std::string CDlgEntrance::ms_strEntrancePostUrlPort;
std::string CDlgEntrance::ms_strEntrancePostUrlPortV2;

void CDlgEntrance::deletParkingForEntrance(std::map<AcDbObjectId, AcGePoint2d>& parkingIdAndPt, AcGePoint2dArray& useDeletParkingPts)
{
	AcDbObjectIdArray allParkingIds = DBHelper::GetEntitiesByLayerName(_T("parkings"));
	std::map<AcDbObjectId, AcGePoint2d> newParkingAndPt;
	AcGePoint2dArray parkingPts;
	AcDbObjectIdArray parkingIds;//图形界面剩余车位ID
	for (int i=0; i<allParkingIds.length(); i++)
	{
		if (parkingIdAndPt.count(allParkingIds[i]) > 0)
		{
			AcDbObjectId keyId = allParkingIds[i];
			AcGePoint2d tempPts = parkingIdAndPt[keyId];
			std::pair<AcDbObjectId, AcGePoint2d> value(keyId, tempPts);
			newParkingAndPt.insert(value);
			parkingPts.append(tempPts);
			parkingIds.append(keyId);
		}	  
	}
	parkingIdAndPt.clear();
	AcDbObjectIdArray deletIds;
	for (int j=0; j<parkingPts.length(); j++)
	{
		if (GeHelper::IsPointOnPolygon(useDeletParkingPts, parkingPts[j]))
		{
			std::map<AcDbObjectId, AcGePoint2d>::iterator tempIter;
			for (tempIter = newParkingAndPt.begin(); tempIter != newParkingAndPt.end(); tempIter++)
			{
				if (tempIter->second == parkingPts[j])
				{
					AcDbObjectId deletId = tempIter->first;
					deletIds.append(deletId);
					AcDbEntity* pEntity = NULL;
					if (acdbOpenAcDbEntity(pEntity, deletId, kForWrite) != eOk)							//这里是可读
					{
						acutPrintf(_T("获取单个实体指针失败！"));
						continue;
					}
					if (pEntity->isKindOf(AcDbBlockReference::desc()))
					{
						pEntity->erase();
					}
					pEntity->close();
				}
			}
		}	
	}
	AcDbObjectIdArray lineSelectUseIds;
	for (int g=0; g<parkingIds.length(); g++)
	{
		if (deletIds.contains(parkingIds[g]))
		{
			continue;  
		}
		lineSelectUseIds.append(parkingIds[g]);
	}
	deletParkingByLineSelect(lineSelectUseIds, useDeletParkingPts);
}

void CDlgEntrance::deletParkingByLineSelect(const AcDbObjectIdArray& parkingIds, AcGePoint2dArray& useDeletParkingPts)
{
	//只能选到该id数组中的实体
	m_parkingSel.init(parkingIds);
	std::vector<AcDbObjectIdArray> allNeedDeletParkingIds;

	useDeletParkingPts.removeLast();
	AcGeVector2dArray vecs;
	for (int a=0; a<useDeletParkingPts.length(); a++)
	{
		AcGeVector2d vec = useDeletParkingPts[(a + 1) % useDeletParkingPts.length()] - useDeletParkingPts[a];
		vec.rotateBy(ARX_PI / 4);
		vecs.append(vec);
	}

	int n = vecs.length();

	for (int b=0; b<vecs.length(); b++)
	{
		AcGeVector2d unitvec = vecs[b].normalize();
		useDeletParkingPts[b].transformBy(unitvec * 5);
	}

	for (int i=0; i<useDeletParkingPts.length();i++)
	{
		AcDbObjectIdArray sideSelectIds = m_parkingSel.select(useDeletParkingPts[i], useDeletParkingPts[(i + 1) % useDeletParkingPts.length()]);
		allNeedDeletParkingIds.push_back(sideSelectIds);
	}
	for (int x=0; x<allNeedDeletParkingIds.size(); x++)
	{
		for (int y=0; y<allNeedDeletParkingIds[x].length(); y++)
		{
			AcDbEntity* pEntity = NULL;
			if (acdbOpenAcDbEntity(pEntity, allNeedDeletParkingIds[x][y], kForWrite) != eOk)//这里只读即可
			{
				continue;
			}
			pEntity->erase();
			pEntity->close();
		}
	}
}

void CDlgEntrance::getParkingIdAndPtMap(std::map < AcDbObjectId, AcGePoint2d>& parkingIdAndPtMap)
{
	AcDbObjectIdArray allParkingIds = DBHelper::GetEntitiesByLayerName(_T("parkings"));
	for (int i=0; i<allParkingIds.length(); i++)
	{
		AcDbEntity* pEntity = NULL;
		if (acdbOpenAcDbEntity(pEntity, allParkingIds[i], kForRead) != eOk)//这里只读即可
		{
			acutPrintf(_T("获取单个车位信息操作失败！"));
			continue;
		}
		if (pEntity->isKindOf(AcDbBlockReference::desc()))
		{
			AcDbBlockReference *pBlcRef = AcDbBlockReference::cast(pEntity);
			AcGePoint3d pInsert = pBlcRef->position();
			AcGePoint2d tempPts(pInsert.x, pInsert.y);
			std::pair<AcDbObjectId, AcGePoint2d> value(allParkingIds[i], tempPts);
			parkingIdAndPtMap.insert(value);
		}
		pEntity->close();
	}
}

void CDlgEntrance::showEntrance(const AcGePoint2dArray& oneEntrancePts)
{
	CString sEntranceLayer(CEquipmentroomTool::getLayerName("entrancelayer").c_str());
	CEquipmentroomTool::layerSet(sEntranceLayer, 4);
	AcDbPolyline *pPoly = new AcDbPolyline(oneEntrancePts.length());
	double width = 0;//线宽
	for (int i = 0; i < oneEntrancePts.length(); i++)
	{
		pPoly->addVertexAt(0, oneEntrancePts[i], 0, width, width);
	}
	pPoly->setClosed(true);
	AcDbObjectId entranceId;
	DBHelper::AppendToDatabase(entranceId, pPoly);
	pPoly->close();
	CEquipmentroomTool::setEntToLayer(entranceId, _T("entrance"));
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
	int re = acedGetPoint(NULL, _T("\n请输入一个点作为出入口起始点："), pt);
	if (re == RTNORM)
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
	else if (re == RTCAN)
	{
		strStartPoint = "";
		m_StartPointEdit.SetWindowText(strStartPoint);
		return;
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
	int re = acedGetPoint(NULL, _T("\n输入一个点作为出入终止点："), pt);
	if (re == RTNORM)
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
	else if (re == RTCAN)
	{
		strEndPoint = "";
		m_EndPointEdit.SetWindowText(strEndPoint);
		return;
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
	CString sStartPt;
	m_StartPointEdit.GetWindowText(sStartPt);
	if (sStartPt == _T(""))
	{
		acedAlert(_T("没有选择出入口起始点信息!"));
		return;
	}
	CString strEndPt;
	m_EndPointEdit.GetWindowText(strEndPt);
	if (strEndPt == _T(""))
	{
		acedAlert(_T("没有选择出入口终止点信息!"));
		return;
	}

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
			if (ms_strEntrancePostUrlPort=="")
			{
				sMsg = "请先进行车位排布操作！";
			}
			else
			{
				sMsg = ms_strEntrancePostUrlPort + ":网络或服务器错误。";
			}		
		}
		else
		{
			if (ms_strEntrancePostUrlPortV2 == "")
			{
				sMsg = "请先进行车位排布操作！";
			}
			else
			{
				sMsg = ms_strEntrancePostUrlPortV2 + ":网络或服务器错误。";
			}
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
	AcGePoint2dArray delParkingPoints;
	std::vector<AcGePoint2dArray> showParkingPoints;
	if (reader.parse(json, root))
	{
		Json::Value& delCells = root["ent_for_del_cells"];
		if (delCells.isArray())
		{
			int delCellsSize = delCells.size();
			for (int a = 0; a < delCellsSize; a++)
			{
				if (delCells[a].isArray())
				{									
					double ptX = delCells[a][0].asDouble();
					double ptY = delCells[a][1].asDouble();
					AcGePoint2d tempPt(ptX, ptY);
					delParkingPoints.append(tempPt);
				}
			}
		}
		else
		{
			sMsg = "没有返回ent_for_del_cells字段。";
			return 4;
		}
		Json::Value& entShow = root["ent_for_show"];
		if (entShow.isArray())
		{
			int entShowSize = entShow.size();
			for (int x=0; x<entShowSize; x++)
			{
				int oneEntShowCount = entShow[x].size();
				AcGePoint2dArray oneEntShowPts;
				for (int y=0; y<oneEntShowCount;y++)
				{				
					if (entShow[x][y].isArray())
					{											
							if (entShow[x][y][0].isDouble())
							{
								double ptX = entShow[x][y][0].asDouble();
								double ptY = entShow[x][y][1].asDouble();
								AcGePoint2d tempPt(ptX, ptY);
								oneEntShowPts.append(tempPt);
							}												
					}
					//装入容器
				}
				showParkingPoints.push_back(oneEntShowPts);
			}
		}
		else
		{
			sMsg = "没有返回ent_for_del_cells字段。";
			return 4;
		}
		int gg = delParkingPoints.length();
		int jj = showParkingPoints.size();
		Doc_Locker _locker;
		CEquipmentroomTool::layerSet(_T("0"), 7);
		std::map < AcDbObjectId, AcGePoint2d> parkingIdAndPtMap;
		getParkingIdAndPtMap(parkingIdAndPtMap);
		deletParkingForEntrance(parkingIdAndPtMap, delParkingPoints);
		for (int b=0; b<showParkingPoints.size(); b++)
		{
			showEntrance(showParkingPoints[b]);
		}
		CEquipmentroomTool::layerSet(_T("0"), 7);
		return 0;
	}
	sMsg = "json解析错误";
	return 4;
}