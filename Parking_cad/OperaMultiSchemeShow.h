#pragma once
#include <json/json.h>
#include "IOperaLog.h"
class COperaMultiSchemeShow :
	public CIOperaLog
{
public:
	COperaMultiSchemeShow(const AcString& group, const AcString& cmd, const AcString& alias, Adesk::Int32 cmdFlag);
	~COperaMultiSchemeShow();
	virtual void Start();
	virtual void Ended() override;

	static std::string ms_json;//ai返回数据待解析使用
	static AcDbDatabase *ms_prootDb;
	static CString ms_newFileName;
	static void getJsonData(const std::string& json);
	static void getRootDataBaseAndFileName(AcDbDatabase* backUpDataBase, const CString& fileName);
	bool addEntToDb(Json::Value json, AcDbDatabase *pDataBase,int scheme = 0);
	void creatNewParkingBlock(const double& dParkingLength, const double& dParkingWidth, CString& blockName, AcDbDatabase *pDb = acdbCurDwg());
	void parkingShow(AcDbObjectId& parkingId, const AcGePoint2d& parkingShowPt, const double& parkingShowRotation, 
		const CString& blockName, AcDbDatabase *pDb = acdbCurDwg());
	AcDbObjectId axisShow(const AcGePoint2dArray& axisPts, AcDbDatabase *pDb = acdbCurDwg());
	void creatNewDwg(AcDbDatabase *rootPDb = acdbCurDwg());
	void loadModelFile(AcDbDatabase *pDb = acdbCurDwg());
	AcDbObjectId laneShow(const AcGePoint2dArray& lanePts, AcDbDatabase *pDb = acdbCurDwg());
	void scopeShow(const AcGePoint2dArray& park_columnPts, AcDbObjectId& scopeId, AcDbDatabase *pDb = acdbCurDwg());
	void pillarShow(const AcGePoint2dArray& onePillarPts, AcDbObjectId& pillarId, AcDbDatabase *pDb = acdbCurDwg());
	void arrowShow(const AcGePoint2dArray& oneArrowPts, AcDbObjectId& arrowId, AcDbDatabase *pDb = acdbCurDwg());
	void setLandDismensions(double m_dDis, const AcDbObjectIdArray& RoadLineIds,AcDbDatabase *pDb = acdbCurDwg());
	AcDbObjectId createDimAligned(const AcGePoint3d& pt1, const AcGePoint3d& pt2, const AcGePoint3d& ptLine, const ACHAR* dimText,AcDbDatabase *pDb = acdbCurDwg());

	bool parsingParkingData(Json::Value& parkings, CString& sMsg, CString& blockName, AcDbDatabase *pDb = acdbCurDwg());
	bool parsingData(Json::Value& data, double& dParkingLength, double& dParkingWidth, double& dLaneWidth, CString& sMsg);
	bool parsingAxisData(Json::Value& axis, CString& sMsg, std::map<AcDbObjectId, AcString>& idAndNumMap,AcDbObjectIdArray& axisIds, AcDbDatabase *pDb = acdbCurDwg());
	bool parsingLaneData(Json::Value& lane, CString& sMsg, AcDbObjectIdArray& RoadLineIds, AcDbDatabase *pDb = acdbCurDwg());
	bool parsingScopeData(Json::Value& scope, CString& sMsg, AcDbDatabase *pDb = acdbCurDwg());
	bool parsingArrowData(Json::Value& arrow, CString& sMsg, AcDbObjectIdArray& arrowIds,AcDbDatabase *pDb = acdbCurDwg());
	bool parsingPillarData(Json::Value& pillar, CString& sMsg, AcDbObjectIdArray& arrowIds, AcDbDatabase *pDb = acdbCurDwg());
	bool parsingBlanksData(Json::Value& blanks, CString& sMsg, AcDbObjectIdArray& blankIds, AcDbDatabase *pDb = acdbCurDwg());

private:
	Json::Value m_json;
};

