#include "stdafx.h"
#include "EquipmentroomTool.h"
#include<cmath>
#include "JigHelper.h"
#include "RTreeEx.h"
#include "DBHelper.h"
#include <algorithm>
#include "GeHelper.h"

CEquipmentroomTool::CEquipmentroomTool()
{
}


CEquipmentroomTool::~CEquipmentroomTool()
{
}


void CEquipmentroomTool::jigShow(AcDbObjectIdArray useJigIds, double sideLength)
{
	std::vector<AcDbEntity*> vctJigEnt;
	for (int i = 0; i < useJigIds.length(); i++)
	{
		AcDbEntity *pEnt = NULL;
		acdbOpenObject(pEnt, useJigIds[i], AcDb::kForWrite);
		//判断自定义实体的类型
		if (pEnt == NULL)
			continue;
		pEnt->setVisibility(AcDb::kInvisible);
		vctJigEnt.push_back(pEnt);
	}
	if (vctJigEnt.size() < 1)
		return;
	//获得实体的范围的左下角点作为jig基点
	AcDbExtents extsAll;
	AcDbExtents exts;
	for (int i = 0; i < vctJigEnt.size(); ++i)
	{
		if (vctJigEnt[i]->getGeomExtents(exts) == Acad::eOk)
			extsAll.addExt(exts);
	}
	AcGePoint3d ptBase = extsAll.minPoint();
	CJigHelper jig;
	jig.setDispPrompt(_T("请选择放置点："));
	jig.SetBasePoint(ptBase);
	jig.RegisterAsJigEntity(vctJigEnt);
	CJigHelper::RESULT ec = jig.startJig();
	if (ec != CJigHelper::RET_POINT)
	{
		//无效输入则撤销
		//AcGeVector3d vec = jig.GetBasePoint() - jig.GetPosition();
		for (int i = 0; i < vctJigEnt.size(); ++i)
		{
			//vctJigEnt[i]->transformBy(vec);
			vctJigEnt[i]->erase();
		}
	}
	//关闭图形实体
	for (int i = 0; i < vctJigEnt.size(); ++i)
	{
		vctJigEnt[i]->setVisibility(AcDb::kVisible);
		vctJigEnt[i]->close();
	}
	if (ec == CJigHelper::RET_POINT)
	{
		//吸附移动
		AcGePoint2d basePoint(jig.GetPosition().x, jig.GetPosition().y);
		AdsorbentShow(useJigIds, basePoint, sideLength);
	}
}

AcDbObjectIdArray CEquipmentroomTool::createArea(double areaSize, CString areaName, double& sideLength)
{
	sideLength = sqrt(areaSize);

	AcDbObjectIdArray useJigIds;

	AcGePoint2d squarePt1(0, 0);
	AcGePoint2d squarePt2(0, sideLength);
	AcGePoint2d squarePt3(sideLength, sideLength);
	AcGePoint2d squarePt4(sideLength, 0);
	AcDbPolyline *pPoly = new AcDbPolyline(4);
	double width = 0;//正方形线宽
	pPoly->addVertexAt(0, squarePt1, 0, width, width);
	pPoly->addVertexAt(1, squarePt2, 0, width, width);
	pPoly->addVertexAt(2, squarePt3, 0, width, width);
	pPoly->addVertexAt(3, squarePt4, 0, width, width);
	pPoly->setClosed(true);
	AcDbObjectId squareId;
	DBHelper::AppendToDatabase(squareId, pPoly);
	pPoly->close();
	//插入拓展数据
	DBHelper::AddXData(squareId, _T("设备房类型"), areaName);
	useJigIds.append(squareId);
	// 创建单行文字
	AcGePoint3d ptInsert(sideLength / 2, sideLength / 2, 0);
	CString  textStyleName = _T("设备房用字体");
	creatTextStyle(textStyleName);
	AcDbObjectId textStyleId = DBHelper::GetTextStyle(textStyleName);
	AcDbObjectId textId = CreateText(ptInsert, areaName, sideLength / 3,textStyleId);
	textMove(ptInsert, textId);
	useJigIds.append(textId);
	return useJigIds;
}

