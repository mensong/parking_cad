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
//----- ArxDialog.h : Declaration of the CArxDialog
//-----------------------------------------------------------------------------
#pragma once

//-----------------------------------------------------------------------------
#include "acui.h"
#include "afxwin.h"
#include "Resource.h"
#include "MyEdit.h"
#include <vector>

//-----------------------------------------------------------------------------
class CArxDialog
	: public CAcUiDialog 
{
	DECLARE_DYNAMIC (CArxDialog)

public:
	CArxDialog (CWnd *pParent =NULL, HINSTANCE hInstance =NULL) ;
	~CArxDialog();

	enum { IDD = IDD_ARX_MODAL} ;

	CString m_sOutlineLayer;
	CString m_sShearwallLayer;
	int m_nDirectionCombo;
	CString m_sStartPoint;
	CString m_strEndPoint;
	CString m_strXPt;
	CString m_strYPt;
	CString m_strEndXPt;
	CString m_strEndYPt;
	double startPtx;
	double startPty;
	double dEndPtx;
	double dEndPty;
	CString m_StrRetreatLine;
	CString m_strLength;
	double defaultLength;//Ĭ�ϵ���λ����ǽ����
	double defaultCloseWallLength;//Ĭ�ϵ���λ��ǽ����
	bool isPartition;
	CString m_strWidth;
	CString m_StrLaneWidth;
	CString m_StrSquarcolumnLength;
	CString m_StrSquareColumnWidth;
	CString m_sParkingCount;
	CString m_sNonConvexLevel;
	CString m_sPartitionLine;
	AcGePoint2dArray GetretreatlinePts;//װȡȥ���ص���Ч��
	std::string m_strComputerId;
	std::string m_strUserId;
	std::vector<AcGePoint2dArray> allPartitionPts;//��������
	static void setPostUrlPortone(std::string& posturlPortone);
	static std::string ms_posturlPortone;
	static void setPostUrlPorttwo(std::string& posturlPorttwo);
	static std::string ms_posturlPorttwo;

protected:
	void loadoutlineLayers();
	void loadshearwallLayers();
	void loaddirectionCombo();
	bool compare(AcGePoint2dArray& targetPts, AcGePoint2d &comparePt);
	std::vector<AcGePoint2dArray> getPlinePointForLayer(CString& layername, bool bClosed = true);
	std::vector<AcGePoint2dArray> getPlinePointForLayer(CString& layername, std::vector<int>& types);
	void setInitData();
	int postToAIApi(const std::string& js, std::string& sMsg, const bool& useV1);
	void selectPort(const bool& useV1);

protected:
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange (CDataExchange *pDX) ;
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg LRESULT OnAcadKeepFocus (WPARAM, LPARAM) ;
	DECLARE_MESSAGE_MAP()

private:
	// ����������ͼ��
	CComboBox m_outlineLayer;
	// ����ǽͼ��
	CComboBox m_shearwallLayer;
	// ��λ�Ų�����
	CComboBox m_directionCombo;
	// //ʰȡ��λ�Ų����
	CButton m_btnGetStartPoint;
	// //ʰȡ�ؿ�����
	CButton m_btnGetRetreatLine;
	// ʰȡ��λ�Ų������ʾ��
	CEdit m_editStartPoint;
	// �ؿ�����
	CEdit m_editRetreatLine;

public:
	afx_msg void OnBnClickedButtonGetretreatline();
	afx_msg void OnBnClickedButtonGetstartpoint();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedRadioOtherlength();
	afx_msg void OnBnClickedRadioDefault();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedCheckPartition();
	afx_msg void OnEnKillfocusEditNonConvexlevel();

private:
	// ����ǽ��λ���ȱ༭��
	CMyEdit m_editLength;
	// ����λ��ȱ༭��
	CMyEdit m_Width;
	// ������ȱ༭��
	CMyEdit m_LaneWidth;
	// �������ȱ༭��
	CMyEdit m_SquarcolumnLength;
	// ������ȱ༭��
	CMyEdit m_SquareColumnWidth;
	// �Ƿ����ѡ���
	CButton m_checkPartition;	
	// ��λ�����༭��
	CEdit m_ParkingCount;
	// ���ݳ̶ȱ༭��
	CMyEdit m_Non_Convexlevel;
	// �����༭��
	CEdit m_PartitionLineEdit;
public:
	afx_msg void OnBnClickedButtonV2ok();
private:
	// ��ʾ��λ�Ų��յ�����
	CEdit m_EditShowEndPoint;
public:
	afx_msg void OnBnClickedButtonGetendpoint();
} ;
