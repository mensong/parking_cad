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
#include "OperaSetConfig.h"
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC (CDlgSetConfig, CAcUiDialog)

BEGIN_MESSAGE_MAP(CDlgSetConfig, CAcUiDialog)
	ON_MESSAGE(WM_ACAD_KEEPFOCUS, OnAcadKeepFocus)
	ON_BN_CLICKED(IDOK, &CDlgSetConfig::OnBnClickedOk)
	ON_NOTIFY(NM_DBLCLK, IDC_LAYERLIST, &CDlgSetConfig::OnNMDblclkLayerlist)
	ON_NOTIFY(NM_CLICK, IDC_LAYERLIST, &CDlgSetConfig::OnNMClickLayerlist)
	ON_WM_HSCROLL()
	ON_CBN_KILLFOCUS(IDC_COMBO_LINETYPE, &CDlgSetConfig::OnCbnKillfocusComboLinetype)
	ON_CBN_KILLFOCUS(IDC_COMBO_LINEWIDTH, &CDlgSetConfig::OnCbnKillfocusComboLinewidth)
	ON_CBN_KILLFOCUS(IDC_COMBO_CONFIGCHOOSE, &CDlgSetConfig::OnCbnKillfocusComboConfigchoose)
	ON_EN_KILLFOCUS(IDC_EDIT_LIST, &CDlgSetConfig::OnEnKillfocusEditList)
END_MESSAGE_MAP()

//-----------------------------------------------------------------------------
CDlgSetConfig::CDlgSetConfig (CWnd *pParent /*=NULL*/, HINSTANCE hInstance /*=NULL*/) : CAcUiDialog (CDlgSetConfig::IDD, pParent, hInstance) {
	m_nLastRow = -1;
	m_nLastCol = -1;
}

static void __smartLog(bool*& data, bool isDataValid)
{
	if (isDataValid)
	{
		bool end = (*data);
		if (!end)
			CParkingLog::AddLogA("DEBUG_不支持尝试执行的操作", 0, "CDlgSetConfig::DoDataExchange");
		delete data;
	}
}

//-----------------------------------------------------------------------------
void CDlgSetConfig::DoDataExchange (CDataExchange *pDX) {
	Smart<bool*> end(new bool(false), __smartLog);
	CAcUiDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LAYERLIST, m_ctrlConfigSetList);
	DDX_Control(pDX, IDC_EDIT_LIST, m_EditTest);
	DDX_Control(pDX, IDC_COMBO_CONFIGCHOOSE, m_PrintableCombo);
	DDX_Control(pDX, IDC_COMBO_LINEWIDTH, m_LineWidthCombo);
	DDX_Control(pDX, IDC_COMBO_LINETYPE, m_LineTypeCombo);
	*(end()) = true;
}

//-----------------------------------------------------------------------------
//----- Needed for modeless dialogs to keep focus.
//----- Return FALSE to not keep the focus, return TRUE to keep the focus
LRESULT CDlgSetConfig::OnAcadKeepFocus (WPARAM, LPARAM) {
	return (TRUE) ;
}