void CEquipmentroomTool::textMove(AcGePoint3d ptInsert, AcDbObjectId textId)
{
	AcDbEntity *pEnt = NULL;
	Acad::ErrorStatus es;
	es = acdbOpenObject(pEnt, textId, AcDb::kForWrite);
	if (es != eOk)
		return;

	AcDbExtents textExtents;
	DBHelper::GetEntityExtents(textExtents, pEnt);
	AcGePoint3d maxPoint = textExtents.maxPoint();
	AcGePoint3d minPoint = textExtents.minPoint();
	double textEnterX = (minPoint.x + maxPoint.x) / 2;
	double textEnterY = (minPoint.y + maxPoint.y) / 2;
	AcGePoint3d textEnter(textEnterX, textEnterY, 0);
	AcGeVector3d textMoveVec = ptInsert - textEnter;
	pEnt->transformBy(textMoveVec);
	pEnt->close();
}

ads_real CEquipmentroomTool::getTotalArea(CString totalName)
{
	CString str = _T("\n请输入") + totalName;
	ads_real totalArea = 0;
	if (acedGetReal(str, &totalArea) == RTNORM)
	{
		return totalArea;
	}
	else
	{
		return 0;
	}
}

AcDbObjectId CEquipmentroomTool::CreateText(const AcGePoint3d& ptInsert,
	CString text, double height, AcDbObjectId style, double rotation)
{
	AcDbText *pText = new AcDbText(ptInsert, text, style, height, rotation);
	AcDbObjectId id;
	DBHelper::AppendToDatabase(id,pText);
	pText->close();
	return id;
}

AcDbObjectId CEquipmentroomTool::CreateMText(const AcGePoint3d& ptInsert,
	CString text, double height, double width , AcDbObjectId style)
{
	AcDbMText *pMText = new AcDbMText();

	// 设置多行文字的特性
	pMText->setTextStyle(style);
	pMText->setContents(text);
	pMText->setLocation(ptInsert);
	pMText->setTextHeight(height);
	pMText->setWidth(width);
	pMText->setAttachment(AcDbMText::kBottomLeft);
	AcDbObjectId textId;
	DBHelper::AppendToDatabase(textId, pMText);
	pMText->close();
	return textId;
}

