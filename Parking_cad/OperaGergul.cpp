#include "stdafx.h"
#include "OperaGergul.h"
#include "GeHelper.h"


COperaGergul::COperaGergul(void)
{
}


COperaGergul::~COperaGergul(void)
{
}

void COperaGergul::Start()
{
	AcGePoint2dArray pts;
	pts.append(AcGePoint2d(0,0));
	pts.append(AcGePoint2d(100,0));
	pts.append(AcGePoint2d(100,100));
	pts.append(AcGePoint2d(0,100));

	bool b = GeHelper::IsPointOnPolygon(pts, AcGePoint2d(150,50));

}

REG_CMD(COperaGergul, BGY, Gergul);