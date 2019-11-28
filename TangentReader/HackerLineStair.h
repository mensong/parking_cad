#pragma once
#include "HackARX.h"

class HackerLineStair :
	public IDbDwgFiler
{
public:
	HackerLineStair(AcDbObject* pObj);
	~HackerLineStair(void);

	static const ACHAR* Desc();

	AcString ToString();

protected:
	virtual Acad::ErrorStatus writePoint3d(int evalCount, const AcGePoint3d& pt);
	virtual Acad::ErrorStatus writeDouble(int evalCount, double);
	virtual Acad::ErrorStatus writeUInt16(int evalCount, Adesk::UInt16);
	virtual Acad::ErrorStatus writeUInt8(int evalCount, Adesk::UInt8);
	virtual Acad::ErrorStatus writeUInt32(int evalCount, Adesk::UInt32);

private:
	AcGePoint3d m_position;//λ��
	double m_rotation;//�ݶ�ת��
	Adesk::UInt16 m_stepCount;//̤����Ŀ
	double m_stepHeight;//̤���߶�
	double m_width;//�ݶο��
	double m_stepWidth;//̤�����
	Adesk::UInt8 m_sideType;//�������������͡����ʶϷ�ʽ��
	double m_sideBeamHeight;//�����߶�
	double m_sideBeamWidth;//�������
	Adesk::UInt32 m_rampType;//����/����Ϊ�µ�������/���з�����������/����ء�����ǽ��ʽ��
};

