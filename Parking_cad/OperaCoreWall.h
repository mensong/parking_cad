#pragma once
#include "opera.h"
#include <vector>
#include <set>

class COperaCoreWall :
	public COpera
{
public:
	COperaCoreWall(void);
	~COperaCoreWall(void);

	virtual void Start();

protected:
	void seg2AABBBox(const AcGePoint2d& pt1, const AcGePoint2d& pt2, double minPt[2], double maxPt[2]);
	void removeOverlapPoint(std::vector<AcGePoint2d>& points, const AcGeTol& tol = AcGeContext::gTol);
	AcGePoint2dArray getSegsOrderPoints(std::vector<AcGeLineSeg2d>& segs, AcGeTol tol = AcGeContext::gTol);
	AcGePoint2dArray getSegsOrderPoints2(std::vector<AcGeLineSeg2d>& segs, AcGeTol tol = AcGeContext::gTol);
	void cycleProcessing(const std::vector<AcGeLineSeg2d>& segs, std::set<AcGeLineSeg2d*>& compareSegs, AcGeLineSeg2d*& lastSeg, AcGeTol tol, AcGePoint2dArray& outpoints);
	void point2AABBBox(double minPt[2], double maxPt[2], const AcGePoint2d& pt, double width);

	AcGeTol m_tol;
};
