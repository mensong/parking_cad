#include "stdafx.h"
#include "OperaShowToolbar.h"


REG_CMD(COperaShowToolbar, BGY, ParkingToolbar);

COperaShowToolbar::COperaShowToolbar()
{
}


COperaShowToolbar::~COperaShowToolbar()
{
}

void COperaShowToolbar::Start()
{
	//加载工具条
	AcString filepath = (GetUserDir() + _T("parking_cad.cuix")).GetString();
	LoadCuix::Load(filepath);

	std::string sCheckToobar;
	AcString sCadVer = DBHelper::CadVersionString();
	sCheckToobar = GetUserDirA() + GL::WideByte2Ansi(sCadVer.constPtr()) + "\\";
	if (!FileHelper::IsFolderExistA(sCheckToobar))
		FileHelper::CreateMultipleDirectoryA(sCheckToobar.c_str());
	sCheckToobar += "toolbar.shown";
	if (!FileHelper::IsFileExitstA(sCheckToobar))
	{//只加载一次
		LoadCuix::ShowToolbarAsyn(_T("智能地库"));
		LoadCuix::SetUnloadOnExit(_T("PARKING_CAD"));
		FileHelper::WriteFile(sCheckToobar.c_str(), "1", 1);//标记一下已经加载过图标
	}
}
