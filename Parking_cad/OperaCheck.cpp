#include "stdafx.h"
#include "OperaCheck.h"
#include "DBHelper.h"
#include <json/json.h>
#include "ModulesManager.h"
#include "EquipmentroomTool.h"
#include "GeHelper.h"
#include "DelayExecuter.h"
#include "stdafx.h"
#include <ctime>
#include "RTreeEx.h"
#include "Convertor.h"
#include "LibcurlHttp.h"

std::string COperaCheck::ms_uuid;
std::string COperaCheck::ms_strGetCheckUrl;
CString COperaCheck::ms_shearWallLayerName;

COperaCheck::COperaCheck(const AcString& group, const AcString& cmd, const AcString& alias, Adesk::Int32 cmdFlag)
	: CIOperaLog(group, cmd, alias, cmdFlag)
{
}


COperaCheck::~COperaCheck()
{
}

void __stdcall ExeObjsCllecoter(WPARAM wp, LPARAM lp, void* anyVal)
{
	ObjectCollector* oc = (ObjectCollector*)anyVal;
	AcDbObjectIdArray appendEntIds = oc->m_objsAppended;
	AcDbObjectIdArray clodLineIds;
	for (int i=0; i<appendEntIds.length(); i++)
	{
		AcDbEntity* pEntity = NULL;
		if (acdbOpenAcDbEntity(pEntity, appendEntIds[i], kForRead) != eOk)
		{
			continue;
		}
		if (pEntity->isKindOf(AcDbPolyline::desc()))
		{
			AcDbObjectId coldLineID = pEntity->objectId();
			clodLineIds.append(coldLineID);
		}
		pEntity->close();
	}
	delete oc;
	CString sCloudLineLayer(CEquipmentroomTool::getLayerName("cloud_line").c_str());
	Doc_Locker _locker;
	for (int j = 0; j<clodLineIds.length(); j++)
	{
		CEquipmentroomTool::setEntToLayer(clodLineIds[j], sCloudLineLayer);
		COperaCheck::setCloudLineWidth(clodLineIds[j]);
	}
	COperaCheck::setCurrentLayer(_T("0"));
}

void COperaCheck::Start()
{
	//CEquipmentroomTool::test();
	CEquipmentroomTool::creatLayerByjson("cloud_line");

	/*std::string json;
	std::string sMsg;
	int status = getCheckData(sMsg, json);
	if (status != 1)
	{
		acedAlert(GL::Ansi2WideByte(sMsg.c_str()).c_str());
		return;
	}
	CString serrorMsg;
	if (!getDataforJson(json, serrorMsg))
	{
		acedAlert(serrorMsg);
		return;
	}*/

	clock_t start, finish;
	double totaltime;
	start = clock();
	overlapShow();
	finish = clock();
	totaltime = (double)(finish - start) / CLOCKS_PER_SEC;
	/*acedGetAcadDwgView()->SetFocus();
	DBHelper::CallCADCommandEx(_T("REGEN"));*/
	int ct = 0;
}

void COperaCheck::setUuid(const std::string& uuid)
{
	ms_uuid = uuid;
}

void COperaCheck::setGetCheckUrl(const std::string& strGetCheckUrl)
{
	ms_strGetCheckUrl = strGetCheckUrl;
}

void COperaCheck::setShearWallLayerName(const CString& layerName)
{
	ms_shearWallLayerName = layerName;
}