void CDlgSetConfig::OnBnClickedOk()
{
	OnEditerEnter();

    std::vector<std::vector<std::string>> items;
    int max_row = 0, max_col = 0;
	if (m_ctrlConfigSetList.GetHeaderCtrl())
	{
		max_col = m_ctrlConfigSetList.GetHeaderCtrl()->GetItemCount();
		max_row = m_ctrlConfigSetList.GetItemCount();
		if (max_col > 0 && max_row > 0)
		{
			for (int Item = 0; Item < max_row; Item++)
			{
				std::vector <std::string> pargarms;
				for (int subItem = 0; subItem < max_col; subItem++)
				{
					CString ItemStr = m_ctrlConfigSetList.GetItemText(Item, subItem);
					std::string strItem = CStringA(ItemStr);
					pargarms.push_back(strItem);
				}
				items.push_back(pargarms);
			}
		}
	}
	int oo = items.size();

	std::vector<std::string> layerconfigs;
	layerconfigs.push_back("axis_dimensions");
	layerconfigs.push_back("arrow");
	layerconfigs.push_back("mapsign");
	layerconfigs.push_back("column");
	layerconfigs.push_back("pictureframe");
	layerconfigs.push_back("dimensions_dimensiontext");
	layerconfigs.push_back("equipmentroom");
	layerconfigs.push_back("core_wall");
	layerconfigs.push_back("lane_center_line_and_driving_direction");
	layerconfigs.push_back("entrance");
	layerconfigs.push_back("ordinary_parking");
	layerconfigs.push_back("parking_axis");
	layerconfigs.push_back("rangeline");
	layerconfigs.push_back("cloud_line");
	std::vector<CString> oldLayerName;
	for (int a = 0; a < layerconfigs.size(); a++)
	{
		CString sOldTempLayerName(CEquipmentroomTool::getLayerNameByJson(layerconfigs[a]).c_str());
		oldLayerName.push_back(sOldTempLayerName);
	}

	for (int aCount = 0; aCount < items.size(); aCount++)
	{
		CString sTemp(items[aCount][3].c_str());
		CString sTemp1(items[aCount][4].c_str());
		CString sTemp2(items[aCount][5].c_str());
		CString sTemp3(items[aCount][6].c_str());
		CString sTemp4(items[aCount][7].c_str());
		CString sTemp5(items[aCount][2].c_str());
		changeLayerName(oldLayerName[aCount], sTemp5, sTemp, sTemp2, sTemp1, sTemp4, sTemp3);
	}
// 
// 	//添加线型数组
// 	m_root["alllinetype"]["tag"] = linetypearray;

	//Json::Value root;//根节点
	Json::Value params;
	//字节点属性
	params["posturl"] = Json::Value(m_strUiPostUrl);
	params["geturl"] = Json::Value(m_strUiGetUrl);
	params["part_posturl"] = Json::Value(m_strUiPartPostUrl);
	params["part_geturl"] = Json::Value(m_strUiPartGetUrl);

	m_root["params"] = Json::Value(params);

	Json::Value layer_config;
	for (int i=0; i<items.size();i++)
	{
		Json::Value temp;
		temp["professional_attributes"] = Json::Value(items[i][1]);
		temp["layer_name"] = Json::Value(items[i][2]);
		temp["layer_color"] = Json::Value(items[i][3]);
		temp["layer_linetype"] = Json::Value(items[i][4]);
		temp["layer_width"] = Json::Value(items[i][5]);
		temp["isprintf"] = Json::Value(items[i][6]);
		temp["transparency"] = Json::Value(items[i][7]);
		layer_config[layerconfigs[i]] = Json::Value(temp);
		m_root["layer_config"] = Json::Value(layer_config);
	}

	std::string strData = m_root.toStyledString();
	Json::StyledWriter sw;
	std::ofstream os;
	std::string sConfigFile = CEquipmentroomTool::getConfigPath();
	os.open(sConfigFile.c_str());
	if (!os.is_open())
	{
		acedAlert(_T("设置配置参数失败"));
		return;
	}
	os << sw.write(m_root);
	os.close();
	//测试代码
	/*std::string strProfessionalAttributes = "";
	std::string strLayerName = "";
	std::string strLayerColor = "";
	std::string strLayerLinetype = "";
	std::string strLayerWidth = "";
	std::string strIsPrintf = "";
	std::string strTransparency = "";
	CEquipmentroomTool::getLayerConfigForJson("ordinary_parking", strProfessionalAttributes, strLayerName, strLayerColor, strLayerLinetype, strLayerWidth, strIsPrintf, strTransparency);
	CString sProfessionalAttributes(strProfessionalAttributes.c_str());
	CString sLayerName(strLayerName.c_str());
	CString sLayerColor(strLayerColor.c_str());
	CString sLayerLinetype(strLayerLinetype.c_str());
	CString sLayerWidth(strLayerWidth.c_str());
	CString sIsPrintf(strIsPrintf.c_str());
	CString sTransparency(strTransparency.c_str());
	CString sCount;
	CEquipmentroomTool::layerConfigSet(sLayerName, sLayerColor, sLayerWidth, sLayerLinetype, sTransparency, sIsPrintf);*/

	DBHelper::CallCADCommand(_T("REGEN "));
	CAcUiDialog::OnOK();
}


