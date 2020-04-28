#include "stdafx.h"
#include "OperaMultiSchemeShow.h"
#include "DBHelper.h"
#include <json/json.h>
#include "EquipmentroomTool.h"
#include "OperaAddFrame.h"
#include "OperaAxleNetMaking.h"
#include "GeHelper.h"
#include "CommonFuntion.h"
#include "ArxProgressBar.h"
#include "DelayExecuter.h"


std::string COperaMultiSchemeShow::ms_json;
int COperaMultiSchemeShow::ms_count;
//void OpenDoc( void *pData)
//{
//	AcApDocument* pDoc = acDocManager->curDocument();
//	if (acDocManager->isApplicationContext()) 
//	{
//		acDocManager->appContextOpenDocument((const ACHAR *)pData);
//	} 
//	else
//	{
//		acutPrintf(_T("\nERROR To Open Doc!\n"));
//	}
//}


COperaMultiSchemeShow::COperaMultiSchemeShow(const AcString& group, const AcString& cmd, const AcString& alias, Adesk::Int32 cmdFlag)
	: CIOperaLog(group, cmd, alias, cmdFlag)
	,m_ProgressBar(NULL)
{
}


COperaMultiSchemeShow::~COperaMultiSchemeShow()
{
}

void COperaMultiSchemeShow::Start()
{
	m_ProgressBar = new ArxProgressBar(_T("正在进行多方案排布"), 0, 3);
	creatNewDwg();
	//delete rootPDb;  //pDb不是数据库的常驻对象，必须手工销毁
	/*CString sLineTypeFile = _T("C:\\Users\\admin\\AppData\\Roaming\\Autodesk\\AutoCAD 2014\\R19.1\\chs\\Support\\acad.lin");
	acdbLoadLineTypeFile(_T("continuousx"),sLineTypeFile,NULL);*/
	/*acDocManager->executeInApplicationContext(OpenDoc, (void *)pData);
	if (acDocManager->isApplicationContext())
	{
		static TCHAR pData[] = _T("C:\\Users\\admin\\Desktop\\CAD测试用图纸\\示例1.dwg");
		Acad::ErrorStatus es = acDocManager->appContextOpenDocument(pData);
	}*/
}

void COperaMultiSchemeShow::getJsonData(const std::string& json, const int& count)
{
	ms_json = json;
	ms_count = count;
}

