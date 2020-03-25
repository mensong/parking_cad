#pragma once
#include "IOperaLog.h"
#include <vector>
#include <set>

class COperaCoreWall :
	public CIOperaLog
{
public:
	COperaCoreWall(const AcString& group, const AcString& cmd, const AcString& alias, Adesk::Int32 cmdFlag);
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

	/*暂时简化核心筒功能*/
	void setCoreWallData();
	void coreWallDataCulation();
};
