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
//----- DlgSetConfig.cpp : Implementation of CDlgSetConfig
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "resource.h"
#include "DlgSetConfig.h"

#include <fstream>
#include <iostream>
#include <json/json.h>
#include "DBHelper.h"
#include "FileHelper.h"
#include "EquipmentroomTool.h"
//-----------------------------------------------------------------------------
IMPLEMENT_DYNAMIC (CDlgSetConfig, CAcUiDialog)

BEGIN_MESSAGE_MAP(CDlgSetConfig, CAcUiDialog)
	ON_MESSAGE(WM_ACAD_KEEPFOCUS, OnAcadKeepFocus)
	ON_BN_CLICKED(IDOK, &CDlgSetConfig::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON_INIT, &CDlgSetConfig::OnBnClickedButtonInit)
END_MESSAGE_MAP()

//-----------------------------------------------------------------------------
CDlgSetConfig::CDlgSetConfig (CWnd *pParent /*=NULL*/, HINSTANCE hInstance /*=NULL*/) : CAcUiDialog (CDlgSetConfig::IDD, pParent, hInstance) {
}

//-----------------------------------------------------------------------------
void CDlgSetConfig::DoDataExchange (CDataExchange *pDX) {
	CAcUiDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_AXIS, m_EditAxis);
	DDX_Control(pDX, IDC_EDIT_AXISDIM, m_EditAxisDim);
	DDX_Control(pDX, IDC_EDIT_LANE, m_EditLane);
	DDX_Control(pDX, IDC_EDIT_LANEDIM, m_EditLaneDim);
	DDX_Control(pDX, IDC_EDIT_PARKINGS, m_EditParkings);
	DDX_Control(pDX, IDC_EDIT_ARROW, m_EditArrow);
	DDX_Control(pDX, IDC_EDIT_PILLAR, m_EditPillar);
	DDX_Control(pDX, IDC_EDIT_SCOPE, m_EditScope);
	DDX_Control(pDX, IDC_EDIT_MAPSIGN, m_EditMapSign);
	DDX_Control(pDX, IDC_EDIT_PICTUREFRAME, m_EditPictureFrame);
	DDX_Control(pDX, IDC_EDIT_EQUIPMENTROOM, m_EditEquipmentroom);
	DDX_Control(pDX, IDC_EDIT_COREWALL, m_EditCoreWall);
	DDX_Control(pDX, IDC_EDIT_ENTRANCE, m_EditEntrance);
}

//-----------------------------------------------------------------------------
//----- Needed for modeless dialogs to keep focus.
//----- Return FALSE to not keep the focus, return TRUE to keep the focus
LRESULT CDlgSetConfig::OnAcadKeepFocus (WPARAM, LPARAM) {
	return (TRUE) ;
}