bool COperaMultiSchemeShow::addEntToDb(const std::string& json, CString& sMsg, AcDbDatabase *pDataBase, int scheme /*= 0*/)
{
	Json::Reader reader;
	Json::Value root;
	//从字符串中读取数据
	AcGePoint2dArray parkingPts;
	std::vector<double> parkingDirections;
	std::vector<AcGePoint2dArray> axisesPoints;
	std::vector<AcGePoint2dArray> lanesPoints;
	AcGePoint2dArray scopePts;
	std::vector<AcGePoint2dArray> pillarPoints;
	std::vector<AcGePoint2dArray> arrowPoints;
	double dParkingLength;
	double dParkingWidth;
	double dLaneWidth;
	if (reader.parse(json, root))
	{
		if (root["result"].isArray())
		{
			Json::Value& oneScheme = root["result"][scheme];
			Json::Value& parkings = oneScheme["parkings"];
			if (parkings.isNull())
			{
				sMsg = _T("回传json不存在[\"result\"][\"parkings\"]字段！");
				return false;
			}
			else
			{
				if (parkings.isArray())
				{
					int nArraySize = parkings.size();
					for (int i = 0; i < nArraySize; i++)
					{
						double ptX = parkings[i]["position"][0].asDouble();
						double ptY = parkings[i]["position"][1].asDouble();
						AcGePoint2d pt(ptX, ptY);
						parkingPts.append(pt);
						double direction = parkings[i]["direction"].asDouble();
						parkingDirections.push_back(direction);
					}
				}
				else
				{
					sMsg = _T("回传json中[\"result\"][\"parkings\"]字段格式不匹配！");
					return false;
				}
			}
			Json::Value& data = oneScheme["data"];
			if (data.isNull())
			{
				sMsg = _T("回传json不存在[\"result\"][\"data\"]字段！");
				return false;
			}
			else
			{
				if (data["cell_length"].isDouble() && data["cell_num"].isInt())
				{
					dParkingLength = data["cell_length"].asDouble();
					dParkingWidth = data["cell_width"].asDouble();
					dLaneWidth = data["lane_width"].asDouble();
					std::vector<double> dataVector;
					std::vector<std::string> strDataNameVector;
					std::map < std::string, double > tableData;

					double dSp = data["Sp"].asDouble();
					strDataNameVector.push_back("SP");
					dataVector.push_back(dSp);

					double dSpt = data["Spt"].asDouble();
					strDataNameVector.push_back("SPT");
					dataVector.push_back(dSpt);

					double dSpf = data["Spf"].asDouble();
					strDataNameVector.push_back("SPF");
					dataVector.push_back(dSpf);

					double dSpf1 = data["Spf1"].asDouble();
					strDataNameVector.push_back("SPF1");
					dataVector.push_back(dSpf1);

					double dSpf2 = data["Spf2"].asDouble();
					strDataNameVector.push_back("SPF2");
					dataVector.push_back(dSpf2);

					double dSpf3 = data["Spf3"].asDouble();
					strDataNameVector.push_back("SPF3");
					dataVector.push_back(dSpf3);

					double dCp = data["Cp"].asDouble();
					strDataNameVector.push_back("CP");
					dataVector.push_back(dCp);

					double dJSpc = data["JSpc"].asDouble();
					strDataNameVector.push_back("JSPC");
					dataVector.push_back(dJSpc);

					double dSpc = data["Spc"].asDouble();
					strDataNameVector.push_back("SPC");
					dataVector.push_back(dSpc);

					double dH = data["H"].asDouble();
					strDataNameVector.push_back("H");
					dataVector.push_back(dH);

					double Ht = data["Ht"].asDouble();
					strDataNameVector.push_back("HT");
					dataVector.push_back(Ht);
					CString resultText;
					for (int i = 0; i < dataVector.size(); i++)
					{
						CString strTempData;
						strTempData.Format(_T("%.2f"), dataVector[i]);
						std::pair<std::string, double> value(strDataNameVector[i], dataVector[i]);
						tableData.insert(value);//插入新元素
												//resultText += (strDataNameVector[i] + _T("=") + strTempData + _T("|"));
					}
					int gg = tableData.size();
					COperaAddFrame::setTableDataMap(tableData);
				}
				else
				{
					sMsg = _T("回传json中[\"result\"][\"parkings\"]字段格式不匹配！");
					return false;
				}
			}
			//Json::Value& axis = oneScheme["axis"];
			/*if (axis.isNull())
			{
				sMsg = _T("回传json不存在[\"result\"][\"axis\"]字段！");
				return false;
			}
			else
			{
				if (axis.isArray())
				{
					int nAxisSize = axis.size();
					for (int j = 0; j < nAxisSize; j++)
					{
						double ptX1 = axis[j]["points"][0][0].asDouble();
						double ptY1 = axis[j]["points"][0][1].asDouble();
						double ptX2 = axis[j]["points"][1][0].asDouble();
						double ptY2 = axis[j]["points"][1][1].asDouble();
						CString snum =  axis[j]["no"].asString();
						AcGePoint2d startPt(ptX1, ptY1);
						AcGePoint2d endPt(ptX2, ptY2);
						AcGePoint2dArray axisPts;
						axisPts.append(startPt);
						axisPts.append(endPt);
						axisesPoints.push_back(axisPts);
					}
				}
				else
				{
					sMsg = _T("回传json中[\"result\"][\"axis\"]字段格式不匹配！");
					return false;
				}
			}*/
			Json::Value& lane = oneScheme["lane"];
			if (lane.isNull())
			{
				sMsg = _T("回传json不存在[\"result\"][\"lane\"]字段！");
				return false;
			}
			else
			{
				if (lane.isArray())
				{
					int nLaneSize = lane.size();
					for (int m = 0; m < nLaneSize; m++)
					{
						double ptX1 = lane[m][0][0].asDouble();
						double ptY1 = lane[m][0][1].asDouble();
						double ptX2 = lane[m][1][0].asDouble();
						double ptY2 = lane[m][1][1].asDouble();
						AcGePoint2d startPt(ptX1, ptY1);
						AcGePoint2d endPt(ptX2, ptY2);
						AcGePoint2dArray lanePts;
						lanePts.append(startPt);
						lanePts.append(endPt);
						lanesPoints.push_back(lanePts);
					}
				}
				else
				{
					sMsg = _T("回传json中[\"result\"][\"lane\"]字段格式不匹配！");
					return false;
				}
			}
			Json::Value& scope = oneScheme["scope"];
			if (scope.isNull())
			{
				sMsg = _T("回传json不存在[\"result\"][\"scope\"]字段！");
				return false;
			}
			else
			{
				if (scope.isArray())
				{
					int nPark_columSize = scope.size();
					for (int n = 0; n < nPark_columSize; n++)
					{
						double ptX = scope[n][0].asDouble();
						double ptY = scope[n][1].asDouble();
						AcGePoint2d plinePt(ptX, ptY);
						scopePts.append(plinePt);
					}
				}
				else
				{
					sMsg = _T("回传json中[\"result\"][\"scope\"]字段格式不匹配！");
					return false;
				}
			}
			Json::Value& pillar = oneScheme["pillar"];
			if (pillar.isNull())
			{
				sMsg = _T("回传json不存在[\"result\"][\"pillar\"]字段！");
				return false;
			}
			else
			{
				if (pillar.isArray())
				{
					int npillarSize = pillar.size();
					for (int k = 0; k < npillarSize; k++)
					{
						AcGePoint2dArray onePillarPts;
						if (pillar[k].isArray())
						{
							int onepillarSize = pillar[k].size();
							for (int g = 0; g < onepillarSize; g++)
							{
								double ptX = pillar[k][g][0].asDouble();
								double ptY = pillar[k][g][1].asDouble();
								AcGePoint2d tempPt(ptX, ptY);
								onePillarPts.append(tempPt);
							}
						}
						pillarPoints.push_back(onePillarPts);
					}
				}
				else
				{
					sMsg = _T("回传json中[\"result\"][\"pillar\"]字段格式不匹配！");
					return false;
				}
			}
			/*Json::Value& arrow = oneScheme["arrow"];
			if (arrow.isNull())
			{
				sMsg = _T("回传json不存在[\"result\"][\"arrow\"]字段！");
				return false;
			}
			else
			{
				if (arrow.isArray())
				{
					int narrowSize = arrow.size();
					for (int k = 0; k < narrowSize; k++)
					{
						AcGePoint2dArray oneArrowPts;
						if (arrow[k].isArray())
						{
							int nonearrowSize = arrow[k].size();
							for (int g = 0; g < nonearrowSize; g++)
							{
								double ptX = arrow[k][g][0].asDouble();
								double ptY = arrow[k][g][1].asDouble();
								AcGePoint2d tempPt(ptX, ptY);
								oneArrowPts.append(tempPt);
							}
						}
						arrowPoints.push_back(oneArrowPts);
					}
				}
				else
				{
					sMsg = _T("回传json中[\"result\"][\"arrow\"]字段格式不匹配！");
					return false;
				}
			}*/
		}
		else
		{
			sMsg = _T("返回json文件数据格式解析出错！");
			return false;
		}
	}
	else
	{
		sMsg = _T("解析回传json文件出错！");
		return false;
	}
	Doc_Locker _locker;
	CEquipmentroomTool::layerSet(_T("0"), 7,pDataBase);
	CString blockName;
	creatNewParkingBlock(dParkingLength, dParkingWidth, blockName, pDataBase);
	for (int a = 0; a < parkingPts.length(); a++)
	{
		//double = (rotation/180)*Π(顺时针和逆时针)
		double rotation = ((360 - parkingDirections[a]) / 180)*ARX_PI;
		AcDbObjectId parkingId;
		AcGePoint2d parkingShowPt = parkingPts[a];
		parkingShow(parkingId, parkingPts[a], rotation, blockName,pDataBase);
	}
	/*AcDbObjectIdArray axisIds;
	for (int b = 0; b < axisesPoints.size(); b++)
	{
		axisIds.append(axisShow(axisesPoints[b], pDataBase));
	}*/

	AcDbObjectIdArray RoadLineIds;
	for (int c = 0; c < lanesPoints.size(); c++)
	{
		RoadLineIds.append(laneShow(lanesPoints[c],pDataBase));
	}

	scopeShow(scopePts,pDataBase);

	for (int d = 0; d < pillarPoints.size(); d++)
	{
		pillarShow(pillarPoints[d],pDataBase);
	}

	/*for (int e = 0; e < arrowPoints.size(); e++)
	{
		arrowShow(arrowPoints[e],pDataBase);
	}*/
	//生成车道标注
	double dTransLaneWidth = dLaneWidth * 1000;
	if (dTransLaneWidth == 0)
	{
		acutPrintf(_T("车道宽度数据有误！"));
	}
	else
	{
		setLandDismensions(dTransLaneWidth, RoadLineIds,pDataBase);
	}
	//int tagnum = 0;
	//int addtagnum = 0;
	//int num = 0;//横向编号
	//CString tagvalue;//轴号属性值
	//CString csAddtagvalue = _T("");
	//std::map<AcDbObjectId, AcString> AxisNumberMap;
	//for (int i = 0; i < axisIds.length(); ++i)
	//{
	//	COperaAxleNetMaking::LongitudinalNumbering(tagnum, addtagnum, tagvalue, csAddtagvalue);
	//	AxisNumberMap[axisIds[i]] = tagvalue;
	//}
	
	//COperaAxleNetMaking::drawAxisNumber(axisIds, AxisNumberMap,pDataBase);

	CString sAxisLayerName(CEquipmentroomTool::getLayerName("parking_axis").c_str());
	CEquipmentroomTool::setLayerClose(sAxisLayerName,pDataBase);
	CString sAxisDimLayerName(CEquipmentroomTool::getLayerName("axis_dimensions").c_str());
	CEquipmentroomTool::setLayerClose(sAxisDimLayerName,pDataBase);
	CEquipmentroomTool::layerSet(_T("0"), 7,pDataBase);
	return true;
}

