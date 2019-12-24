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
//-----------------------------------------------------------------------------
class CDlgSetConfig : public CAcUiDialog {
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
	afx_msg void OnBnClickedButtonInit();
	// ����ͼ��༭��
	CEdit m_EditAxis;
	// ���߱�עͼ�����༭��
	CEdit m_EditAxisDim;
	// ����ͼ��༭��
	CEdit m_EditLane;
	// ������עͼ�����༭��
	CEdit m_EditLaneDim;
	// ��λͼ�����༭��
	CEdit m_EditParkings;
	// ����ָʾ��ͷͼ�����༭��
	CEdit m_EditArrow;
	// ����ͼ�����༭��
	CEdit m_EditPillar;
	// �ؿ�������ͼ�����༭��
	CEdit m_EditScope;
	// ͼǩͼ�����༭��
	CEdit m_EditMapSign;
	// ͼ��ͼ�����༭��
	CEdit m_EditPictureFrame;
	// �豸��ͼ�����༭��
	CEdit m_EditEquipmentroom;
	// ����Ͳ��ѡͼ�����༭��
	CEdit m_EditCoreWall;
	// �����ͼ�����༭��
	CEdit m_EditEntrance;

	void changeLayerName(const CString& oldLayerName,const CString& newLayerName);
	std::string m_strUiPostUrl;
	std::string m_strUiGetUrl;
	std::string m_strEntranceUrl;
	
} ;
