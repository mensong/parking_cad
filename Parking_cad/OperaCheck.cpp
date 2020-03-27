#include "stdafx.h"
#include "OperaCheck.h"
#include "DBHelper.h"
#include <json/json.h>
#include "ModulesManager.h"
#include "EquipmentroomTool.h"
#include "GeHelper.h"
#include "DelayExecuter.h"


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

	CEquipmentroomTool::getOpenDwgFilePath();
	CEquipmentroomTool::creatLayerByjson("cloud_line");
	//CString sCloudLineLayer(CEquipmentroomTool::getLayerName("cloud_line").c_str());
	//CString init = _T("_layer S ") + sCloudLineLayer+_T("\r\r");
	//CString init = _T("_layer s 0\r\r");
	//DBHelper::CallCADCommand(init);
	AcDbDatabase *pDb = acdbCurDwg();
	ObjectCollector* oc = new ObjectCollector;
	Acad::ErrorStatus es = oc->start(pDb);
	overlapShow();
	SetDelayExecute(ExeObjsCllecoter, 0, 0, (void*)oc, 0, false);
		
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
		sMsg = "uuid����Ϊ�ա�";
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
		sMsg = "get Httpģ�����ʧ�ܣ�";
		return 5;
	}
	int code = fn_get(sendUrl, true);

	if (code != 200)
	{
		char szCode[10];
		sprintf(szCode, "%d", code);
		sMsg = ms_strGetCheckUrl + ":��������������(" + szCode + ")";
		return 3;
	}
	//std::string sRes = GL::Utf82Ansi(http.response.body.c_str());

	typedef const char* (*FN_getBody)(int&);
	FN_getBody fn_getBody = ModulesManager::Instance().func<FN_getBody>(getHttpModule(), "getBody");
	if (!fn_getBody)
	{
		sMsg = "getBody Httpģ�����ʧ�ܣ�";
		return 5;
	}
	int len = 0;
	json = fn_getBody(len);

	Json::Reader reader;
	Json::Value root;
	//���ַ����ж�ȡ����
	if (reader.parse(json, root))
	{
#ifdef _DEBUG
		WriteFile("check.json", json.c_str(), json.size(), NULL, 0, false);
#endif
	}

	sMsg = "�����ʽ����";
	return 3;
}

bool COperaCheck::getDataforJson(const std::string& json, CString& sMsg)
{
	Json::Reader reader;
	Json::Value root;
	//���ַ����ж�ȡ����
	std::vector<AcGePoint2dArray> blankPoints;
	if (reader.parse(json, root))
	{	
		Json::Value& blank = root["blank"];
		if (blank.isNull())
		{
			sMsg = _T("�ش�json������[\"blank\"]�ֶΣ�");
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
				sMsg = _T("�ش�json��[\"blank\"]�ֶθ�ʽ��ƥ�䣡");
				return false;
			}
		}
	}
	else
	{
		sMsg = _T("�����ش�json�ļ�������");
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
	AcGePoint2dArray plineExtentPts = getPlineExtentPts(blankCheckPts);
	creatCloudLine(plineExtentPts);
	//CString sPillarLayer(CEquipmentroomTool::getLayerName("column").c_str());
	//CEquipmentroomTool::creatLayerByjson("column");
	//AcDbPolyline *pPoly = new AcDbPolyline(blankCheckPts.length());
	//double width = 0;//�߿�
	//for (int i = 0; i < blankCheckPts.length(); i++)
	//{
	//	pPoly->addVertexAt(0, blankCheckPts[i], 0, width, width);
	//}
	//pPoly->setClosed(true);
	//AcDbObjectId pillarId;
	//DBHelper::AppendToDatabase(pillarId, pPoly);
	//pPoly->close();
	//CEquipmentroomTool::setEntToLayer(pillarId, sPillarLayer);
}


