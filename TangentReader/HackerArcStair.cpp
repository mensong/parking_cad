#include "stdafx.h"
#include "HackerArcStair.h"



HackerArcStair::HackerArcStair(AcDbObject* pObj)
	: m_rotation(0)
	, m_stepCount(0)
	, m_stepHeight(0)
	, m_width(0)
	, m_stepWidth(0)
	, m_sideType(0)
	, m_sideBeamHeight(0)
	, m_sideBeamWidth(0)
	, m_rampType(0)
{
	if (AcString(pObj->isA()->dxfName()) != Desc())
		throw false;
}

HackerArcStair::~HackerArcStair(void)
{
}

const ACHAR* HackerArcStair::Desc()
{
	return _T("TCH_ARCSTAIR");
}

Acad::ErrorStatus HackerArcStair::writePoint3d(int evalCount, const AcGePoint3d& pt)
{
	if (evalCount == 1)
		m_position = pt;
	return Acad::eOk;
}

Acad::ErrorStatus HackerArcStair::writeDouble(int evalCount, double d)
{
	switch (evalCount)
	{
	case 12:
		m_rotation = d;
		break;
	case 4:
		m_stepHeight = d;
		break;
	case 11:
		m_width = d;
		break;
	case 10:
		m_stepWidth = d;
		break;
	case 7:
		m_sideBeamHeight = d;
		break;
	case 8:
		m_sideBeamWidth = d;
		break;
	default:
		break;
	}
	return Acad::eOk;
}

Acad::ErrorStatus HackerArcStair::writeUInt16(int evalCount, Adesk::UInt16 u)
{
	switch (evalCount)
	{
	case 3:
		m_stepCount = u;
		break;
	default:
		break;
	}
	return Acad::eOk;
}

Acad::ErrorStatus HackerArcStair::writeUInt8(int evalCount, Adesk::UInt8 u)
{
	switch (evalCount)
	{
	case 7:
		m_sideType = u;
		break;
	default:
		break;
	}
	return Acad::eOk;
}


Acad::ErrorStatus HackerArcStair::writeUInt32(int evalCount, Adesk::UInt32 u)
{
	switch (evalCount)
	{
	case 2:
		m_rampType = u;
		break;
	default:
		break;
	}
	return Acad::eOk;
}


AcString HackerArcStair::ToString()
{
	AcString sRet;
	sRet.format(
		_T("λ��:(%.2f,%.2f,%.2f)\n")
		_T("�ݶ�ת��:%.2f\n")
		_T("̤����Ŀ:%d\n")
		_T("̤���߶�:%.2f\n")
		_T("�ݶο��:%.2f\n")
		_T("̤�����:%.2f\n")
		_T("sideType:%d\n")
		_T("�����߶�:%.2f\n")
		_T("�������:%.2f\n")
		_T("�µ���Ϣ:%d\n")
		, m_position.x, m_position.y, m_position.z,
		m_rotation, m_stepCount, m_stepHeight,
		m_width, m_stepWidth, m_sideType, 
		m_sideBeamHeight,m_sideBeamWidth,
		m_rampType
		);
	return sRet;
}