int COperaCheck::getCheckData(std::string& sMsg, std::string& json)
{
	if (ms_uuid == "")
	{
		sMsg = "uuid不能为空。";
		return 4;
	}

	//std::string httpUrl = "http://10.8.212.187/query/";
	std::string tempUrl;
	tempUrl = ms_strGetCheckUrl + ms_uuid;

	const char * sendUrl = tempUrl.c_str();
	
	HTTP_CLIENT::Ins().setTimeout(60);
	int code = HTTP_CLIENT::Ins().get(sendUrl, true);
	if (code != 200)
	{
		char szCode[10];
		sprintf(szCode, "%d", code);
		sMsg = ms_strGetCheckUrl + ":网络或服务器错误。(" + szCode + ")";
		return 3;
	}
	//std::string sRes = GL::Utf82Ansi(http.response.body.c_str());

	int len = 0;
	json = HTTP_CLIENT::Ins().getBody(len);

	Json::Reader reader;
	Json::Value root;
	//从字符串中读取数据
	if (reader.parse(json, root))
	{
#ifdef _DEBUG
		WriteFile("check.json", json.c_str(), json.size(), NULL, 0, false);
#endif
		Json::Value& blank = root["blank"];
		if (blank.isNull())
		{
			return 0;
			sMsg = "数据缺失。";
		}
		return 1;
	}

	sMsg = "结果格式有误。";
	return 3;
}

bool COperaCheck::getDataforJson(const std::string& json, CString& sMsg)
{
	Json::Reader reader;
	Json::Value root;
	//从字符串中读取数据
	std::vector<AcGePoint2dArray> blankPoints;
	if (reader.parse(json, root))
	{	
		Json::Value& blank = root["blank"];
		if (blank.isNull())
		{
			sMsg = _T("回传json不存在[\"blank\"]字段！");
			return false;
		}
		else
		{
			if (blank.isArray())
			{
				int nblankSize = blank.size();
				for (int k = 0; k < nblankSize; k++)
				{
					AcGePoint2dArray oneBlankPts;
					if (blank[k].isArray())
					{
						int oneblankSize = blank[k].size();
						for (int g = 0; g < oneblankSize; g++)
						{
							double ptX = blank[k][g][0].asDouble();
							double ptY = blank[k][g][1].asDouble();
							AcGePoint2d tempPt(ptX, ptY);
							oneBlankPts.append(tempPt);
						}
					}
					blankPoints.push_back(oneBlankPts);
				}
			}
			else
			{
				sMsg = _T("回传json中[\"blank\"]字段格式不匹配！");
				return false;
			}
		}
	}
	else
	{
		sMsg = _T("解析回传json文件出错！");
		return false;
	}
	Doc_Locker _locker;
	CEquipmentroomTool::layerSet(_T("0"), 7);
	CString blockName;
	
	for (int d = 0; d < blankPoints.size(); d++)
	{
		AcDbObjectId blankId;
		blankCheckShow(blankPoints[d], blankId);
	}
	CEquipmentroomTool::layerSet(_T("0"), 7);
	return true;
}


void COperaCheck::blankCheckShow(const AcGePoint2dArray& blankCheckPts, AcDbObjectId& blankId, AcDbDatabase *pDb /*= acdbCurDwg()*/)
{
	AcGePoint2dArray plineExtentPts = getPlineExtentPts(blankCheckPts);
	creatCloudLine(plineExtentPts,blankId,pDb);
}