void CEquipmentroomTool::AdsorbentShow(AcDbObjectIdArray useJigIds, AcGePoint2d basePoint, double sideLength)
{
	//将所有直线装入
	std::vector<AcGeLineSeg2d*> needDeleteEnt;//装取需要析构掉的ge实体指针
	RT::RTreeEx<AcGeLineSeg2d*, double, 2> rTreeOfSelection;
	AcDbDatabase *pCurDb = NULL;
	pCurDb = acdbHostApplicationServices()->workingDatabase();
	AcDbBlockTable *pBlkTbl;
	if (pCurDb->getBlockTable(pBlkTbl, AcDb::kForRead) != Acad::eOk)
	{
		acutPrintf(_T("读取块表失败！"));
		delete pCurDb;
		return;
	}
	AcDbBlockTableRecord *pBlkTblRcd;
	if (pBlkTbl->getAt(ACDB_MODEL_SPACE, pBlkTblRcd, AcDb::kForRead) != Acad::eOk)
	{
		acutPrintf(_T("读取块表记录失败！"));
		pBlkTbl->close();
		return;
	}
	pBlkTbl->close();
	AcDbBlockTableRecordIterator *pItr = NULL;		                	            //块表记录遍历器
	if (pBlkTblRcd->newIterator(pItr) != Acad::eOk)
	{
		acutPrintf(_T("创建块表记录遍历器失败！"));
		pBlkTblRcd->close();
		return;
	}
	pBlkTblRcd->close();
	AcDbEntity *pEnt = NULL;//遍历的临时实体指针
	for (pItr->start(); !pItr->done(); pItr->step())
	{
		if (pItr->getEntity(pEnt, AcDb::kForRead) != Acad::eOk)
		{
			acutPrintf(_T("读取实体失败！"));
			continue;
		}
		if (pEnt->isKindOf(AcDbLine::desc()))
		{
			//如果实体所在图层关闭则不放入选择
			if (!isLayerClose(pEnt))
			{
				AcGePoint3d startPt, endPt;
				AcDbLine *pdbLine = AcDbLine::cast(pEnt);
				pdbLine->getStartPoint(startPt);
				pdbLine->getEndPoint(endPt);
				AcGePoint2d start2dPt(startPt.x, startPt.y);
				AcGePoint2d end2dPt(endPt.x, endPt.y);
				AcGeLineSeg2d* pgeLine = new AcGeLineSeg2d(start2dPt, end2dPt);
				AcDbExtents geLineExt;
				geLineExt.addPoint(GeHelper::PT23(start2dPt));
				geLineExt.addPoint(GeHelper::PT23(end2dPt));

				AcGePoint3d minPoint = geLineExt.minPoint();
				AcGePoint3d maxPoint = geLineExt.maxPoint();
				double positionMin[2];
				double positionMax[2];
				positionMin[0] = minPoint.x;
				positionMin[1] = minPoint.y;
				positionMax[0] = maxPoint.x;
				positionMax[1] = maxPoint.y;

				rTreeOfSelection.Insert(positionMin, positionMax, pgeLine);//装构造出的ge指针
				needDeleteEnt.push_back(pgeLine);
			}		
		}
		else if (pEnt->isKindOf(AcDbPolyline::desc()))
		{
			if (!isLayerClose(pEnt))
			{
				AcDbPolyline *pPline = AcDbPolyline::cast(pEnt);
				AcGeLineSeg2d line;
				int n = pPline->numVerts();
				for (int i = 0; i < n; i++)
				{
					if (pPline->segType(i) == AcDbPolyline::kLine)
					{
						pPline->getLineSegAt(i, line);
						AcGePoint2d startPoint;
						AcGePoint2d endPoint;
						startPoint = line.startPoint();
						endPoint = line.endPoint();

						AcGeLineSeg2d* pgePLine = new AcGeLineSeg2d(startPoint, endPoint);
						AcDbExtents gePLineExt;
						gePLineExt.addPoint(GeHelper::PT23(startPoint));
						gePLineExt.addPoint(GeHelper::PT23(endPoint));

						AcGePoint3d maxtempPoint = gePLineExt.maxPoint();
						AcGePoint3d mintempPoint = gePLineExt.minPoint();
						double positionTempMin[2];
						double positionTempMax[2];
						positionTempMin[0] = mintempPoint.x;
						positionTempMin[1] = mintempPoint.y;
						positionTempMax[0] = maxtempPoint.x;
						positionTempMax[1] = maxtempPoint.y;
						rTreeOfSelection.Insert(positionTempMin, positionTempMax, pgePLine);//装指针
						needDeleteEnt.push_back(pgePLine);
					}
				}
			}		
		}
		pEnt->close();
	}

	//装取完成，进行筛选

	//1.拿到目标设备房的四个点构造成矩形
	AcGePoint2dArray squarePts;

	squarePts.append(basePoint);
	AcGePoint2d squarePt1 = basePoint;
	AcGeVector2d vec1(1, 0);
	squarePt1.transformBy(vec1*sideLength);
	squarePts.append(squarePt1);
	AcGePoint2d squarePt2 = squarePt1;
	AcGeVector2d vec2(0, 1);
	squarePt2.transformBy(vec2*sideLength);
	squarePts.append(squarePt2);
	AcGePoint2d squarePt3 = squarePt2;
	AcGeVector2d vec3(-1, 0);
	squarePt3.transformBy(vec3*sideLength);
	squarePts.append(squarePt3);

	AcGeVector2dArray vecs;
	for (int i = 0; i < squarePts.length(); i++)
	{
		AcGeVector2d vec = squarePts[(i + 1) % squarePts.length()] - squarePts[i];
		vecs.append(vec);
	}
	std::vector<AcGePoint2dArray> smallRectangles;
	AcGeVector2dArray closeMoveVecs;
	for (int j = 0; j < vecs.length(); j++)
	{
		AcGePoint2dArray rectanglePts;
		rectanglePts.append(squarePts[j]);
		rectanglePts.append(squarePts[(j + 1) % vecs.length()]);
		AcGeVector2d stretchVec = vecs[j];
		stretchVec.rotateBy(4.7123889803846897);
		AcGeVector2d unitvec = stretchVec.normalize();
		closeMoveVecs.append(unitvec);

		AcGePoint2d segstarPoint = squarePts[j];
		segstarPoint.transformBy(500 * unitvec);
		rectanglePts.append(segstarPoint);
		AcGePoint2d segendPoint = squarePts[(j + 1) % vecs.length()];
		segendPoint.transformBy(500 * unitvec);
		rectanglePts.append(segendPoint);

		smallRectangles.push_back(rectanglePts);
	}
	//第一条边
	double pointMin[2];
	double pointMax[2];
	point2dToDouble(pointMin, pointMax, smallRectangles[0][2], smallRectangles[0][1]);
	std::set<AcGeLineSeg2d*> getEntHandles;
	rTreeOfSelection.Search(pointMin, pointMax, &getEntHandles);
	std::set<AcGeLineSeg2d*>::iterator setentIter;
	std::vector<AcGePoint2dArray> targetLines;
	for (setentIter = getEntHandles.begin(); setentIter != getEntHandles.end(); setentIter++)
	{
		AcGeLineSeg2d* ptempLine = *setentIter;
		//判断是否平行

		AcGePoint2d start2dPoint = ptempLine->startPoint();
		AcGePoint2d end2dPoint = ptempLine->endPoint();

		AcGeVector2d tempvecs = end2dPoint - start2dPoint;
		AcGeVector2d unitTempvecs = tempvecs.normalize();
		AcGeVector2d anotherVec = vecs[0].normalize();

		if (compareVec(unitTempvecs, anotherVec) || compareVec(unitTempvecs, -anotherVec))
		{
			//判断是否为被放置图形边
			AcGePoint2dArray compareLinePoints;
			compareLinePoints.append(start2dPoint);
			compareLinePoints.append(end2dPoint);
			if (isSquareSide(compareLinePoints, squarePts))
			{
				continue;
			}
			//判断其长度是否大于原边
			double lineLength = start2dPoint.distanceTo(end2dPoint);
			if ((lineLength - sideLength) > 0)
			{
				AcGePoint2dArray targetLinePoints;
				//装入容器进行距离原边比较操作
				targetLinePoints.append(start2dPoint);
				targetLinePoints.append(end2dPoint);
				targetLines.push_back(targetLinePoints);
			}
		}
	}
	//取满足以上两个条件中的距离原边最近的距离
	

	//第二条边
	double point1Min[2];
	double point1Max[2];
	point2dToDouble(point1Min, point1Max, smallRectangles[1][0], smallRectangles[1][3]);
	std::set<AcGeLineSeg2d*> getEnt1Handles;
	rTreeOfSelection.Search(point1Min, point1Max, &getEnt1Handles);
	//同样操作
	std::set<AcGeLineSeg2d*>::iterator setent1Iter;
	std::vector<AcGePoint2dArray> target1Lines;
	for (setent1Iter = getEnt1Handles.begin(); setent1Iter != getEnt1Handles.end(); setent1Iter++)
	{
		AcGeLineSeg2d* ptempLine = *setent1Iter;
		//判断是否平行

		AcGePoint2d start2dPoint = ptempLine->startPoint();
		AcGePoint2d end2dPoint = ptempLine->endPoint();

		AcGeVector2d tempvecs = end2dPoint - start2dPoint;
		AcGeVector2d unitTempvecs = tempvecs.normalize();
		AcGeVector2d anotherVec = vecs[1].normalize();
		//if (tempvecs==vecs[0]|| tempvecs == anotherVec)
		if (compareVec(unitTempvecs, anotherVec) || compareVec(unitTempvecs, -anotherVec))
		{
			//判断是否为被放置图形边
			AcGePoint2dArray compareLinePoints;
			compareLinePoints.append(start2dPoint);
			compareLinePoints.append(end2dPoint);
			if (isSquareSide(compareLinePoints, squarePts))
			{
				continue;
			}
			//判断其长度是否大于原边
			double lineLength = start2dPoint.distanceTo(end2dPoint);
			if ((lineLength - sideLength) > 0)
			{
				AcGePoint2dArray targetLinePoints;
				//装入容器进行距离原边比较操作
				targetLinePoints.append(start2dPoint);
				targetLinePoints.append(end2dPoint);
				target1Lines.push_back(targetLinePoints);
			}
		}
	}
	//取满足以上两个条件中的距离原边最近的距离
	

	//第三条边
	double point2Min[2];
	double point2Max[2];
	point2dToDouble(point2Min, point2Max, smallRectangles[2][1], smallRectangles[2][2]);
	std::set<AcGeLineSeg2d*> getEnt2Handles;
	rTreeOfSelection.Search(point2Min, point2Max, &getEnt2Handles);

	//同样操作
	std::set<AcGeLineSeg2d*>::iterator setent2Iter;
	std::vector<AcGePoint2dArray> target2Lines;
	for (setent2Iter = getEnt2Handles.begin(); setent2Iter != getEnt2Handles.end(); setent2Iter++)
	{
		AcGeLineSeg2d* ptempLine = *setent2Iter;
		//判断是否平行

		AcGePoint2d start2dPoint = ptempLine->startPoint();
		AcGePoint2d end2dPoint = ptempLine->endPoint();

		AcGeVector2d tempvecs = end2dPoint - start2dPoint;
		AcGeVector2d unitTempvecs = tempvecs.normalize();
		AcGeVector2d anotherVec = vecs[2].normalize();
		//if (tempvecs==vecs[0]|| tempvecs == anotherVec)
		if (compareVec(unitTempvecs, anotherVec) || compareVec(unitTempvecs, -anotherVec))
		{
			//判断是否为被放置图形边
			AcGePoint2dArray compareLinePoints;
			compareLinePoints.append(start2dPoint);
			compareLinePoints.append(end2dPoint);
			if (isSquareSide(compareLinePoints, squarePts))
			{
				continue;
			}
			//判断其长度是否大于原边
			double lineLength = start2dPoint.distanceTo(end2dPoint);
			if ((lineLength - sideLength) > 0)
			{
				AcGePoint2dArray targetLinePoints;
				//装入容器进行距离原边比较操作
				targetLinePoints.append(start2dPoint);
				targetLinePoints.append(end2dPoint);
				target2Lines.push_back(targetLinePoints);
			}
		}
	}
	//取满足以上两个条件中的距离原边最近的距离
	

	//第四条边
	double point3Min[2];
	double point3Max[2];
	point2dToDouble(point3Min, point3Max, smallRectangles[3][3], smallRectangles[3][0]);
	std::set<AcGeLineSeg2d*> getEnt3Handles;
	rTreeOfSelection.Search(point3Min, point3Max, &getEnt3Handles);

	//同样操作（加入多线段后改为函数调用）
	std::set<AcGeLineSeg2d*>::iterator setent3Iter;
	std::vector<AcGePoint2dArray> target3Lines;
	for (setent3Iter = getEnt3Handles.begin(); setent3Iter != getEnt3Handles.end(); setent3Iter++)
	{
		AcGeLineSeg2d* ptempLine = *setent3Iter;
		//判断是否平行

		AcGePoint2d start2dPoint = ptempLine->startPoint();
		AcGePoint2d end2dPoint = ptempLine->endPoint();

		AcGeVector2d tempvecs = end2dPoint - start2dPoint;
		AcGeVector2d unitTempvecs = tempvecs.normalize();
		AcGeVector2d anotherVec = vecs[3].normalize();
		//if (tempvecs==vecs[0]|| tempvecs == anotherVec)
		if (compareVec(unitTempvecs, anotherVec) || compareVec(unitTempvecs, -anotherVec))
		{
			//判断是否为被放置图形边
			AcGePoint2dArray compareLinePoints;
			compareLinePoints.append(start2dPoint);
			compareLinePoints.append(end2dPoint);
			if (isSquareSide(compareLinePoints, squarePts))
			{
				continue;
			}
			//判断其长度是否大于原边
			double lineLength = start2dPoint.distanceTo(end2dPoint);
			if ((lineLength - sideLength) > 0)
			{
				AcGePoint2dArray targetLinePoints;
				//装入容器进行距离原边比较操作
				targetLinePoints.append(start2dPoint);
				targetLinePoints.append(end2dPoint);
				target3Lines.push_back(targetLinePoints);
			}
		}
	}
	//取满足以上两个条件中的距离原边最近的距离
	

	//判断第1、3边和2、4边移动问题

	//1
	double moveDistance = getMinDistance(basePoint, targetLines);
	
	//2
	double move1Distance = getMinDistance(squarePts[1], target1Lines);
	
	//3
	double move2Distance = getMinDistance(squarePts[2], target2Lines);
	
	//4
	double move3Distance = getMinDistance(squarePts[3], target3Lines);
	if (moveDistance<= move2Distance || move2Distance == 0)
	{
		moveTest(useJigIds, closeMoveVecs[0], moveDistance);
	}
	else
	{
		moveTest(useJigIds, closeMoveVecs[2], move2Distance);
	}
	if (move1Distance<= move3Distance || move3Distance==0)
	{
		moveTest(useJigIds, closeMoveVecs[1], move1Distance);
	}
	else
	{
		moveTest(useJigIds, closeMoveVecs[3], move3Distance);
	}
	//2.对目标设备房各边进行面域放大 （完成）
	//3.去域值内的目标直线，判断找到符合吸附条件的那条(完成)
	//4.吸附上去(完成)
	//5.增加多线段上直线的吸附
	for (int m = 0; m < needDeleteEnt.size(); m++)
	{
		delete needDeleteEnt[m];
	}
}
void CEquipmentroomTool::point2dToDouble(double minPt[2], double maxPt[2], AcGePoint2d& MinPoint, AcGePoint2d& MaxPoint)
{
	minPt[0] = MinPoint.x;
	minPt[1] = MinPoint.y;
	maxPt[0] = MaxPoint.x;
	maxPt[1] = MaxPoint.y;
}
AcGePoint2d CEquipmentroomTool::point3dTo2d(AcGePoint3d targetPt)
{
	return AcGePoint2d(targetPt.x, targetPt.y);
}

