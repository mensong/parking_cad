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
//-----------------------------------------------------------------------------

struct InfoStructLine
{
	AcDbObjectId entId;
	AcGePoint3d startpoint;
	AcGePoint3d endpoint;
};
struct InfoStructArc
{
	AcDbObjectId entId;
	double startAngle;
	double endAngle;
};

class CDlgEntrance : public CAcUiDialog {
	DECLARE_DYNAMIC (CDlgEntrance)

public:
	CDlgEntrance (CWnd *pParent =NULL, HINSTANCE hInstance =NULL) ;

	enum {
		IDD = IDD_DLG_ENTRANCESET
	};

protected:
	virtual void DoDataExchange (CDataExchange *pDX) ;
	afx_msg LRESULT OnAcadKeepFocus (WPARAM, LPARAM) ;

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
	// 获取出入口多线段数据按钮
	CButton m_btn_getEntrancePline;
	// 选取到的出入口数据展示
	CEdit m_show_entranceData;
	// 地下室高度编辑框
	CMyEdit m_edit_basementHeight;
	// 出入口宽度编辑框
	CMyEdit m_edit_entranceWidth;
	afx_msg void OnBnClickedButtonGetentrancepl();
	void creatEntrance(double& dBasementHeight, double& dEntranceWidth);
	AcDbObjectIdArray explodeEnty(AcDbObjectId& entId);
	bool addDim(const AcDbObjectIdArray entIds, const double  dHeight, const double dWidth);
	AcDbObjectId creatDim(const AcGePoint3d& pt1, const AcGePoint3d& pt2, const AcGePoint3d& pt3, const CString sLegth);
	void creatDimStyle(const CString &styleName);
	void DealIntersectEnt(AcDbObjectIdArray& inputIds);
	bool DealEnt(AcDbEntity* pEnt, AcGePoint3d& intersectPoint);
	void BatchStorageEnt(AcDbObjectIdArray& inputId, std::vector<std::vector<AcDbObjectId>>& outputId);
	bool isIntersect(AcDbEntity* pEnt, AcDbEntity* pTempEnt, double tol);
	void getpoint(AcDbEntity* pEnt, AcGePoint3d& startpt, AcGePoint3d& endpt);
	void GenerateGuides(double& changdistance, std::vector<AcDbObjectId>& operaIds, AcDbObjectIdArray& GuideIds);
	void MultipleCycles(double& inputdistance, AcDbObjectIdArray& GuideIds);
	void SpecialSaveEntInfo(double& movedistance, AcDbObjectIdArray& inputIds, std::vector<InfoStructLine>& saveLineInfoVector,
		std::vector<InfoStructArc>& saveArcInfoVector, std::vector<AcGePoint3d>& inserpoint);
	void ConnectionPoint(AcDbObjectIdArray& inputIds);
	bool findPoint(AcGePoint3d& startpt, AcGePoint3d& endpt, std::vector<AcGePoint3d>& points, AcGePoint3dArray& outpts, int tol= 0 );
	bool IsOnLine(AcGePoint2d& pt1, AcGePoint2d& pt2, AcGePoint2d& pt3);
	void addWideDim(const double showWidth);
	void creatPlinePoints(const AcDbObjectIdArray allLineIds);
	bool checkClosed(const AcGePoint2d checkPt, const std::vector<AcGePoint2dArray> allLinePts);
	AcGePoint2d getPlineNextPoint(const AcGePoint2d targetPt, AcGePoint2dArray &nextUsedPts,
		const std::vector<AcGePoint2dArray> allLinePts, AcGePoint2dArray &resultPts);
	void deleParkInEntrance(const AcGePoint2dArray plinePts);
	void changeLine2Polyline(AcDbObjectIdArray targetEntIds);
	static CString ms_sBasementHeight;
	static CString ms_sEntranceWidth;

protected:
	AcDbObjectId m_targetId;
	AcDbObjectIdArray m_addBlockIds;
	AcDbObjectIdArray m_widthLineIds;
	AcGePoint3d m_insertPoint;
} ;
