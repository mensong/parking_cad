#include "StdAfx.h"
#include "OperaCoreWall.h"
#include "Boundary.h"
#include "ModulesManager.h"
#include <id.h>
#include "../HackARX_SDK/HackARX.h"
#include "DBHelper.h"
#include "../TangentReader/HackerWall_VD.h"
#include "../TangentReader/HackerColumn_VD.h"
#include "PointKey.h"
#include "GeHelper.h"
#include "RTreeEx.h"
#include "Convertor.h"
#include "EquipmentroomTool.h"

COperaCoreWall::COperaCoreWall(void)
{
	m_tol.setEqualPoint(1);
}

COperaCoreWall::~COperaCoreWall(void)
{
}

void COperaCoreWall::Start()
{
	setCoreWallData();
}

void COperaCoreWall::coreWallDataCulation()
{
	if (ACADV_RELMAJOR > 20)
	{
		acutPrintf(_T("\n核心筒生成功能只能在天正环境下工作。后继版本再优化。"));
		return;
	}

	typedef int(*FN_calcConvexHull)(CPoint_2 resuls[], CPoint_2 points[], int pointsCount);
	typedef int(*FN_calcArrangement)(CPoint_2 resuls[], int* arrFaceSegsLen, CPoint_2 segPoints[], int nSegs);

	FN_calcConvexHull calcConvexHull = ModulesManager::Instance().func<FN_calcConvexHull>("Boundary.dll", "calcConvexHull");
	FN_calcArrangement calcArrangement = ModulesManager::Instance().func<FN_calcArrangement>("Boundary.dll", "calcArrangement");
	if (!calcConvexHull || !calcArrangement)
	{
		acutPrintf(_T("\r\n不能加载模块:Boundary.dll"));
		return;
	}

	typedef IWatcherDwgOutFields* (*FN_CreateDwgOutFieldsWatcher)();
	typedef IWatcherSubWorldDraw* (*FN_CreateSubWorldDrawWatcher)();
	typedef IWatcherSubViewportDraw* (*FN_CreateSubViewportDrawWatcher)();
	typedef void(*FN_ReleaseDwgOutFieldsWatcher)(IWatcherDwgOutFields* pWatcher);
	typedef void(*FN_ReleaseSubWorldDrawWatcher)(IWatcherSubWorldDraw* pWatcher);
	typedef void(*FN_ReleaseSubViewportDrawWatcher)(IWatcherSubViewportDraw* pWatcher);

	char szHackARX[20];
	sprintf(szHackARX, "BGYHackARX%d.arx", ACADV_RELMAJOR);
	FN_CreateDwgOutFieldsWatcher CreateDwgOutFieldsWatcher = ModulesManager::Instance().func
		<FN_CreateDwgOutFieldsWatcher>(szHackARX, "CreateDwgOutFieldsWatcher");
	FN_CreateSubWorldDrawWatcher CreateSubWorldDrawWatcher = ModulesManager::Instance().func
		<FN_CreateSubWorldDrawWatcher>(szHackARX, "CreateSubWorldDrawWatcher");
	FN_CreateSubViewportDrawWatcher CreateSubViewportDrawWatcher = ModulesManager::Instance().func
		<FN_CreateSubViewportDrawWatcher>(szHackARX, "CreateSubViewportDrawWatcher");
	FN_ReleaseDwgOutFieldsWatcher ReleaseDwgOutFieldsWatcher = ModulesManager::Instance().func
		<FN_ReleaseDwgOutFieldsWatcher>(szHackARX, "ReleaseDwgOutFieldsWatcher");
	FN_ReleaseSubWorldDrawWatcher ReleaseSubWorldDrawWatcher = ModulesManager::Instance().func
		<FN_ReleaseSubWorldDrawWatcher>(szHackARX, "ReleaseSubWorldDrawWatcher");
	FN_ReleaseSubViewportDrawWatcher ReleaseSubViewportDrawWatcher = ModulesManager::Instance().func
		<FN_ReleaseSubViewportDrawWatcher>(szHackARX, "ReleaseSubViewportDrawWatcher");
	if (!CreateDwgOutFieldsWatcher || !CreateSubWorldDrawWatcher || !CreateSubViewportDrawWatcher ||
		!ReleaseDwgOutFieldsWatcher || !ReleaseSubWorldDrawWatcher || !ReleaseSubViewportDrawWatcher)
	{
		AcString sHackARX;
		sHackARX.format(_T("\r\n不能加载模块:BGYHackARX%d.arx"), ACADV_RELMAJOR);
		acutPrintf(sHackARX);
		return;
	}

	IWatcherSubViewportDraw* pVdWatcher = CreateSubViewportDrawWatcher();

	std::vector<AcGeLineSeg2d*> segs;

	std::vector<AcDbObjectId> ids = DBHelper::SelectEntity(_T("\r\n请选择天正墙自定义实体:"));

	actrTransactionManager->startTransaction();

	//先删除门窗洞，再处理
	std::set<int> openingIdx;
	for (int i = 0; i < ids.size(); ++i)
	{
		AcDbEntity* pEnt = NULL;
		if (Acad::eOk != acdbOpenObject(pEnt, ids[i], AcDb::kForRead))
			continue;

		const ACHAR* name = pEnt->isA()->dxfName();
		if (name != HackerWall_VD::Desc() && name != HackerColumn_VD::Desc())
		{
			pEnt->upgradeOpen();
			pEnt->erase();
			pEnt->downgradeOpen();
			openingIdx.insert(i);
		}

		pEnt->close();
	}

	for (int i = 0; i < ids.size(); ++i)
	{
		if (openingIdx.find(i) != openingIdx.end())
			continue;

		AcDbEntity* pEnt = NULL;
		if (Acad::eOk != acdbOpenObject(pEnt, ids[i], AcDb::kForRead))
			continue;

		const ACHAR* name = pEnt->isA()->dxfName();

		if (name == HackerWall_VD::Desc())
		{
			HackerWall_VD hacker;
			pVdWatcher->setReader(&hacker);
			pVdWatcher->watch(pEnt);
			if (hacker.isValid())
			{
				AcGeLineSeg3d seg1 = hacker.getWallLine1();
				AcGeLineSeg3d seg2 = hacker.getWallLine2();

				AcGePoint3d ptStart1 = seg1.startPoint();
				AcGePoint3d ptEnd1 = seg1.endPoint();
				AcGePoint3d ptStart2 = seg2.startPoint();
				AcGePoint3d ptEnd2 = seg2.endPoint();

				segs.push_back(new AcGeLineSeg2d(GeHelper::PT32(ptStart1), GeHelper::PT32(ptEnd1)));
				segs.push_back(new AcGeLineSeg2d(GeHelper::PT32(ptStart2), GeHelper::PT32(ptEnd2)));
				segs.push_back(new AcGeLineSeg2d(GeHelper::PT32(ptStart1), GeHelper::PT32(ptStart2)));
				segs.push_back(new AcGeLineSeg2d(GeHelper::PT32(ptEnd1), GeHelper::PT32(ptEnd2)));
			}
		}
		else if (name == HackerColumn_VD::Desc())
		{
			HackerColumn_VD hacker;
			pVdWatcher->setReader(&hacker);
			pVdWatcher->watch(pEnt);
			if (hacker.isValid())
			{
				AcGePoint3dArray& pts = hacker.getSection();
				for (int p = 0; p < pts.length(); ++p)
				{
					AcGePoint3d& pt1 = pts[p];
					AcGePoint3d& pt2 = pts[(p + 1) % pts.length()];
					segs.push_back(new AcGeLineSeg2d(GeHelper::PT32(pt1), GeHelper::PT32(pt2)));
				}
			}
		}

		pEnt->close();
	}

	actrTransactionManager->abortTransaction();

	ReleaseSubViewportDrawWatcher(pVdWatcher);

	if (segs.size() == 0)
		return;

	PointKey pk;

	RT::RTreeEx<AcGeLineSeg2d*, double, 2> rtSegs;
	CPoint_2* result = new CPoint_2[segs.size() * segs.size()];
	CPoint_2* segPointsForHull = new CPoint_2[2 * segs.size()];
	for (int i = 0; i < segs.size(); ++i)
	{
		AcGeLineSeg2d* seg = segs[i];
		int idx = 2 * i;
		AcGePoint2d pt1 = pk.getPoint(seg->startPoint());
		segPointsForHull[idx].x = pt1.x;
		segPointsForHull[idx].y = pt1.y;
		//acutPrintf(_T("\r\n%.4f,%.4f"), pt.x, pt.y);
		AcGePoint2d pt2 = pk.getPoint(seg->endPoint());
		segPointsForHull[idx + 1].x = pt2.x;
		segPointsForHull[idx + 1].y = pt2.y;
		//acutPrintf(_T("\r\n%.4f,%.4f"), pt.x, pt.y);
		double minPt[2], maxPt[2];
		seg2AABBBox(pt1, pt2, minPt, maxPt);
		rtSegs.Insert(minPt, maxPt, seg);
	}
	//acutPrintf(_T("\r\n========================="));


	int nHullPointCount = calcConvexHull(result, segPointsForHull, 2 * segs.size());
	std::vector<AcGePoint2d> convexHullPoints;
	std::vector<AcGeLineSeg2d> convexHullSegs;
	for (int i = 0; i < nHullPointCount; ++i)
	{
		convexHullPoints.push_back(AcGePoint2d(result[i].x, result[i].y));
	}
	removeOverlapPoint(convexHullPoints);

#ifdef _DEBUG
	acutPrintf(_T("\r\n==============start convex hull=============="));
	for (int i = 0; i < convexHullPoints.size(); ++i)
	{
		acutPrintf(_T("\r\nline %.4f,%.4f %.4f,%.4f\r\n"), convexHullPoints[i].x, convexHullPoints[i].y, convexHullPoints[(i + 1) % convexHullPoints.size()].x, convexHullPoints[(i + 1) % convexHullPoints.size()].y);
	}
	acutPrintf(_T("\r\n==============end convex hull=============="));
#endif

	std::vector<AcGeLineSeg2d> segsNeedOnHull;
	std::vector<AcGeLineSeg2d> segsNoNeedOnHull;
	for (int i = 0; i < convexHullPoints.size(); ++i)
	{
		AcGePoint2d pt1 = pk.getPoint(convexHullPoints[i]);
		AcGePoint2d pt2 = pk.getPoint(convexHullPoints[(i + 1) % convexHullPoints.size()]);
		convexHullSegs.push_back(AcGeLineSeg2d(pt1, pt2));

		bool bNeed = false;
		double minPt[2], maxPt[2];
		seg2AABBBox(pt1, pt2, minPt, maxPt);
		std::set<AcGeLineSeg2d*> setFind;
		rtSegs.Search(minPt, maxPt, &setFind);
		for (std::set<AcGeLineSeg2d*>::iterator it = setFind.begin(); it != setFind.end(); ++it)
		{
			AcGePoint2d pt3 = pk.getPoint((*it)->startPoint());
			AcGePoint2d pt4 = pk.getPoint((*it)->endPoint());
			if ((pt1.isEqualTo(pt3, m_tol) && pt2.isEqualTo(pt4, m_tol)) || (pt1.isEqualTo(pt4, m_tol) && pt2.isEqualTo(pt3, m_tol)))
			{
				segsNeedOnHull.push_back(AcGeLineSeg2d(pt1, pt2));
				bNeed = true;
				break;
			}
		}

		if (!bNeed)
			segsNoNeedOnHull.push_back(AcGeLineSeg2d(pt1, pt2));
	}

#ifdef _DEBUG
	acutPrintf(_T("\n===========segsNeedOnHull===========\n"));
	for (int i = 0; i < segsNeedOnHull.size(); ++i)
	{
		AcGeLineSeg2d& line = segsNeedOnHull[i];
		acutPrintf(_T("LINE %.4f,%.4f %.4f,%.4f\r\n"), line.startPoint().x, line.startPoint().y, line.endPoint().x, line.endPoint().y);
	}

	acutPrintf(_T("\n===========segsNoNeedOnHull===========\n"));
	for (int i = 0; i < segsNoNeedOnHull.size(); ++i)
	{
		AcGeLineSeg2d& line = segsNoNeedOnHull[i];
		acutPrintf(_T("LINE %.4f,%.4f %.4f,%.4f\r\n"), line.startPoint().x, line.startPoint().y, line.endPoint().x, line.endPoint().y);
	}
#endif

	const double SEG_EX = 0.5;

	int arrangementPointsCount = 2 * segs.size() + 2 * segsNoNeedOnHull.size();
	CPoint_2* segPointsForArrangement = new CPoint_2[arrangementPointsCount];
	int p = 0;
	for (int i = 0; i < 2 * segs.size(); i += 2, p += 2)
	{
		//segPointsForArrangement[p].x = segPointsForHull[i].x;
		//segPointsForArrangement[p].y = segPointsForHull[i].y;
		//segPointsForArrangement[p + 1].x = segPointsForHull[i + 1].x;
		//segPointsForArrangement[p + 1].y = segPointsForHull[i + 1].y;

		AcGeVector2d dir(segPointsForHull[i + 1].x - segPointsForHull[i].x,
			segPointsForHull[i + 1].y - segPointsForHull[i].y);
		dir = dir.normal() * SEG_EX;

		AcGePoint2d ptStart(segPointsForHull[i].x, segPointsForHull[i].y);
		AcGePoint2d ptEnd(segPointsForHull[i + 1].x, segPointsForHull[i + 1].y);
		//ptStart.transformBy(-dir);
		//ptEnd.transformBy(dir);
		ptStart = pk.getPoint(ptStart);
		ptEnd = pk.getPoint(ptEnd);

		segPointsForArrangement[p].x = ptStart.x;
		segPointsForArrangement[p].y = ptStart.y;

		segPointsForArrangement[p + 1].x = ptEnd.x;
		segPointsForArrangement[p + 1].y = ptEnd.y;
	}
	for (int i = 0; i < segsNoNeedOnHull.size(); ++i, p += 2)
	{
		//segPointsForArrangement[p].x = segsNoNeedOnHull[i].startPoint().x;
		//segPointsForArrangement[p].y = segsNoNeedOnHull[i].startPoint().y;
		//segPointsForArrangement[p + 1].x = segsNoNeedOnHull[i].endPoint().x;
		//segPointsForArrangement[p + 1].y = segsNoNeedOnHull[i].endPoint().y;

		AcGeVector2d dir = segsNoNeedOnHull[i].endPoint() - segsNoNeedOnHull[i].startPoint();
		dir = dir.normal() * SEG_EX;

		AcGePoint2d ptStart = segsNoNeedOnHull[i].startPoint();
		AcGePoint2d ptEnd = segsNoNeedOnHull[i].endPoint();
		//ptStart.transformBy(-dir);
		//ptEnd.transformBy(dir);
		ptStart = pk.getPoint(ptStart);
		ptEnd = pk.getPoint(ptEnd);

		segPointsForArrangement[p].x = ptStart.x;
		segPointsForArrangement[p].y = ptStart.y;

		segPointsForArrangement[p + 1].x = ptEnd.x;
		segPointsForArrangement[p + 1].y = ptEnd.y;
	}

	//acutPrintf(_T("\r\n==============start segPointsForArrangement=============="));
	//for (int i=0; i<arrangementPointsCount; i += 2)
	//{
	//	acutPrintf(_T("\nline %f,%f %f,%f\r\n"), segPointsForArrangement[i].x, segPointsForArrangement[i].y, segPointsForArrangement[i+1].x, segPointsForArrangement[i+1].y);
	//}
	//acutPrintf(_T("\r\n==============end segPointsForArrangement=============="));

	std::vector<std::vector<AcGeLineSeg2d> > faces;
	int* arrFaceSegsLen = new int[arrangementPointsCount * arrangementPointsCount];
	int nFace = calcArrangement(result, arrFaceSegsLen, segPointsForArrangement, segs.size() + segsNoNeedOnHull.size());
	p = 0;
	for (int i = 0; i < nFace; ++i)
	{
		int faceSegsLen = arrFaceSegsLen[i];
		std::vector<AcGeLineSeg2d> face;
		for (int j = 0; j < faceSegsLen; ++j)
		{
			if (AcGePoint2d(result[p].x, result[p].y).distanceTo(AcGePoint2d(result[p + 1].x, result[p + 1].y)) < SEG_EX * 2)
			{
				p += 2;
				continue;
			}

			face.push_back(AcGeLineSeg2d(AcGePoint2d(result[p].x, result[p].y), AcGePoint2d(result[p + 1].x, result[p + 1].y)));

			//acutPrintf(_T("\r\nline %.4f,%.4f %.4f,%.4f\r\n"), result[p].x, result[p].y, result[p+1].x, result[p+1].y);

			p += 2;
		}

		if (face.size() >= 3)
			faces.push_back(face);

		//acutPrintf(_T("\r\n========================="));
	}

	delete[] result;
	delete[] segPointsForHull;
	delete[] segPointsForArrangement;
	delete[] arrFaceSegsLen;

	//根据面边线构造多段线
	std::vector<AcGePoint2dArray> plines;
	for (int f = 0; f < faces.size(); ++f)
	{
		std::vector<AcGeLineSeg2d>& face = faces[f];

		AcGePoint2dArray pts = getSegsOrderPoints(face, m_tol);
		plines.push_back(pts);
	}

#ifdef _DEBUG
	acutPrintf(_T("\n========FACES========\n"));
	for (int i = 0; i < plines.size(); ++i)
	{
		AcGePoint2dArray& pts = plines[i];
		acutPrintf(_T("\r\nline "));
		for (int j = 0; j < pts.length(); ++j)
		{
			acutPrintf(_T("%.4f,%.4f "), pts[j].x, pts[j].y);
		}
	}
#endif

	//找到与不需要的hull边与之相向望的所有边
	for (int i = 0; i < segsNoNeedOnHull.size(); ++i)
	{
		AcGeLineSeg2d& seg = segsNoNeedOnHull[i];
		AcGePoint2d ptCenter = seg.evalPoint(0.5);
		AcGeVector2d prepDir = seg.direction().perpVector().normal();

		bool bFound = false;
		AcGePoint2dArray ptsConcave;

		//找到NoNeedOnHull所在的凹闭合区域
		AcGePoint2d ptTest = ptCenter;
		ptTest.transformBy(prepDir * 1);
		for (int f = 0; f < plines.size(); ++f)
		{
			AcGePoint2dArray& pts = plines[f];
			if (GeHelper::IsPointOnPolygon(pts, ptTest))
			{
				bFound = true;
				ptsConcave = pts;
				break;
			}
		}
		if (!bFound)
		{
			ptTest = ptCenter;
			ptTest.transformBy(prepDir * -1);
			for (int f = 0; f < plines.size(); ++f)
			{
				AcGePoint2dArray& pts = plines[f];
				if (GeHelper::IsPointOnPolygon(pts, ptTest))
				{
					bFound = true;
					ptsConcave = pts;
					break;
				}
			}
		}
		if (!bFound)
			continue;

#ifdef _DEBUG
		acutPrintf(_T("\n========valid face========\n"));
#endif
		for (int j = 0; j < ptsConcave.length(); ++j)
		{
			AcGePoint2d& pt1 = ptsConcave[j];
			AcGePoint2d& pt2 = ptsConcave[(j + 1) % ptsConcave.length()];

#ifdef _DEBUG
			acutPrintf(_T("\r\nline %.4f,%.4f %.4f,%.4f\n"), pt1.x, pt1.y, pt2.x, pt2.y);
#endif

			if ((pt1.isEqualTo(seg.startPoint(), m_tol) && pt2.isEqualTo(seg.endPoint(), m_tol)) ||
				(pt1.isEqualTo(seg.endPoint(), m_tol) && pt2.isEqualTo(seg.startPoint(), m_tol)))
			{//排除掉NoNeedOnHull的边
				continue;
			}

			segsNeedOnHull.push_back(AcGeLineSeg2d(pt1, pt2));
		}
	}

#ifdef _DEBUG
	acutPrintf(_T("\n===========RESULT1===========\n"));
	for (int i = 0; i < segsNeedOnHull.size(); ++i)
	{
		AcGeLineSeg2d& line = segsNeedOnHull[i];
		acutPrintf(_T("LINE %.4f,%.4f %.4f,%.4f\r\n"), line.startPoint().x, line.startPoint().y, line.endPoint().x, line.endPoint().y);
	}
#endif

	AcGePoint2dArray ptsMaxEdge = getSegsOrderPoints(segsNeedOnHull, m_tol);
#ifdef _DEBUG
	std::string s = GeHelper::getCadDebugLine(ptsMaxEdge, false);
	acutPrintf(_T("\n===========RESULT2===========\n"));
	acutPrintf(GL::Ansi2WideByte(s.c_str()).c_str());
#endif

	AcDbPolyline* pPline = new AcDbPolyline;
	for (int i = 0; i < ptsMaxEdge.length(); ++i)
	{
		pPline->addVertexAt(i, ptsMaxEdge[i]);
	}
	pPline->setClosed(Adesk::kTrue);
	pPline->setColorIndex(2);
	AcDbObjectId coreWallId;
	DBHelper::AppendToDatabase(coreWallId, pPline);
	pPline->close();
	CEquipmentroomTool::layerSet(_T("0"), 7);
	CString sCoreWallLayer(CEquipmentroomTool::getLayerName("core_wall").c_str());
	CEquipmentroomTool::creatLayerByjson("core_wall");
	CEquipmentroomTool::setEntToLayer(coreWallId, sCoreWallLayer);
	CEquipmentroomTool::layerSet(_T("0"), 7);
}