void COperaCheck::overlapShow()
{
	//��ȡ�����г�λ����
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
		acutPrintf(_T("\n��ȡ��λ�������ʧ�ܣ�"));
		return;
	}
	//��ȡ���з�������
	CString columnLayer(CEquipmentroomTool::getLayerName("column").c_str());
	std::vector<AcGePoint2dArray> columnPts = getPlinePtsByLayer(columnLayer);
	if (columnPts.empty())
	{
		acutPrintf(_T("\n��ȡ�����������ʧ�ܣ�"));
		return;
	}
	//��ȡ����ǽ������
	std::vector<AcGePoint2dArray> shearWallPts = getPlinePtsByLayer(ms_shearWallLayerName);
	if (shearWallPts.empty())
	{
		acutPrintf(_T("\n��ȡ����ǽ�������ʧ�ܣ�"));
		return;
	}
	for (int one=0; one<parkingsPoints.size(); one++)
	{
		for (int two=0; two<columnPts.size(); two++)
		{
			std::vector<AcGePoint2dArray> polyIntersections;
			GeHelper::GetIntersectionOfTwoPolygon(parkingsPoints[one], columnPts[two], polyIntersections);
			if (polyIntersections.size()>0)
			{
				for (int three=0; three<polyIntersections.size(); three++)
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
					if (ss<1&&sss<1)
					{
						continue;
					}
					/*for (int four = 0; four < polyIntersections[three].length(); four++)
					{
						AcGePoint2d look = polyIntersections[three][four];
						int as = polyIntersections[three].length();
					}*/
					AcGePoint2dArray plineExtentPts = getPlineExtentPts(polyIntersections[three]);
					creatCloudLine(plineExtentPts);
				}
			}
		}	
	}
	for (int first=0; first<parkingsPoints.size(); first++)
	{
		for (int second=0; second<shearWallPts.size(); second++)
		{
			std::vector<AcGePoint2dArray> polyIntersections;
			GeHelper::GetIntersectionOfTwoPolygon(parkingsPoints[first], shearWallPts[second], polyIntersections);
			if (polyIntersections.size() > 0)
			{
				for (int three = 0; three < polyIntersections.size(); three++)
				{
					if (polyIntersections[three].length() < 3)
					{
						continue;
					}
					double ss = GeHelper::CalcPolygonArea(polyIntersections[three]);
					if (polyIntersections[three][polyIntersections[three].length() - 1] != polyIntersections[three][0])
					{
						polyIntersections[three].append(polyIntersections[three][0]);
					}
					double sss = GeHelper::CalcPolygonArea(polyIntersections[three]);
					if (ss < 1 && sss < 1)
					{
						continue;
					}
					/*for (int four=0; four<polyIntersections[three].length(); four++)
					{
						AcGePoint2d look = polyIntersections[three][four];
						int as = polyIntersections[three].length();
					}*/
					AcGePoint2dArray plineExtentPts = getPlineExtentPts(polyIntersections[three]);
					creatCloudLine(plineExtentPts);
				}
			}
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
		if (acdbOpenAcDbEntity(pEntity, entIds[i], kForRead) != eOk)							//�����ǿɶ�
		{
			//acutPrintf(_T("\n��ȡ����ʵ��ָ��ʧ�ܣ�"));
			//return outputPoints;
			continue;
		}

		if (pEntity->isKindOf(AcDbPolyline::desc()))
		{
			std::vector<AcGePoint2d> allPoints;//�õ������е�
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

			AcGePoint2dArray onePlinePts;//װȡȥ���ص���Ч��
			for (int x = 0; x < allPoints.size(); x++)
			{
				if (onePlinePts.contains(allPoints[x]))
				{
					continue;
				}
				onePlinePts.append(allPoints[x]);
			}
			bool bClosed = true;
			//���պ�
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
	double width = 0;//�߿�
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
	AcGePoint2d minPoint = plineExtentPts[0];
	AcGePoint2d maxPoint = plineExtentPts[1];
	AcGeVector2d dirMove = maxPoint - minPoint;
	AcGeVector2d vecDirMove = dirMove.normalize();
	maxPoint.transformBy(dirMove * 50);
	minPoint.transformBy(dirMove.negate() * 50);
	double distance = minPoint.distanceTo(maxPoint);
	double minArcLength = distance / 20;
	double maxArcLength = minArcLength * 2;
	CString sminArcLength = COperaCheck::doubleToCString(minArcLength);
	CString smaxArcLength = COperaCheck::doubleToCString(maxArcLength);
	CString setArcLength = _T("REVCLOUD A ") + sminArcLength + _T(" ") + smaxArcLength + _T(" ");
	CString sMinPtX = COperaCheck::doubleToCString(minPoint.x);
	CString sMinPtY = COperaCheck::doubleToCString(minPoint.y);
	CString sMinPt = sMinPtX + _T(",") + sMinPtY + _T(" ");
	CString sMaxPtX = COperaCheck::doubleToCString(maxPoint.x);
	CString sMaxPtY = COperaCheck::doubleToCString(maxPoint.y);
	CString sMaxPt = sMaxPtX + _T(",") + sMaxPtY + _T(" ");
	CString command = setArcLength + _T("R ") + sMinPt + sMaxPt;
	DBHelper::CallCADCommand(command);
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
	//��ȡ��ǰͼ�β��
	Acad::ErrorStatus es;
	es = acdbCurDwg()->getLayerTable(pLayerTbl, AcDb::kForRead);
	if (es != eOk)
	{
		return;
	}
	if (pLayerTbl->has(layerName))//�ж��Ѿ����˸�ͼ�㣬Ӧ��Ϊ��ǰͼ��
	{
		AcDbObjectId layerId;
		if (pLayerTbl->getAt(layerName, layerId) != Acad::eOk)
		{
			pLayerTbl->close();
			return;
		}
		pLayerTbl->close();

		es = acdbCurDwg()->setClayer(layerId);//��Ϊ��ǰͼ��		
	}
}

REG_CMD_P(COperaCheck, BGY, Check);//ͼֽ���ܻ����