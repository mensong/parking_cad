#include "stdafx.h"
#include "OperaNavToolbar.h"
#include "PaletteToolbar.h"

REG_CMD(COperaNavToolbar, BGY, aipaknav);

COperaNavToolbar::COperaNavToolbar()
{
}


COperaNavToolbar::~COperaNavToolbar()
{
}

void COperaNavToolbar::Start()
{
	CPaletteToolbar::show();
}