void COperaCheck::overlapShow()
{

	//用于直线筛选的rtree
	RT::RTreeEx<UINT32, double, 2> rTreeOfLineFilter;

	//获取到所有车位面域
	CString parkingLayer(CEquipmentroomTool::getLayerName("ordinary_parking").c_str());
	AcDbObjectIdArray allParkingIds = DBHelper::GetEntitiesByLayerName(parkingLayer);
	for (int a=0; a<allParkingIds.length(); a++)
	{
		AcDbEntity* pEntity = NULL;
		if (acdbOpenAcDbEntity(pEntity, allParkingIds[a], kForRead) != eOk)
			continue;
		if (pEntity->isKindOf(AcDbBlockReference::desc()))
		{
			AcDbExtents LineExtents;
			DBHelper::GetEntityExtents(LineExtents, pEntity);
			AcGePoint3d maxPoint = LineExtents.maxPoint();
			AcGePoint3d minPoint = LineExtents.minPoint();
			double positionMin[2];
			double positionMax[2];
			positionMin[0] = minPoint.x;
			positionMin[1] = minPoint.y;
			positionMax[0] = maxPoint.x;
			positionMax[1] = maxPoint.y;
			AcDbObjectId id = pEntity->objectId();
			UINT32 handle = DBHelper::GetObjectIdHandle(id);
			rTreeOfLineFilter.Insert(positionMin, positionMax, handle);//装handle
		}
		pEntity->close();
	}

	CString columnLayer(CEquipmentroomTool::getLayerName("column").c_str());
	AcDbObjectIdArray columnEntIds = DBHelper::GetEntitiesByLayerName(columnLayer);
	for (int b=0; b<columnEntIds.length(); b++)
	{
		AcDbEntity* pEnt = NULL;
		if (acdbOpenAcDbEntity(pEnt, columnEntIds[b], kForRead) != eOk)
			continue;
		if (pEnt->isKindOf(AcDbPolyline::desc()))
		{
			std::vector<AcGePoint2dArray> allChekParkPts;
			AcDbExtents columnExtents;
			DBHelper::GetEntityExtents(columnExtents, pEnt);
			AcGePoint3d maxPoint = columnExtents.maxPoint();
			AcGePoint3d minPoint = columnExtents.minPoint();
			double positionMin[2];
			double positionMax[2];
			positionMin[0] = minPoint.x;
			positionMin[1] = minPoint.y;
			positionMax[0] = maxPoint.x;
			positionMax[1] = maxPoint.y;
			std::set<UINT32 > parkEntset;
			rTreeOfLineFilter.Search(positionMin, positionMax, &parkEntset);
			std::set<UINT32>::const_iterator columnEntIter;
			for (columnEntIter = parkEntset.begin(); columnEntIter != parkEntset.end(); columnEntIter++)
			{
				UINT32 ptempEntHandle = *columnEntIter;//取得句柄
				AcGePoint2dArray allPoints = COperaCheck::getOneParkPts(ptempEntHandle);
				if (allPoints.length() == 0)
				{
					continue;
				}
				allChekParkPts.push_back(allPoints);
			}
			AcGePoint2dArray allPoints = getOnePlineEntPts(pEnt);
			for (int b = 0; b < allChekParkPts.size(); b++)
			{
				std::vector<AcGePoint2dArray> polyIntersections;
				GeHelper::GetIntersectionOfTwoPolygon(allChekParkPts[b], allPoints, polyIntersections);
				if (polyIntersections.size()>0)
				{
					for (int three = 0; three<polyIntersections.size(); three++)
					{
						if (polyIntersections[three].length()<3)
						{
							continue;
						}
						double ss = GeHelper::CalcPolygonArea(polyIntersections[three]);
						if (polyIntersections[three][polyIntersections[three].length() - 1] != polyIntersections[three][0])
						{
							polyIntersections[three].append(polyIntersections[three][0]);
						}
						double sss = GeHelper::CalcPolygonArea(polyIntersections[three]);
						if (ss<1 && sss<1)
						{
							continue;
						}
						/*for (int four = 0; four < polyIntersections[three].length(); four++)
						{
						AcGePoint2d look = polyIntersections[three][four];
						int as = polyIntersections[three].length();
						}*/
						AcGePoint2dArray plineExtentPts = getPlineExtentPts(polyIntersections[three]);
						AcDbObjectId cloudId;
						creatCloudLine(plineExtentPts, cloudId);
					}
				}
			}
		}
		pEnt->close();
	}

	AcDbObjectIdArray shearWallEntIds = DBHelper::GetEntitiesByLayerName(ms_shearWallLayerName);
	for (int c = 0; c<shearWallEntIds.length(); c++)
	{
		AcDbEntity* pWallEnt = NULL;
		if (acdbOpenAcDbEntity(pWallEnt, shearWallEntIds[c], kForRead) != eOk)
			continue;
		if (pWallEnt->isKindOf(AcDbPolyline::desc()))
		{
			std::vector<AcGePoint2dArray> allChekParkPts;
			AcDbExtents shearWallExtents;
			DBHelper::GetEntityExtents(shearWallExtents, pWallEnt);
			AcGePoint3d maxPoint = shearWallExtents.maxPoint();
			AcGePoint3d minPoint = shearWallExtents.minPoint();
			double positionMin[2];
			double positionMax[2];
			positionMin[0] = minPoint.x;
			positionMin[1] = minPoint.y;
			positionMax[0] = maxPoint.x;
			positionMax[1] = maxPoint.y;
			std::set<UINT32 > parkEntset;
			rTreeOfLineFilter.Search(positionMin, positionMax, &parkEntset);
			std::set<UINT32>::const_iterator columnEntIter;
			for (columnEntIter = parkEntset.begin(); columnEntIter != parkEntset.end(); columnEntIter++)
			{
				UINT32 ptempEntHandle = *columnEntIter;//取得句柄
				AcGePoint2dArray allPoints = COperaCheck::getOneParkPts(ptempEntHandle);
				if (allPoints.length() == 0)
				{
					continue;
				}
				allChekParkPts.push_back(allPoints);
			}
			AcGePoint2dArray allPoints = getOnePlineEntPts(pWallEnt);
			for (int b = 0; b < allChekParkPts.size(); b++)
			{
				std::vector<AcGePoint2dArray> polyIntersections;
				GeHelper::GetIntersectionOfTwoPolygon(allChekParkPts[b], allPoints, polyIntersections);
				if (polyIntersections.size()>0)
				{
					for (int three = 0; three<polyIntersections.size(); three++)
					{
						if (polyIntersections[three].length()<3)
						{
							continue;
						}
						double ss = GeHelper::CalcPolygonArea(polyIntersections[three]);
						if (polyIntersections[three][polyIntersections[three].length() - 1] != polyIntersections[three][0])
						{
							polyIntersections[three].append(polyIntersections[three][0]);
						}
						double sss = GeHelper::CalcPolygonArea(polyIntersections[three]);
						if (ss<1 && sss<1)
						{
							continue;
						}
						AcGePoint2dArray plineExtentPts = getPlineExtentPts(polyIntersections[three]);
						AcDbObjectId cloudID;
						creatCloudLine(plineExtentPts, cloudID);
					}
				}
			}
		}
		pWallEnt->close();
	}
}