void COperaCoreWall::setCoreWallData()
{
	CString sCoreWallLayer(CEquipmentroomTool::getLayerName("core_wall").c_str());
	CEquipmentroomTool::creatLayerByjson("core_wall");

	std::vector<AcDbEntity*> vctEquipmentEnt;
	ads_name ssname;
	ads_name ent;
	//获取选择集
	acedPrompt(_T("\n请选择核心筒的多线段实体："));
	struct resbuf *rb; // 结果缓冲区链表
	rb = acutBuildList(RTDXF0, _T("LWPOLYLINE"), RTNONE);
	// 选择复合要求的实体
	acedSSGet(NULL, NULL, NULL, rb, ssname);
	//获取选择集的长度
	Adesk::Int32 len = 0;
	int nRes = acedSSLength(ssname, &len);
	if (RTNORM == nRes)
	{
		//遍历选择集
		for (int i = 0; i < len; i++)
		{
			//获取实体名
			int nRes = acedSSName(ssname, i, ent);
			if (nRes != RTNORM)
				continue;
			//根据实体名得到ID，然后打开自定义实体
			AcDbObjectId id;
			acdbGetObjectId(id, ent);
			if (!id.isValid())
				continue;
			AcDbEntity *pEnt = NULL;
			acdbOpenObject(pEnt, id, AcDb::kForWrite);
			//判断自定义实体的类型
			if (pEnt == NULL)
				continue;
			vctEquipmentEnt.push_back(pEnt);
		}
	}
	//释放选择集和结果缓存区
	acutRelRb(rb);
	acedSSFree(ssname);
	if (vctEquipmentEnt.size() < 1)
		return;
	//拿到用户选择多线段实体指针
	AcDbObjectIdArray EquipmentIds;
	for (int i = 0; i < vctEquipmentEnt.size(); i++)
	{
		if (vctEquipmentEnt[i]->isKindOf(AcDbPolyline::desc()))
		{
			AcDbObjectId EquipmentId = vctEquipmentEnt[i]->objectId();
			EquipmentIds.append(EquipmentId);
		}
		vctEquipmentEnt[i]->close();
	}
	for (int j = 0; j < EquipmentIds.length(); j++)
	{
		CEquipmentroomTool::setEntToLayer(EquipmentIds[j], sCoreWallLayer);
	}
	CEquipmentroomTool::layerSet(_T("0"), 7);//操作完成回到零图层
}