double CEquipmentroomTool::getMinDistance(AcGePoint2d squarePoint, std::vector<AcGePoint2dArray>& targetLines)
{
	std::vector<double> distanceVector;
	for (int i = 0; i < targetLines.size(); i++)
	{
		AcGeLine2d targetPLine(targetLines[i][0], targetLines[i][1]);
		AcGePointOnCurve2d RectPtOnCurve;
		targetPLine.getClosestPointTo(squarePoint, RectPtOnCurve);
		AcGePoint2d RectClosedPt = RectPtOnCurve.point2d();
		double distance = squarePoint.distanceTo(RectClosedPt);
		distanceVector.push_back(distance);
	}

	if (distanceVector.size() == 0)
	{
		return 0;
	}
	else
	{
		std::vector<double>::iterator smallest = std::min_element(distanceVector.begin(), distanceVector.end());
		double result = *smallest;
		return result;
	}
}

bool CEquipmentroomTool::compareVec(AcGeVector2d vec1, AcGeVector2d vec2)
{
	if (Equal(vec1.x, vec2.x) && Equal(vec1.y, vec2.y))
	{
		return true;
	}
	return false;
}

void CEquipmentroomTool::moveTest(AcDbObjectIdArray useJigIds, AcGeVector2d moveVec, double moveDistance)
{
	std::vector<AcDbEntity*> vctJigEnt;
	for (int i = 0; i < useJigIds.length(); i++)
	{
		AcDbEntity *pEnt = NULL;
		acdbOpenObject(pEnt, useJigIds[i], AcDb::kForWrite);
		//判断自定义实体的类型
		if (pEnt == NULL)
			continue;
		pEnt->setVisibility(AcDb::kInvisible);
		vctJigEnt.push_back(pEnt);
	}
	if (vctJigEnt.size() < 1)
		return;

	AcGeVector3d vec(moveVec.x, moveVec.y, 0);
	for (int i = 0; i < vctJigEnt.size(); ++i)
	{
		vctJigEnt[i]->transformBy(vec*moveDistance);
	}

	//关闭图形实体
	for (int i = 0; i < vctJigEnt.size(); ++i)
	{
		vctJigEnt[i]->setVisibility(AcDb::kVisible);
		vctJigEnt[i]->close();
	}
}