std::vector<AcGePoint2dArray> COperaCheck::getPlinePtsByLayer(const CString& layername)
{
	std::vector<AcGePoint2dArray> outputPoints;
	AcDbObjectIdArray entIds;
	entIds = DBHelper::GetEntitiesByLayerName(layername);
	if (entIds.length() == 0)
	{
		return outputPoints;
	}

	for (int i = 0; i < entIds.length(); i++)
	{
		AcDbEntity* pEntity = NULL;
		if (acdbOpenAcDbEntity(pEntity, entIds[i], kForRead) != eOk)							//这里是可读
		{
			//acutPrintf(_T("\n获取单个实体指针失败！"));
			//return outputPoints;
			continue;
		}

		if (pEntity->isKindOf(AcDbPolyline::desc()))
		{
			std::vector<AcGePoint2d> allPoints;//得到的所有点
			AcDbVoidPtrArray entsTempArray;
			AcDbPolyline *pPline = AcDbPolyline::cast(pEntity);
			AcGeLineSeg2d line;
			AcGeCircArc3d arc;
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
					allPoints.push_back(startPoint);
					allPoints.push_back(endPoint);
				}
				else if (pPline->segType(i) == AcDbPolyline::kArc)
				{
					pPline->getArcSegAt(i, arc);
					AcGePoint3dArray result = GeHelper::CalcArcFittingPoints(arc, 16);
					for (int x = 0; x < result.length(); x++)
					{
						AcGePoint2d onArcpoint(result[x].x, result[x].y);
						allPoints.push_back(onArcpoint);
					}
				}
			}

			AcGePoint2dArray onePlinePts;//装取去完重的有效点
			for (int x = 0; x < allPoints.size(); x++)
			{
				if (onePlinePts.contains(allPoints[x]))
				{
					continue;
				}
				onePlinePts.append(allPoints[x]);
			}
			bool bClosed = true;
			//检查闭合
			if (onePlinePts.length() > 1 && pPline->isClosed() ||
				(bClosed && onePlinePts.length() > 1 && onePlinePts[onePlinePts.length() - 1] != onePlinePts[0]))
			{
				onePlinePts.append(onePlinePts[0]);
			}

			outputPoints.push_back(onePlinePts);
		}
		pEntity->close();
	}
	return outputPoints;
}

