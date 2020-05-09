#include "stdafx.h"
#include "OperaFindCloudShow.h"
#include "PaletteCmpResTwoCad.h"


COperaFindCloudShow::COperaFindCloudShow()
{
}


COperaFindCloudShow::~COperaFindCloudShow()
{
}

void COperaFindCloudShow::Start()
{
	CPaletteCmpResTwoCad::show();
}


REG_CMD(COperaFindCloudShow, BGY, FindCloudShow);