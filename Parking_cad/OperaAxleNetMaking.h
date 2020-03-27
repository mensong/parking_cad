#pragma once
#include "IOperaLog.h"


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

public:
	static void InitialProcessing(const CString& layerNameofAXSI,AcDbObjectIdArray& sortIds,AcGePoint3d& inputstartpt, AcGePoint3d& inputendpt, std::vector<AcDbObjectId>& outputIds);
	static void DrowBigDimaligned(const CString& layerNameofAXSI,std::vector<AcDbObjectId>& inputids, AcGePoint3d& inputstartpt, AcGePoint3d& inputendpt);
	static AcDbObjectIdArray FilterIds(AcDbObjectIdArray& inputids, std::vector<AcDbObjectId>& inputids1, std::vector<AcDbObjectId>& inputids2);
	static AcDbObjectIdArray FilterIds(AcDbObjectIdArray& inputtotalids, std::vector<AcDbObjectId>& inputids);
	static void AxisLineInfo(AcGePoint3d& inputstartpt, AcGePoint3d& inputendpt, AcGePoint3d& inputSecondstartpt, AcGePoint3d& inputSecondendpt, AcGeVector3d& projectvect_90, AcGeVector3d& projectvect_180,
		AcGePoint3d& startpt, AcGePoint3d& endpt, AcGePoint3d& outpt1, AcGePoint3d& outpt2, AcGePoint3d& outpoint);
	static void SpecialDeal(const CString& layerNameofAXSI,AcDbObjectIdArray& inputIds, AcGePoint3d& inputstartpt, AcGePoint3d& inputendpt, AcGePoint3d& inputSecondstartpt, AcGePoint3d& inputSecondendpt, const ACHAR *LineLayerName);
	//add by yangbing use set axisId data
	static void setAxisIds(AcDbObjectIdArray axisIds);
	static AcDbObjectIdArray ms_axisIds;

public:
	static double getBlockRadius(AcDbObjectId& blockid, double scaleFactor, AcGePoint3d& basept);
	static void getLinePoint(AcDbObjectId& lineid, AcGePoint3d& startpt, AcGePoint3d& endpt);
	static AcGePoint3d getChangPoint(AcGePoint3d& basept, AcGeVector3d& moveagvec, double movedis);
	static void inserAadAxleNum(AcDbObjectIdArray& sortIds, AcGePoint3d& inputstartpt, AcGePoint3d& inputendpt, int casenum, int plusorminus = 1);
	static void inserBlockRec(const AcString& sBlockName, AcGePoint3d& basept, AcGeVector3d& moveagvec, double circleradius
		, const AcString& tagvalue);
	static void dealBlock(const AcString& sBlockName, AcGePoint3d& startpt, AcGeVector3d& moveagvec1
		, AcGePoint3d& endpt, AcGeVector3d& moveagvec2, double scaleFactor, double circleradius, const AcString& tagvalue);
	//纵向编号,tagnum为个位字母，addtagnum为十位字母，tagvalue为个位字母值，csAddtagvalue为十位字母值
	static void LongitudinalNumbering(int& tagnum, int& addtagnum, CString& tagvalue, CString& csAddtagvalue);
	//创建折线，scaleFactor为扩大比例值，plusorminus偏折的角度（只有1和-1）
	static void creatOtherGuideLine(AcGePoint3d& startpt, AcGePoint3d& endpt, AcGePoint3d& movept1, AcGeVector3d& moveagvec1
		, AcGePoint3d& movept2, AcGeVector3d& moveagvec2, double scaleFactor, int plusorminus = 1);
	//创建轴号块定义
	static void createAxleNumBlockTableRecord(const CString& blockname);
	//设置实体的颜色
	static void setEntColor(AcDbObjectId& id, uint16_t colornum);
	static AcDbObjectId DrowLine(AcGePoint3d& starpt, AcGePoint3d& endpt);
};