AcGePoint2dArray COperaCheck::getPlineExtentPts(AcGePoint2dArray plinePts)
{
	AcDbPolyline *pPline = new AcDbPolyline(plinePts.length());
	double width = 0;//线宽
	for (int i = 0; i < plinePts.length(); i++)
	{
		pPline->addVertexAt(0, plinePts[i], 0, width, width);
	}
	pPline->setClosed(true);
	AcDbExtents LineExtents;
	DBHelper::GetEntityExtents(LineExtents, pPline);
	AcGePoint2d maxPoint(LineExtents.maxPoint().x, LineExtents.maxPoint().y);
	AcGePoint2d minPoint(LineExtents.minPoint().x, LineExtents.minPoint().y);
	AcGePoint2dArray plineExtentPts;
	plineExtentPts.append(minPoint);
	plineExtentPts.append(maxPoint);
	/*AcDbObjectId plineId;
	DBHelper::AppendToDatabase(plineId, pPline);*/
	pPline->close();
	return plineExtentPts;
}

void COperaCheck::creatCloudLine(AcGePoint2dArray plineExtentPts, AcDbObjectId& cloudId, AcDbDatabase *pDb/*= acdbCurDwg()*/)
{
//TODO: 杨兵
	AcGePoint2d minPoint = plineExtentPts[0];
	AcGePoint2d maxPoint = plineExtentPts[1];

	AcGeVector2d dirMove = maxPoint - minPoint;
	AcGeVector2d vecDirMove = dirMove.normalize();
	maxPoint.transformBy(dirMove * 50);
	minPoint.transformBy(dirMove.negate() * 50);
	double distance = minPoint.distanceTo(maxPoint);
	double minArcLength = distance / 20;
	double maxArcLength = minArcLength * 2;

	AcGePoint2d leftUpPt(minPoint.x,maxPoint.y);
	AcGePoint2d rightDownPt(maxPoint.x,minPoint.y);
	AcGePoint2dArray midUpPt = COperaCheck::getLineOtherPoint(maxPoint,leftUpPt,minArcLength,maxArcLength);
	AcGePoint2dArray midDownPt = COperaCheck::getLineOtherPoint(minPoint,rightDownPt, minArcLength, maxArcLength);
	AcGePoint2dArray midLeftPt = COperaCheck::getLineOtherPoint(leftUpPt,minPoint, minArcLength, maxArcLength);
	AcGePoint2dArray midRightPt = COperaCheck::getLineOtherPoint(rightDownPt,maxPoint, minArcLength, maxArcLength);
	AcGePoint2dArray plineUsePts;
	plineUsePts.append(minPoint);
	for (int a=0;a<midDownPt.length(); a++)
	{
		AcGePoint2d look = midDownPt[a];
		plineUsePts.append(midDownPt[a]);
	}
	//plineUsePts.append(midDownPt);
	plineUsePts.append(rightDownPt);
	for (int b = 0; b < midRightPt.length(); b++)
	{
		AcGePoint2d look = midRightPt[b];
		plineUsePts.append(midRightPt[b]);
	}
	//plineUsePts.append(midRightPt);
	plineUsePts.append(maxPoint);
	for (int c = 0; c < midUpPt.length(); c++)
	{
		AcGePoint2d look = midUpPt[c];
		plineUsePts.append(midUpPt[c]);
	}
	//plineUsePts.append(midUpPt);
	plineUsePts.append(leftUpPt);
	for (int d = 0; d < midLeftPt.length(); d++)
	{
		AcGePoint2d look = midLeftPt[d];
		plineUsePts.append(midLeftPt[d]);
	}
	//plineUsePts.append(midLeftPt);
	plineUsePts.append(minPoint);
	Doc_Locker _locker;
	cloudId = COperaCheck::creatArcPline(plineUsePts, 10,pDb);
	CString sCloudLineLayer(CEquipmentroomTool::getLayerName("cloud_line").c_str());
	CEquipmentroomTool::setEntToLayer(cloudId, sCloudLineLayer);
}