void COperaMultiSchemeShow::creatNewParkingBlock(const double& dParkingLength, const double& dParkingWidth, CString& blockName, AcDbDatabase *pDb/* = acdbCurDwg()*/)
{
	double dUseLength = dParkingLength * 1000;
	double dUseWidth = dParkingWidth * 1000;
	AcGePoint2d squarePt1(0, 0);
	AcGePoint2d squarePt2(0, dUseLength);
	AcGePoint2d squarePt3(dUseWidth, dUseLength);
	AcGePoint2d squarePt4(dUseWidth, 0);
	AcDbPolyline *pPoly = new AcDbPolyline(4);
	double width = 20;//矩形方形线宽
	pPoly->addVertexAt(0, squarePt1, 0, width, width);
	pPoly->addVertexAt(1, squarePt2, 0, width, width);
	pPoly->addVertexAt(2, squarePt3, 0, width, width);
	pPoly->addVertexAt(3, squarePt4, 0, width, width);
	pPoly->setClosed(true);
	AcDbObjectId squareId;
	pPoly->setColorIndex(6);
	DBHelper::AppendToDatabase(squareId, pPoly,pDb);
	pPoly->close();
	AcDbObjectIdArray allIds;
	allIds.append(squareId);
	AcGePoint3d centerPt(dUseWidth / 2, dUseLength / 2, 0);
	AcDbObjectId parkingId;
	DBHelper::InsertBlkRef(parkingId, _T("car_1"), centerPt,pDb);
	allIds.append(parkingId);
	std::vector<AcDbEntity*> blockEnts;
	for (int i = 0; i < allIds.length(); i++)
	{
		AcDbEntity *pEnt = NULL;
		acdbOpenObject(pEnt, allIds[i], AcDb::kForWrite);
		//判断自定义实体的类型
		if (pEnt == NULL)
			continue;
		blockEnts.push_back(pEnt);
	}
	if (blockEnts.size() < 1)
		return;
	CString sParkingLength;
	CString sParkingWidth;
	sParkingLength.Format(_T("%.1f"), dParkingLength);
	sParkingWidth.Format(_T("%.1f"), dParkingWidth);
	CString parkingName = _T("parking");
	blockName = parkingName + _T("_") + sParkingLength + _T("_") + sParkingWidth;
	DBHelper::CreateBlock(blockName, blockEnts, centerPt,NULL,pDb);
	for (int j = 0; j < blockEnts.size(); j++)
	{
		blockEnts[j]->erase();
		blockEnts[j]->close();
	}
}