void CDlgSetConfig::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码

	CString sAxisLayerName;
	m_EditAxis.GetWindowText(sAxisLayerName);
	std::string strAxisLayerName = CStringA(sAxisLayerName);
	CString sAxisDimLayerName;
	m_EditAxisDim.GetWindowText(sAxisDimLayerName);
	std::string strAxisDimLayerName = CStringA(sAxisDimLayerName);
	CString sLaneLayerName;
	m_EditLane.GetWindowText(sLaneLayerName);
	std::string strLaneLayerName = CStringA(sLaneLayerName);
	CString sLaneDimLayerName;
	m_EditLaneDim.GetWindowText(sLaneDimLayerName);
	std::string strLaneDimLayerName = CStringA(sLaneDimLayerName);
	CString sParkingsLayerName;
	m_EditParkings.GetWindowText(sParkingsLayerName);
	std::string strParkingsLayerName = CStringA(sParkingsLayerName);
	CString sArrowLayerName;
	m_EditArrow.GetWindowText(sArrowLayerName);
	std::string strArrowLayerName = CStringA(sArrowLayerName);
	CString sPillarLayerName;
	m_EditPillar.GetWindowText(sPillarLayerName);
	std::string strPillarLayerName = CStringA(sPillarLayerName);
	CString sScopeLayerName;
	m_EditScope.GetWindowText(sScopeLayerName);
	std::string strScopeLayerName = CStringA(sScopeLayerName);
	CString sMapSignLayerName;
	m_EditMapSign.GetWindowText(sMapSignLayerName);
	std::string strMapSignLayerName = CStringA(sMapSignLayerName);
	CString sPictureFrameLayerName;
	m_EditPictureFrame.GetWindowText(sPictureFrameLayerName);
	std::string strPictureFrameLayerName = CStringA(sPictureFrameLayerName);
	CString sEquipmentroomLayerName;
	m_EditEquipmentroom.GetWindowText(sEquipmentroomLayerName);
	std::string strEquipmentroomLayerName = CStringA(sEquipmentroomLayerName);
	CString sCoreWallLayerName;
	m_EditCoreWall.GetWindowText(sCoreWallLayerName);
	std::string strCoreWallLayerName = CStringA(sCoreWallLayerName);
	CString sEntranceLayerName;
	m_EditEntrance.GetWindowText(sEntranceLayerName);
	std::string strEntranceLayerName = CStringA(sEntranceLayerName);

	CString sOldAxisLayerName(CEquipmentroomTool::getLayerName("axislayer").c_str());
	changeLayerName(sOldAxisLayerName, sAxisLayerName);
	CString sOldAxisDimLayerName(CEquipmentroomTool::getLayerName("axisdimlayer").c_str());
	changeLayerName(sOldAxisDimLayerName, sAxisDimLayerName);
	CString sOldLaneLayerName(CEquipmentroomTool::getLayerName("lanelayer").c_str());
	changeLayerName(sOldLaneLayerName, sLaneLayerName);
	CString sOldLaneDimLayerName(CEquipmentroomTool::getLayerName("lanesdimlayer").c_str());
	changeLayerName(sOldLaneDimLayerName, sLaneDimLayerName);
	CString sOldParkingsLayerName(CEquipmentroomTool::getLayerName("parkingslayer").c_str());
	changeLayerName(sOldParkingsLayerName, sParkingsLayerName);
	CString sOldArrowLayerName(CEquipmentroomTool::getLayerName("arrowlayer").c_str());
	changeLayerName(sOldArrowLayerName, sArrowLayerName);
	CString sOldPillarLayerName(CEquipmentroomTool::getLayerName("pillarlayer").c_str());
	changeLayerName(sOldPillarLayerName, sPillarLayerName);
	CString sOldScopeLayerName(CEquipmentroomTool::getLayerName("scopelayer").c_str());
	changeLayerName(sOldScopeLayerName, sScopeLayerName);
	CString sOldMapSignLayerName(CEquipmentroomTool::getLayerName("mapsignlayer").c_str());
	changeLayerName(sOldMapSignLayerName, sMapSignLayerName);
	CString sOldPictureFrameLayerName(CEquipmentroomTool::getLayerName("pictureframelayer").c_str());
	changeLayerName(sOldPictureFrameLayerName, sPictureFrameLayerName);
	CString sOldEquipmentroomLayerName(CEquipmentroomTool::getLayerName("equipmentroomlayer").c_str());
	changeLayerName(sOldEquipmentroomLayerName, sEquipmentroomLayerName);
	CString sOldCoreWallLayerName(CEquipmentroomTool::getLayerName("corewalllayer").c_str());
	changeLayerName(sOldCoreWallLayerName, sCoreWallLayerName);
	CString sOldEntranceLayerName(CEquipmentroomTool::getLayerName("entrancelayer").c_str());
	changeLayerName(sOldEntranceLayerName, sEntranceLayerName);

	Json::Value root;//根节点
	Json::Value params;
	//字节点属性
	params["posturl"] = Json::Value(m_strUiPostUrl);
	params["geturl"] = Json::Value(m_strUiGetUrl);
	params["entrance_posturl"] = Json::Value(m_strEntranceUrl);
	root["params"] = Json::Value(params);
	Json::Value layernames;
	//字节点属性
	layernames["axislayer"] = Json::Value(strAxisLayerName);//轴线图层
	layernames["axisdimlayer"] = Json::Value(strAxisDimLayerName);//轴网标注图层
	layernames["lanelayer"] = Json::Value(strLaneLayerName);//车道图层
	layernames["lanesdimlayer"] = Json::Value(strLaneDimLayerName);//车道标注图层
	layernames["arrowlayer"] = Json::Value(strArrowLayerName);//车道指示箭头图层
	layernames["parkingslayer"] = Json::Value(strParkingsLayerName);//车位图层
	layernames["pillarlayer"] = Json::Value(strPillarLayerName);//方柱图层
	layernames["scopelayer"] = Json::Value(strScopeLayerName);//地库外轮廓图层
	layernames["pictureframelayer"] = Json::Value(strPictureFrameLayerName);//图框图层
	layernames["mapsignlayer"] = Json::Value(strMapSignLayerName);//图签图层
	layernames["equipmentroomlayer"] = Json::Value(strEquipmentroomLayerName);//设备房图层
	layernames["corewalllayer"] = Json::Value(strCoreWallLayerName);//设备房图层
	layernames["entrancelayer"] = Json::Value(strEntranceLayerName);//出入口图层

	root["layernames"] = Json::Value(layernames);
	//输出到文件
	Json::StyledWriter sw;
	std::ofstream os;
	std::string sConfigFile = DBHelper::GetArxDirA() + "ParkingConfig.json";
	os.open(sConfigFile.c_str());
	if (!os.is_open())
	{
		acedAlert(_T("设置配置参数失败"));
		return;
	}
	os << sw.write(root);
	os.close();
	CAcUiDialog::OnOK();
}