void COperaCoreWall::seg2AABBBox(const AcGePoint2d& pt1, const AcGePoint2d& pt2, double minPt[2], double maxPt[2])
{
	AcDbExtents ext;
	ext.addPoint(GeHelper::PT23(pt1));
	ext.addPoint(GeHelper::PT23(pt2));
	minPt[0] = ext.minPoint().x;
	minPt[1] = ext.minPoint().y;
	maxPt[0] = ext.maxPoint().x;
	maxPt[1] = ext.maxPoint().y;
}

void COperaCoreWall::removeOverlapPoint(std::vector<AcGePoint2d>& points, const AcGeTol& tol /*= AcGeContext::gTol*/)
{
	std::vector<AcGePoint2d> willBeRemoved;
	for (int i=0; i<points.size(); )
	{
		int j = i+1;
		for (; j<points.size() + 1; ++j)
		{
			if (points[i].isEqualTo(points[j % points.size()], tol))
			{
				willBeRemoved.push_back(points[j % points.size()]);
			}
			else
			{
				break;
			}
		}
		i = j;
	}

	for (int i=0; i<willBeRemoved.size(); ++i)
	{
		std::vector<AcGePoint2d>::iterator itFinder = std::find(points.begin(), points.end(), willBeRemoved[i]);
		if (itFinder != points.end())
		{
			points.erase(itFinder);
		}
	}
}

