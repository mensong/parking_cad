#pragma once
class CMinimumRectangle
{
public:
	CMinimumRectangle();
	~CMinimumRectangle();

	static AcGePoint3dArray getMinRact(AcGePoint3dArray& allpoints);
	static AcGePoint3dArray testfilterPoints(AcGePoint3dArray& allpoints, AcGePoint3dArray& filterpoints);
};

