#include "StdAfx.h"
#include "HackerWall_VD.h"

HackerWall_VD::HackerWall_VD()
	: m_vaild(false)
	, m_height(0.0)
{

}

HackerWall_VD::~HackerWall_VD()
{

}

AcString HackerWall_VD::Desc()
{
	return _T("TCH_WALL");
}

bool HackerWall_VD::isValid()
{
	return m_vaild;
}

AcGeLineSeg3d& HackerWall_VD::getWallLine1()
{
	return m_wallLine1;
}

AcGeLineSeg3d& HackerWall_VD::getWallLine2()
{
	return m_wallLine2;
}

AcString HackerWall_VD::ToString()
{
	if (!m_vaild)
		return _T("");

	AcString sMsg;
	AcString s;
	AcGePoint3d pt1 = m_wallLine1.startPoint();
	AcGePoint3d pt2 = m_wallLine1.endPoint();
	s.format(_T("%.4f,%.4f,%.4f,%.4f,%.4f,%.4f\r\n"), pt1.x, pt1.y, pt1.z, pt2.x, pt2.y, pt2.z);
	sMsg += s;
	pt1 = m_wallLine2.startPoint();
	pt2 = m_wallLine2.endPoint();
	s.format(_T("%.4f,%.4f,%.4f,%.4f,%.4f,%.4f\r\n"), pt1.x, pt1.y, pt1.z, pt2.x, pt2.y, pt2.z);
	sMsg += s;
	s.format(_T("%.4f"), m_height);
	sMsg += s;
	return sMsg;
}

Adesk::Boolean HackerWall_VD::mesh(int eval, const Adesk::UInt32 rows, const Adesk::UInt32 columns, 
	const AcGePoint3d* pVertexList, const AcGiEdgeData* pEdgeData /*= NULL*/, const AcGiFaceData* pFaceData /*= NULL*/, 
	const AcGiVertexData* pVertexData /*= NULL*/, const bool bAutoGenerateNormals /*= true*/)
{
	if (eval == 1 && rows * columns > 7)
	{
		m_wallLine1.set(pVertexList[0], pVertexList[1]);
		m_wallLine2.set(pVertexList[5], pVertexList[6]);

		m_height = pVertexList[1].distanceTo(pVertexList[2]);

		m_vaild = true;
	}
	return Adesk::kTrue;
}

