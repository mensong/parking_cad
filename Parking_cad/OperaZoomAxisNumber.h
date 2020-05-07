#pragma once
#include "Opera.h"

class COperaZoomAxisNumber :
	public COpera
{
public:
	COperaZoomAxisNumber();
	~COperaZoomAxisNumber();
	virtual void Start();

public:
	static AcGePoint3dArray getLinesPoints(AcDbObjectIdArray& LineIds);
	//求最近距离
	static double getMinDis(AcGePoint3d& pt, AcDbObjectId& lineid);
	//删除指定图层所有的实体
	static void deleteEntitys(const AcString& layername);

	std::vector<std::vector<AcDbObjectId>> batchStorageAxis(AcDbObjectIdArray& AxisIds);

};

