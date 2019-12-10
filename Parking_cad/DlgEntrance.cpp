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

	double dLimitHeight = _ttof(strLimitHeight.GetString());
	double dEntranceWidth = _ttof(strEntranceWidth.GetString());
	double dOneGentleSlope = _ttof(strOneGentleSlope.GetString());
	double dTwoGentleSlope = _ttof(strTwoGentleSlope.GetString());
	double dThireGentleSlope = _ttof(strThireGentleSlope.GetString());
	double dOneHorizontalDistance = _ttof(strOneHorizontalDistance.GetString());
	double dTwoHorizontalDistance = _ttof(strTwoHorizontalDistance.GetString());
	double dThireHorizontalDistance = _ttof(strThireHorizontalDistance.GetString());

	Json::Value root;//根节点
	//创建子节点
	Json::Value params;
	//字节点属性
	params["limit_height"] = Json::Value(dLimitHeight);
	params["entrance_width"] = Json::Value(dEntranceWidth);
	params["one_gentle_slope"] = Json::Value(dOneGentleSlope);
	params["two_gentle_slope"] = Json::Value(dTwoGentleSlope);
	params["thire_gentle_slope"] = Json::Value(dThireGentleSlope);
	params["one_horizontal_distance"] = Json::Value(dOneHorizontalDistance);
	params["two_horizontalDistance"] = Json::Value(dTwoHorizontalDistance);
	params["thire_horizontal_distance"] = Json::Value(dThireHorizontalDistance);
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
}
