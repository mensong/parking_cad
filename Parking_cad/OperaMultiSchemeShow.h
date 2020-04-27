#pragma once
#include "IOperaLog.h"
class COperaMultiSchemeShow :
	public CIOperaLog
{
public:
	COperaMultiSchemeShow(const AcString& group, const AcString& cmd, const AcString& alias, Adesk::Int32 cmdFlag);
	~COperaMultiSchemeShow();
	virtual void Start();
	static std::string ms_json;//ai返回数据待解析使用
	static int ms_count;//多方案总数
	static void getJsonData(const std::string& json,const int& count);
	bool addEntToDb(const std::string& json, CString& sMsg, AcDbDatabase *pDataBase,int scheme = 0);
	void creatNewParkingBlock(const double& dParkingLength, const double& dParkingWidth, CString& blockName, AcDbDatabase *pDb = acdbCurDwg());
	void parkingShow(AcDbObjectId& parkingId, const AcGePoint2d& parkingShowPt, const double& parkingShowRotation, 
		const CString& blockName, AcDbDatabase *pDb = acdbCurDwg());
	AcDbObjectId axisShow(const AcGePoint2dArray& axisPts, AcDbDatabase *pDb = acdbCurDwg());
	void creatNewDwg(AcDbDatabase *rootPDb = acdbCurDwg());
	void loadModelFile(AcDbDatabase *pDb = acdbCurDwg());
	AcDbObjectId laneShow(const AcGePoint2dArray& lanePts,AcDbDatabase *pDb = acdbCurDwg());
	void scopeShow(const AcGePoint2dArray& park_columnPts,AcDbDatabase *pDb = acdbCurDwg());
	void pillarShow(const AcGePoint2dArray& onePillarPts,AcDbDatabase *pDb = acdbCurDwg());
	void arrowShow(const AcGePoint2dArray& oneArrowPts,AcDbDatabase *pDb = acdbCurDwg());
	void setLandDismensions(double m_dDis, const AcDbObjectIdArray& RoadLineIds,AcDbDatabase *pDb = acdbCurDwg());
	AcDbObjectId createDimAligned(const AcGePoint3d& pt1, const AcGePoint3d& pt2, const AcGePoint3d& ptLine, const ACHAR* dimText,AcDbDatabase *pDb = acdbCurDwg());

	class ArxProgressBar* m_ProgressBar;
	virtual void Ended() override;

};

