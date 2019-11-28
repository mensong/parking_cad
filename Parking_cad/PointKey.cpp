#include "stdafx.h"
#include "PointKey.h"
#include "GeHelper.h"


PointKey::PointKey(void)
{
}


PointKey::~PointKey(void)
{
}

AcGePoint2d PointKey::getPoint(const AcGePoint2d& pt)
{
	AcString sKey = getPointKey(pt);
	std::map<AcString, AcGePoint2d>::iterator itFinder = m_mpPointKey.find(sKey);
	if (itFinder != m_mpPointKey.end())
		return itFinder->second;
	m_mpPointKey[sKey] = pt;
	return pt;
}

AcString PointKey::getPointKey(const AcGePoint2d& pt)
{
	AcString sKey;
	sKey.format(_T("%.4f,%.4f"), 
		GeHelper::eq(pt.x, 0.0) ? fabs(pt.x) : pt.x, 
		GeHelper::eq(pt.y, 0.0) ? fabs(pt.y) : pt.y);
	return sKey;
}
