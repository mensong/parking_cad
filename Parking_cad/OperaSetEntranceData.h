#pragma once
#include "Opera.h"
#include "DlgEntrance.h"

struct InfoStructLine
{
	AcDbObjectId entId;
	AcGePoint3d startpoint;
	AcGePoint3d endpoint;
};
struct InfoStructArc
{
	AcDbObjectId entId;
	double startAngle;
	double endAngle;
};

class COperaSetEntranceData :
	public COpera
{
public:
	COperaSetEntranceData();
	~COperaSetEntranceData();
	virtual void Start();
	AcGeTol m_tol;
	static class CDlgEntrance* ms_EntranceDlg;
	AcDbObjectId creatDim(const AcGePoint3d& pt1, const AcGePoint3d& pt2, const AcGePoint3d& pt3, const CString sLegth);
	AcDbObjectId creatArcDim(const AcGePoint3d& pt1, const AcGePoint3d& pt2, const AcGePoint3d& pt3, const AcGePoint3d& pt4);
	void test(const AcDbObjectIdArray entIds);
	void creatEntrance();

	void BatchStorageEnt(AcDbObjectIdArray& inputId, std::vector<std::vector<AcDbObjectId>>& outputId);//分批存储相连实体
	void GenerateGuides(double& changdistance, std::vector<AcDbObjectId>& operaIds, AcDbObjectIdArray& GuideIds);//中线引出两边辅助线，changdistance为车道宽度，GuideIds为辅助线ID
	void Select_Ent(std::vector<AcDbEntity*>& vctEnts, AcGePoint3d& centerpoint);
	bool PromptSelectEnts(const TCHAR* prompt, AcRxClass* classDesc, AcDbObjectIdArray &entIds);
	bool PromptSelectEnts(const TCHAR* prompt, const std::vector<AcRxClass*> &classDescs, AcDbObjectIdArray &entIds);
	AcDbObjectId PostToModelSpace(AcDbEntity* pEnt); //将实体添加到模型空间中
	void DealEnt(AcDbEntity* pEnt, AcGePoint3dArray& intersectPoints);//处理相交的实体，使离交点最近的端点设置成交点的值
	void SaveEntInfo(double& movedistance, AcDbObjectIdArray& inputIds, std::vector<InfoStructLine>& saveLineInfoVector, std::vector<InfoStructArc>& saveArcInfoVector, std::vector<AcGePoint3d>& inserpoint);
	void DealIntersectEnt(AcDbObjectIdArray& inputIds);//处理相交的实体
	void ConnectionPoint(AcDbObjectIdArray& inputIds);//将处于同一条直线上的三个点，连接成线段
	bool IsOnLine(AcGePoint2d& pt1, AcGePoint2d& pt2, AcGePoint2d& pt3);//判断点是否在同一条直线上
	void SpecialSaveEntInfo(double& movedistance, AcDbObjectIdArray& inputIds, std::vector<InfoStructLine>& saveLineInfoVector, std::vector<InfoStructArc>& saveArcInfoVector, std::vector<AcGePoint3d>& inserpoint);
	void MultipleCycles(double& movedistance, AcDbObjectIdArray& inputIds);//多次循环将实体延长，直达所有的实体相交

	//删除区域内车位
	void deleParkInEntrance(const AcGePoint2dArray plinePts);
	void creatPlinePoints(const AcDbObjectIdArray allLineIds);
	AcGePoint2d getPlineNextPoint(const AcGePoint2d targetPt, AcGePoint2dArray &nextUsedPts, const std::vector<AcGePoint2dArray> allLinePts, AcGePoint2dArray &resultPts);
	bool checkClosed(const AcGePoint2d checkPt, const std::vector<AcGePoint2dArray> allLinePts);
	AcDbObjectIdArray explodeEnty(AcDbObjectIdArray& entIds);
	void getpoint(AcDbEntity* pEnt, AcGePoint3d& startpt, AcGePoint3d& endpt);
	bool isIntersect(AcDbEntity* pEnt, AcDbEntity* pTempEnt, double tol);//判断是否相交

};