void COperaMultiSchemeShow::parkingShow(AcDbObjectId& parkingId, const AcGePoint2d& parkingShowPt, 
	const double& parkingShowRotation, const CString& blockName, AcDbDatabase *pDb /*= acdbCurDwg()*/)
{
	CString sParkingsLayer(CEquipmentroomTool::getLayerNameByJson("ordinary_parking").c_str());
	CEquipmentroomTool::creatLayerByjson("ordinary_parking",pDb);
	AcGeVector3d pt(parkingShowPt.x, parkingShowPt.y, 0);
	AcGeMatrix3d mat;
	AcGeVector3d vec(0, 0, 1);
	mat.setToRotation(parkingShowRotation, vec);//double = (rotation/180)*Π
	mat.setTranslation(pt);
	DBHelper::InsertBlkRef(parkingId, blockName, mat, pDb);
	CEquipmentroomTool::setEntToLayer(parkingId, sParkingsLayer);
}

AcDbObjectId COperaMultiSchemeShow::axisShow(const AcGePoint2dArray& axisPts, AcDbDatabase *pDb /*= acdbCurDwg()*/)
{
	CString sAxisLayer(CEquipmentroomTool::getLayerName("parking_axis").c_str());
	CEquipmentroomTool::creatLayerByjson("parking_axis",pDb);
	AcGePoint3d ptStart(axisPts[0].x, axisPts[0].y, 0);
	AcGePoint3d ptEnd(axisPts[1].x, axisPts[1].y, 0);
	AcDbLine *pLine = new AcDbLine(ptStart, ptEnd);
	AcDbObjectId axisId;
	DBHelper::AppendToDatabase(axisId, pLine, pDb);
	pLine->close();
	CEquipmentroomTool::setEntToLayer(axisId, sAxisLayer);
	return axisId;
}

