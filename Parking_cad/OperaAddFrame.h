#pragma once
#include "Opera.h"

class COperaAddFrame :
	public COpera
{
public:
	COperaAddFrame();
	~COperaAddFrame();
	virtual void Start();

public:
	//zhangzechi
	static AcGePoint2d GetChangePoint(AcGePoint2d& centerpt, AcGePoint2d& changpt);
	static std::vector<AcGePoint2dArray> getPlinePointForLayer(std::vector<AcDbObjectId>& inputIds, const AcString& layerNameofEnty);
	static double getPloyLineArea(std::vector<AcDbObjectId>& inputIds, const AcString& layerNameofEnty);
	static double getNumberOfCars(std::vector<AcDbObjectId>& inputIds, const AcString& layerNameofEnty);
	static std::string setPicAttributeData(double SPF1value, double CPvalue,std::map<std::string, double>& picAttributedata);
	static double getPicAttributeValue(std::map<std::string, double>& picAttributedata, const std::string& picAttributeTage);
	static std::string setStringData(double inputValue, const std::string& Keydata);
	static bool isHasBlockName(const AcString& blockname, AcString& outblockname);
	//yangbing
	static void setTextStr(const std::string& strText);
	static std::string ms_strText;
};

