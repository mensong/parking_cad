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

	void BatchStorageEnt(AcDbObjectIdArray& inputId, std::vector<std::vector<AcDbObjectId>>& outputId);//�����洢����ʵ��
	void GenerateGuides(double& changdistance, std::vector<AcDbObjectId>& operaIds, AcDbObjectIdArray& GuideIds);//�����������߸����ߣ�changdistanceΪ������ȣ�GuideIdsΪ������ID
	void Select_Ent(std::vector<AcDbEntity*>& vctEnts, AcGePoint3d& centerpoint);
	bool PromptSelectEnts(const TCHAR* prompt, AcRxClass* classDesc, AcDbObjectIdArray &entIds);
	bool PromptSelectEnts(const TCHAR* prompt, const std::vector<AcRxClass*> &classDescs, AcDbObjectIdArray &entIds);
	AcDbObjectId PostToModelSpace(AcDbEntity* pEnt); //��ʵ����ӵ�ģ�Ϳռ���
	void DealEnt(AcDbEntity* pEnt, AcGePoint3dArray& intersectPoints);//�����ཻ��ʵ�壬ʹ�뽻������Ķ˵����óɽ����ֵ
	void SaveEntInfo(double& movedistance, AcDbObjectIdArray& inputIds, std::vector<InfoStructLine>& saveLineInfoVector, std::vector<InfoStructArc>& saveArcInfoVector, std::vector<AcGePoint3d>& inserpoint);
	void DealIntersectEnt(AcDbObjectIdArray& inputIds);//�����ཻ��ʵ��
	void ConnectionPoint(AcDbObjectIdArray& inputIds);//������ͬһ��ֱ���ϵ������㣬���ӳ��߶�
	bool IsOnLine(AcGePoint2d& pt1, AcGePoint2d& pt2, AcGePoint2d& pt3);//�жϵ��Ƿ���ͬһ��ֱ����
	void SpecialSaveEntInfo(double& movedistance, AcDbObjectIdArray& inputIds, std::vector<InfoStructLine>& saveLineInfoVector, std::vector<InfoStructArc>& saveArcInfoVector, std::vector<AcGePoint3d>& inserpoint);
	void MultipleCycles(double& movedistance, AcDbObjectIdArray& inputIds);//���ѭ����ʵ���ӳ���ֱ�����е�ʵ���ཻ

	//ɾ�������ڳ�λ
	void deleParkInEntrance(const AcGePoint2dArray plinePts);
	void creatPlinePoints(const AcDbObjectIdArray allLineIds);
	AcGePoint2d getPlineNextPoint(const AcGePoint2d targetPt, AcGePoint2dArray &nextUsedPts, const std::vector<AcGePoint2dArray> allLinePts, AcGePoint2dArray &resultPts);
	bool checkClosed(const AcGePoint2d checkPt, const std::vector<AcGePoint2dArray> allLinePts);
	AcDbObjectIdArray explodeEnty(AcDbObjectIdArray& entIds);
	void getpoint(AcDbEntity* pEnt, AcGePoint3d& startpt, AcGePoint3d& endpt);
	bool isIntersect(AcDbEntity* pEnt, AcDbEntity* pTempEnt, double tol);//�ж��Ƿ��ཻ

};

