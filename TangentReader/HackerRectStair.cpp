#include "stdafx.h"
#include "HackerRectStair.h"


HackerRectStair::HackerRectStair(AcDbObject* pObj)
	: m_rotation(0.0)
	, m_width(0)
	, m_singleWidth(0)
	, m_step1Count(0)
	, m_step2Count(0)
	, handrailWidth(0)
	, handrailSideDis(0)
	, handrailsParts(0)
	, rectHandrailExtende(0)
	, startAndEndHandrailExtende(0)
	, restingPlatformWidth(0)
	, restingPlatformThickness(0)
	, stepAlignmentType(0)
	, leftUpstairs(0)
	, floorFeatures(0)
	, evacuationDetectionLine(0)
{
}


HackerRectStair::~HackerRectStair(void)
{
}

AcString HackerRectStair::Desc()
{
	return _T("TCH_RECTSTAIR");
}

Acad::ErrorStatus HackerRectStair::writeDouble(int evalCount, double d)
{
	switch (evalCount)
	{
	case 4:
		m_rotation = d;
		break;
	case 9:
		m_width = d;
		break;
	case 7:
		m_singleWidth = d;
		break;
	case 5:
		stepHeight = d;
		break;
	case 6:
		stepWidth = d;
		break;
	case 11:
		handrailWidth = d;
		break;
	case 12:
		handrailSideDis = d;
		break;
	case 18:
		rectHandrailExtende = d;
		break;
	case 19:
		startAndEndHandrailExtende = d;
		break;
	case 8:
		restingPlatformWidth = d;
		break;
	case 15:
		restingPlatformThickness = d;
		break;
	default:
		break;
	}

	return Acad::eOk;
}

Acad::ErrorStatus HackerRectStair::writePoint3d(int evalCount, const AcGePoint3d& pt)
{
	switch (evalCount)
	{
	case 1:
		m_position = pt;
		break;
	default:
		break;
	}

	return Acad::eOk;
}

Acad::ErrorStatus HackerRectStair::writeUInt8(int evalCount, Adesk::UInt8 u)
{
	switch (evalCount)
	{
	case 8:
		m_step1Count = u;
		break;
	case 9:
		m_step2Count = u;
		break;
	case 12:
		stepAlignmentType = u;
		break;
	case 11:
		leftUpstairs = (u == 0);
		break;
	case 10:
		floorFeatures = u;
		break;
	default:
		break;
	}
	return Acad::eOk;
}

Acad::ErrorStatus HackerRectStair::writeInt32(int evalCount, Adesk::Int32 i)
{
	switch (evalCount)
	{
	case 1:
		m_customGoUpstairsText = i;
		break;
	default:
		break;
	}
	return Acad::eOk;
}

Acad::ErrorStatus HackerRectStair::writeUInt32(int evalCount, Adesk::UInt32 u)
{
	switch (evalCount)
	{
	case 3:
		handrailsParts = u;
		break;
	default:
		break;
	}
	return Acad::eOk;
}

Acad::ErrorStatus HackerRectStair::writeUInt16(int evalCount, Adesk::UInt16 u)
{
	switch (evalCount)
	{
	case 3:
		evacuationDetectionLine = u;
		break;
	default:
		break;
	}
	return Acad::eOk;
}

int HackerRectStair::_getString2Offset()
{
	switch (m_customGoUpstairsText)
	{
	case 1:
		return 0;
	case 2:
		return 2;
	case 3:
		return 2*2;
	}

	return 0;
}

int HackerRectStair::_getInt16Offset()
{
	switch (m_customGoUpstairsText)
	{
	case 1:
		return 0;
	case 2:
		return 1;
	case 3:
		return 1*2;
	}

	return 0;
}

AcString HackerRectStair::ToString()
{
	AcString sRet;
	sRet.format(
		_T("λ��:(%.2f,%.2f,%.2f)\n")
		_T("��ת:%.2f\n")
		_T("��:%.2f\n")
		_T("���ݿ�:%.2f\n")
		_T("1̤������:%d\n")
		_T("2̤������:%d\n")
		_T("̤���߶�:%.2f\n")
		_T("̤�����:%.2f\n")
		_T("���ֿ��:%.2f\n")
		_T("���ֱ�Ե:%.2f\n")
		_T("¥������:%d\n")
		_T("ת�Ƿ������:%.2f\n")
		_T("���������:%.2f\n")
		_T("��Ϣƽ̨���:%.2f\n")
		_T("��Ϣƽ̨���:%.2f\n")
		_T("̤��ȡ��:%d\n")
		_T("�����¥:%s\n")
		_T("¥����:%d\n")
		_T("��ɢ�뾶��ɫ:%d\n")
		, m_position.x, m_position.y, m_position.z, m_rotation,
		m_width, m_singleWidth, m_step1Count, m_step2Count,
		stepHeight, stepWidth, handrailWidth, handrailSideDis,
		handrailsParts, rectHandrailExtende, startAndEndHandrailExtende,
		restingPlatformWidth, restingPlatformThickness, stepAlignmentType,
		leftUpstairs?_T("��"):_T("��"), floorFeatures, evacuationDetectionLine
		);
	return sRet;
}