BOOL CDlgSetConfig::OnInitDialog()
{
	CAcUiDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
	init();
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}

void CDlgSetConfig::init()
{

	//从文件中读取
	std::string sConfigFile = DBHelper::GetArxDirA() + "ParkingConfig.json";
	std::string sConfigStr = FileHelper::ReadText(sConfigFile.c_str());
	Json::Reader reader;
	Json::Value root;
	if (reader.parse(sConfigStr, root))
	{
		if (root["params"]["posturl"].isNull() || root["params"]["geturl"].isNull())
		{
			acedAlert(_T("获取上次数据失败！"));
			return;
		}
		if (root["params"]["posturl"].isString() && root["params"]["geturl"].isString())
		{
			m_strUiPostUrl = root["params"]["posturl"].asString();
			m_strUiGetUrl = root["params"]["geturl"].asString();
			m_strEntranceUrl = root["params"]["entrance_posturl"].asString();
		}
		else
		{
			acedAlert(_T("数据格式不匹配！"));
			return;
		}
		if (root["layernames"]["arrowlayer"].isNull() || root["layernames"]["parkingslayer"].isNull())
		{
			acedAlert(_T("获取上次数据失败！"));
			return;
		}
		if (root["layernames"]["arrowlayer"].isString() && root["layernames"]["parkingslayer"].isString())
		{
			std::string strAxisLayerName = root["layernames"]["axislayer"].asString();
			std::string strAxisDimLayerName = root["layernames"]["axisdimlayer"].asString();
			std::string strLaneLayerName = root["layernames"]["lanelayer"].asString();
			std::string strLaneDimLayerName = root["layernames"]["lanesdimlayer"].asString();
			std::string strArrowLayerName = root["layernames"]["arrowlayer"].asString();
			std::string strParkingsLayerName = root["layernames"]["parkingslayer"].asString();
			std::string strPillarLayerName = root["layernames"]["pillarlayer"].asString();
			std::string strScopeLayerName = root["layernames"]["scopelayer"].asString();
			std::string strPictureFrameLayerName = root["layernames"]["pictureframelayer"].asString();
			std::string strMapSignLayerName = root["layernames"]["mapsignlayer"].asString();
			std::string strEquipmentroomLayerName = root["layernames"]["equipmentroomlayer"].asString();
			std::string strCoreWallLayerName = root["layernames"]["corewalllayer"].asString();
			std::string strEntranceLayerName = root["layernames"]["entrancelayer"].asString();

			CString sAxisLayerName(strAxisLayerName.c_str());
			m_EditAxis.SetWindowText(sAxisLayerName);
			CString sAxisDimLayerName(strAxisDimLayerName.c_str());
			m_EditAxisDim.SetWindowText(sAxisDimLayerName);
			CString sLaneLayerName(strLaneLayerName.c_str());
			m_EditLane.SetWindowText(sLaneLayerName);
			CString sLaneDimLayerName(strLaneDimLayerName.c_str());
			m_EditLaneDim.SetWindowText(sLaneDimLayerName);
			CString sArrowLayerName(strArrowLayerName.c_str());
			m_EditArrow.SetWindowText(sArrowLayerName);
			CString sParkingsLayerName(strParkingsLayerName.c_str());
			m_EditParkings.SetWindowText(sParkingsLayerName);
			CString sPillarLayerName(strPillarLayerName.c_str());
			m_EditPillar.SetWindowText(sPillarLayerName);
			CString sScopeLayerName(strScopeLayerName.c_str());
			m_EditScope.SetWindowText(sScopeLayerName);
			CString sMapSignLayerName(strMapSignLayerName.c_str());
			m_EditMapSign.SetWindowText(sMapSignLayerName);
			CString sPictureFrameLayerName(strPictureFrameLayerName.c_str());
			m_EditPictureFrame.SetWindowText(sPictureFrameLayerName);
			CString sEquipmentroomLayerName(strEquipmentroomLayerName.c_str());
			m_EditEquipmentroom.SetWindowText(sEquipmentroomLayerName);
			CString sCoreWallLayerName(strCoreWallLayerName.c_str());
			m_EditCoreWall.SetWindowText(sCoreWallLayerName);
			CString sEntranceLayerName(strEntranceLayerName.c_str());
			m_EditEntrance.SetWindowText(sEntranceLayerName);

		}
		else
		{
			acedAlert(_T("数据格式不匹配！"));
			return;
		}
	}
	else
	{
		acedAlert(_T("获取数据失败！"));
		return;
	}	
}