CString COperaCheck::doubleToCString(double num)
{
	CString sNum;
	sNum.Format(_T("%.2f"), num);
	return sNum;
}

void COperaCheck::setCloudLineWidth(AcDbObjectId cloudLineId)
{
	AcDbEntity *pEnty = NULL;
	Acad::ErrorStatus es;
	es = acdbOpenObject(pEnty, cloudLineId, AcDb::kForWrite);
	if (es == Acad::eOk)
	{
		AcDbPolyline *pLine = AcDbPolyline::cast(pEnty);
		es = pLine->setConstantWidth(10); 
		pLine->close();
	}
}

void COperaCheck::setCurrentLayer(CString layerName)
{
	//Doc_Locker _locker;
	AcDbLayerTable *pLayerTbl;
	//获取当前图形层表
	Acad::ErrorStatus es;
	es = acdbCurDwg()->getLayerTable(pLayerTbl, AcDb::kForRead);
	if (es != eOk)
	{
		return;
	}
	if (pLayerTbl->has(layerName))//判断已经有了该图层，应置为当前图层
	{
		AcDbObjectId layerId;
		if (pLayerTbl->getAt(layerName, layerId) != Acad::eOk)
		{
			pLayerTbl->close();
			return;
		}
		pLayerTbl->close();

		es = acdbCurDwg()->setClayer(layerId);//设为当前图层		
	}
}

AcGePoint2dArray COperaCheck::getLineOtherPoint(const AcGePoint2d& lineStartPoint, const AcGePoint2d& lineEndPoint, 
	const double& minArcLength, const double& maxArcLength)
{
	AcGePoint3d startPt(lineStartPoint.x, lineStartPoint.y,0);
	AcGePoint3d endPt(lineEndPoint.x, lineEndPoint.y, 0);
	double lineDistance = startPt.distanceTo(endPt);
	int sum = 0;
	std::vector<int> moveDistances;
	while (sum < lineDistance)
	{
		int temp = getRandNum(minArcLength, maxArcLength);
		sum += temp;
		moveDistances.push_back(temp);
	}
	double sub = sum - lineDistance;
	int endNum = moveDistances.back();
	double newNum = endNum - sub;
	if (newNum >= minArcLength)
	{
		moveDistances.back() = newNum;
	}
	moveDistances.pop_back();
	AcGePoint2dArray allUsePts;
	AcGeVector2d dirVec = lineEndPoint - lineStartPoint;
	AcGeVector2d vecDirMove = dirVec.normalize();
	AcGePoint2d lineMidPoint = lineStartPoint;
	for (int i=0; i<moveDistances.size();i++)
	{
		lineMidPoint.transformBy(vecDirMove*moveDistances[i]);
		allUsePts.append(lineMidPoint);
	}
	return allUsePts;
}

AcDbObjectId COperaCheck::creatArcPline(AcGePoint2dArray points, double width, AcDbDatabase *pDb/* = acdbCurDwg()*/)
{
	int numCount = points.length();
	AcDbPolyline *pPline = new AcDbPolyline(numCount);
	for (int i=0; i<numCount; i++)
	{
		pPline->addVertexAt(i, points.at(i), 0.516, width, width);
	}
	AcDbObjectId polyId;
	DBHelper::AppendToDatabase(polyId, pPline,pDb);
	pPline->close();
	return polyId;
}