bool CEquipmentroomTool::isSquareSide(AcGePoint2dArray targetLinePoints, AcGePoint2dArray squarePoints)
{
	std::vector<AcGePoint2dArray> squareSides;
	for (int i = 0; i < squarePoints.length(); i++)
	{
		AcGePoint2dArray sidePoints;
		sidePoints.append(squarePoints[i]);
		sidePoints.append(squarePoints[(i + 1) % squarePoints.length()]);
		squareSides.push_back(sidePoints);
	}

	int flag = 0;
	AcGePoint2d look1 = targetLinePoints[0];
	AcGePoint2d look2 = targetLinePoints[1];
	for (int j = 0; j < squareSides.size(); j++)
	{
		AcGePoint2d look3 = squareSides[j][0];
		AcGePoint2d look4 = squareSides[j][1];
		if (comparePoint(targetLinePoints[0], squareSides[j][0]) && comparePoint(targetLinePoints[1], squareSides[j][1]))
		{
			flag = 1;
		}
		else if (comparePoint(targetLinePoints[0], squareSides[j][1]) && comparePoint(targetLinePoints[1], squareSides[j][0]))
		{
			flag = 1;
		}
	}
	if (flag == 1)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool CEquipmentroomTool::comparePoint(AcGePoint2d a, AcGePoint2d b)
{
	if (Equal(a.x, b.x) && Equal(a.y, b.y))
	{
		return true;
	}
	return false;
}

bool CEquipmentroomTool::layerSet()
{
	// 判断是否存在名称为“设备房”的图层
	AcDbLayerTable *pLayerTbl;
	//获取当前图形层表
	acdbHostApplicationServices()->workingDatabase()->getLayerTable(pLayerTbl, AcDb::kForWrite);
	if (pLayerTbl->has(_T("设备房")))
	{
		pLayerTbl->close();
		return true;
	}
	//acutPrintf(_T("\n当前图形中未包含\"设备房\"图层!"));
	AcDbLayerTableRecord *pLayerTblRcd = new AcDbLayerTableRecord();
	pLayerTblRcd->setName(_T("设备房"));
	AcDbObjectId layerTblRcdId;
	pLayerTbl->add(layerTblRcdId, pLayerTblRcd);

	AcCmColor color;//设置图层颜色
	color.setColorIndex(4);
	pLayerTblRcd->setColor(color);
	// 将新建的层表记录添加到层表中
	pLayerTblRcd->close();
	pLayerTbl->close();
	Acad::ErrorStatus es;
	es = acdbHostApplicationServices()->workingDatabase()->setClayer(layerTblRcdId);//设为当前图层
	if (es != eOk)
	{
		return false;
	}
	return true;
}

bool CEquipmentroomTool::layerSet(const CString& layerName, const int& layerColor)
{
	// 判断是否存在名称为“设备房”的图层
	AcDbLayerTable *pLayerTbl;
	//获取当前图形层表
	Acad::ErrorStatus es;
	es = acdbCurDwg()->getLayerTable(pLayerTbl, AcDb::kForWrite);
	if (es != eOk)
	{
		return false;
	}
	if (pLayerTbl->has(layerName))//判断已经有了该图层，应置为当前图层
	{
		AcDbObjectId layerId;
		if (pLayerTbl->getAt(layerName, layerId) != Acad::eOk)
		{
			pLayerTbl->close();
			return false;
		}
		es = acdbCurDwg()->setClayer(layerId);//设为当前图层
		pLayerTbl->close();
		if (es != eOk)
		{
			return false;
		}
		return true;
	}

	AcDbLayerTableRecord *pLayerTblRcd = new AcDbLayerTableRecord();
	pLayerTblRcd->setName(layerName);
	AcDbObjectId layerTblRcdId;
	pLayerTbl->add(layerTblRcdId, pLayerTblRcd);

	AcCmColor color;//设置图层颜色
	color.setColorIndex(layerColor);
	pLayerTblRcd->setColor(color);
	// 将新建的层表记录添加到层表中
	pLayerTblRcd->close();
	pLayerTbl->close();
	es = acdbCurDwg()->setClayer(layerTblRcdId);//设为当前图层
	if (es != eOk)
	{
		return false;
	}
	return true;
}

void CEquipmentroomTool::setEntToLayer(AcDbObjectIdArray objectIds)
{
	for (int i=0;i<objectIds.length();i++)
	{
		AcDbEntity *pEnty = NULL;
		AcDbObjectId idEnty = objectIds[i]; //传如打开的实体ID，事先必须先获取到。否则会打开失败
		Acad::ErrorStatus es;
		es = acdbOpenObject(pEnty, idEnty, AcDb::kForWrite);
		if (es == Acad::eOk)
		{
			pEnty->setLayer(_T("设备房")); //设置实体所在的图层
			pEnty->close();
		}
		//打开失败不需要关闭实体
	}	
}

bool CEquipmentroomTool::isLayerClose(AcDbEntity *pEnt)
{
	CString strLayerName = pEnt->layer();
	////获得当前的图层列表  
	AcDbLayerTable* pLayerTbl;
	if (acdbCurDwg()->getLayerTable(pLayerTbl, AcDb::kForRead)!=eOk)
	{
		return false;
	}
	////获得制定层表记录的指针  
	AcDbLayerTableRecord* pLayerTblRcd;
	if (pLayerTbl->getAt(strLayerName, pLayerTblRcd, AcDb::kForWrite)!=eOk)
	{
		pLayerTbl->close();
		return false;
	}
	bool result = pLayerTblRcd->isOff();
	pLayerTblRcd->close();
	pLayerTbl->close();
	return result;
}

AcDbObjectId CEquipmentroomTool::CreateHatch( const CString& patName, const AcGePoint2dArray& allPlinePts,const AcGeDoubleArray& bulges)
{
	Acad::ErrorStatus es;
	AcDbHatch *pHatch = new AcDbHatch();

	// 设置填充平面
	AcGeVector3d normal(0, 0, 1);
	es = pHatch->setNormal(normal);
	es = pHatch->setElevation(0);

	// 设置关联性
	es = pHatch->setAssociative(Adesk::kFalse);

	// 设置填充图案
	es = pHatch->setPattern(AcDbHatch::kPreDefined, patName);

	// 添加填充边界
	es = pHatch->appendLoop(AcDbHatch::kExternal,allPlinePts,bulges);

	// 显示填充对象
	es = pHatch->evaluateHatch();

	// 添加到模型空间
	AcDbObjectId hatchId;
	DBHelper::AppendToDatabase(hatchId, pHatch);
	pHatch->close();
	return hatchId;


	/*AcDbHatch* pHatch = new AcDbHatch();
	AcGeVector3d normal(0.0, 0.0, 1.0);
	pHatch->setNormal(normal);
	pHatch->setElevation(0.0);
	if (Acad::eOk != pHatch->setAssociative(Adesk::kFalse) ||
		Acad::eOk != pHatch->setPattern(AcDbHatch::kPreDefined, _T("SOLID")) ||
		Acad::eOk != pHatch->setHatchStyle(AcDbHatch::kNormal) ||
		Acad::eOk != pHatch->appendLoop(AcDbHatch::kExternal, objIds) ||
		Acad::eOk != pHatch->setColorIndex(3) ||
		Acad::eOk != pHatch->evaluateHatch()
		)
	{
		ads_printf(_T("\n区域封闭检测失败,有未闭合点或区域没有经过Trim处理!"));
		pHatch->close();
		pHatch = NULL;
	}
	AcDbObjectId pHatchobjId;
	DBHelper::AppendToDatabase(pHatchobjId, pHatch);
	return pHatchobjId;*/
}

void CEquipmentroomTool::creatTextStyle(CString& textStyleName)
{
	TEXT_STYLE ts;
	ts.FileName = _T("gbenor.shx");
	ts.BigFontFileName = _T("gbcbig.shx");
	ts.XScale = 1.0;
	DBHelper::SetTextStyle(textStyleName, ts, false);
}
