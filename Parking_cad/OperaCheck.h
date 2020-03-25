#pragma once
#include "E:\Programming\arxtools\Opera.h"
class COperaCheck :
	public COpera
{
public:
	COperaCheck();
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
	static void setCurrentLayer(CString layerName);
};

