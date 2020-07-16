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
//----- DlgSetConfig.h : Declaration of the CDlgSetConfig
//-----------------------------------------------------------------------------
#pragma once

//-----------------------------------------------------------------------------
#include "acui.h"
#include "afxwin.h"
#include "Resource.h"
#include "afxcmn.h"
#include "ListCtrlCustom.h"
#include "DlgSliderTransparency.h"
#include "json\value.h"

//-----------------------------------------------------------------------------
class CDlgSetConfig 
	: public CAcUiDialog
{
	DECLARE_DYNAMIC (CDlgSetConfig)

public:
	CDlgSetConfig (CWnd *pParent =NULL, HINSTANCE hInstance =NULL) ;

	enum { IDD = IDD_DIALOG_CONFIG} ;

protected:
	virtual void DoDataExchange (CDataExchange *pDX) ;
	afx_msg LRESULT OnAcadKeepFocus (WPARAM, LPARAM) ;

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	virtual BOOL OnInitDialog();
	void init();
	
	void changeLayerName(const CString& oldLayerName,const CString& newLayerName, const CString& layerColor, const CString& lineWidth,
		const CString& lineType, const CString& transparency, const CString& isPrint);
	std::string m_strUiPostUrl;
	std::string m_strUiGetUrl;
	std::string m_strUiPartPostUrl;
	std::string m_strUiPartGetUrl;
	Json::Value linetypearray;
	
	Json::Value m_root;

	int hitRow;
	int hitCol;
	afx_msg void OnNMDblclkLayerlist(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMClickLayerlist(NMHDR *pNMHDR, LRESULT *pResult);
	void setListValueText(int hitRow, const CString& sCount, const CString& sAttribute, const CString& sLayerName,
		const CString& sLayerColor,const CString& sLayerLinetype,const CString& sLayerLineWidth, CString sTransparency, const CString& isPrint = "是");
	std::map<CString, int> m_mpColumnName;
	// 层表记录
	CListCtrlCustom m_ctrlConfigSetList;
	int m_nLastRow;
	int m_nLastCol;

	// 下拉选择框
	CComboBox m_PrintableCombo;
	// 层表编辑
	CEdit m_EditTest;
	void OnEditerEnter();
	// 线宽选择下拉框
	CComboBox m_LineWidthCombo;
	// 图层线型下拉框
	CComboBox m_LineTypeCombo;
	// 透明度控制窗口
	CDlgSliderTransparency* m_SliderDialog;

	CString m_sLineStyle;
	void initLinetypeCombo();
	afx_msg void OnCbnKillfocusComboLinetype();
	afx_msg void OnCbnKillfocusComboLinewidth();
	afx_msg void OnCbnKillfocusComboConfigchoose();
	afx_msg void OnEnKillfocusEditList();
} ;
