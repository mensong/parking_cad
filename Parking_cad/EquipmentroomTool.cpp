#include "stdafx.h"
#include "EquipmentroomTool.h"
#include<cmath>
#include "JigHelper.h"
#include "RTreeEx.h"
#include "DBHelper.h"
#include <algorithm>
#include "GeHelper.h"
#include <fstream>
#include <json/json.h>
#include "FileHelper.h"
#include "Convertor.h"
#include<ctime>

CEquipmentroomTool::CEquipmentroomTool()
{
}


CEquipmentroomTool::~CEquipmentroomTool()
{
}

bool CEquipmentroomTool::jigShow(AcDbObjectIdArray useJigIds, double sideLength)
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
		return false;
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
	bool flag = false;
	if (ec != CJigHelper::RET_POINT)
	{
		//无效输入则撤销
		//AcGeVector3d vec = jig.GetBasePoint() - jig.GetPosition();
		for (int i = 0; i < vctJigEnt.size(); ++i)
		{
			//vctJigEnt[i]->transformBy(vec);
			vctJigEnt[i]->erase();
		}
		flag = true;
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
		//AdsorbentShow(useJigIds, basePoint, sideLength);
		return true;
	}
	if (flag)
	{
		return false;
	}
	else
	{
		return true;
	}
}

AcDbObjectIdArray CEquipmentroomTool::createArea(double areaSize, CString areaName, double& sideLength, double& limitLength, bool limitType)
{
	sideLength = sqrt(areaSize);
	if (limitType)
	{
		if (sideLength <= limitLength)
		{
			sideLength = areaSize / limitLength;
			if (sideLength <= limitLength)
			{
				sideLength = limitLength;
			}
		}
		else
		{
			limitLength = sideLength;
		}
	}
	else
	{
		if (sideLength >= limitLength)
		{
			sideLength = areaSize / limitLength;
			if (sideLength >= limitLength)
			{
				sideLength = limitLength;
			}
		}
		else
		{
			limitLength = sideLength;
		}
	}
	
	AcDbObjectIdArray useJigIds;

	AcGePoint2d squarePt1(0, 0);
	AcGePoint2d squarePt2(0, sideLength);
	AcGePoint2d squarePt3(limitLength, sideLength);
	AcGePoint2d squarePt4(limitLength, 0);
	AcDbPolyline *pPoly = new AcDbPolyline(4);
	double width = 0;//正方形线宽/满足条件变形成矩形
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

bool CEquipmentroomTool::getTotalArea(CString totalName, ads_real& totalArea)
{
	CString str = _T("\n请输入") + totalName;
	if (acedGetReal(str, &totalArea) == RTNORM)
	{
		return true;
	}
	else
	{
		totalArea = 0;
		return false;
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
		stretchVec.rotateBy((ARX_PI2/4)*3);
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

		if (unitTempvecs == anotherVec || unitTempvecs == anotherVec.negate())
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
		if (unitTempvecs==anotherVec || unitTempvecs== anotherVec.negate())
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
		if (unitTempvecs==anotherVec || unitTempvecs==anotherVec.negate())
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
		if (unitTempvecs==anotherVec || unitTempvecs==anotherVec.negate())
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
		/*-----------测试用-----------------*/
		AcGePoint2d a = targetLines[i][0];
		AcGePoint2d b = targetLines[i][1];
		/*----------------------------*/
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
	CString sEquipmentroomLayer(getLayerName("equipmentroom").c_str());
	if (pLayerTbl->has(sEquipmentroomLayer))
	{
		pLayerTbl->close();
		return true;
	}
	//acutPrintf(_T("\n当前图形中未包含\"设备房\"图层!"));
	//AcDbLayerTableRecord *pLayerTblRcd = new AcDbLayerTableRecord();
	//pLayerTblRcd->setName(sEquipmentroomLayer);
	//AcDbObjectId layerTblRcdId;
	//pLayerTbl->add(layerTblRcdId, pLayerTblRcd);

	//AcCmColor color;//设置图层颜色
	//color.setColorIndex(4);
	//pLayerTblRcd->setColor(color);
	//// 将新建的层表记录添加到层表中
	//pLayerTblRcd->close();
	//Acad::ErrorStatus es;
	//es = acdbHostApplicationServices()->workingDatabase()->setClayer(layerTblRcdId);//设为当前图层
	pLayerTbl->close();
	CEquipmentroomTool::creatLayerByjson("equipmentroom");
	return true;
}

bool CEquipmentroomTool::layerSet(const CString& layerName, const int& layerColor, AcDbDatabase *pDb /*= acdbCurDwg()*/)
{
	//----------------------------------------------------
	//判断有就退出无就生成图层生，并设置颜色
	AcDbLayerTable *pLayerTbl;
	//获取当前图形层表
	Acad::ErrorStatus es;
	es = pDb->getLayerTable(pLayerTbl, AcDb::kForWrite);
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
		es = pDb->setClayer(layerId);//设为当前图层
		AcDbLayerTableRecord *pLayerTblRcd;
		pLayerTbl->getAt(layerName, pLayerTblRcd, AcDb::kForWrite);
		AcCmColor color;//设置图层颜色
		color.setColorIndex(layerColor);
		pLayerTblRcd->setColor(color);
		pLayerTblRcd->close();
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
	es = pDb->setClayer(layerTblRcdId);//设为当前图层
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
			CString sEquipmentroomLayer(getLayerName("equipmentroom").c_str());
			pEnty->setLayer(sEquipmentroomLayer); //设置实体所在的图层
			pEnty->close();
		}
		//打开失败不需要关闭实体
	}	
}

