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
#include "DlgSlider.h"
#include "DBHelper.h"

//-----------------------------------------------------------------------------
IMPLEMENT_DYNAMIC (CDlgSlider, CAcUiDialog)

BEGIN_MESSAGE_MAP(CDlgSlider, CAcUiDialog)
	ON_MESSAGE(WM_ACAD_KEEPFOCUS, OnAcadKeepFocus)
	ON_WM_HSCROLL()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

//-----------------------------------------------------------------------------
CDlgSlider::CDlgSlider (CWnd *pParent /*=NULL*/, HINSTANCE hInstance /*=NULL*/) : CAcUiDialog (CDlgSlider::IDD, pParent, hInstance) {
}

static void __smartLog(bool*& data, bool isDataValid)
{
	if (isDataValid)
	{
		bool end = (*data);
		if (!end)
			CParkingLog::AddLogA("DEBUG_��֧�ֳ���ִ�еĲ���", 0, "CDlgSlider::DoDataExchange");
		delete data;
	}
}

//-----------------------------------------------------------------------------
void CDlgSlider::DoDataExchange (CDataExchange *pDX) {
	Smart<bool*> end(new bool(false), __smartLog);
	CAcUiDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SLIDER_CONTROL, m_SliderControl);
	DDX_Control(pDX, IDC_EDIT_SHOWVALUE, m_EditShowValue);
	*(end()) = true;
}

//-----------------------------------------------------------------------------
//----- Needed for modeless dialogs to keep focus.
//----- Return FALSE to not keep the focus, return TRUE to keep the focus
LRESULT CDlgSlider::OnAcadKeepFocus (WPARAM, LPARAM) {
	return (TRUE) ;
}



BOOL CDlgSlider::OnInitDialog()
{
	CAcUiDialog::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	m_SliderControl.SetRange(0, 900);//���û�����ΧΪ1��90
	m_SliderControl.SetTicFreq(1);//ÿ1����λ��һ�̶�
	//m_SliderControl.SetPos(0);//���û����ʼλ��Ϊ1 

	return TRUE;  // return TRUE unless you set the focus to a control
				  // �쳣: OCX ����ҳӦ���� FALSE
}


void CDlgSlider::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	UpdateData(TRUE);
	//m_int ��Ϊ��ǰ�����ֵ��
	int value;
	value = 0.1*m_SliderControl.GetPos();//ȡ�õ�ǰλ��ֵ  
	CString sCount;
	sCount.Format(_T("%d"), value);
	CString sShowValue = sCount + "%";
	m_EditShowValue.SetWindowText(sShowValue);
	CAcUiDialog::OnHScroll(nSBCode, nPos, pScrollBar);
	UpdateData(FALSE);
}


BOOL CDlgSlider::OnEraseBkgnd(CDC* pDC)
{
	//����brushΪϣ���ı�����ɫ
	CBrush backBrush(RGB(255,255,255));

	//����ɵ�brush
	CBrush* pOldBrush = pDC->SelectObject(&backBrush);
	CRect rect;
	pDC->GetClipBox(&rect);

	//����Ҫ������
	pDC->PatBlt(rect.left, rect.top, rect.Width(), rect.Height(), PATCOPY);
	pDC->SelectObject(pOldBrush);

	return TRUE;
}
