#pragma once
#include "HackARX.h"

class HackerWall_VD
	: public IGiViewportGeometry
{
public:
	HackerWall_VD();
	~HackerWall_VD();
	
	static AcString Desc();

	bool isValid();

	AcGeLineSeg3d& getWallLine1();
	AcGeLineSeg3d& getWallLine2();

	AcString ToString();

protected:
	virtual Adesk::Boolean mesh(int eval, const Adesk::UInt32 rows, const Adesk::UInt32 columns, 
		const AcGePoint3d* pVertexList,const AcGiEdgeData* pEdgeData = NULL, 
		const AcGiFaceData* pFaceData = NULL, const AcGiVertexData* pVertexData = NULL, 
		const bool bAutoGenerateNormals = true);

private:
	AcGeLineSeg3d m_wallLine1;
	AcGeLineSeg3d m_wallLine2;
	double m_height;
	bool m_vaild;
};