void CEquipmentroomTool::setEntToLayer(const AcDbObjectId& entId, const CString& strLayerName)
{
	AcDbEntity *pEnty = NULL;
	AcDbObjectId idEnty = entId; //传如打开的实体ID，事先必须先获取到。否则会打开失败
	Acad::ErrorStatus es;
	es = acdbOpenObject(pEnty, idEnty, AcDb::kForWrite);
	if (es == Acad::eOk)
	{
		es = pEnty->setLayer(strLayerName); //设置实体所在的图层
		pEnty->close();
	}
	//打开失败不需要关闭实体
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

std::string CEquipmentroomTool::getLayerName(const std::string& strLayer)
{
	//从文件中读取
	std::string sConfigFile = DBHelper::GetArxDirA() + "ParkingConfig.json";
	std::string sConfigStr = FileHelper::ReadText(sConfigFile.c_str());
	Json::Reader reader;
	Json::Value root;
	std::string strLayerName = "";
	if (reader.parse(sConfigStr, root))
	{
		if (root["layer_config"][strLayer].isNull())
		{
			CString strErrorMessage(strLayer.c_str());
			acutPrintf(_T("配置文件不存在[\"layer_config\"][\"%s]字段！"), strErrorMessage.GetString());
			return strLayerName;
		}
		if (root["layer_config"][strLayer]["layer_name"].isString())
		{
			strLayerName = root["layer_config"][strLayer]["layer_name"].asString();
		}
		else
		{
			acedAlert(_T("配置文件字段格式不匹配！"));
			return strLayerName;
		}
	}
	else
	{
		acedAlert(_T("加载配置文件出错！"));
		return strLayerName;
	}
	return strLayerName;
}

std::string CEquipmentroomTool::getJsonInformation(const std::string& inputroot, const std::string& object, const std::string& key)
{
	//从文件中读取
	std::string sConfigFile = DBHelper::GetArxDirA() + "ParkingConfig.json";
	std::string sConfigStr = FileHelper::ReadText(sConfigFile.c_str());
	Json::Reader reader;
	Json::Value root;
	std::string keyvalue = "";
	if (reader.parse(sConfigStr, root))
	{
		if (root[inputroot][object].isNull())
		{
			if (!root[inputroot][key].isNull())
			{
				if (root[inputroot][key].isString())
				{
					keyvalue = root[inputroot][key].asString();
					return keyvalue;
				}
				else
				{
					acedAlert(_T("配置文件字段格式不匹配！"));
					return keyvalue;
				}
			}

			CString strErrorMessage(object.c_str());
			acutPrintf(_T("配置文件不存在[\"layer_config\"][\"%s]字段！"), strErrorMessage.GetString());
			return keyvalue;
		}
		if (root[inputroot][object][key].isString())
		{
			keyvalue = root[inputroot][object][key].asString();
		}
		else
		{
			acedAlert(_T("配置文件字段格式不匹配！"));
			return keyvalue;
		}
	}
	else
	{
		acedAlert(_T("加载配置文件出错！"));
		return keyvalue;
	}
	return keyvalue;
}

void CEquipmentroomTool::getJsonInformation(const std::string& inputroot, const std::string& object, const std::string& key, std::vector<std::string>& arrayvector)
{
	//从文件中读取
	std::string sConfigFile = DBHelper::GetArxDirA() + "ParkingConfig.json";
	std::string sConfigStr = FileHelper::ReadText(sConfigFile.c_str());
	Json::Reader reader;
	Json::Value root;

	if (reader.parse(sConfigStr, root))
	{
		if (root[inputroot][object].isNull())
		{
			if (!root[inputroot][key].isNull())
			{
				if (root[inputroot][key].isArray())
				{
					Json::Value& keyvalue = root[inputroot][key];
					for (int i = 0; i < keyvalue.size(); i++)
					{
						std::string str = keyvalue[i].asString();
						arrayvector.push_back(str);
					}
					return;
				}
				else
				{
					acedAlert(_T("配置文件字段格式不匹配！"));
					return ;
				}
			}

			CString strErrorMessage(object.c_str());
			acutPrintf(_T("配置文件不存在[\"layer_config\"][\"%s]字段！"), strErrorMessage.GetString());
			return ;
		}
		if (root[inputroot][object][key].isArray())
		{
			Json::Value& keyvalue = root[inputroot][key];
			for (int i = 0; i < keyvalue.size(); i++)
			{
				std::string str = keyvalue[i].asString();
				arrayvector.push_back(str);
			}
		}
		else
		{
			acedAlert(_T("配置文件字段格式不匹配！"));
			return ;
		}
	}
	else
	{
		acedAlert(_T("加载配置文件出错！"));
		return ;
	}	

}

bool CEquipmentroomTool::deletLayerByName(const CString& layerName)
{
	AcDbLayerTable *pLayerTbl;
	//获取当前图形层表
	Acad::ErrorStatus es;
	es = acdbCurDwg()->getLayerTable(pLayerTbl, AcDb::kForWrite);
	if (es != eOk)
	{
		return false;
	}
	if (pLayerTbl->has(layerName))
	{
		AcDbLayerTableRecord *pLTR;
		pLayerTbl->getAt(layerName, pLTR, AcDb::kForWrite);
		if (CEquipmentroomTool::deletLayer(pLTR)!= eOk)
		{
			pLTR->close();
			pLayerTbl->close();
			return false;
		}
		pLTR->close();
	}
	pLayerTbl->close();
	return true;
}

Acad::ErrorStatus CEquipmentroomTool::deletLayer(AcDbLayerTableRecord* pLTR, AcDbLayerTable* pLT/* = NULL*/)
{
	bool bUp = false;
	if (!pLTR->isWriteEnabled())
	{
		pLTR->upgradeOpen();
		bUp = true;
	}

	Acad::ErrorStatus es = Acad::eOk;

	do
	{
		AcDbObjectId idCurLayer = acdbCurDwg()->clayer();

		if (idCurLayer != pLTR->objectId())
		{//如果不是当前图层
			pLTR->erase();
		}
		else
		{//如果是当前图层
			AcDbLayerTable* pLT1 = NULL;

			bool bUpLT = false;
			if (!pLT)
			{
				es = acdbCurDwg()->getLayerTable(pLT1, AcDb::kForWrite);
				if (Acad::eOk != es)
					break;
			}
			else
			{
				pLT1 = pLT;
				if (!pLT1->isWriteEnabled())
				{
					pLT1->upgradeOpen();
					bUpLT = true;
				}
			}

			AcDbLayerTableRecord* pTmp = new AcDbLayerTableRecord();
			pTmp->setName(_T("__0__temp__"));

			AcDbObjectId idTempLayer;
			es = pLT1->add(idTempLayer, pTmp);

			if (!pLT)
				pLT1->close();
			else if (bUpLT)
				pLT1->downgradeOpen();

			if (Acad::eOk != es)
			{
				delete pTmp;
				break;
			}

			es = acdbCurDwg()->setClayer(idTempLayer);
			pLTR->erase();
			acdbCurDwg()->setClayer(idCurLayer);

			pTmp->erase();
			pTmp->close();
		}
	} while (0);

	if (bUp)
		pLTR->downgradeOpen();

	return es;
}

void CEquipmentroomTool::setLayerClose(const CString& layerName, AcDbDatabase *pDb /*= acdbCurDwg()*/)
{
	AcDbLayerTable *pLayerTbl;
	//获取当前图形层表
	Acad::ErrorStatus es;
	es = pDb->getLayerTable(pLayerTbl, AcDb::kForWrite);
	if (es != eOk)
	{
		return;
	}
	if (pLayerTbl->has(layerName))
	{
		AcDbLayerTableRecord *pLTR = NULL;
		es = pLayerTbl->getAt(layerName, pLTR, AcDb::kForWrite);
		if (es != eOk)
		{
			pLayerTbl->close();
			return;
		}
		DBHelper::SetLayerIsOff(pLTR,pLayerTbl);
		pLTR->close();
	}
	pLayerTbl->close();
}

bool CEquipmentroomTool::isLayerClosed(const CString& strLayerName)
{
	AcDbLayerTable *pLayerTbl;
	bool result = false;
	//获取当前图形层表
	Acad::ErrorStatus es;
	es = acdbCurDwg()->getLayerTable(pLayerTbl, AcDb::kForWrite);
	if (es != eOk)
	{
		return result;
	}
	if (pLayerTbl->has(strLayerName))
	{
		AcDbLayerTableRecord *pLTR = NULL;
		es = pLayerTbl->getAt(strLayerName, pLTR, AcDb::kForRead);
		if (es != eOk)
		{
			pLayerTbl->close();
			return result;
		}
		result = pLTR->isOff();
		pLTR->close();
	}
	pLayerTbl->close();
	return result;
}

void CEquipmentroomTool::setLayerOpen(const CString& strLayerName)
{
	AcDbLayerTable *pLayerTbl;
	//获取当前图形层表
	Acad::ErrorStatus es;
	es = acdbCurDwg()->getLayerTable(pLayerTbl, AcDb::kForWrite);
	if (es != eOk)
	{
		return;
	}
	if (pLayerTbl->has(strLayerName))
	{
		AcDbLayerTableRecord *pLTR = NULL;
		es = pLayerTbl->getAt(strLayerName, pLTR, AcDb::kForWrite);
		if (es != eOk)
		{
			pLayerTbl->close();
			return;
		}
		es = pLTR->upgradeOpen();
		pLTR->setIsOff(false);
		es = pLTR->downgradeOpen();
		pLTR->close();
	}
	pLayerTbl->close();
}

bool CEquipmentroomTool::getLayerConfigForJson(const std::string& sLayerInfo, std::string& sProfessionalAttributes, 
	std::string& sLayerName, std::string& sLayerColor, std::string& sLayerLinetype, std::string& sLayerWidth, std::string& sIsPrintf, std::string& sTransparency)
{
	//从文件中读取
	std::string sConfigFile = DBHelper::GetArxDirA() + "ParkingConfig.json";
	std::string sConfigStr = FileHelper::ReadText(sConfigFile.c_str());
	Json::Reader reader;
	Json::Value root;
	if (reader.parse(sConfigStr, root))
	{
		if (root["layer_config"][sLayerInfo].isNull())
		{
			CString strErrorMessage(sLayerInfo.c_str());
			//char* ErrorMessage = UnicodeToANSI(strErrorMessage);
			acutPrintf(_T("配置文件不存在[\"layer_config\"][\"%s]字段！"), strErrorMessage.GetString());
			return false;
		}
		if (root["layer_config"][sLayerInfo]["professional_attributes"].isString())
		{
			sProfessionalAttributes = root["layer_config"][sLayerInfo]["professional_attributes"].asString();
			sLayerName = root["layer_config"][sLayerInfo]["layer_name"].asString();
			sLayerColor = root["layer_config"][sLayerInfo]["layer_color"].asString();
			sLayerLinetype = root["layer_config"][sLayerInfo]["layer_linetype"].asString();
			sLayerWidth = root["layer_config"][sLayerInfo]["layer_width"].asString();
			sIsPrintf = root["layer_config"][sLayerInfo]["isprintf"].asString();
			sTransparency = root["layer_config"][sLayerInfo]["transparency"].asString();
			return true;
		}
		else
		{
			acedAlert(_T("配置文件字段格式不匹配！"));
			return false;
		}
	}
	else
	{
		acedAlert(_T("加载配置文件出错！"));
		return false;
	}	
}

bool CEquipmentroomTool::layerConfigSet(const CString& layerName, const CString& layerColor, const CString& lineWidth, const CString& lineType, const CString& transparency, const CString& isPrint, AcDbDatabase *pDb /*= acdbCurDwg()*/ )
{
	Doc_Locker _locker;
	//----------------------------------------------------
	//判断有就退出无就生成图层生，并设置颜色
	AcDbLayerTable *pLayerTbl;
	//获取当前图形层表
	Acad::ErrorStatus es;
	es = pDb->getLayerTable(pLayerTbl, AcDb::kForWrite);
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
		AcDbLayerTableRecord *pLayerTblRcd;
		pLayerTbl->getAt(layerName, pLayerTblRcd, AcDb::kForWrite);
		AcCmColor color;//设置图层颜色
		int iLayerColor = _ttoi(layerColor);
		color.setColorIndex(iLayerColor);
		pLayerTblRcd->setColor(color);
		pLayerTblRcd->close();
		pLayerTbl->close();
		es = pDb->setClayer(layerId);//设为当前图层
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
	int iLayerColor = _ttoi(layerColor);
	es = color.setColorIndex(iLayerColor);
	if (es != eOk)
	{
		acutPrintf(_T("\n图层颜色设置失败！"));
		return false;
	}
	pLayerTblRcd->setColor(color);
	if (lineWidth==_T("0.09"))
	{
		pLayerTblRcd->setLineWeight(AcDb::kLnWt009); // 设置线宽为0.09
	}
	else if(lineWidth == _T("0.13"))
	{
		pLayerTblRcd->setLineWeight(AcDb::kLnWt013); // 设置线宽为0.13
	}
	else if (lineWidth == _T("0.15"))
	{
		pLayerTblRcd->setLineWeight(AcDb::kLnWt015); // 设置线宽为0.15
	}
	else if (lineWidth == _T("0.2"))
	{
		pLayerTblRcd->setLineWeight(AcDb::kLnWt020); // 设置线宽为0.2
	}
	else if (lineWidth == _T("0.25"))
	{
		pLayerTblRcd->setLineWeight(AcDb::kLnWt025); // 设置线宽为0.25
	}
	else if (lineWidth == _T("0.35"))
	{
		pLayerTblRcd->setLineWeight(AcDb::kLnWt035); // 设置线宽为0.35
	}
	else if (lineWidth == _T("0.4"))
	{
		pLayerTblRcd->setLineWeight(AcDb::kLnWt040); // 设置线宽为0.4
	}
	else
	{
		pLayerTblRcd->setLineWeight(AcDb::kLnWtByLwDefault); // 设置线宽为默认
	}
	AcDbLinetypeTable *pLineTbl;
	es = pDb->getLinetypeTable(pLineTbl, AcDb::kForRead);
	if (es != eOk)
	{
		pLineTbl->close();
		pLayerTblRcd->close();
		return false;
	}
	AcDbObjectId dashId; //dash线形ID，你自己去得到，在线形表中查询
	if (pLineTbl->getAt(lineType, dashId)!=eOk)
	{
		acutPrintf(_T("\n获取线型id失败！"));
		pLineTbl->close();
		pLayerTblRcd->close();
		pLayerTbl->close();
		return false;
	}
	pLayerTblRcd->setLinetypeObjectId(dashId); // 设置为dash线形
	int iTransparency = _ttoi(transparency);
	double dTransparency = (100 - iTransparency)*0.01;
	AcCmTransparency trans = AcCmTransparency(dTransparency);
	pLayerTblRcd->setTransparency(trans);
	if (isPrint == _T("否")|| isPrint == _T(""))
	{
		pLayerTblRcd->setIsPlottable(false);
	}
	else
	{
		pLayerTblRcd->setIsPlottable(true);
	}
	// 将新建的层表记录添加到层表中
	pLineTbl->close();
	pLayerTblRcd->close();
	pLayerTbl->close();
	es = pDb->setClayer(layerTblRcdId);//设为当前图层
	if (es != eOk)
	{
		return false;
	}
	return true;
}

std::string CEquipmentroomTool::getLayerNameByJson(const std::string& sLayerInfo)
{
	//从文件中读取
	std::string sConfigFile = DBHelper::GetArxDirA() + "ParkingConfig.json";
	std::string sConfigStr = FileHelper::ReadText(sConfigFile.c_str());
	Json::Reader reader;
	Json::Value root;
	std::string strLayerName = "";
	if (reader.parse(sConfigStr, root))
	{
		if (root["layer_config"][sLayerInfo].isNull())
		{
			CString strErrorMessage(sLayerInfo.c_str());
			//char* ErrorMessage = UnicodeToANSI(strErrorMessage);
			acutPrintf(_T("配置文件不存在[\"layer_config\"][\"%s]字段！"), strErrorMessage.GetString());
			return strLayerName;
		}
		if (root["layer_config"][sLayerInfo]["layer_name"].isString())
		{
			strLayerName = root["layer_config"][sLayerInfo]["layer_name"].asString();
			return strLayerName;
		}
		else
		{
			acedAlert(_T("配置文件字段格式不匹配！"));
			return strLayerName;
		}
	}
	else
	{
		acedAlert(_T("加载配置文件出错！"));
		return strLayerName;
	}
}

int CEquipmentroomTool::SelColor(int& textColor)
{
	////先获得当前层的ID
	AcDbObjectId layerId = acdbHostApplicationServices()->workingDatabase()->clayer();
	////然后获得当前层指针
	AcDbLayerTableRecordPointer ptLayer(layerId, AcDb::OpenMode::kForRead);
	////获得当前层的颜色
	//AcCmColor oldColor = ptLayer->color();
	//int nCurColor = oldColor.colorIndex();//当前层的颜色
	//int nNewColor = oldColor.colorIndex();//用户选择的颜色

	if (acedSetColorDialog(textColor, Adesk::kFalse, textColor))
	{
		return textColor;
	}
	else
	{
		return textColor;
	}
}

void CEquipmentroomTool::creatLayerByjson(const std::string& sLayerInfo,AcDbDatabase *pDb /*= acdbCurDwg()*/)
{
	std::string strProfessionalAttributes = "";
	std::string strLayerName = "";
	std::string strLayerColor = "";
	std::string strLayerLinetype = "";
	std::string strLayerWidth = "";
	std::string strIsPrintf = "";
	std::string strTransparency = "";
	getLayerConfigForJson(sLayerInfo, strProfessionalAttributes, strLayerName, strLayerColor, strLayerLinetype, strLayerWidth, strIsPrintf, strTransparency);
	CString sProfessionalAttributes(strProfessionalAttributes.c_str());
	CString sLayerName(strLayerName.c_str());
	CString sLayerColor(strLayerColor.c_str());
	CString sLayerLinetype(strLayerLinetype.c_str());
	CString sLayerWidth(strLayerWidth.c_str());
	CString sIsPrintf(strIsPrintf.c_str());
	CString sTransparency(strTransparency.c_str());
	CString sCount;
	if (!CEquipmentroomTool::layerConfigSet(sLayerName, sLayerColor, sLayerWidth, sLayerLinetype, sTransparency, sIsPrintf, pDb))
	{
		acutPrintf(_T("\n创建图层时属性设置失败！"));
	}
	return;
}

double CEquipmentroomTool::areaScale(double oldArea)
{
	double dScale = 0;
	if (!CEquipmentroomTool::getTotalArea(_T("缩放程度百分比（1-100）:"), dScale))
		return 0;

	if (dScale<1||dScale>100)
	{
		dScale = 100;
	}
	double dnewArea = oldArea*(dScale / 100);
	return dnewArea;
}


void CEquipmentroomTool::getParkingExtentPts(std::vector<AcGePoint2dArray>& parkingExtentPts, const std::vector<AcDbObjectId>& allChooseIds, const CString& parkingLayerName, std::map<AcDbObjectId, AcGePoint2dArray>& parkIdAndPts)
{
	for (int i = 0; i < allChooseIds.size(); i++)
	{
		AcDbEntity* pEntity = NULL;
		if (acdbOpenAcDbEntity(pEntity, allChooseIds[i], kForRead) != eOk)
			continue;

		CString layername = pEntity->layer();
		if (layername.Compare(parkingLayerName) != 0)
		{
			pEntity->close();
			continue;
		}

		if (pEntity->isKindOf(AcDbBlockReference::desc()))
		{
			AcDbVoidPtrArray tempEnts;
			AcGePoint2dArray arrTempPlinePts;
			DBHelper::ExplodeEntity(pEntity, tempEnts);
			for (int j = 0; j < tempEnts.length(); j++)
			{
				AcDbEntity* pEnty = (AcDbEntity*)tempEnts.at(j);
				if (pEnty != NULL)
				{
					if (pEnty->isKindOf(AcDbPolyline::desc()))
					{
						std::vector<AcGePoint2d> allPoints;//得到的所有点
						AcDbPolyline *pPline = AcDbPolyline::cast(pEnty);
						AcGeLineSeg2d line;
						AcGeCircArc3d arc;
						int n = pPline->numVerts();
						for (int a = 0; a < n; a++)
						{
							if (pPline->segType(a) == AcDbPolyline::kLine)
							{
								pPline->getLineSegAt(a, line);
								AcGePoint2d startPoint;
								AcGePoint2d endPoint;
								startPoint = line.startPoint();
								endPoint = line.endPoint();
								allPoints.push_back(startPoint);
								allPoints.push_back(endPoint);
							}
							else if (pPline->segType(a) == AcDbPolyline::kArc)
							{
								pPline->getArcSegAt(a, arc);
								AcGePoint3dArray result = GeHelper::CalcArcFittingPoints(arc, 3);
								for (int x = 0; x < result.length(); x++)
								{
									AcGePoint2d onArcpoint(result[x].x, result[x].y);
									allPoints.push_back(onArcpoint);
								}
							}
						}
						for (int x = 0; x < allPoints.size(); x++)
						{
							if (arrTempPlinePts.contains(allPoints[x]))
							{
								continue;
							}
							arrTempPlinePts.append(allPoints[x]);
						}
						//检测闭合
						if (arrTempPlinePts.length() > 2 && arrTempPlinePts[arrTempPlinePts.length() - 1] != arrTempPlinePts[0])
							arrTempPlinePts.append(arrTempPlinePts[0]);
					}
					delete pEnty;
				}
			}
			std::pair<AcDbObjectId, AcGePoint2dArray> value(allChooseIds[i], arrTempPlinePts);
			parkIdAndPts.insert(value);
			parkingExtentPts.push_back(arrTempPlinePts);
		}
		pEntity->close();
	}
}

CString CEquipmentroomTool::getOpenDwgFilePath()
{
	const TCHAR* filePath;
	acdbCurDwg()->getFilename(filePath);
	CString s = filePath;
	const ACHAR* file;
	file = acdbHostApplicationServices()->workingDatabase()->originalFileName();
	CString ss = file;
	return s;
}

void CEquipmentroomTool::creatNewDwg()
{
	CString path = getOpenDwgFilePath();
	CString deleName = _T(".dwg");
	path = path.Trim(deleName);
	CTime t = CTime::GetCurrentTime();
	int day = t.GetDay(); //获得几号  
	int year = t.GetYear(); //获取年份  
	int month = t.GetMonth(); //获取当前月份  
	int hour = t.GetHour(); //获取当前为几时   
	int minute = t.GetMinute(); //获取分钟  
	int second = t.GetSecond(); //获取秒  
								//int w = t.GetDayOfWeek(); //获取星期几，注意1为星期天，7为星期六</span>
	CString sDay;
	sDay.Format(_T("%d"), day);
	CString sYear;
	sYear.Format(_T("%d"), year);
	CString sMonth;
	sMonth.Format(_T("%d"), month);
	CString sHour;
	sHour.Format(_T("%d"), hour);
	CString sMinute;
	sMinute.Format(_T("%d"), minute);
	CString sSecond;
	sSecond.Format(_T("%d"), second);
	CString sNum = _T("_")+sYear + sMonth + sDay + sHour + sMinute + sSecond+_T(".dwg");
	CString newFileName = path + sNum;
	Acad::ErrorStatus es;
	// 创建新的图形数据库，分配内存空间
	AcDbDatabase *pDb = new AcDbDatabase(true, false);
	es = acdbHostApplicationServices()->workingDatabase()->wblock(pDb);
	allEntMoveAndClone(pDb);
	es =  pDb->saveAs(newFileName);
	delete pDb;  //pDb不是数据库的常驻对象，必须手工销毁

	int stop = 0;
}

bool CEquipmentroomTool::allEntMoveAndClone(AcDbDatabase *pDataBase,int iCount /*= 0*/)
{
	    AcGeMatrix3d xform;
		AcGeVector3d VectrorPt;
		AcGePoint2dArray extenPts = getAllEntCreatExten();
		if (extenPts.length()!=2)
		{
			acutPrintf(_T("\n获取最大包围框失败！"));
			return false;
		}
		AcGePoint3d endPt(extenPts[1].x, extenPts[0].y, 0);
		AcGePoint3d starPt(extenPts[0].x, extenPts[0].y, 0);
		VectrorPt = endPt - starPt;
		xform.setToTranslation(VectrorPt*iCount);
        //AcDbDatabase *pDataBase = NULL;
		//pDataBase = acdbCurDwg();  //根据需要传入不同AcDbDatabase 就可以做到不同dwg克隆实体
	
		Acad::ErrorStatus es = Acad::eOk;
			
		//
		AcDbBlockTable *pBlockTable = NULL;
		es = pDataBase->getBlockTable(pBlockTable, AcDb::kForRead); //得到块表指针
		if (Acad::eOk != es)
			return false;
 
		AcDbBlockTableRecord *pBlockTableRecord = NULL;
		es = pBlockTable->getAt(ACDB_MODEL_SPACE,pBlockTableRecord,AcDb::kForWrite);	//得到块表记录指针
		if (Acad::eOk != es)
			return false;
 
		pBlockTable->close();
		pBlockTable = NULL;
 
		AcDbBlockTableRecordIterator *pBlockIter = NULL;
		es = pBlockTableRecord->newIterator(pBlockIter);
		if (Acad::eOk != es)
			return false;
		AcDbObjectId objTmpId = AcDbObjectId::kNull;
		std::vector<AcDbEntity*> pEnts;
		for (pBlockIter->start(); !pBlockIter->done(); pBlockIter->step())
		{
			pBlockIter->getEntityId(objTmpId);
			AcDbObjectPointer<AcDbEntity> pEnt(objTmpId, AcDb::kForWrite);
			if (pEnt.openStatus() == Acad::eOk)
			{
				   AcDbEntity *pEntity = NULL;
					//pEntity = AcDbEntity::cast(pEnt->clone());//克隆不移动
					es = pEnt->getTransformedCopy(xform,(AcDbEntity*&)pEntity); //克隆移动实体
					if (es!=Acad::eOk)
					{
						continue;
					}
					pEnt->erase();
					pEnts.push_back(pEntity);
			}
		}
		for (int size=0; size<pEnts.size(); size++)
		{
			AcDbObjectId tempId;
			//pEnts[size]->setColorIndex(1);
			pBlockTableRecord->appendAcDbEntity(tempId, pEnts[size]);
			pEnts[size]->close();
		}

		pBlockTableRecord->close();
		pBlockTableRecord = NULL;
	
		if (pBlockIter != NULL)
		{
			delete pBlockIter;
			pBlockIter = NULL;
		}
		return true;
}

AcGePoint2dArray CEquipmentroomTool::getAllEntCreatExten()
{
	AcDbDatabase *pCurDb = NULL;
	AcGePoint2dArray useExtenPts;
	pCurDb = acdbHostApplicationServices()->workingDatabase();
	AcDbBlockTable *pBlkTbl;
	//Acad::ErrorStatus es;
	if (pCurDb->getBlockTable(pBlkTbl, AcDb::kForRead) != Acad::eOk)
	{
		acutPrintf(_T("读取块表失败！"));
		delete pCurDb;
		return useExtenPts;
	}
	AcDbBlockTableRecord *pBlkTblRcd;
	if (pBlkTbl->getAt(ACDB_MODEL_SPACE, pBlkTblRcd, AcDb::kForRead) != Acad::eOk)
	{
		acutPrintf(_T("读取块表记录失败！"));
		pBlkTbl->close();
		return useExtenPts;
	}
	pBlkTbl->close();

	AcDbBlockTableRecordIterator *pItr = NULL;		                	            //块表记录遍历器
	if (pBlkTblRcd->newIterator(pItr) != Acad::eOk)
	{
		acutPrintf(_T("创建块表记录遍历器失败！"));
		pBlkTblRcd->close();
		return useExtenPts;
	}
	pBlkTblRcd->close();

	AcDbEntity *pEnt = NULL;
	AcDbExtents allEntCreatExtents;
	for (pItr->start(); !pItr->done(); pItr->step())
	{
		if (pItr->getEntity(pEnt, AcDb::kForRead) != Acad::eOk)
		{
			acutPrintf(_T("读取实体失败！"));
			continue;
		}
		AcDbExtents entExtents;
		DBHelper::GetEntityExtents(entExtents, pEnt);
		allEntCreatExtents.addExt(entExtents);
		pEnt->close();
	}
	AcGePoint2d maxPoint(allEntCreatExtents.maxPoint().x, allEntCreatExtents.maxPoint().y);
	AcGePoint2d minPoint(allEntCreatExtents.minPoint().x, allEntCreatExtents.minPoint().y);
	useExtenPts.append(minPoint);
	useExtenPts.append(maxPoint);
	return useExtenPts;
}

void CEquipmentroomTool::test()
{
	creatNewDwg();
	//getAllEntCreatExten();
}

