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
//----- DlgEntrance.h : Declaration of the CDlgEntrance
//-----------------------------------------------------------------------------
#pragma once

//-----------------------------------------------------------------------------
#include "acui.h"
#include "afxwin.h"
#include "Resource.h"
#include "MyEdit.h"
#include <vector>
#include "LineSelect.h"
//-----------------------------------------------------------------------------
class CDlgEntrance : public CAcUiDialog {
	DECLARE_DYNAMIC (CDlgEntrance)

public:
	CDlgEntrance (CWnd *pParent =NULL, HINSTANCE hInstance =NULL) ;

	enum { IDD = IDD_DIALOG_ENTRANCE} ;

	double dStartPtx;
	double dStartPty;
	double dEndPtx;
	double dEndPty;

protected:
	virtual void DoDataExchange (CDataExchange *pDX) ;
	afx_msg LRESULT OnAcadKeepFocus (WPARAM, LPARAM) ;

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonGetstartpoint();
	afx_msg void OnBnClickedButtonGetendpoint();
	void init();
private:
	// 展示出入口起始点编辑框
	CEdit m_StartPointEdit;
	// 出入口终止点展示编辑框
	CEdit m_EndPointEdit;
	// 限高编辑框
	CMyEdit m_LimitHeight;
	// 出入口宽度编辑框
	CMyEdit m_EntranceWidth;
	// 缓坡1坡度
	CMyEdit m_OneGentleSlope;
	// 缓坡2坡度
	CMyEdit m_TwoGentleSlope;
	// 缓坡3坡度
	CMyEdit m_ThireGentleSlope;
	// 缓坡1水平距离
	CMyEdit m_OneHorizontalDistance;
	// 缓坡2水平距离
	CMyEdit m_TwoHorizontalDistance;
	// 缓坡3水平距离
	CMyEdit m_ThireHorizontalDistance;
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
	int CDlgEntrance::postToAIApi(const std::string& sData, std::string& sMsg, const bool& useV1);
	static void setEntrancePostUrl(std::string& strEntrancePostUrl);
	static std::string ms_strEntrancePostUrlPort;
	static void setEntrancePostUrlV2(std::string& strEntrancePostUrlV2);
	static std::string ms_strEntrancePostUrlPortV2;

	void deletParkingForEntrance(std::map<AcDbObjectId,AcGePoint2d>& parkingIdAndPt,const AcGePoint2dArray& useDeletParkingPts);
	void deletParkingByLineSelect(const AcDbObjectIdArray& parkingIds,const AcGePoint2dArray& useDeletParkingPts);
	void getParkingIdAndPtMap(std::map < AcDbObjectId, AcGePoint2d>& parkingIdAndPtMap);
	void showEntrance(const AcGePoint2dArray& oneEntrancePts);

	LineSelect m_parkingSel;
} ;