BOOL CDlgSetConfig::OnInitDialog()
{
	CAcUiDialog::OnInitDialog();
	
	DlgHelper::AdjustPosition(this, DlgHelper::CENTER);

	init();

	//加载天正所有线型
	COperaSetConfig::loadAllLinetype();
	initLinetypeCombo();

	m_EditTest.ShowWindow(SW_HIDE);
	m_PrintableCombo.ShowWindow(SW_HIDE);
	m_LineWidthCombo.ShowWindow(SW_HIDE);
	m_LineTypeCombo.ShowWindow(SW_HIDE);
	// TODO:  在此添加额外的初始化
	
	m_PrintableCombo.SetDroppedWidth(100);
	m_LineWidthCombo.SetDroppedWidth(100);

	int nRow = 0;// m_ConfigChooseCombo.AddString(_T("是否打印"));
	nRow = m_PrintableCombo.AddString(_T("是"));
	nRow = m_PrintableCombo.AddString(_T("否"));

	int nLineWidth = 0;
	nLineWidth = m_LineWidthCombo.AddString(_T("默认"));
	nLineWidth = m_LineWidthCombo.AddString(_T("0.09"));
	nLineWidth = m_LineWidthCombo.AddString(_T("0.15"));
	nLineWidth = m_LineWidthCombo.AddString(_T("0.2"));
	nLineWidth = m_LineWidthCombo.AddString(_T("0.25"));
	nLineWidth = m_LineWidthCombo.AddString(_T("0.35"));
	nLineWidth = m_LineWidthCombo.AddString(_T("0.4"));

	/*int nLineType = 0;
	nLineType = m_LineTypeCombo.AddString(_T("CENTER"));
	nLineType = m_LineTypeCombo.AddString(_T("DASHED"));
	nLineType = m_LineTypeCombo.AddString(_T("CONTINUOUS"));
	nLineType = m_LineTypeCombo.AddString(_T("DASH"));
	nLineType = m_LineTypeCombo.AddString(_T("ACAD_ISO05W100"));
	nLineType = m_LineTypeCombo.AddString(_T("DOTE"));*/
	
	//m_ctrlConfigSetList.InsertColumn(0, _T(""), LVCFMT_LEFT, 0);
	//m_mpColumnName[_T("强排专业实体")] = m_ctrlConfigSetList.InsertColumn(m_ctrlConfigSetList.GetHeaderCtrl()->GetItemCount(), _T("强排专业实体"), LVCFMT_CENTER, 150);
	m_mpColumnName[_T("序号")] = m_ctrlConfigSetList.InsertColumn(m_ctrlConfigSetList.GetHeaderCtrl()->GetItemCount(), _T("序号"), LVCFMT_CENTER, 40);
	m_mpColumnName[_T("专业属性")] = m_ctrlConfigSetList.InsertColumn(m_ctrlConfigSetList.GetHeaderCtrl()->GetItemCount(), _T("专业属性"), LVCFMT_CENTER, 100);
	m_mpColumnName[_T("图层名称")] = m_ctrlConfigSetList.InsertColumn(m_ctrlConfigSetList.GetHeaderCtrl()->GetItemCount(), _T("名称"), LVCFMT_CENTER, 200);
	m_mpColumnName[_T("图层颜色")] = m_ctrlConfigSetList.InsertColumn(m_ctrlConfigSetList.GetHeaderCtrl()->GetItemCount(), _T("颜色"), LVCFMT_CENTER, 50);
	m_mpColumnName[_T("图层线型")] = m_ctrlConfigSetList.InsertColumn(m_ctrlConfigSetList.GetHeaderCtrl()->GetItemCount(), _T("线型"), LVCFMT_CENTER, 120);
	m_mpColumnName[_T("线宽")] = m_ctrlConfigSetList.InsertColumn(m_ctrlConfigSetList.GetHeaderCtrl()->GetItemCount(), _T("线宽"), LVCFMT_CENTER, 50);
	m_mpColumnName[_T("是否打印")] = m_ctrlConfigSetList.InsertColumn(m_ctrlConfigSetList.GetHeaderCtrl()->GetItemCount(), _T("是否打印"), LVCFMT_CENTER, 60);
	m_mpColumnName[_T("淡显")] = m_ctrlConfigSetList.InsertColumn(m_ctrlConfigSetList.GetHeaderCtrl()->GetItemCount(), _T("透明度"), LVCFMT_CENTER, 150);

	m_ctrlConfigSetList.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT); // 整行选择、网格线
	m_ctrlConfigSetList.SetRowHeight(20);

	CAcModuleResourceOverride resOverride;//资源定位
	m_SliderDialog = new CDlgSliderTransparency();
	m_SliderDialog->Create(CDlgSliderTransparency::IDD, &m_ctrlConfigSetList);
	m_SliderDialog->ShowWindow(SW_HIDE);

	std::vector<std::string> layerconfigs;
	layerconfigs.push_back("axis_dimensions");
	layerconfigs.push_back("arrow");
	layerconfigs.push_back("mapsign");
	layerconfigs.push_back("column");
	layerconfigs.push_back("pictureframe");
	layerconfigs.push_back("dimensions_dimensiontext");
	layerconfigs.push_back("equipmentroom");
	layerconfigs.push_back("core_wall");
	layerconfigs.push_back("lane_center_line_and_driving_direction");
	layerconfigs.push_back("entrance");
	layerconfigs.push_back("ordinary_parking");
	layerconfigs.push_back("parking_axis");
	layerconfigs.push_back("rangeline");
	layerconfigs.push_back("cloud_line");
	for (int i=0; i<layerconfigs.size();i++)
	{
		std::string strProfessionalAttributes = "";
		std::string strLayerName = "";
		std::string strLayerColor = "";
		std::string strLayerLinetype = "";
		std::string strLayerWidth = "";
		std::string strIsPrintf = "";
		std::string strTransparency = "";
		CEquipmentroomTool::getLayerConfigForJson(layerconfigs[i], strProfessionalAttributes, strLayerName, strLayerColor, strLayerLinetype, strLayerWidth, strIsPrintf, strTransparency);
		CString sProfessionalAttributes(strProfessionalAttributes.c_str());
		CString sLayerName(strLayerName.c_str());
		CString sLayerColor(strLayerColor.c_str());
		CString sLayerLinetype(strLayerLinetype.c_str());
		CString sLayerWidth(strLayerWidth.c_str());
		CString sIsPrintf(strIsPrintf.c_str());
		CString sTransparency(strTransparency.c_str());
		CString sCount;
		sCount.Format(_T("%d"), i+1);
		setListValueText(i, sCount, sProfessionalAttributes, sLayerName, sLayerColor, sLayerLinetype, sLayerWidth, sTransparency, sIsPrintf);
	}
	
	return TRUE;
}