void COperaCoreWall::point2AABBBox(double minPt[2], double maxPt[2], const AcGePoint2d& pt, double width)
{
	AcGePoint2d ptTmp = pt;
	ptTmp.transformBy(AcGeVector2d(-1, -1) * (width / 2));
	minPt[0] = ptTmp.x;
	minPt[1] = ptTmp.y;
	maxPt[0] = ptTmp.x + width;
	maxPt[1] = ptTmp.y + width;
}

AcGePoint2dArray COperaCoreWall::getSegsOrderPoints(std::vector<AcGeLineSeg2d>& segs, AcGeTol tol/* = AcGeContext::gTol*/)
{

	std::set<AcGeLineSeg2d*> comparedSegs;
	AcGePoint2dArray outpoints;

	bool tag = true;
	AcGeLineSeg2d* lastSeg = NULL;

	for (int i = 0; i < segs.size(); ++i)
	{
		AcGePoint2d startpt = segs[i].startPoint();
		AcGePoint2d endpt = segs[i].endPoint();

		for (int k = i + 1; k < segs.size(); ++k)
		{
			AcGePoint2d tempstartpt = segs[k].startPoint();
			AcGePoint2d tempendpt = segs[k].endPoint();

			if (startpt.isEqualTo(tempstartpt, tol))
			{
				comparedSegs.insert(&segs[i]);
				comparedSegs.insert(&segs[k]);
				lastSeg = &segs[k];
				outpoints.append(endpt);
				outpoints.append(startpt);
				outpoints.append(tempstartpt);
				cycleProcessing(segs, comparedSegs, lastSeg, tol, outpoints);
				tag = false;
				break;
			}
			else if (startpt.isEqualTo(tempendpt,tol))
			{
				comparedSegs.insert(&segs[i]);
				comparedSegs.insert(&segs[k]);
				lastSeg = &segs[k];
				outpoints.append(endpt);
				outpoints.append(startpt);
				outpoints.append(tempendpt);
				cycleProcessing(segs, comparedSegs, lastSeg, tol, outpoints);
				tag = false;
				break;
			}
			else if (endpt.isEqualTo(tempstartpt, tol))
			{
				comparedSegs.insert(&segs[i]);
				comparedSegs.insert(&segs[k]);
				lastSeg = &segs[k];
				outpoints.append(startpt);
				outpoints.append(endpt);
				outpoints.append(tempstartpt);
				cycleProcessing(segs, comparedSegs, lastSeg, tol, outpoints);
				tag = false;
				break;
			}
			else if (endpt.isEqualTo(tempendpt, tol))
			{
				comparedSegs.insert(&segs[i]);
				comparedSegs.insert(&segs[k]);
				lastSeg = &segs[k];
				outpoints.append(startpt);
				outpoints.append(endpt);
				outpoints.append(tempendpt);
				cycleProcessing(segs, comparedSegs, lastSeg, tol, outpoints);
				tag = false;
				break;
			}
		}

		if (!tag)
			break;
	}

	int len = outpoints.length() / 2;
	for (int i=1; i<=len; ++i)
	{
		outpoints.remove(outpoints[i]);
	}

	return outpoints;

}