static void ActiveDocExecute(WPARAM wp, LPARAM lp, void* anyVal)
{
	AcApDocument* pDoc = (AcApDocument*)anyVal;
	Acad::ErrorStatus es = acDocManager->activateDocument(pDoc);
	if (es!=eOk)
	{
		acutPrintf(_T("文件打开失败！"));
	}
}

void COperaMultiSchemeShow::creatNewDwg(AcDbDatabase *rootPDb /*= acdbCurDwg()*/)
{
	m_ProgressBar->set(_T("创建新文件"));
	CString path = CEquipmentroomTool::getOpenDwgFilePath();
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
	CString sNum = _T("_") + sYear + sMonth + sDay + sHour + sMinute + sSecond + _T(".dwg");
	CString newFileName = path + sNum;

	Acad::ErrorStatus es;
	// 创建新的图形数据库，分配内存空间
	
	AcDbDatabase *pDb = new AcDbDatabase(true, false);
	es = rootPDb->wblock(pDb);
	if (es!=eOk)
	{
		acutPrintf(_T("\n内含天正实体,请加载天正后尝试！"));
		return;
	}
	m_ProgressBar->forward();
	//pDb->saveAs(newFileName);
	//return;
	m_ProgressBar->set(_T("排布进行中"));
	CString sMsg;
	loadModelFile(pDb);
	if (!COperaMultiSchemeShow::addEntToDb(ms_json, sMsg,pDb))
	{
		acedAlert(sMsg);
		return;
	}
	
	for (int i=1; i<ms_count; i++)
	{
		AcDbDatabase *pTempDb; // 临时图形数据库
		es = rootPDb->wblock(pTempDb);
		if (es!=eOk)
		{
			acutPrintf(_T("\n生成图纸失败:%d"), i);
			continue;
		}

		CString sMsg1;
		loadModelFile(pTempDb);
		COperaMultiSchemeShow::addEntToDb(ms_json, sMsg1,pTempDb,i);
		//CEquipmentroomTool::allEntMoveAndClone(pTempDb,(i*1.5));

		AcDbExtents extDb;
		es = DBHelper::GetBlockExtents(extDb, ACDB_MODEL_SPACE, pTempDb);
		if (es!=eOk)
		{
			acutPrintf(_T("\n生成图纸失败:%d"), i);
			continue;
		}
		AcGeVector3d dir(1,0,0);
		double lenght = extDb.maxPoint().x - extDb.minPoint().x;
		double offset = (lenght * 1.5) * i;
		AcGeMatrix3d mat;
		mat.setTranslation(dir * offset);

		es = pDb->insert(mat, pTempDb);
		if (es!=eOk)
		{
			acutPrintf(_T("\n生成图纸失败:%d"), i);
		}
	}
	es = pDb->saveAs(newFileName);
	delete pDb;  //pDb不是数据库的常驻对象，必须手工销毁
	m_ProgressBar->forward();

	m_ProgressBar->set(_T("打开排布好文件"), 0, 3);
	Doc_Locker _locker;
	//static ACHAR *pData = newFileName;//_T("C:\\Users\\admin\\Desktop\\CAD测试用图纸\\示例1.dwg");
	AcApDocument* pDoc = DBHelper::OpenFile(newFileName);

	SetDelayExecute(ActiveDocExecute, 0, 0, (void*)pDoc, 1, false);
		
	m_ProgressBar->forward();
}

