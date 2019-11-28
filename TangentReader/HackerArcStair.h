#pragma once
#include "HackARX.h"

class HackerArcStair :
	public IDbDwgFiler
{
public:
	HackerArcStair(AcDbObject* pObj);
	~HackerArcStair(void);

	static const ACHAR* Desc();

	AcString ToString();

protected:
	virtual Acad::ErrorStatus writePoint3d(int evalCount, const AcGePoint3d& pt);
	virtual Acad::ErrorStatus writeDouble(int evalCount, double);
	virtual Acad::ErrorStatus writeUInt16(int evalCount, Adesk::UInt16);
	virtual Acad::ErrorStatus writeUInt8(int evalCount, Adesk::UInt8);
	virtual Acad::ErrorStatus writeUInt32(int evalCount, Adesk::UInt32);

private:
	AcGePoint3d m_position;//位置
	double m_rotation;//旋转
	Adesk::UInt16 m_stepCount;//踏步数目
	double m_stepHeight;//踏步高度
	double m_width;//梯段宽度
	double m_stepWidth;//踏步宽度
	Adesk::UInt8 m_sideType;//“边梁有无类型”“剖断方式”
	double m_sideBeamHeight;//边梁高度
	double m_sideBeamWidth;//边梁宽度
	Adesk::UInt32 m_rampType;//“是/否作为坡道”“是/否有防滑条”“是/否落地”“靠墙方式”
};