int COperaCheck::getRandNum(const int& min, const int& max)
{
	//根据系统时间设置随机数种子
	srand((unsigned)time(NULL));
	int inum = rand() % (max - min + 1) + min;
	return inum;
}

AcGePoint2dArray COperaCheck::getOneParkPts(UINT32 parkHandle)
{
	AcGePoint2dArray arrTempPlinePts;
	AcDbObjectId idOfTheEntity = DBHelper::GetObjectIdByHandle(parkHandle);
	AcDbEntity* ptempEnt = NULL;
	Acad::ErrorStatus es = acdbOpenAcDbEntity(ptempEnt, idOfTheEntity, kForRead);
	if (es != eOk)
	{
		acutPrintf(_T("\n获取单个实体指针失败！"));
		return arrTempPlinePts;
	}
	if (ptempEnt->isKindOf(AcDbBlockReference::desc()))//对search到的车位进行取点操作
	{
		AcDbVoidPtrArray tempEnts;
		DBHelper::ExplodeEntity(ptempEnt, tempEnts);
		for (int j = 0; j < tempEnts.length(); j++)
		{
			AcDbEntity* pEnty = (AcDbEntity*)tempEnts.at(j);
			if (pEnty != NULL)
			{
				if (pEnty->isKindOf(AcDbPolyline::desc()))
				{
					AcGePoint2dArray allPoints;
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
							allPoints.append(startPoint);
							allPoints.append(endPoint);
						}
						else if (pPline->segType(a) == AcDbPolyline::kArc)
						{
							pPline->getArcSegAt(a, arc);
							AcGePoint3dArray result = GeHelper::CalcArcFittingPoints(arc, 3);
							for (int x = 0; x < result.length(); x++)
							{
								AcGePoint2d onArcpoint(result[x].x, result[x].y);
								allPoints.append(onArcpoint);
							}
						}
					}
					for (int x = 0; x < allPoints.length(); x++)
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
	}
	ptempEnt->close();
	return arrTempPlinePts;
}

AcGePoint2dArray COperaCheck::getOnePlineEntPts(AcDbEntity* pEntity)
{
	AcGePoint2dArray onePlinePts;//装取去完重的有效点
	if (pEntity->isKindOf(AcDbPolyline::desc()))
	{
		std::vector<AcGePoint2d> allPoints;//得到的所有点
		AcDbVoidPtrArray entsTempArray;
		AcDbPolyline *pPline = AcDbPolyline::cast(pEntity);
		AcGeLineSeg2d line;
		AcGeCircArc3d arc;
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
				allPoints.push_back(startPoint);
				allPoints.push_back(endPoint);
			}
			else if (pPline->segType(i) == AcDbPolyline::kArc)
			{
				pPline->getArcSegAt(i, arc);
				AcGePoint3dArray result = GeHelper::CalcArcFittingPoints(arc, 16);
				for (int x = 0; x < result.length(); x++)
				{
					AcGePoint2d onArcpoint(result[x].x, result[x].y);
					allPoints.push_back(onArcpoint);
				}
			}
		}
		for (int x = 0; x < allPoints.size(); x++)
		{
			if (onePlinePts.contains(allPoints[x]))
			{
				continue;
			}
			onePlinePts.append(allPoints[x]);
		}
		bool bClosed = true;
		//检查闭合
		if (onePlinePts.length() > 1 && pPline->isClosed() ||
			(bClosed && onePlinePts.length() > 1 && onePlinePts[onePlinePts.length() - 1] != onePlinePts[0]))
		{
			onePlinePts.append(onePlinePts[0]);
		}
	}
	return onePlinePts;
}


REG_CMD_P(COperaCheck, BGY, Check);//图纸智能化检测