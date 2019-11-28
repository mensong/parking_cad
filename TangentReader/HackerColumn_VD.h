#pragma once
#include "HackARX.h"

class HackerColumn_VD
	: public IGiViewportGeometry
{
public:
	HackerColumn_VD();
	~HackerColumn_VD();
	
	static AcString Desc();

	bool isValid();

	AcGePoint3dArray& getSection();

	AcString ToString();

protected:
	virtual Adesk::Boolean mesh(int eval, const Adesk::UInt32 rows, const Adesk::UInt32 columns, 
		const AcGePoint3d* pVertexList,const AcGiEdgeData* pEdgeData = NULL, 
		const AcGiFaceData* pFaceData = NULL, const AcGiVertexData* pVertexData = NULL, 
		const bool bAutoGenerateNormals = true);

private:
	AcGePoint3dArray m_section;
	double m_height;
	bool m_vaild;	
};