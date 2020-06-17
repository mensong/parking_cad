#include "stdafx.h"
#include "OperaFindCloudShow.h"
#include "PaletteDrawingAbnormalCheck.h"


COperaFindCloudShow::COperaFindCloudShow()
{
}


COperaFindCloudShow::~COperaFindCloudShow()
{
}

void COperaFindCloudShow::Start()
{
	CPaletteDrawingAbnormalCheck::show();
}


REG_CMD(COperaFindCloudShow, BGY, FindCloudShow);