void COperaMultiSchemeShow::loadModelFile(AcDbDatabase *pDb/*= acdbCurDwg()*/)
{
	AcString sTemplateFile = DBHelper::GetArxDir() + _T("template.dwg");
	std::set<AcString> setBlockNames;
	setBlockNames.insert(_T("car_1"));
	ObjectCollector oc;
	oc.start(pDb);
	if (!DBHelper::ImportBlkDef(sTemplateFile, setBlockNames/*_T("Parking_1")*/,pDb))
	{
		acedAlert(_T("加载模板文件出错！"));
		return;
	}
	if (oc.m_objsAppended.length() > 0)
	{
		CString sParkingsLayer(CEquipmentroomTool::getLayerName("ordinary_parking").c_str());
		CEquipmentroomTool::creatLayerByjson("dimensions_dimensiontext",pDb);
		//CEquipmentroomTool::layerSet(sParkingsLayer, 7,pDb);

		for (int i = 0; i < oc.m_objsAppended.length(); i++)
		{
			CEquipmentroomTool::setEntToLayer(oc.m_objsAppended[i], sParkingsLayer);
		}
	}
}

AcDbObjectId COperaMultiSchemeShow::laneShow(const AcGePoint2dArray& lanePts,AcDbDatabase *pDb/*= acdbCurDwg()*/)
{
	CString sLaneLayer(CEquipmentroomTool::getLayerName("lane_center_line_and_driving_direction").c_str());
	CEquipmentroomTool::creatLayerByjson("lane_center_line_and_driving_direction",pDb);
	AcGePoint3d ptStart(lanePts[0].x, lanePts[0].y, 0);
	AcGePoint3d ptEnd(lanePts[1].x, lanePts[1].y, 0);
	AcDbLine *pLine = new AcDbLine(ptStart, ptEnd);
	AcDbObjectId laneId;
	DBHelper::AppendToDatabase(laneId,pLine,pDb);
	pLine->close();
	CEquipmentroomTool::setEntToLayer(laneId, sLaneLayer);
	return laneId;
}

