#pragma once
#include <map>

class PointKey
{
public:
	PointKey(void);
	~PointKey(void);

	AcGePoint2d getPoint(const AcGePoint2d& pt);

	AcString getPointKey(const AcGePoint2d& pt);

protected:
	std::map<AcString, AcGePoint2d> m_mpPointKey;
};