AcGePoint2dArray COperaCoreWall::getSegsOrderPoints2(std::vector<AcGeLineSeg2d>& segs, AcGeTol tol /*= AcGeContext::gTol*/)
{
	RT::RTreeEx<AcGeLineSeg2d*, double, 2> rt;
	double minPt[2], maxPt[2];
	for (int i=0; i<segs.size(); ++i)
	{
		point2AABBBox(minPt, maxPt, segs[i].startPoint(), 1);

	}

	return AcGePoint2dArray();
}

void COperaCoreWall::cycleProcessing(const std::vector<AcGeLineSeg2d>& segs, std::set<AcGeLineSeg2d*>& compareSegs, AcGeLineSeg2d*& lastSeg, AcGeTol tol, AcGePoint2dArray& outpoints)
{

	//int num = compareSegs.size() - 1;
	AcGePoint2d tempstartpt = lastSeg->startPoint();
	AcGePoint2d tempendpt =  lastSeg->endPoint();

	AcGePoint2d lastpt;

	bool bIllegal = true;
	for (int i = 0; i < segs.size(); ++i)
	{
		if(compareSegs.find(const_cast<AcGeLineSeg2d*>(&segs[i])) != compareSegs.end())
			continue;
		AcGePoint2d startpt = segs[i].startPoint();
		AcGePoint2d endpt = segs[i].endPoint();

		if (startpt.isEqualTo(tempstartpt, tol))
		{
			lastSeg = const_cast<AcGeLineSeg2d*>(&segs[i]);
			compareSegs.insert(lastSeg);
			outpoints.append(tempstartpt);
			outpoints.append(startpt);
			lastpt = endpt;
			bIllegal = false;
			break;
		}
		else if (startpt.isEqualTo(tempendpt, tol))
		{
			lastSeg = const_cast<AcGeLineSeg2d*>(&segs[i]);
			compareSegs.insert(lastSeg);
			outpoints.append(tempendpt);
			outpoints.append(startpt);
			lastpt = endpt;
			bIllegal = false;
			break;
		}
		else if (endpt.isEqualTo(tempstartpt, tol))
		{
			lastSeg = const_cast<AcGeLineSeg2d*>(&segs[i]);
			compareSegs.insert(lastSeg);
			outpoints.append(tempstartpt);
			outpoints.append(endpt);
			lastpt = startpt;
			bIllegal = false;
			break;
		}
		else if (endpt.isEqualTo(tempendpt, tol))
		{
			lastSeg = const_cast<AcGeLineSeg2d*>(&segs[i]);
			compareSegs.insert(lastSeg);
			outpoints.append(tempendpt);
			outpoints.append(endpt);
			lastpt = startpt;
			bIllegal = false;
			break;
		}
	}

	if (bIllegal)
		return;

	if (compareSegs.size() != segs.size())
	{
		cycleProcessing(segs, compareSegs, lastSeg, tol, outpoints);
	}
	else
	{
		outpoints.append(lastpt);
		return;
	}
}


REG_CMD(COperaCoreWall, BGY, CoreWall);