void CDlgSetConfig::OnBnClickedButtonInit()
{
	// TODO: 在此添加控件通知处理程序代码

	CString strAxisLayerName = _T("axis");
	m_EditAxis.SetWindowText(strAxisLayerName);

	CString strAxisDimLayerName = _T("axisDim");
	m_EditAxisDim.SetWindowText(strAxisDimLayerName);

	CString strLaneLayerName = _T("lane");
	m_EditLane.SetWindowText(strLaneLayerName);

	CString strLaneDimLayerName = _T("laneDim");
	m_EditLaneDim.SetWindowText(strLaneDimLayerName);

	CString strParkingsLayerName = _T("parkings");
	m_EditParkings.SetWindowText(strParkingsLayerName);

	CString strArrowLayerName = _T("arrow");
	m_EditArrow.SetWindowText(strArrowLayerName);

	CString strPillarLayerName = _T("pillar");
	m_EditPillar.SetWindowText(strPillarLayerName);

	CString strScopeLayerName = _T("scope");
	m_EditScope.SetWindowText(strScopeLayerName);

	CString strMapSignLayerName = _T("图签");
	m_EditMapSign.SetWindowText(strMapSignLayerName);

	CString strPictureFrameLayerName = _T("图框");
	m_EditPictureFrame.SetWindowText(strPictureFrameLayerName);

	CString strEquipmentroomLayerName = _T("设备房");
	m_EditEquipmentroom.SetWindowText(strEquipmentroomLayerName);

	CString strCoreWallLayerName = _T("CoreWall");
	m_EditCoreWall.SetWindowText(strCoreWallLayerName);

	CString strEntranceLayerName = _T("entrance");
	m_EditEntrance.SetWindowText(strEntranceLayerName);
}

void CDlgSetConfig::changeLayerName(const CString& oldLayerName, const CString& newLayerName)
{
	Doc_Locker _locker;
	AcDbLayerTable *pLayerTbl;
	//获取当前图形层表
	Acad::ErrorStatus es;
	es = acdbCurDwg()->getLayerTable(pLayerTbl, AcDb::kForWrite);
	if (es != eOk)
	{
		return;
	}
	if (pLayerTbl->has(oldLayerName))
	{
		AcDbLayerTableRecord *pLTR = NULL;
		es = pLayerTbl->getAt(oldLayerName, pLTR, AcDb::kForWrite);
		if (es != eOk)
		{
			pLayerTbl->close();
			return;
		}
		es = pLTR->upgradeOpen();
		es = pLTR->setName(newLayerName);
		es = pLTR->downgradeOpen();
		pLTR->close();
	}
	pLayerTbl->close();
}
