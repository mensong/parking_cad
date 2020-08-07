// (C) Copyright 2002-2012 by Autodesk, Inc. 
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
//------ StdAfx.cpp : source file that includes just the standard includes
//------  StdAfx.pch will be the pre-compiled header
//------  StdAfx.obj will contain the pre-compiled type information
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include <shlobj.h> 

//static void initConfigs()
//{
//	DBHelper::GetArxDirA() + "..\\..\\"
//}

std::string GetUserDirA()
{
	char szDir[MAX_PATH];
	SHGetSpecialFolderPathA(NULL, szDir, CSIDL_APPDATA, 0);
	std::string sDir = szDir;
	sDir += "\\碧桂园车库智能设计系统\\";
	return sDir;
}

CString GetUserDir()
{
	TCHAR szDir[MAX_PATH];
	SHGetSpecialFolderPath(NULL, szDir, CSIDL_APPDATA, 0);
	CString sDir = szDir;
	sDir += _T("\\碧桂园车库智能设计系统\\");
	return sDir;
}
