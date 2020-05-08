#pragma once
#include "IOperaLog.h"

typedef signed char        int8_t;
typedef short              int16_t;
typedef int                int32_t;
typedef long long          int64_t;
typedef unsigned char      uint8_t;
typedef unsigned short     uint16_t;
typedef unsigned int       uint32_t;
typedef unsigned long long uint64_t;


typedef struct LineDistance
{
	AcDbObjectId Lineid;
	double distance;

}LineDistance;

class COperaAxleNetMaking :
	public CIOperaLog
{
public:
	COperaAxleNetMaking(const AcString& group, const AcString& cmd, const AcString& alias, Adesk::Int32 cmdFlag);
	~COperaAxleNetMaking();
	virtual void Start();

	static void drawAxisNumber(AcDbObjectIdArray& LineIds, std::map<AcDbObjectId, AcString>& AxisNumberMap, AcDbDatabase *pDb = acdbCurDwg());

public:
	static void InitialProcessing(const CString& layerNameofAXSI, AcDbObjectIdArray& sortIds, AcGePoint3d& inputstartpt
		, AcGePoint3d& inputendpt, std::vector<AcDbObjectId>& outputIds, AcDbDatabase *pDb = acdbCurDwg());
	static void DrowBigDimaligned(const CString& layerNameofAXSI, std::vector<AcDbObjectId>& inputids, AcGePoint3d& inputstartpt, AcGePoint3d& inputendpt, AcDbDatabase *pDb = acdbCurDwg());
	static AcDbObjectIdArray FilterIds(AcDbObjectIdArray& inputids, std::vector<AcDbObjectId>& inputids1, std::vector<AcDbObjectId>& inputids2);
	static AcDbObjectIdArray FilterIds(AcDbObjectIdArray& inputtotalids, std::vector<AcDbObjectId>& inputids);
	static void AxisLineInfo(AcGePoint3d& inputstartpt, AcGePoint3d& inputendpt, AcGePoint3d& inputSecondstartpt, AcGePoint3d& inputSecondendpt, AcGeVector3d& projectvect_90, AcGeVector3d& projectvect_180,
		AcGePoint3d& startpt, AcGePoint3d& endpt, AcGePoint3d& outpt1, AcGePoint3d& outpt2, AcGePoint3d& outpoint);
	static void SpecialDeal(const CString& layerNameofAXSI, AcDbObjectIdArray& inputIds, AcGePoint3d& inputstartpt, AcGePoint3d& inputendpt
		, AcGePoint3d& inputSecondstartpt, AcGePoint3d& inputSecondendpt, const ACHAR *LineLayerName, AcDbDatabase *pDb = acdbCurDwg());
	//add by yangbing use set axisId data
	static void setAxisIds(AcDbObjectIdArray axisIds);
	static AcDbObjectIdArray ms_axisIds;

public:
	static double getBlockRadius(AcDbObjectId& blockid, double scaleFactor, AcGePoint3d& basept);
	static void getLinePoint(AcDbObjectId& lineid, AcGePoint3d& startpt, AcGePoint3d& endpt);
	static AcGePoint3d getChangPoint(AcGePoint3d& basept, AcGeVector3d& moveagvec, double movedis);
	//inputstartpt,inputendpt辅助线的两点，zoomcoefficient缩放系数，plusorminus轴号线偏折方向,outIds是轴号和与轴号相连接的绿线的ID集合
	static void inserAadAxleNum(AcDbObjectIdArray& sortIds, AcGePoint3d& inputstartpt, AcGePoint3d& inputendpt, double zoomcoefficient = 1, AcDbDatabase *pDb = acdbCurDwg());
	static void inserBlockRec(const AcString& sBlockName, AcGePoint3d& basept, AcGeVector3d& moveagvec, double circleradius
		, const AcString& tagvalue, AcDbDatabase *pDb = acdbCurDwg());
	static void dealBlock(const AcString& sBlockName, AcGePoint3d& startpt, AcGeVector3d& moveagvec1, AcGePoint3d& endpt
		, AcGeVector3d& moveagvec2, double scaleFactor, double circleradius, const AcString& tagvalue, AcDbDatabase *pDb = acdbCurDwg());
	//纵向编号,tagnum为个位字母，addtagnum为十位字母，tagvalue为个位字母值，csAddtagvalue为十位字母值
	static void LongitudinalNumbering(int& tagnum, int& addtagnum, CString& tagvalue, CString& csAddtagvalue);
	//设置实体的颜色
	static void setEntColor(AcDbObjectId& id, uint16_t colornum);
	static AcDbObjectId DrowLine(AcGePoint3d& starpt, AcGePoint3d& endpt, AcDbDatabase *pDb = acdbCurDwg());
	static AcString getTagvaluefromMap(std::map<AcDbObjectId, AcString>& AxisNumberMap, AcDbObjectId& id);

	static void currency(AcDbObjectId& lineid, const AcString& sBlockName, double scaleFactor, double circleradius,
		const AcString& tagvalue, AcDbDatabase *pDb = acdbCurDwg());
	static void _currency(AcDbObjectId& lineid, AcDbObjectId& contrastId, const AcString& sBlockName, double scaleFactor, double circleradius,
		const AcString& tagvalue, AcDbDatabase *pDb = acdbCurDwg());
	static AcGeVector3d getAcgVec(AcGePoint3d& inputPt, AcGeVector3d& inputVec1, AcGeVector3d& inputVec2, double circleradius);
	static AcDbObjectId createGuideLine(AcGePoint3d& inputPt, AcGeVector3d& inputVec, double scaleFactor, AcGePoint3d& outpt, AcDbDatabase *pDb = acdbCurDwg());
};
