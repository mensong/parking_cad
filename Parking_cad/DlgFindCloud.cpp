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
//----- DlgFindCloud.cpp : Implementation of CDlgFindCloud
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "resource.h"
#include "DlgFindCloud.h"
#include <set>
#include "DBHelper.h"
#include "EquipmentroomTool.h"

//-----------------------------------------------------------------------------
IMPLEMENT_DYNAMIC (CDlgFindCloud, CAcUiDialog)

BEGIN_MESSAGE_MAP(CDlgFindCloud, CAcUiDialog)
	ON_MESSAGE(WM_ACAD_KEEPFOCUS, OnAcadKeepFocus)
	ON_WM_SIZE()
	ON_NOTIFY(NM_CLICK, IDC_LIST, &CDlgFindCloud::OnNMClickList)
	ON_BN_CLICKED(IDOK, &CDlgFindCloud::OnBnClickedOk)
END_MESSAGE_MAP()

//-----------------------------------------------------------------------------
CDlgFindCloud::CDlgFindCloud (CWnd *pParent /*=NULL*/, HINSTANCE hInstance /*=NULL*/) : CAcUiDialog(CDlgFindCloud::IDD, pParent, hInstance) {
}

void CDlgFindCloud::DestroyMe()
{
	this->OnOK();
}

void CDlgFindCloud::clearCmpResult()
{
	m_listRes.DeleteAllItems();
}

//-----------------------------------------------------------------------------
void CDlgFindCloud::DoDataExchange (CDataExchange *pDX) {
	CAdUiBaseDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST, m_listRes);
}

//-----------------------------------------------------------------------------
//----- Needed for modeless dialogs to keep focus.
//----- Return FALSE to not keep the focus, return TRUE to keep the focus
LRESULT CDlgFindCloud::OnAcadKeepFocus (WPARAM, LPARAM) {
	return (TRUE) ;
}


void CDlgFindCloud::OnSize(UINT nType, int cx, int cy)
{
	CAcUiDialog::OnSize(nType, cx, cy);

	CRect rectDlg;
	this->GetWindowRect(rectDlg);

	CRect rectList;
	m_listRes.GetWindowRect(rectList);
	this->ScreenToClient(rectList);
	rectList.right = rectDlg.Width() - rectList.left;
	rectList.bottom = rectDlg.Height() - rectList.left;
	m_listRes.MoveWindow(rectList);

}


BOOL CDlgFindCloud::OnInitDialog()
{
	CAcUiDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
	init();

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


void CDlgFindCloud::OnNMClickList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	CPoint point;
	GetCursorPos(&point);
	//将屏幕坐标转到客户区坐标
	m_listRes.ScreenToClient(&point);

	LVHITTESTINFO info;
	info.pt = point;
	int Itsub = m_listRes.SubItemHitTest(&info);
	int row = info.iItem;//行号从0开始
	int col = info.iSubItem;//列号从0开始

	std::map<int, AcDbObjectId>::const_iterator itTag = m_listRowAndIds.find(row);
	if (itTag != m_listRowAndIds.end() && itTag->second.isValid())
	{
		DBHelper::ZoomOptimum(itTag->second);
		reDraw(itTag->second);
	}

	*pResult = 0;
}


void CDlgFindCloud::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	clearCmpResult();
	m_listRowAndIds.erase(m_listRowAndIds.begin(), m_listRowAndIds.end());
	init(true);
}


BOOL CDlgFindCloud::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE)
		return TRUE;
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN)
		return TRUE;
	return CAcUiDialog::PreTranslateMessage(pMsg);
}

void CDlgFindCloud::init(bool isRefresh /*= false*/)
{
	CString sCloudLineLayer(CEquipmentroomTool::getLayerName("cloud_line").c_str());
	AcDbObjectIdArray allIds = DBHelper::GetEntitiesByLayerName(sCloudLineLayer);
	if (!isRefresh)
	{
		m_listRes.InsertColumn(m_listRes.GetHeaderCtrl()->GetItemCount(), _T("检测出区域"), LVCFMT_LEFT, 150);
		m_listRes.InsertColumn(m_listRes.GetHeaderCtrl()->GetItemCount(), _T("区域面积(m²)"), LVCFMT_LEFT, 150);
	}
	int iBlankCount = 0;
	int iColumnOverlapCount = 0;
	int iShearWallOverlapCount = 0;
	for (int i = 0; i<allIds.length(); i++)
	{
		AcString XRecordText;
		DBHelper::GetXRecord(allIds[i], _T("cloud"), XRecordText);
		CString showText;
		AcString araeText;
		if (XRecordText == _T("空白区域"))
		{
			iBlankCount++;
			CString sCount;
			sCount.Format(_T("%d"), iBlankCount);
			showText = _T("空白区域") + sCount;
			DBHelper::GetXRecord(allIds[i], _T("cloud_area"), araeText);
		}
		else if (XRecordText == _T("车位与方柱重叠区域"))
		{
			iColumnOverlapCount++;
			CString sCount;
			sCount.Format(_T("%d"), iColumnOverlapCount);
			showText = _T("车位与方柱重叠区域") + sCount;
			DBHelper::GetXRecord(allIds[i], _T("cloud_area"), araeText);
		}
		else if (XRecordText == _T("车位与剪力墙重叠区域"))
		{
			iShearWallOverlapCount++;
			CString sCount;
			sCount.Format(_T("%d"), iShearWallOverlapCount);
			showText = _T("车位与剪力墙重叠区域") + sCount;
			DBHelper::GetXRecord(allIds[i], _T("cloud_area"), araeText);
		}
		int nRow = m_listRes.InsertItem(i, showText);
		m_listRes.SetItemText(nRow, 1, araeText);
		std::pair<int, AcDbObjectId> value(i, allIds[i]);
		m_listRowAndIds.insert(value);
		//m_listRowAndIds.insert(std::pair<int, AcDbObjectId>(i + 1, allIds[i]));
	}
	m_listRes.SetExtendedStyle(m_listRes.GetExtendedStyle() | LVS_EX_FULLROWSELECT);
	m_listRes.ModifyStyle(0, LVS_SINGLESEL | LVS_SHOWSELALWAYS);
}

void CDlgFindCloud::reDraw(const AcDbObjectId& targetId)
{
	Doc_Locker _locker;
	AcDbEntity *pEnt = NULL;
	Acad::ErrorStatus es = targetId.isValid()? acdbOpenObject(pEnt, targetId, AcDb::kForWrite) : Acad::eInvalidObjectId;
	if (es == eOk)
	{
		pEnt->setVisibility(AcDb::kInvisible);
		pEnt->setVisibility(AcDb::kVisible);
		pEnt->close();

		acedGetAcadDwgView()->SetFocus();
		m_listRes.SetFocus();
	}
	else
	{
		acutPrintf(_T("打开实体失败"));
	}
}
