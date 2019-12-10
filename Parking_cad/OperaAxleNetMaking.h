#pragma once
#include "Opera.h"


typedef struct LineDistance
{
	AcDbObjectId Lineid;
	double distance;

}LineDistance;

class COperaAxleNetMaking :
	public COpera
{
public:
	COperaAxleNetMaking();
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
		
};
