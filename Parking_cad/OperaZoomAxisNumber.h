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
	//���������
	static double getMinDis(AcGePoint3d& pt, AcDbObjectId& lineid);
	//ɾ��ָ��ͼ�����е�ʵ��
	static void deleteEntitys(const AcString& layername);
};

