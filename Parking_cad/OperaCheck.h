#pragma once
#include "IOperaLog.h"

class COperaCheck :
	public CIOperaLog
{
public:
	COperaCheck(const AcString& group, const AcString& cmd, const AcString& alias, Adesk::Int32 cmdFlag);
	~COperaCheck();
	virtual void Start();
	static std::string ms_uuid;//从界面类获取到回传的uuid的接口
	static void setUuid(const std::string& uuid);
	static void setGetCheckUrl(const std::string& strGetCheckUrl);
	static std::string ms_strGetCheckUrl;
	static CString ms_shearWallLayerName;
	static void setShearWallLayerName(const CString& layerName);
	static void blankCheckShow(AcGePoint2dArray& blankCheckPts,AcDbObjectId& blankId, AcDbDatabase *pDb = acdbCurDwg());
	static AcGePoint2dArray getPlineExtentPts(AcGePoint2dArray plinePts);
	static void creatCloudLine(const AcGePoint2dArray& plineExtentPts, AcGePoint2dArray& plinePts, AcDbObjectId& cloudId, AcDbDatabase *pDb = acdbCurDwg());
	static void setCloudLineWidth(AcDbObjectId cloudLineId);
	static void setCurrentLayer(CString layerName);
	static AcGePoint2dArray getLineOtherPoint(const AcGePoint2d& lineStartPoint,const AcGePoint2d& lineEndPoint,const double& minArcLength, const double& maxArcLength);
	static AcDbObjectId creatArcPline(AcGePoint2dArray points, double startWidth, double endWidth, AcDbDatabase *pDb = acdbCurDwg());
	static int getRandNum(const int& min,const int& max);
	static bool jugeCloudDirection(const AcGePoint2dArray &targetPts);
	//AcGePoint2d afterMobile(AcGeVector2d moveDir,AcGePoint2d startPoint);
	AcGePoint2dArray getOneParkPts(UINT32 parkHandle);
	AcGePoint2dArray getOnePlineEntPts(AcDbEntity* pEntity);
	void overlapShow();
	std::vector<AcGePoint2dArray>  getPlinePtsByLayer(const CString& layername);
	CString doubleToCString(double num);
	int getCheckData(std::string& sMsg, std::string& json);
	bool getDataforJson(const std::string& json, CString& sMsg);
	static AcGePoint2dArray curvePartPointsHandle(AcGePoint2dArray& plinePts,const double& minArcLength);
};

