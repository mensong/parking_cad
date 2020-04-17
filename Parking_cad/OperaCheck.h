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
	int getCheckData(std::string& sMsg, std::string& json);
	bool getDataforJson(const std::string& json, CString& sMsg);
	void blankCheckShow(const AcGePoint2dArray& blankCheckPts);
	void overlapShow();
	std::vector<AcGePoint2dArray>  getPlinePtsByLayer(const CString& layername);
	AcGePoint2dArray getPlineExtentPts(AcGePoint2dArray plinePts);
	void creatCloudLine(AcGePoint2dArray plineExtentPts);
	CString doubleToCString(double num);
	static void setCloudLineWidth(AcDbObjectId cloudLineId);
	static void setCurrentLayer(CString layerName);
	AcGePoint2dArray getLineOtherPoint(const AcGePoint2d& lineStartPoint,const AcGePoint2d& lineEndPoint,const double& minArcLength, const double& maxArcLength);
	AcDbObjectId creatArcPline(AcGePoint2dArray points, double width);
	int getRandNum(const int& min,const int& max);
	//AcGePoint2d afterMobile(AcGeVector2d moveDir,AcGePoint2d startPoint);
	AcGePoint2dArray getOneParkPts(UINT32 parkHandle);
	AcGePoint2dArray getOnePlineEntPts(AcDbEntity* pEntity);
};

