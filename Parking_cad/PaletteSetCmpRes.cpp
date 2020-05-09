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
//----- MyPaletteSet.cpp : Implementation of CMyPaletteSet
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "resource.h"
#include "PaletteSetCmpRes.h"
//#include "CadDiffFramework.h"
//#include "CadDiffOneCadFramework.h"

static CLSID CLSID_TUNNLESECTION_PALETTESET = { 0x76e12808, 0x4eb1, 0x47a0, { 0xad, 0x38, 0xfd, 0x5c, 0xec, 0x70, 0x8a, 0x38 } };

CPaletteSetCmpRes* CPaletteSetCmpRes::s_pPaletteSet = NULL;

IMPLEMENT_DYNAMIC(CPaletteSetCmpRes, CAdUiPaletteSet)

BEGIN_MESSAGE_MAP(CPaletteSetCmpRes, CAdUiPaletteSet)
	ON_WM_CREATE()
	ON_WM_DESTROY()
END_MESSAGE_MAP()

CPaletteSetCmpRes::~CPaletteSetCmpRes()
{
	s_pPaletteSet = NULL;
}

CPaletteSetCmpRes::CPaletteSetCmpRes()
{

}

CSize CPaletteSetCmpRes::CalcFixedLayout(BOOL bStretch, BOOL bHorz)
{
	CSize size = CAdUiPaletteSet::CalcFixedLayout(bStretch, bHorz);
	if (size.cx < 300)
		size.cx = 300;
	return size/*DockRect().Size()*/;
}

int CPaletteSetCmpRes::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CAdUiPaletteSet::OnCreate(lpCreateStruct) == -1)
	{
		return -1;
	}

	return 0;
}

void CPaletteSetCmpRes::OnDestroy()
{
	CAdUiPaletteSet::OnDestroy();

	delete s_pPaletteSet;
}

void CPaletteSetCmpRes::PaletteActivated(CAdUiPalette* pActivated, CAdUiPalette* pDeactivated)
{
	CAdUiPaletteSet::PaletteActivated(pActivated, pDeactivated);

	//CTunnelSectionPalette *pTunnelPalete = dynamic_cast<CTunnelSectionPalette*>(pActivated);
	//if (pTunnelPalete)
	//{
	//	pTunnelPalete->UpdateTree();
	//}
}

BOOL CPaletteSetCmpRes::OnCommand(WPARAM wParam, LPARAM lParam)
{
	BOOL b = CAdUiPaletteSet::OnCommand(wParam, lParam);

	UINT_PTR cmd = wParam;
	switch (cmd)
	{
	case 10001:
		/*CadDiffFramework::Instance()->adjustLayout();
		CadDiffFramework::Instance()->Release();

		CadDiffOneCadFramework::Instance()->adjustLayout();
		CadDiffOneCadFramework::Instance()->Release();*/
		break;
	}

	return b;
}

CPaletteSetCmpRes* CPaletteSetCmpRes::Instance()
{
	if (!s_pPaletteSet)
	{
		CAcModuleResourceOverride resOverride; 

		s_pPaletteSet = new CPaletteSetCmpRes;
		CRect rect(0,0,200,500);	
		s_pPaletteSet->Create(_T("Í¼Ö½¼ì²â½á¹û"), WS_OVERLAPPED|WS_DLGFRAME, rect, acedGetAcadFrame());
		s_pPaletteSet->EnableDocking(CBRS_ALIGN_ANY);
		s_pPaletteSet->DockControlBar(AFX_IDW_DOCKBAR_RIGHT, &rect);
		acedGetAcadFrame()->ShowControlBar(s_pPaletteSet, TRUE, FALSE);
	}

	return s_pPaletteSet;
}

void CPaletteSetCmpRes::Destroy()
{
	if (s_pPaletteSet)
	{
		s_pPaletteSet->DestroyWindow();

		delete s_pPaletteSet;		
	}
}

CRect CPaletteSetCmpRes::DockRect()
{
	long nHight = 0;
	if (!s_pPaletteSet)
	{
		CRect rect;
		::GetWindowRect(acedGetAcadFrame()->m_hWndMDIClient, &rect);
		acedGetAcadFrame()->ScreenToClient(&rect);

		nHight = rect.bottom - rect.top;
	}
	else
	{
		CRect rect;
		s_pPaletteSet->GetWindowRect(&rect);
		s_pPaletteSet->ScreenToClient(&rect);
		nHight = rect.bottom - rect.top;
		if (s_pPaletteSet->IsFloating())
		{
			::GetWindowRect(acedGetAcadFrame()->m_hWndMDIClient, &rect);
			acedGetAcadFrame()->ScreenToClient(&rect);

			nHight = rect.bottom - rect.top;
			if (nHight < 600)
			{
				nHight = 600;
			}
		}
	}

	CRect dockRect(0, 0, 300, nHight);

	return dockRect;
}

void CPaletteSetCmpRes::SetVisible(bool bVisible /*= true*/)
{
	CMDIFrameWnd *pAcadFrame = acedGetAcadFrame();
	//s_pPaletteSet->RestoreControlBar();
	pAcadFrame->ShowControlBar(this, bVisible, FALSE);
}
