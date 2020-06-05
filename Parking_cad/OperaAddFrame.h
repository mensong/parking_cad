#pragma once
#include "IOperaLog.h"

class COperaAddFrame :
	public CIOperaLog
{
public:
	COperaAddFrame(const AcString& group, const AcString& cmd, const AcString& alias, Adesk::Int32 cmdFlag);
	~COperaAddFrame();

	virtual void Start();
	
public:
	//zhangzechi
	static AcGePoint2d GetChangePoint(AcGePoint2d& centerpt, AcGePoint2d& changpt);
	static std::vector<AcGePoint2dArray> getPlinePointForLayer(std::vector<AcDbObjectId>& inputIds, const AcString& layerNameofEnty, bool isCreat = true);
	static double getPloyLineArea(std::vector<AcDbObjectId>& inputIds, const AcString& layerNameofEnty, bool isCreat = true);
	static double getNumberOfCars(std::vector<AcDbObjectId>& inputIds, const AcString& layerNameofEnty);
	static std::string setPicAttributeData(double SPvalue, double SPF1value, double CPvalue, double SPF2value, std::map<std::string, double>& picAttributedata);
	static double getPicAttributeValue(std::map<std::string, double>& picAttributedata, const std::string& picAttributeTage);
	static std::string setStringData(double inputValue, const std::string& Keydata);
	//static bool isHasBlockName(const AcString& blockname, AcString& outblockname);
	static bool isHasBlockName(const CString& blockname, CString& outblockname);
	//yangbing
	static void setTableDataMap(const std::map<std::string, double>& tableData);
	static std::map<std::string, double> ms_mapTableData;
	static void setOutLineLayerName(const CString& sOutLineLayerName);
	static CString ms_sOutLineLayerName;
	static double getIntersectionArea(const std::vector<AcGePoint2dArray>& targetPlinePts,const std::vector<AcGePoint2dArray>& usePlinePts);
};

