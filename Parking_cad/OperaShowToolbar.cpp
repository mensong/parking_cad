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
	//���ع�����
	AcString filepath = (GetUserDir() + _T("parking_cad.cuix")).GetString();
	LoadCuix::Load(filepath);

	std::string sCheckToobar;
	AcString sCadVer = DBHelper::CadVersionString();
	sCheckToobar = GetUserDirA() + GL::WideByte2Ansi(sCadVer.constPtr()) + "\\";
	if (!FileHelper::IsFolderExistA(sCheckToobar))
		FileHelper::CreateMultipleDirectoryA(sCheckToobar.c_str());
	sCheckToobar += "toolbar.shown";
	if (!FileHelper::IsFileExitstA(sCheckToobar))
	{//ֻ����һ��
		LoadCuix::ShowToolbarAsyn(_T("���ܵؿ�"));
		LoadCuix::SetUnloadOnExit(_T("PARKING_CAD"));
		FileHelper::WriteFile(sCheckToobar.c_str(), "1", 1);//���һ���Ѿ����ع�ͼ��
	}
}