void COperaMultiSchemeShow::scopeShow(const AcGePoint2dArray& park_columnPts,AcDbDatabase *pDb/*= acdbCurDwg()*/)
{
	CString sScopeLayer(CEquipmentroomTool::getLayerName("rangeline").c_str());
	CEquipmentroomTool::creatLayerByjson("rangeline",pDb);
	AcDbPolyline *pPoly = new AcDbPolyline(park_columnPts.length());
	double width = 0;//线宽
	for (int i = 0; i < park_columnPts.length(); i++)
	{
		pPoly->addVertexAt(0, park_columnPts[i], 0, width, width);
	}
	pPoly->setClosed(true);
	AcDbObjectId scopeId;
	DBHelper::AppendToDatabase(scopeId,pPoly,pDb);
	pPoly->close();
	CEquipmentroomTool::setEntToLayer(scopeId, sScopeLayer);
}

void COperaMultiSchemeShow::pillarShow(const AcGePoint2dArray& onePillarPts,AcDbDatabase *pDb/*= acdbCurDwg()*/)
{
	CString sPillarLayer(CEquipmentroomTool::getLayerName("column").c_str());
	CEquipmentroomTool::creatLayerByjson("column",pDb);
	AcDbPolyline *pPoly = new AcDbPolyline(onePillarPts.length());
	double width = 0;//线宽
	for (int i = 0; i < onePillarPts.length(); i++)
	{
		pPoly->addVertexAt(0, onePillarPts[i], 0, width, width);
	}
	pPoly->setClosed(true);
	AcDbObjectId pillarId;
	DBHelper::AppendToDatabase(pillarId,pPoly,pDb);
	pPoly->close();
	CEquipmentroomTool::setEntToLayer(pillarId, sPillarLayer);
}

void COperaMultiSchemeShow::arrowShow(const AcGePoint2dArray& oneArrowPts,AcDbDatabase *pDb/*= acdbCurDwg()*/)
{
	CString sArrowLayer(CEquipmentroomTool::getLayerName("arrow").c_str());
	CEquipmentroomTool::creatLayerByjson("arrow",pDb);
	AcDbPolyline *pPoly = new AcDbPolyline(oneArrowPts.length());
	double width = 0;//线宽
	for (int i = 0; i < oneArrowPts.length(); i++)
	{
		pPoly->addVertexAt(0, oneArrowPts[i], 0, width, width);
	}
	pPoly->setClosed(true);
	AcDbObjectId arrowId;
	DBHelper::AppendToDatabase(arrowId,pPoly,pDb);
	pPoly->close();
	CEquipmentroomTool::setEntToLayer(arrowId, sArrowLayer);
}

