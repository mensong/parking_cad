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
//----- DlgSlider.cpp : Implementation of CDlgSlider
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "resource.h"
#include "DlgSliderTransparency.h"
#include "DBHelper.h"
#include "DlgSetConfig.h"

//-----------------------------------------------------------------------------
IMPLEMENT_DYNAMIC (CDlgSliderTransparency, CAcUiDialog)

BEGIN_MESSAGE_MAP(CDlgSliderTransparency, CAcUiDialog)
	ON_MESSAGE(WM_ACAD_KEEPFOCUS, OnAcadKeepFocus)
	ON_WM_HSCROLL()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

//-----------------------------------------------------------------------------
CDlgSliderTransparency::CDlgSliderTransparency (CWnd *pParent /*=NULL*/, HINSTANCE hInstance /*=NULL*/) 
	: CAcUiDialog (CDlgSliderTransparency::IDD, pParent, hInstance) {
}

void CDlgSliderTransparency::SetPercent(const CString& percentStr)
{
	m_EditShowValue.SetWindowText(percentStr);
	CString s = percentStr;
	s.Replace(_T("%"), _T(""));
	int percent = _ttoi(s.GetString());
	m_SliderControl.SetPos(percent);
}

BOOL CDlgSliderTransparency::PreTranslateMessage(MSG* pMsg)
{
	if ((pMsg->message == WM_KEYDOWN) && (pMsg->wParam == VK_RETURN))
	{
		CDlgSetConfig* pDlgConfig = dynamic_cast<CDlgSetConfig*>(GetParent()->GetParent());
		if (pDlgConfig)
		{
			pDlgConfig->OnEditerEnter();
		}
	}
	
	return CAcUiDialog::PreTranslateMessage(pMsg);
}

static void __smartLog(bool*& data, bool isDataValid)
{
	if (isDataValid)
	{
		bool end = (*data);
		if (!end)
			CParkingLog::AddLogA("DEBUG_不支持尝试执行的操作", 0, "CDlgSlider::DoDataExchange");
		delete data;
	}
}

//-----------------------------------------------------------------------------
void CDlgSliderTransparency::DoDataExchange (CDataExchange *pDX) {
	Smart<bool*> end(new bool(false), __smartLog);
	CAcUiDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SLIDER_CONTROL, m_SliderControl);
	DDX_Control(pDX, IDC_EDIT_SHOWVALUE, m_EditShowValue);
	*(end()) = true;
}

//-----------------------------------------------------------------------------
//----- Needed for modeless dialogs to keep focus.
//----- Return FALSE to not keep the focus, return TRUE to keep the focus
LRESULT CDlgSliderTransparency::OnAcadKeepFocus (WPARAM, LPARAM) {
	return (TRUE) ;
}



BOOL CDlgSliderTransparency::OnInitDialog()
{
	CAcUiDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
	m_SliderControl.SetRange(0, 100);//设置滑动范围为1到90
	m_SliderControl.SetTicFreq(1);//每1个单位画一刻度
	//m_SliderControl.SetPos(0);//设置滑块初始位置为1 
		
	return TRUE; 
}


void CDlgSliderTransparency::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	UpdateData(TRUE);
	//m_int 即为当前滑块的值。
	int value;
	value = m_SliderControl.GetPos();//取得当前位置值  
	CString sCount;
	sCount.Format(_T("%d"), value);
	CString sShowValue = sCount + "%";
	m_EditShowValue.SetWindowText(sShowValue);
	CAcUiDialog::OnHScroll(nSBCode, nPos, pScrollBar);
	UpdateData(FALSE);
}


BOOL CDlgSliderTransparency::OnEraseBkgnd(CDC* pDC)
{
	//设置brush为希望的背景颜色
	CBrush backBrush(RGB(255,255,255));

	//保存旧的brush
	CBrush* pOldBrush = pDC->SelectObject(&backBrush);
	CRect rect;
	pDC->GetClipBox(&rect);

	//画需要的区域
	pDC->PatBlt(rect.left, rect.top, rect.Width(), rect.Height(), PATCOPY);
	pDC->SelectObject(pOldBrush);

	return TRUE;
}
