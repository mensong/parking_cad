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
//----- MyPaletteSet.h : Declaration of the CMyPaletteSet
//-----------------------------------------------------------------------------
#pragma once

//-----------------------------------------------------------------------------
#include "adui.h"

//-----------------------------------------------------------------------------
class CPaletteSetDrawingAbnormalCheck 
	: public CAdUiPaletteSet
{
	DECLARE_DYNAMIC (CPaletteSetDrawingAbnormalCheck)

public:
	virtual ~CPaletteSetDrawingAbnormalCheck();

public:
	static void Refresh();

	// ������ʵ������
	static CPaletteSetDrawingAbnormalCheck* Instance();

	static void Destroy();

	// ͣ��λ��
	static CRect DockRect();

public:
	// ���ÿɼ���
	void SetVisible(bool bVisible = true);
	
protected:
	CPaletteSetDrawingAbnormalCheck();

	//����ͣ��λ��
	virtual CSize CalcFixedLayout (BOOL bStretch, BOOL bHorz);

	virtual BOOL CanFloat() const{return FALSE;};

	// ����ı��С 
	virtual BOOL CanBeResized() const{ return TRUE;}; 
	
	virtual void PaletteActivated(CAdUiPalette* pActivated, CAdUiPalette* pDeactivated);

	 virtual BOOL OnCommand (WPARAM wParam, LPARAM lParam);

	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();

	DECLARE_MESSAGE_MAP()

private:
	static CPaletteSetDrawingAbnormalCheck* s_pPaletteSet;
} ;
