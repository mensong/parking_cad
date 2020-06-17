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
#include "PaletteToolbar.h"
#include "PaletteSetToolbar.h"

CPaletteToolbar* CPaletteToolbar::ms_palette = NULL;


//-----------------------------------------------------------------------------
IMPLEMENT_DYNAMIC (CPaletteToolbar, CAdUiPalette)

BEGIN_MESSAGE_MAP(CPaletteToolbar, CAdUiPalette)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_SIZING()
END_MESSAGE_MAP()

//-----------------------------------------------------------------------------

void CPaletteToolbar::show()
{
	int nCount = CPaletteSetToolbar::Instance()->GetPaletteCount();
	for (int i=0; i<nCount; ++i)
	{
		CPaletteToolbar* pPalette = (CPaletteToolbar*)CPaletteSetToolbar::Instance()->GetPalette(i);
		CPaletteSetToolbar::Instance()->RemovePalette(pPalette);
		if (pPalette->m_pChildDlg)
		{
			pPalette->m_pChildDlg->DestroyWindow();
		}
		pPalette->DestroyWindow();
		delete pPalette;
	}

	CPaletteToolbar::instance();
	CPaletteSetToolbar::Instance()->SetVisible(true);
}

CPaletteToolbar* CPaletteToolbar::instance()
{
	if (!ms_palette)
	{
		CAcModuleResourceOverride resOverride; 

		CPaletteSetToolbar* paletteSet = CPaletteSetToolbar::Instance();

		ms_palette = new CPaletteToolbar();
		ms_palette->Create(WS_CHILD | WS_VISIBLE, _T("智能地库设计系统"), paletteSet, PS_EDIT_NAME);
		paletteSet->AddPalette(ms_palette);
	}

	return ms_palette;
}

CPaletteToolbar::CPaletteToolbar (HINSTANCE hInstance) 
	: CAdUiPalette ()
	, m_pChildDlg(NULL) 
{
}

//-----------------------------------------------------------------------------
CPaletteToolbar::~CPaletteToolbar () 
{
	ms_palette = NULL;
}

//-----------------------------------------------------------------------------
//- Used to add a dialog resource
int CPaletteToolbar::OnCreate (LPCREATESTRUCT lpCreateStruct) {
	if ( CAdUiPalette::OnCreate (lpCreateStruct) == -1 )
		return (-1) ;

	CRect rect;
	GetClientRect(&rect);
	m_pChildDlg = new CDlgToolbarNav;
	m_pChildDlg->Create(CDlgToolbarNav::IDD, this);
	m_pChildDlg->MoveWindow(rect);
	m_pChildDlg->ShowWindow(SW_SHOW);

	return (0) ;
}

//-----------------------------------------------------------------------------
//- Called by the palette set when the palette is made active
void CPaletteToolbar::OnSetActive () {
	CAdUiPalette::OnSetActive () ;
}

//-----------------------------------------------------------------------------
//- Called by AutoCAD to steal focus from the palette
bool CPaletteToolbar::CanFrameworkTakeFocus () {
	//- Not simply calling IsFloating() (a BOOL) avoids warning C4800
	return (GetPaletteSet ()->IsFloating () == TRUE ? true : false) ;
}

//-----------------------------------------------------------------------------
void CPaletteToolbar::OnSize (UINT nType, int cx, int cy) {
	CAdUiPalette::OnSize (nType, cx, cy) ;
	//- If valid
	if ( ::IsWindow (m_pChildDlg->GetSafeHwnd ()) ) {
		//- Forward the call
		m_pChildDlg->MoveWindow (0, 0, cx, cy) ;
	}
}

