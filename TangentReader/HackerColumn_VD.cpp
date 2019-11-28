#include "StdAfx.h"
#include "HackerColumn_VD.h"

HackerColumn_VD::HackerColumn_VD()
	: m_vaild(false)
	, m_height(0.0)
{

}

HackerColumn_VD::~HackerColumn_VD()
{

}

AcString HackerColumn_VD::Desc()
{
	return _T("TCH_COLUMN");
}

bool HackerColumn_VD::isValid()
{
	return m_vaild;
}

AcGePoint3dArray& HackerColumn_VD::getSection()
{
	return m_section;
}

AcString HackerColumn_VD::ToString()
{
	if (!m_vaild)
		return _T("");

	AcString sMsg;
	AcString s;
	for (int i=0; i<m_section.length(); ++i)
	{	
		AcGePoint3d& pt = m_section[i];
		s.format(_T("%.4f,%.4f,%.4f\r\n"), pt.x, pt.y, pt.z);
		sMsg += s;
	}

	s.format(_T("%.4f"), m_height);
	sMsg += s;

	return sMsg;
}

Adesk::Boolean HackerColumn_VD::mesh(int eval, const Adesk::UInt32 rows, const Adesk::UInt32 columns, 
	const AcGePoint3d* pVertexList, const AcGiEdgeData* pEdgeData /*= NULL*/, const AcGiFaceData* pFaceData /*= NULL*/, 
	const AcGiVertexData* pVertexData /*= NULL*/, const bool bAutoGenerateNormals /*= true*/)
{
	if (eval == 1 && rows * columns > 3)
	{
		m_section.removeAll();
		int n = (rows * columns)/2 - 1;//上下截面各一个，只取底下的截面，所以/2；-1为去掉重合的首尾点
		for (int i=0; i<n; ++i)
		{
			m_section.append(pVertexList[i]);
		}

		m_height = pVertexList[0].distanceTo(pVertexList[(rows * columns) / 2]);

		m_vaild = true;
	}
	return Adesk::kTrue;
}