void COperaMultiSchemeShow::setLandDismensions(double m_dDis, const AcDbObjectIdArray& RoadLineIds,AcDbDatabase *pDb/*= acdbCurDwg()*/)
{
	CCommonFuntion::creatLaneGridDimensionsDimStyle(_T("车道轴网尺寸标注样式"),pDb);//创建新的标注样式
	CString sLanesDimLayer(CEquipmentroomTool::getLayerName("dimensions_dimensiontext").c_str());
	CEquipmentroomTool::creatLayerByjson("dimensions_dimensiontext",pDb);
	if (RoadLineIds.length() == 0)
	{
		acutPrintf(_T("\n没有车道信息，生成车道标注失败！"));
		return;
	}
	AcDbEntity *pEnt = NULL;
	Acad::ErrorStatus es;
	for (int i = 0; i < RoadLineIds.length(); ++i)
	{
		es = acdbOpenObject(pEnt, RoadLineIds[i], AcDb::kForRead);
		if (es != eOk)
			continue;
		if (pEnt->isKindOf(AcDbLine::desc()))
		{
			AcDbLine *pLine = AcDbLine::cast(pEnt);
			AcGePoint3d startpoint;
			pLine->getStartPoint(startpoint);
			AcGePoint3d endpoint;
			pLine->getEndPoint(endpoint);
			AcGePoint3d centerpoint = AcGePoint3d((startpoint.x + endpoint.x) / 2, (startpoint.y + endpoint.y) / 2, (startpoint.z + endpoint.z) / 2);

			AcGeVector3d movevec = AcGeVector3d(startpoint - endpoint);
			movevec.normalize();
			centerpoint.transformBy(movevec * 800);

			AcGeVector3d tempVec = AcGeVector3d(startpoint - endpoint);
			AcGeVector3d vec = tempVec.rotateBy(ARX_PI / 2, AcGeVector3d(0, 0, 1));
			vec.normalize();

			AcGePoint3d tempPoint = centerpoint;
			AcGePoint3d movePt1 = tempPoint.transformBy(vec * (m_dDis / 2));
			tempPoint = centerpoint;
			AcGePoint3d movePt2 = tempPoint.transformBy(-vec * (m_dDis / 2));

			CString disText;
			int iDistance = ceil(m_dDis);
			disText.Format(_T("%d"), iDistance);

			centerpoint.transformBy(movevec * 300);
			AcDbObjectId dimId;
			dimId = createDimAligned(movePt1, movePt2, centerpoint, disText,pDb);
			CEquipmentroomTool::setEntToLayer(dimId, sLanesDimLayer);
			pLine->close();
			pEnt->close();
		}
		pEnt->close();
	}
}

AcDbObjectId COperaMultiSchemeShow::createDimAligned(const AcGePoint3d& pt1, const AcGePoint3d& pt2, const AcGePoint3d& ptLine, const ACHAR* dimText,AcDbDatabase *pDb/*= acdbCurDwg()*/)
{
	CString str = _T("车道轴网尺寸标注样式");
	AcDbObjectId id;
	////获得当前图形的标注样式表  
	AcDbDimStyleTable* pDimStyleTbl;
	pDb->getDimStyleTable(pDimStyleTbl, AcDb::kForRead);
	if (pDimStyleTbl->has(str))
		pDimStyleTbl->getAt(str, id);
	else
	{
		pDimStyleTbl->close();
		return id;
	}

	pDimStyleTbl->close();

	AcDbAlignedDimension *pnewdim = new AcDbAlignedDimension(pt1, pt2, ptLine, NULL, id);//创建标注实体
	AcDbObjectId dimid;
	AcDbObjectId dimensionId;
	DBHelper::AppendToDatabase(dimensionId, pnewdim,pDb);

	if (pnewdim)
		pnewdim->close();

	return dimensionId;
}

void COperaMultiSchemeShow::Ended()
{
	if (m_ProgressBar)
		delete m_ProgressBar;
	m_ProgressBar = NULL;
}

REG_CMD_P(COperaMultiSchemeShow, BGY, MultiSchemeShow);//多方案排布展示