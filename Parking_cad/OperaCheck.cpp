#include "stdafx.h"
#include "OperaCheck.h"
#include "DBHelper.h"
#include <json/json.h>
#include "ModulesManager.h"
#include "EquipmentroomTool.h"
#include "GeHelper.h"

std::string COperaCheck::ms_uuid;
std::string COperaCheck::ms_strGetCheckUrl;
CString COperaCheck::ms_shearWallLayerName;

COperaCheck::COperaCheck()
{
}


COperaCheck::~COperaCheck()
{
}

void COperaCheck::Start()
{
	acutPrintf(_T("\nsb"));
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
	
	typedef void(*FN_setTimeout)(int timeout);
	FN_setTimeout fn_setTimeout = ModulesManager::Instance().func<FN_setTimeout>(getHttpModule(), "setTimeout");
	if (fn_setTimeout)
	{
		fn_setTimeout(60);
	}

	typedef int(*FN_get)(const char* url, bool dealRedirect);
	FN_get fn_get = ModulesManager::Instance().func<FN_get>(getHttpModule(), "get");
	if (!fn_get)
	{
		sMsg = "get Http模块加载失败！";
		return 5;
	}
	int code = fn_get(sendUrl, true);

	if (code != 200)
	{
		char szCode[10];
		sprintf(szCode, "%d", code);
		sMsg = ms_strGetCheckUrl + ":网络或服务器错误。(" + szCode + ")";
		return 3;
	}
	//std::string sRes = GL::Utf82Ansi(http.response.body.c_str());

	typedef const char* (*FN_getBody)(int&);
	FN_getBody fn_getBody = ModulesManager::Instance().func<FN_getBody>(getHttpModule(), "getBody");
	if (!fn_getBody)
	{
		sMsg = "getBody Http模块加载失败！";
		return 5;
	}
	int len = 0;
	json = fn_getBody(len);

	Json::Reader reader;
	Json::Value root;
	//从字符串中读取数据
	if (reader.parse(json, root))
	{
#ifdef _DEBUG
		WriteFile("check.json", json.c_str(), json.size(), NULL, 0, false);
#endif
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
		blankCheckShow(blankPoints[d]);
	}
	CEquipmentroomTool::layerSet(_T("0"), 7);
	return true;
}


void COperaCheck::blankCheckShow(const AcGePoint2dArray& blankCheckPts)
{
	CString sPillarLayer(CEquipmentroomTool::getLayerName("column").c_str());
	CEquipmentroomTool::creatLayerByjson("column");
	AcDbPolyline *pPoly = new AcDbPolyline(blankCheckPts.length());
	double width = 0;//线宽
	for (int i = 0; i < blankCheckPts.length(); i++)
	{
		pPoly->addVertexAt(0, blankCheckPts[i], 0, width, width);
	}
	pPoly->setClosed(true);
	AcDbObjectId pillarId;
	DBHelper::AppendToDatabase(pillarId, pPoly);
	pPoly->close();
	CEquipmentroomTool::setEntToLayer(pillarId, sPillarLayer);
}


void COperaCheck::overlapShow()
{
	//获取到所有车位面域
	CString parkingLayer(CEquipmentroomTool::getLayerName("ordinary_parking").c_str());
	AcDbObjectIdArray allParkingIds = DBHelper::GetEntitiesByLayerName(parkingLayer);
	std::vector<AcDbObjectId> parkIds;
	for (int i = 0; i < allParkingIds.length(); i++)
	{
		parkIds.push_back(allParkingIds[i]);
	}
	std::vector<AcGePoint2dArray> parkingsPoints;
	std::map<AcDbObjectId, AcGePoint2dArray> parkIdAndPts;
	CEquipmentroomTool::getParkingExtentPts(parkingsPoints, parkIds, parkingLayer, parkIdAndPts);
	if (parkingsPoints.empty())
	{
		acutPrintf(_T("\n获取车位面域点组失败！"));
		return;
	}
	//获取所有方柱面域
	CString columnLayer(CEquipmentroomTool::getLayerName("ordinary_parking").c_str());
	std::vector<AcGePoint2dArray> columnPts = getPlinePtsByLayer(columnLayer);
	if (columnPts.empty())
	{
		acutPrintf(_T("\n获取方柱面域点组失败！"));
		return;
	}
	//获取剪力墙的面域
	std::vector<AcGePoint2dArray> shearWallPts = getPlinePtsByLayer(ms_shearWallLayerName);
	if (shearWallPts.empty())
	{
		acutPrintf(_T("\n获取剪力墙面域点组失败！"));
		return;
	}
	for (int one=0; one<parkingsPoints.size(); one++)
	{
		for (int two=0; two<columnPts.size(); two++)
		{
			std::vector<AcGePoint2dArray> polyIntersections;
			GeHelper::GetIntersectionOfTwoPolygon(parkingsPoints[one], columnPts[two], polyIntersections);
		}	
	}
	for (int first=0; first<parkingsPoints.size(); first++)
	{
		for (int second=0; second<shearWallPts.size(); second++)
		{
			std::vector<AcGePoint2dArray> polyIntersections;
			GeHelper::GetIntersectionOfTwoPolygon(parkingsPoints[first], shearWallPts[second], polyIntersections);
		}
		
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

void COperaCheck::creatCloudLine(AcGePoint2dArray plineExtentPts)
{

}

REG_CMD(COperaCheck, BGY, Check);//图纸智能化检测