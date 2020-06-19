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
//----- CmpResPalette.cpp : Implementation of CCmpResPalette
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "resource.h"
#include "PaletteDrawingAbnormalCheck.h"
#include "PaletteSetDrawingAbnormalCheck.h"

CPaletteDrawingAbnormalCheck* CPaletteDrawingAbnormalCheck::ms_palette = NULL;


//-----------------------------------------------------------------------------
IMPLEMENT_DYNAMIC (CPaletteDrawingAbnormalCheck, CAdUiPalette)

BEGIN_MESSAGE_MAP(CPaletteDrawingAbnormalCheck, CAdUiPalette)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_SIZING()
END_MESSAGE_MAP()

//-----------------------------------------------------------------------------

void CPaletteDrawingAbnormalCheck::show()
{
	int nCount = CPaletteSetDrawingAbnormalCheck::Instance()->GetPaletteCount();
	for (int i=0; i<nCount; ++i)
	{
		CPaletteDrawingAbnormalCheck* pPalette = (CPaletteDrawingAbnormalCheck*)CPaletteSetDrawingAbnormalCheck::Instance()->GetPalette(i);
		CPaletteSetDrawingAbnormalCheck::Instance()->RemovePalette(pPalette);
		if (pPalette->m_pChildDlg)
		{
			pPalette->m_pChildDlg->DestroyMe();
		}
		pPalette->DestroyWindow();
		delete pPalette;
	}

	CPaletteDrawingAbnormalCheck::instance();
	CPaletteSetDrawingAbnormalCheck::Instance()->SetVisible(true);
}

CPaletteDrawingAbnormalCheck* CPaletteDrawingAbnormalCheck::instance()
{
	if (!ms_palette)
	{
		CAcModuleResourceOverride resOverride; 

		CPaletteSetDrawingAbnormalCheck* paletteSet = CPaletteSetDrawingAbnormalCheck::Instance();

		ms_palette = new CPaletteDrawingAbnormalCheck();
		ms_palette->Create(WS_CHILD | WS_VISIBLE, _T("图纸对比结果"), paletteSet, PS_EDIT_NAME);
		paletteSet->AddPalette(ms_palette);
	}

	return ms_palette;
}

CPaletteDrawingAbnormalCheck::CPaletteDrawingAbnormalCheck (HINSTANCE hInstance) 
	: CAdUiPalette ()
	, m_pChildDlg(NULL) 
{
}

//-----------------------------------------------------------------------------
CPaletteDrawingAbnormalCheck::~CPaletteDrawingAbnormalCheck () 
{
	ms_palette = NULL;
}

//-----------------------------------------------------------------------------
//- Used to add a dialog resource
int CPaletteDrawingAbnormalCheck::OnCreate (LPCREATESTRUCT lpCreateStruct) {
	if ( CAdUiPalette::OnCreate (lpCreateStruct) == -1 )
		return (-1) ;

	CRect rect;
	GetClientRect(&rect);

	CAcModuleResourceOverride resOverride;//资源定位
	m_pChildDlg = new CDlgFindCloud;
	m_pChildDlg->Create(CDlgFindCloud::IDD, this);
	m_pChildDlg->MoveWindow(rect);
	m_pChildDlg->ShowWindow(SW_SHOW);

	return (0) ;
}

//-----------------------------------------------------------------------------
//- Called by the palette set when the palette is made active
void CPaletteDrawingAbnormalCheck::OnSetActive () {
	CAdUiPalette::OnSetActive () ;
}

//-----------------------------------------------------------------------------
//- Called by AutoCAD to steal focus from the palette
bool CPaletteDrawingAbnormalCheck::CanFrameworkTakeFocus () {
	//- Not simply calling IsFloating() (a BOOL) avoids warning C4800
	return (GetPaletteSet ()->IsFloating () == TRUE ? true : false) ;
}

void CPaletteDrawingAbnormalCheck::clearCmpResult()
{
	if (m_pChildDlg)
		m_pChildDlg->clearCmpResult();
}

//-----------------------------------------------------------------------------
void CPaletteDrawingAbnormalCheck::OnSize (UINT nType, int cx, int cy) {
	CAdUiPalette::OnSize (nType, cx, cy) ;
	//- If valid
	if ( ::IsWindow (m_pChildDlg->GetSafeHwnd ()) ) {
		//- Forward the call
		m_pChildDlg->MoveWindow (0, 0, cx, cy) ;
	}
}