void CDlgSetConfig::init()
{

	//从文件中读取
	std::string sConfigFile = CEquipmentroomTool::getConfigPath();
	std::string sConfigStr = FileHelper::ReadText(sConfigFile.c_str());
	Json::Reader reader;

	if (reader.parse(sConfigStr, m_root))
	{
		if (m_root["params"]["posturl"].isNull() || m_root["params"]["geturl"].isNull())
		{
			acedAlert(_T("获取上次数据失败！"));
			return;
		}
		if (m_root["params"]["posturl"].isString() && m_root["params"]["geturl"].isString())

		{
			m_strUiPostUrl = m_root["params"]["posturl"].asString();
			m_strUiGetUrl = m_root["params"]["geturl"].asString();
			m_strUiPartPostUrl = m_root["params"]["part_posturl"].asString();
			m_strUiPartGetUrl = m_root["params"]["part_geturl"].asString();
		}
		else
		{
			acedAlert(_T("数据格式不匹配！"));
			return;
		}

		if(m_root["alllinetype"]["tag"].isNull())
		{
			acedAlert(_T("获取线型数据失败！"));
			return;
		}
		else if (m_root["alllinetype"]["tag"].isArray())
		{
			linetypearray = m_root["alllinetype"]["tag"];
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


void CDlgSetConfig::changeLayerName(const CString& oldLayerName, const CString& newLayerName, const CString& layerColor, const CString& lineWidth,
	const CString& lineType, const CString& transparency, const CString& isPrint)
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
		AcCmColor color;//设置图层颜色
		int iLayerColor = _ttoi(layerColor);
		es = color.setColorIndex(iLayerColor);
		pLTR->setColor(color);
		if (lineWidth == _T("0.09"))
		{
			pLTR->setLineWeight(AcDb::kLnWt009); // 设置线宽为0.09
		}
		else if (lineWidth == _T("0.13"))
		{
			pLTR->setLineWeight(AcDb::kLnWt013); // 设置线宽为0.13
		}
		else if (lineWidth == _T("0.15"))
		{
			pLTR->setLineWeight(AcDb::kLnWt015); // 设置线宽为0.15
		}
		else if (lineWidth == _T("0.2"))
		{
			pLTR->setLineWeight(AcDb::kLnWt020); // 设置线宽为0.2
		}
		else if (lineWidth == _T("0.35"))
		{
			pLTR->setLineWeight(AcDb::kLnWt035); // 设置线宽为0.13
		}
		else if (lineWidth == _T("0.4"))
		{
			pLTR->setLineWeight(AcDb::kLnWt040); // 设置线宽为0.13
		}
		else
		{
			pLTR->setLineWeight(AcDb::kLnWtByLwDefault); // 设置线宽为默认
		}
		AcDbLinetypeTable *pLineTbl;
		es = acdbCurDwg()->getLinetypeTable(pLineTbl, AcDb::kForRead);
		if (es != eOk)
		{
			pLineTbl->close();
			pLTR->close();
			return ;
		}
		AcDbObjectId dashId; //dash线形ID，你自己去得到，在线形表中查询
		if (pLineTbl->getAt(lineType, dashId) != eOk)
		{
			pLineTbl->close();
			pLTR->close();
			pLayerTbl->close();
			return;
		}
		pLTR->setLinetypeObjectId(dashId); // 设置为dash线形
		int iTransparency = _ttoi(transparency);
		double dTransparency = (100 - iTransparency)*0.01;
		AcCmTransparency trans = AcCmTransparency(dTransparency);
		pLTR->setTransparency(trans);
		if (isPrint == _T("否") || isPrint == _T(""))
		{
			pLTR->setIsPlottable(false);
		}
		else
		{
			pLTR->setIsPlottable(true);
		}
		es = pLTR->downgradeOpen();
		pLineTbl->close();
		pLTR->close();
	}
	pLayerTbl->close();
}



void CDlgSetConfig::OnNMDblclkLayerlist(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码

	OnEditerEnter();
	if (m_nLastRow > -1 && m_nLastCol > -1)
	{
		m_ctrlConfigSetList.SetItemExCtrl(m_nLastRow, m_nLastCol, NULL, false, false, false);
		m_ctrlConfigSetList.SetItemExCtrlVisible(m_nLastRow, m_nLastCol, false, true);
		//m_nLastRow = -1;
		//m_nLastCol = -1;
	}

	//显示编辑框
	if (pNMItemActivate->iItem > -1 && pNMItemActivate->iSubItem > -1)
	{
		CString sOldText = m_ctrlConfigSetList.GetItemText(pNMItemActivate->iItem, pNMItemActivate->iSubItem);

		if (pNMItemActivate->iSubItem == m_mpColumnName[_T("专业属性")] || pNMItemActivate->iSubItem == m_mpColumnName[_T("图层名称")])
		{
			m_EditTest.SetWindowText(sOldText);
			m_EditTest.SetSel(0, sOldText.GetLength());
			m_ctrlConfigSetList.SetItemExCtrl(pNMItemActivate->iItem, pNMItemActivate->iSubItem, &m_EditTest, false, false, false);
			m_ctrlConfigSetList.SetItemExCtrlVisible(pNMItemActivate->iItem, pNMItemActivate->iSubItem, true, true);
			m_nLastRow = pNMItemActivate->iItem;
			m_nLastCol = pNMItemActivate->iSubItem;
			m_EditTest.SetFocus();
		}
		else if (pNMItemActivate->iSubItem == m_mpColumnName[_T("图层颜色")])
		{
			int num = _ttoi(sOldText);
			int nColor = CEquipmentroomTool::SelColor(num);
			CString sColor;
			sColor.Format(_T("%d"), nColor);
			m_ctrlConfigSetList.SetItemText(pNMItemActivate->iItem, pNMItemActivate->iSubItem, sColor);
			m_ctrlConfigSetList.SetFocus();
		}
		else if (pNMItemActivate->iSubItem == m_mpColumnName[_T("是否打印")])
		{
			m_PrintableCombo.SetCurSel(m_PrintableCombo.FindStringExact(0, sOldText));
			m_ctrlConfigSetList.SetItemExCtrl(pNMItemActivate->iItem, pNMItemActivate->iSubItem, &m_PrintableCombo, false, false, false);
			m_ctrlConfigSetList.SetItemExCtrlVisible(pNMItemActivate->iItem, pNMItemActivate->iSubItem, true, true);
			m_nLastRow = pNMItemActivate->iItem;
			m_nLastCol = pNMItemActivate->iSubItem;
			m_PrintableCombo.SetFocus();
		}
		else if (pNMItemActivate->iSubItem == m_mpColumnName[_T("线宽")])
		{
			m_LineWidthCombo.SetCurSel(m_LineWidthCombo.FindStringExact(0, sOldText));
			m_ctrlConfigSetList.SetItemExCtrl(pNMItemActivate->iItem, pNMItemActivate->iSubItem, &m_LineWidthCombo, false, false, false);
			m_ctrlConfigSetList.SetItemExCtrlVisible(pNMItemActivate->iItem, pNMItemActivate->iSubItem, true, true);
			m_nLastRow = pNMItemActivate->iItem;
			m_nLastCol = pNMItemActivate->iSubItem;
			m_LineWidthCombo.SetFocus();
		}
		else if (pNMItemActivate->iSubItem == m_mpColumnName[_T("图层线型")])
		{
			m_LineTypeCombo.SetCurSel(m_LineTypeCombo.FindStringExact(0, sOldText));
			m_ctrlConfigSetList.SetItemExCtrl(pNMItemActivate->iItem, pNMItemActivate->iSubItem, &m_LineTypeCombo, false, false, false);
			m_ctrlConfigSetList.SetItemExCtrlVisible(pNMItemActivate->iItem, pNMItemActivate->iSubItem, true, true);
			m_nLastRow = pNMItemActivate->iItem;
			m_nLastCol = pNMItemActivate->iSubItem;
			m_LineTypeCombo.SetFocus();
		}
		else if (pNMItemActivate->iSubItem == m_mpColumnName[_T("淡显")])
		{
			m_SliderDialog->SetPercent(sOldText);
			m_ctrlConfigSetList.SetItemExCtrl(pNMItemActivate->iItem, pNMItemActivate->iSubItem, m_SliderDialog, false, false, false);
			m_ctrlConfigSetList.SetItemExCtrlVisible(pNMItemActivate->iItem, pNMItemActivate->iSubItem, true, true);
			m_nLastRow = pNMItemActivate->iItem;
			m_nLastCol = pNMItemActivate->iSubItem;
			m_SliderDialog->SetFocus();
		}
	}
	*pResult = 0;
}



void CDlgSetConfig::OnNMClickLayerlist(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码

	OnEditerEnter();

	//单击隐藏编辑按钮
	m_EditTest.ShowWindow(SW_HIDE);
	m_PrintableCombo.ShowWindow(SW_HIDE);
	m_LineTypeCombo.ShowWindow(SW_HIDE);
	m_LineWidthCombo.ShowWindow(SW_HIDE);
	if (m_nLastRow > -1 && m_nLastCol > -1)
	{
		m_ctrlConfigSetList.SetItemExCtrl(m_nLastRow, m_nLastCol, NULL, false, false, false);
		m_ctrlConfigSetList.SetItemExCtrlVisible(m_nLastRow, m_nLastCol, false, true);
		//m_nLastRow = -1;
		//m_nLastCol = -1;
	}
	*pResult = 0;
}

void CDlgSetConfig::setListValueText(int hitRow, const CString& sCount, const CString& sAttribute, 
	const CString& sLayerName, const CString& sLayerColor, const CString& sLayerLinetype, 
	const CString& sLayerLineWidth, CString sTransparency, const CString& isPrint /*= "是"*/)
{
	if (sTransparency.IsEmpty())
		sTransparency = _T("0%");

	m_ctrlConfigSetList.InsertItem(hitRow, _T("")); // 插入行
	m_ctrlConfigSetList.SetItemText(hitRow, 0, sCount);
	m_ctrlConfigSetList.SetItemText(hitRow, 1, sAttribute);
	m_ctrlConfigSetList.SetItemText(hitRow, 2, sLayerName);
	m_ctrlConfigSetList.SetItemText(hitRow, 3, sLayerColor);
	m_ctrlConfigSetList.SetItemText(hitRow, 4, sLayerLinetype);
	m_ctrlConfigSetList.SetItemText(hitRow, 5, sLayerLineWidth);
	m_ctrlConfigSetList.SetItemText(hitRow, 6, isPrint);
	m_ctrlConfigSetList.SetItemText(hitRow, 7, sTransparency);
}

void CDlgSetConfig::OnEditerEnter()
{
	if (m_nLastRow < 0 || m_nLastCol < 0)
		return;

	if (m_EditTest.IsWindowVisible())
	{
		CString sText;
		m_EditTest.GetWindowText(sText);
		m_ctrlConfigSetList.SetItemText(m_nLastRow, m_nLastCol, sText);
		m_EditTest.SetWindowText(_T(""));
		m_EditTest.ShowWindow(SW_HIDE);
	}
	else if (m_PrintableCombo.IsWindowVisible())
	{
		//设置值
		CString sText;
		m_PrintableCombo.GetWindowText(sText);
		m_ctrlConfigSetList.SetItemText(m_nLastRow, m_nLastCol, sText);
		//恢复编辑框
		m_PrintableCombo.SetWindowText(_T(""));
		m_PrintableCombo.ShowWindow(SW_HIDE);
	}
	else if (m_LineWidthCombo.IsWindowVisible())
	{
		//设置值
		CString sText;
		m_LineWidthCombo.GetWindowText(sText);
		m_ctrlConfigSetList.SetItemText(m_nLastRow, m_nLastCol, sText);
		//恢复编辑框
		m_LineWidthCombo.SetWindowText(_T(""));
		m_LineWidthCombo.ShowWindow(SW_HIDE);
	}
	else if (m_LineTypeCombo.IsWindowVisible())
	{
		//设置值
		CString sText;
		m_LineTypeCombo.GetWindowText(sText);
		m_ctrlConfigSetList.SetItemText(m_nLastRow, m_nLastCol, sText);
		//恢复编辑框
		m_LineTypeCombo.SetWindowText(_T(""));
		m_LineTypeCombo.ShowWindow(SW_HIDE);
	}
	else if (m_SliderDialog->IsWindowVisible())
	{
		//设置值
		CString sText;
		m_SliderDialog->m_EditShowValue.GetWindowText(sText);

		CString sTextTest = sText;
		sTextTest.Replace(_T("%"), _T(""));
		int nPercent = _ttoi(sTextTest.GetString());
		if (nPercent >= 0 && nPercent <= 100)
		{
			m_ctrlConfigSetList.SetItemText(m_nLastRow, m_nLastCol, sText);
		}
		
		//恢复编辑框
		m_SliderDialog->m_EditShowValue.SetWindowText(_T(""));
		m_SliderDialog->ShowWindow(SW_HIDE);
	}
}

void CDlgSetConfig::initLinetypeCombo()
{
	Doc_Locker _locker;

	AcDbLinetypeTable *pLineTbl = NULL;
	if (acdbCurDwg()->getLinetypeTable(pLineTbl, AcDb::kForRead) != Acad::eOk)
		return;

	do
	{
		AcDbLinetypeTableIterator* pLTIter = NULL;
		if (pLineTbl->newIterator(pLTIter) != Acad::eOk)
			break;

		for (pLTIter->start(); !pLTIter->done(); pLTIter->step())
		{
			AcDbLinetypeTableRecord* pLTR = NULL;
			if (pLTIter->getRecord(pLTR, AcDb::kForRead) != Acad::eOk)
				continue;

			AcString name;
			pLTR->getName(name);
			if (name != _T("") && name != _T("ByLayer") && name != _T("ByBlock"))
				m_LineTypeCombo.AddString(name);

			pLTR->close();
		}

		delete pLTIter;
	} while (0);
	if (pLineTbl)
		pLineTbl->close();

	if (!m_sLineStyle.IsEmpty())
	{
		int n = m_LineTypeCombo.FindStringExact(0, m_sLineStyle);
		if (n >= 0)
			m_LineTypeCombo.SetCurSel(n);
	}
}



void CDlgSetConfig::OnCbnKillfocusComboLinetype()
{
	if (m_nLastRow > -1 && m_nLastCol > -1)
	{
		CString sText;
		m_LineTypeCombo.GetWindowText(sText);
		m_ctrlConfigSetList.SetItemText(m_nLastRow, m_nLastCol, sText);
	}
}


void CDlgSetConfig::OnCbnKillfocusComboLinewidth()
{
	if (m_nLastRow > -1 && m_nLastCol > -1)
	{
		CString sText;
		m_LineWidthCombo.GetWindowText(sText);
		m_ctrlConfigSetList.SetItemText(m_nLastRow, m_nLastCol, sText);
	}
}


void CDlgSetConfig::OnCbnKillfocusComboConfigchoose()
{
	if (m_nLastRow > -1 && m_nLastCol > -1)
	{
		CString sText;
		m_PrintableCombo.GetWindowText(sText);
		m_ctrlConfigSetList.SetItemText(m_nLastRow, m_nLastCol, sText);
	}
}


void CDlgSetConfig::OnEnKillfocusEditList()
{
	if (m_nLastRow > -1 && m_nLastCol > -1)
	{
		CString sText;
		m_EditTest.GetWindowText(sText);
		m_ctrlConfigSetList.SetItemText(m_nLastRow, m_nLastCol, sText);
	}
}
