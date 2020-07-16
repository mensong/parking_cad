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
#include "OperaCheck.h"


std::string COperaMultiSchemeShow::ms_json;
AcDbDatabase* COperaMultiSchemeShow::ms_prootDb;
CString COperaMultiSchemeShow::ms_newFileName;
CString COperaMultiSchemeShow::ms_path;
CString COperaMultiSchemeShow::ms_name;
CString COperaMultiSchemeShow::ms_sOutLineLayerName;
CString COperaMultiSchemeShow::ms_sShearWallLayeNmae;

COperaMultiSchemeShow::COperaMultiSchemeShow(const AcString& group, const AcString& cmd, const AcString& alias, Adesk::Int32 cmdFlag)
	: CIOperaLog(group, cmd, alias, cmdFlag)
{
}


COperaMultiSchemeShow::~COperaMultiSchemeShow()
{
}

void COperaMultiSchemeShow::Start()
{
	WD::Create((DBHelper::GetArxDirA() + "WaitingDialog.exe").c_str());
	WD::SetTitle(_T("正在生成地库排布方案……"));
	if (ms_prootDb==NULL)
	{
		WD::AppendMsg(_T("图形数据库拷贝失败"), WD::GetPos());
		return;
	}
	creatNewDwg(ms_prootDb);
	delete ms_prootDb;
	ms_prootDb = NULL;
}

void COperaMultiSchemeShow::getJsonData(const std::string& json)
{
	ms_json = json;
}

void COperaMultiSchemeShow::setLayerNameToEntUse(const CString& sOutLineLayerName, const CString& sShearWallLayerName)
{
	ms_sOutLineLayerName = sOutLineLayerName;
	ms_sShearWallLayeNmae = sShearWallLayerName;
}

void COperaMultiSchemeShow::getRootDataBaseAndFileName(AcDbDatabase* backUpDataBase, const CString& fileName, const CString& path,const CString& name)
{
	Acad::ErrorStatus es = backUpDataBase->wblock(ms_prootDb);
	ms_newFileName = fileName;
	ms_path = path;
	ms_name = name;
}

void COperaMultiSchemeShow::getRootDataBase(AcDbDatabase* backUpDataBase)
{
	Acad::ErrorStatus es = backUpDataBase->wblock(ms_prootDb);
}

bool COperaMultiSchemeShow::addEntToDb(Json::Value json, AcDbDatabase *pDataBase, int scheme /*= 0*/)
{
	//从字符串中读取数据
	double dParkingLength;
	double dParkingWidth;
	double dLaneWidth;
	CString sMsg;
	Json::Value& oneScheme = json;
	//指标数据解析
	Json::Value& data = oneScheme["data"];
	sMsg.Format(_T("方案%d : 车位指标表"), scheme + 1);
	WD::AppendMsg(sMsg.GetString());
	std::map < std::string, double > tableData;
	if (!parsingData(data, dParkingLength, dParkingWidth, dLaneWidth, sMsg, tableData))
	{
		WD::AppendMsg(sMsg, WD::GetPos());
	}
	//排布结果车位展示
	sMsg.Format(_T("方案%d : 生成车位"), scheme + 1);
	WD::AppendMsg(sMsg.GetString());
	Json::Value& parkings = oneScheme["parkings"];
	Doc_Locker _locker;
	CEquipmentroomTool::layerSet(_T("0"), 7, pDataBase);
	CString blockName;
	creatNewParkingBlock(dParkingLength, dParkingWidth, blockName, pDataBase);
	if (!parsingParkingData(parkings, sMsg, blockName, pDataBase))
	{
		WD::AppendMsg(sMsg, WD::GetPos());
	}
	//轴线展示
	sMsg.Format(_T("方案%d : 生成轴线"), scheme + 1);
	WD::AppendMsg(sMsg.GetString());
	Json::Value& axis = oneScheme["grid"];
	std::map<AcDbObjectId, AcString> idAndNumMap;
	AcDbObjectIdArray axisIds;
	if (parsingAxisData(axis, sMsg, idAndNumMap, axisIds, pDataBase))
	{
		//生成轴线标注
		COperaAxleNetMaking::drawAxisNumber(axisIds, idAndNumMap, pDataBase);
	}
	else
	{
		WD::AppendMsg(sMsg, WD::GetPos());
	}
	//车道线展示
	sMsg.Format(_T("方案%d : 生成车道线"), scheme + 1);
	WD::AppendMsg(sMsg.GetString());
	Json::Value& lane = oneScheme["lane"];
	AcDbObjectIdArray RoadLineIds;
	if (!parsingLaneData(lane, sMsg, RoadLineIds, pDataBase))
	{
		WD::AppendMsg(sMsg, WD::GetPos());
	}
	//生成车道标注
	sMsg.Format(_T("方案%d : 生成车道标注"), scheme + 1);
	WD::AppendMsg(sMsg.GetString());
	double dTransLaneWidth = dLaneWidth * 1000;
	AcDbObjectIdArray laneDimIds;
	if (dTransLaneWidth == 0)
	{
		acutPrintf(_T("车道宽度数据有误！"));
	}
	else
	{
		setLandDismensions(dTransLaneWidth, RoadLineIds, laneDimIds, pDataBase);
		if (laneDimIds.isEmpty())
		{
			WD::AppendMsg(_T("生成车道标注失败"), WD::GetPos());
		}
	}
	//地库范围线展示
	sMsg.Format(_T("方案%d : 生成地库范围线"), scheme + 1);
	WD::AppendMsg(sMsg.GetString());
	Json::Value& scope = oneScheme["scope"];
	if (!parsingScopeData(scope, sMsg, tableData, pDataBase))
	{
		WD::AppendMsg(sMsg, WD::GetPos());
	}
	//方柱展示
	sMsg.Format(_T("方案%d : 生成方柱"), scheme + 1);
	WD::AppendMsg(sMsg.GetString());
	Json::Value& pillar = oneScheme["pillar"];
	AcDbObjectIdArray pillarIds;
	if (!parsingPillarData(pillar, sMsg, pillarIds, pDataBase))
	{
		WD::AppendMsg(sMsg, WD::GetPos());
	}
	//车道方向箭头展示
	sMsg.Format(_T("方案%d : 生成车道方向"), scheme + 1);
	WD::AppendMsg(sMsg.GetString());
	Json::Value& arrow = oneScheme["arrow"];
	AcDbObjectIdArray arrowIds;
	if (!parsingArrowData(arrow, sMsg, arrowIds, pDataBase))
	{
		WD::AppendMsg(sMsg, WD::GetPos());
	}
	checkLaneDimPosition(laneDimIds, arrowIds, pDataBase);
	//空白区域检测
	sMsg.Format(_T("方案%d : 生成空白区域检测线"), scheme + 1);
	WD::AppendMsg(sMsg.GetString());
	Json::Value& blanks = oneScheme["blanks"];
	AcDbObjectIdArray blankIds;
	if (!parsingBlanksData(blanks, sMsg, blankIds, pDataBase))
	{
		WD::AppendMsg(sMsg, WD::GetPos());
	}
	//图层显隐控制
	CString sAxisLayerName(CEquipmentroomTool::getLayerName("parking_axis").c_str());
	CEquipmentroomTool::setLayerClose(sAxisLayerName, pDataBase);
	CString sAxisDimLayerName(CEquipmentroomTool::getLayerName("axis_dimensions").c_str());
	CEquipmentroomTool::setLayerClose(sAxisDimLayerName, pDataBase);
	CString setlayernameofAXSI(CEquipmentroomTool::getLayerName("dimensions_dimensiontext").c_str());
	CEquipmentroomTool::setLayerClose(setlayernameofAXSI, pDataBase);
	CEquipmentroomTool::layerSet(_T("0"), 7, pDataBase);
	return true;
}

void COperaMultiSchemeShow::creatNewParkingBlock(const double& dParkingLength, const double& dParkingWidth, CString& blockName, AcDbDatabase *pDb/* = acdbCurDwg()*/)
{
	double dUseLength = dParkingLength * 1000;
	double dUseWidth = dParkingWidth * 1000;
	if (dUseLength == 0 || dUseWidth == 0)
	{
		acutPrintf(_T("\n待生成车位的长宽数据有误！"));
		return;
	}
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
	pPoly->setColorIndex(253);
	DBHelper::AppendToDatabase(squareId, pPoly, pDb);
	pPoly->close();
	AcDbObjectIdArray allIds;
	allIds.append(squareId);
	AcGePoint3d centerPt(dUseWidth / 2, dUseLength / 2, 0);
	AcDbObjectId parkingId;
	DBHelper::InsertBlkRef(parkingId, _T("car_1"), centerPt, pDb);
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
	if (Acad::eOk != DBHelper::CreateBlock(blockName, blockEnts, centerPt, NULL, pDb))
	{
		acutPrintf(_T("\n创建车位图块失败！"));
	}
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
	CEquipmentroomTool::creatLayerByjson("ordinary_parking", pDb);
	AcGeVector3d pt(parkingShowPt.x, parkingShowPt.y, 0);
	AcGeMatrix3d mat;
	AcGeVector3d vec(0, 0, 1);
	mat.setToRotation(parkingShowRotation, vec);//double = (rotation/180)*Π
	mat.setTranslation(pt);
	DBHelper::InsertBlkRef(parkingId, blockName, mat, pDb);
	DBHelper::AddXRecord(parkingId, _T("实体"), _T("车位"));
	CEquipmentroomTool::setEntToLayer(parkingId, sParkingsLayer);
}

AcDbObjectId COperaMultiSchemeShow::axisShow(const AcGePoint2dArray& axisPts, AcDbDatabase *pDb /*= acdbCurDwg()*/)
{
	CString sAxisLayer(CEquipmentroomTool::getLayerName("parking_axis").c_str());
	CEquipmentroomTool::creatLayerByjson("parking_axis", pDb);
	AcGePoint3d ptStart(axisPts[0].x, axisPts[0].y, 0);
	AcGePoint3d ptEnd(axisPts[1].x, axisPts[1].y, 0);
	AcDbLine *pLine = new AcDbLine(ptStart, ptEnd);
	AcDbObjectId axisId;
	DBHelper::AppendToDatabase(axisId, pLine, pDb);
	pLine->close();
	DBHelper::AddXRecord(axisId, _T("实体"), _T("轴线"));
	CEquipmentroomTool::setEntToLayer(axisId, sAxisLayer);
	return axisId;
}

static void ZoomEExecute(WPARAM wp, LPARAM lp, void* anyVal)
{
	WD::AppendMsg(_T("检测图纸"));
	DBHelper::CallCADCommand(_T("CHECK "));
	WD::AppendMsg(_T("刷新图纸"));
	DBHelper::CallCADCommand(_T("ZOOM E "));

	WD::Close();
}

static void ActiveDocExecute(WPARAM wp, LPARAM lp, void* anyVal)
{
	AcApDocument* pDoc = (AcApDocument*)anyVal;
	Acad::ErrorStatus es = acDocManager->activateDocument(pDoc);
	if (es != eOk)
	{
		acutPrintf(_T("文件打开失败！"));
	}

	SetDelayExecute(ZoomEExecute, 0, 0, NULL, 100, true);
}

void COperaMultiSchemeShow::creatNewDwg(AcDbDatabase *rootPDb /*= acdbCurDwg()*/)
{
	Acad::ErrorStatus es;
	// 创建新的图形数据库，分配内存空间

	AcDbDatabase *pDb = new AcDbDatabase(true, false);

	Json::Reader jsReader;
	if (!jsReader.parse(ms_json, m_json)|| !m_json["result"].isArray())
	{
		acedAlert(_T("返回的结果格式有误。"));
		return;
	}
	const Json::Value& jsArrResult = m_json["result"];

	int nCount = jsArrResult.size();

	if (nCount == 0)
		nCount = 1;

	WD::SetRange(0, nCount + nCount * 9 + 4);

	for (int i = 0; i < nCount; i++)
	{
		CString sMsg;
		sMsg.Format(_T("写入方案%d"), i + 1);
		WD::AppendMsg(sMsg.GetString());

		AcDbDatabase *pTempDb; // 临时图形数据库
		es = rootPDb->wblock(pTempDb);
		if (es != eOk)
		{
			acutPrintf(_T("\n生成图纸失败:%d。请启用必要的天正等专业软件。"), i);
			continue;
		}

		loadModelFile(pTempDb);
		COperaMultiSchemeShow::addEntToDb(jsArrResult[i]/*ms_json*/, pTempDb, i);

		AcDbExtents extDb;
		es = DBHelper::GetBlockExtents(extDb, ACDB_MODEL_SPACE, pTempDb);
		if (es != eOk)
		{
			delete pTempDb;
			acutPrintf(_T("\n生成图纸失败:%d"), i);
			continue;
		}
		AcGeVector3d dir(1, 0, 0);
		double lenght = extDb.maxPoint().x - extDb.minPoint().x;
		double offset = (lenght * 1.5) * i;
		AcGeMatrix3d mat;
		mat.setTranslation(dir * offset);

		es = pDb->insert(mat, pTempDb);
		if (es != eOk)
		{
			acutPrintf(_T("\n生成图纸失败:%d"), i);
		}
		delete pTempDb;
	}
	WD::AppendMsg(_T("保存方案图纸"));
	const ACHAR* filter = _T("dwg文件|*.dwg|dxf文件|*.dxf|All Files(*.*)|*.*||");
	WD::ShowWindow(SW_HIDE);
	m_num = getCurTimeNum();
	CString useName = ms_name + m_num;
	CFileDialog dlg(FALSE, NULL, useName, OFN_OVERWRITEPROMPT, filter, NULL);
	dlg.m_ofn.lpstrInitialDir = ms_path;
	CString useNewFileName = ms_newFileName + m_num;
	if (dlg.DoModal() == IDOK)
	{
		useNewFileName = dlg.GetPathName();
	}
	WD::ShowWindow(SW_SHOW);
	es = pDb->saveAs(useNewFileName);
	delete pDb;  //pDb不是数据库的常驻对象，必须手工销毁
	pDb = NULL;
	WD::AppendMsg(_T("打开排布后的图纸"));

	Doc_Locker _locker;
	//static ACHAR *pData = newFileName;//_T("C:\\Users\\admin\\Desktop\\CAD测试用图纸\\示例1.dwg");

	AcApDocument* pDoc = DBHelper::OpenFile(useNewFileName);

	SetDelayExecute(ActiveDocExecute, 0, 0, (void*)pDoc, 1, false);

}

void COperaMultiSchemeShow::loadModelFile(AcDbDatabase *pDb/*= acdbCurDwg()*/)
{
	AcString sTemplateFile = (GetUserDir() + _T("template.dwg")).GetString();
	std::set<AcString> setBlockNames;
	setBlockNames.insert(_T("car_1"));
	ObjectCollector oc;
	oc.start(pDb);
	if (Acad::eOk != DBHelper::ImportBlkDef(sTemplateFile, setBlockNames/*_T("Parking_1")*/, pDb))
	{
		acedAlert(_T("加载模板文件出错！"));
		return;
	}
	if (oc.objsAppended().length() > 0)
	{
		CString sParkingsLayer(CEquipmentroomTool::getLayerName("ordinary_parking").c_str());
		CEquipmentroomTool::creatLayerByjson("dimensions_dimensiontext", pDb);
		//CEquipmentroomTool::layerSet(sParkingsLayer, 7,pDb);

		for (int i = 0; i < oc.objsAppended().length(); i++)
		{
			CEquipmentroomTool::setEntToLayer(oc.objsAppended()[i], sParkingsLayer);
		}
	}
}

AcDbObjectId COperaMultiSchemeShow::laneShow(const AcGePoint2dArray& lanePts, AcDbDatabase *pDb/*= acdbCurDwg()*/)
{
	CString sLaneLayer(CEquipmentroomTool::getLayerName("lane_center_line_and_driving_direction").c_str());
	CEquipmentroomTool::creatLayerByjson("lane_center_line_and_driving_direction", pDb);
	AcGePoint3d ptStart(lanePts[0].x, lanePts[0].y, 0);
	AcGePoint3d ptEnd(lanePts[1].x, lanePts[1].y, 0);
	AcDbLine *pLine = new AcDbLine(ptStart, ptEnd);
	AcDbObjectId laneId;
	DBHelper::AppendToDatabase(laneId, pLine, pDb);
	pLine->close();
	DBHelper::AddXRecord(laneId, _T("实体"), _T("车道线"));
	CEquipmentroomTool::setEntToLayer(laneId, sLaneLayer);
	return laneId;
}

void COperaMultiSchemeShow::scopeShow(const AcGePoint2dArray& park_columnPts, AcDbObjectId& scopeId, std::map < std::string, double >& tableData, AcDbDatabase *pDb/*= acdbCurDwg()*/)
{
	CString sScopeLayer(CEquipmentroomTool::getLayerName("rangeline").c_str());
	CEquipmentroomTool::creatLayerByjson("rangeline", pDb);
	AcDbPolyline *pPoly = new AcDbPolyline(park_columnPts.length());
	double width = 0;//线宽
	for (int i = 0; i < park_columnPts.length(); i++)
	{
		pPoly->addVertexAt(0, park_columnPts[i], 0, width, width);
	}
	pPoly->setClosed(true);
	DBHelper::AppendToDatabase(scopeId, pPoly, pDb);
	pPoly->close();
	double SPF3value = getPicAttributeValue(tableData, "SPF3");
	CString sSPF3value;
	sSPF3value.Format(_T("%.2f"), SPF3value);
	double SPF4value = getPicAttributeValue(tableData, "SPF4");
	CString sSPF4value;
	sSPF4value.Format(_T("%.2f"), SPF4value);
	double SPF5value = getPicAttributeValue(tableData, "SPF5");
	CString sSPF5value;
	sSPF5value.Format(_T("%.2f"), SPF5value);
	double Hvalue = getPicAttributeValue(tableData, "H");
	CString sHvalue;
	sHvalue.Format(_T("%.2f"), Hvalue);
	double HTvalue = getPicAttributeValue(tableData, "HT");
	CString sHTvalue;
	sHTvalue.Format(_T("%.2f"), HTvalue);
	DBHelper::AddXRecord(scopeId, _T("SPF3"), sSPF3value);
	DBHelper::AddXRecord(scopeId, _T("SPF4"), sSPF4value);
	DBHelper::AddXRecord(scopeId, _T("SPF5"), sSPF5value);
	DBHelper::AddXRecord(scopeId, _T("H"), sHvalue);
	DBHelper::AddXRecord(scopeId, _T("HT"), sHTvalue);
	DBHelper::AddXRecord(scopeId, _T("实体"), _T("地库范围线"));
	bool re = DBHelper::AddXRecord(scopeId, _T("外轮廓图层名"), ms_sOutLineLayerName);
	DBHelper::AddXRecord(scopeId, _T("剪力墙图层名"), ms_sShearWallLayeNmae);
	CEquipmentroomTool::setEntToLayer(scopeId, sScopeLayer);
}

void COperaMultiSchemeShow::pillarShow(const AcGePoint2dArray& onePillarPts, AcDbObjectId& pillarId, AcDbDatabase *pDb/*= acdbCurDwg()*/)
{
	CString sPillarLayer(CEquipmentroomTool::getLayerName("column").c_str());
	CEquipmentroomTool::creatLayerByjson("column", pDb);
	AcDbPolyline *pPoly = new AcDbPolyline(onePillarPts.length());
	double width = 0;//线宽
	for (int i = 0; i < onePillarPts.length(); i++)
	{
		pPoly->addVertexAt(0, onePillarPts[i], 0, width, width);
	}
	pPoly->setClosed(true);
	DBHelper::AppendToDatabase(pillarId, pPoly, pDb);
	pPoly->close();
	DBHelper::AddXRecord(pillarId, _T("实体"), _T("方柱"));
	CEquipmentroomTool::setEntToLayer(pillarId, sPillarLayer);
}

void COperaMultiSchemeShow::arrowShow(const AcGePoint2dArray& oneArrowPts, AcDbObjectId& arrowId, AcDbDatabase *pDb/*= acdbCurDwg()*/)
{
	CString sArrowLayer(CEquipmentroomTool::getLayerName("arrow").c_str());
	CEquipmentroomTool::creatLayerByjson("arrow", pDb);
	AcDbPolyline *pPoly = new AcDbPolyline(oneArrowPts.length());
	double width = 0;//线宽
	for (int i = 0; i < oneArrowPts.length(); i++)
	{
		pPoly->addVertexAt(0, oneArrowPts[i], 0, width, width);
	}
	pPoly->setClosed(true);
	DBHelper::AppendToDatabase(arrowId, pPoly, pDb);
	pPoly->close();
	DBHelper::AddXRecord(arrowId, _T("实体"), _T("车道箭头"));
	CEquipmentroomTool::setEntToLayer(arrowId, sArrowLayer);
}

void COperaMultiSchemeShow::setLandDismensions(double m_dDis, const AcDbObjectIdArray& RoadLineIds, AcDbObjectIdArray& laneDimIds, AcDbDatabase *pDb/*= acdbCurDwg()*/)
{
	CCommonFuntion::creatLaneGridDimensionsDimStyle(_T("车道轴网尺寸标注样式"), pDb);//创建新的标注样式
	CString sLanesDimLayer(CEquipmentroomTool::getLayerName("dimensions_dimensiontext").c_str());
	CEquipmentroomTool::creatLayerByjson("dimensions_dimensiontext", pDb);
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
			double length = startpoint.distanceTo(endpoint);
			if (length<10)
			{
				pEnt->close();
				continue;
			}
			AcGePoint3d centerpoint = AcGePoint3d((startpoint.x + endpoint.x) / 2, (startpoint.y + endpoint.y) / 2, (startpoint.z + endpoint.z) / 2);

			AcGeVector3d movevec = AcGeVector3d(startpoint - endpoint);
			movevec.normalize();
			AcGePoint3d pt = centerpoint;
			pt.transformBy(movevec * 800);

			AcGeVector3d tempVec = AcGeVector3d(startpoint - endpoint);
			AcGeVector3d vec = tempVec.rotateBy(ARX_PI / 2, AcGeVector3d(0, 0, 1));
			vec.normalize();

			AcGePoint3d tempPoint = pt;
			AcGePoint3d movePt1 = tempPoint.transformBy(vec * (m_dDis / 2));
			tempPoint = pt;
			AcGePoint3d movePt2 = tempPoint.transformBy(-vec * (m_dDis / 2));

			CString disText;
			int iDistance = ceil(m_dDis);
			disText.Format(_T("%d"), iDistance);
	
			AcDbObjectId dimId;
			dimId = createDimAligned(movePt1, movePt2, centerpoint, disText, pDb);
			DBHelper::AddXRecord(dimId, _T("实体"), _T("车道标注"));
			CEquipmentroomTool::setEntToLayer(dimId, sLanesDimLayer);
			laneDimIds.append(dimId);
			pLine->close();
		}
		pEnt->close();
	}
}

AcDbObjectId COperaMultiSchemeShow::createDimAligned(const AcGePoint3d& pt1, const AcGePoint3d& pt2, const AcGePoint3d& ptLine, const ACHAR* dimText, AcDbDatabase *pDb/*= acdbCurDwg()*/)
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
	DBHelper::AppendToDatabase(dimensionId, pnewdim, pDb);

	if (pnewdim)
		pnewdim->close();

	// 打开已经创建的标注，对文字的位置进行修改
	AcDbEntity *pEnt = NULL;
	Acad::ErrorStatus es = acdbOpenAcDbEntity(pEnt, dimensionId, AcDb::kForWrite);

	AcDbAlignedDimension *pDimension = AcDbAlignedDimension::cast(pEnt);
	if (pDimension != NULL)
	{
		// 移动文字位置前，设置文字和尺寸线移动时的关系（这里指定为：尺寸线不动，在文字和尺寸线之间加箭头）
		pDimension->setDimtmove(1);

		pDimension->setDimblk(_T("_OPEN"));//设置箭头的形状为建筑标记
		pDimension->setDimexe(300);//设置尺寸界线超出尺寸线距离为400
		pDimension->setDimexo(500);//设置尺寸界线的起点偏移量为300
								 //pDimension->setDimtad(50);//文字位于标注线的上方
		pDimension->setDimtix(0);//设置标注文字始终绘制在尺寸界线之间
		pDimension->setDimtxt(700);//标注文字的高度
		pDimension->setDimdec(2);
		pDimension->setDimasz(150);//箭头长度
		pDimension->setDimlfac(1);//比例因子

		AcCmColor suiceng;
		suiceng.setColorIndex(3);
		pDimension->setDimclrd(suiceng);//为尺寸线、箭头和标注引线指定颜色，0为随图层
		pDimension->setDimclre(suiceng);//为尺寸界线指定颜色。此颜色可以是任意有效的颜色编号

		/*AcCmColor textcolor;
		textcolor.setColorIndex(255);
		pDimension->setDimclrt(textcolor);*/
	}
	if (pEnt)
		pEnt->close();

	return dimensionId;
}

void COperaMultiSchemeShow::Ended()
{
}

bool COperaMultiSchemeShow::parsingParkingData(Json::Value& parkings, CString& sMsg, CString& blockName, AcDbDatabase *pDb /*= acdbCurDwg()*/)
{
	AcGePoint2dArray parkingPts;
	std::vector<double> parkingDirections;
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
	AcDbObjectIdArray parkingIds;
	for (int a = 0; a < parkingPts.length(); a++)
	{
		//double = (rotation/180)*Π(顺时针和逆时针)
		double rotation = (parkingDirections[a] / 180)*ARX_PI;
		AcDbObjectId parkingId;
		AcGePoint2d parkingShowPt = parkingPts[a];
		parkingShow(parkingId, parkingPts[a], rotation, blockName, pDb);
		parkingIds.append(parkingId);
	}
	if (!parkingIds.isEmpty())
	{
		return true;
	}
	else
	{
		sMsg = _T("车位生成失败！");
		return false;
	}

}

bool COperaMultiSchemeShow::parsingData(Json::Value& data, double& dParkingLength, double& dParkingWidth, double& dLaneWidth, CString& sMsg, std::map < std::string, double >& tableData)
{
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
			}
			return true;
		}
		else
		{
			sMsg = _T("回传json中[\"result\"][\"data\"]字段格式不匹配！");
			return false;
		}
	}
}

bool COperaMultiSchemeShow::parsingAxisData(Json::Value& axis, CString& sMsg, std::map<AcDbObjectId, AcString>& idAndNumMap,
	AcDbObjectIdArray& axisIds, AcDbDatabase *pDb /*= acdbCurDwg()*/)
{
	std::vector<AcGePoint2dArray> axisesPoints;
	std::vector<AcString> sNums;
	if (axis.isNull())
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
				std::string snum = axis[j]["no"].asString();
				AcString cs(snum.c_str());
				sNums.push_back(cs);
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
	}
	for (int b = 0; b < axisesPoints.size(); b++)
	{
		AcDbObjectId axisId;
		axisId = axisShow(axisesPoints[b], pDb);
		axisIds.append(axisId);
		std::pair<AcDbObjectId, AcString> value(axisId, sNums[b]);
		idAndNumMap.insert(value);//插入新元素
	}
	if (!axisIds.isEmpty())
	{
		return true;
	}
	else
	{
		sMsg = _T("生成轴线失败！");
		return false;
	}
}

bool COperaMultiSchemeShow::parsingLaneData(Json::Value& lane, CString& sMsg, AcDbObjectIdArray& RoadLineIds, AcDbDatabase *pDb /*= acdbCurDwg()*/)
{
	std::vector<AcGePoint2dArray> lanesPoints;
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
	for (int c = 0; c < lanesPoints.size(); c++)
	{
		RoadLineIds.append(laneShow(lanesPoints[c], pDb));
	}
	if (!RoadLineIds.isEmpty())
	{
		return true;
	}
	else
	{
		sMsg = _T("生成车道线失败！");
		return false;
	}
}

bool COperaMultiSchemeShow::parsingScopeData(Json::Value& scope, CString& sMsg, std::map < std::string, double >& tableData, AcDbDatabase *pDb /*= acdbCurDwg()*/)
{
	AcGePoint2dArray scopePts;
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
	AcDbObjectId scopeId;
	scopeShow(scopePts, scopeId, tableData, pDb);
	if (!scopeId.isNull())
	{
		return true;
	}
	else
	{
		sMsg = _T("生成地库范围线失败！");
		return false;
	}

}

bool COperaMultiSchemeShow::parsingArrowData(Json::Value& arrow, CString& sMsg, AcDbObjectIdArray& arrowIds, AcDbDatabase *pDb /*= acdbCurDwg()*/)
{
	std::vector<AcGePoint2dArray> arrowPoints;
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
	}
	for (int e = 0; e < arrowPoints.size(); e++)
	{
		AcDbObjectId arrowId;
		arrowShow(arrowPoints[e], arrowId, pDb);
		arrowIds.append(arrowId);
	}
	if (!arrowIds.isEmpty())
	{
		return true;
	}
	else
	{
		sMsg = _T("生成车道方向指示线失败！");
		return false;
	}
}

bool COperaMultiSchemeShow::parsingPillarData(Json::Value& pillar, CString& sMsg, AcDbObjectIdArray& pillarIds, AcDbDatabase *pDb /*= acdbCurDwg()*/)
{
	std::vector<AcGePoint2dArray> pillarPoints;
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
	for (int d = 0; d < pillarPoints.size(); d++)
	{
		AcDbObjectId pillarId;
		pillarShow(pillarPoints[d], pillarId, pDb);
		pillarIds.append(pillarId);
	}
	if (!pillarIds.isEmpty())
	{
		return true;
	}
	else
	{
		sMsg = _T("生成方柱失败！");
		return false;
	}
}

bool COperaMultiSchemeShow::parsingBlanksData(Json::Value& blanks, CString& sMsg, AcDbObjectIdArray& blankIds, AcDbDatabase *pDb /*= acdbCurDwg()*/)
{
	std::vector<AcGePoint2dArray> blanksPoints;
	if (blanks.isNull())
	{
		return false;
		sMsg = _T("回传json中没有[\"result\"][\"blank\"]字段！");
	}
	else
	{
		if (blanks.isArray())
		{
			int nblanksSize = blanks.size();
			if (nblanksSize==0)
			{
				return true;
			}
			for (int k = 0; k < nblanksSize; k++)
			{
				AcGePoint2dArray oneBlankPts;
				if (blanks[k].isArray())
				{
					int oneblankSize = blanks[k].size();
					for (int g = 0; g < oneblankSize; g++)
					{
						double ptX = blanks[k][g][0].asDouble();
						double ptY = blanks[k][g][1].asDouble();
						AcGePoint2d tempPt(ptX, ptY);
						oneBlankPts.append(tempPt);
					}
				}
				blanksPoints.push_back(oneBlankPts);
			}
		}
		else
		{
			sMsg = _T("回传json中[\"result\"][\"blank\"]字段格式不匹配！");
			return false;
		}
	}
	for (int d = 0; d < blanksPoints.size(); d++)
	{
		AcDbObjectId blankId;
		//替换成云线生成api
		COperaCheck::blankCheckShow(blanksPoints[d], blankId, pDb);
		double dArea = GeHelper::CalcPolygonArea(blanksPoints[d]) / 1000000;
		CString sArea;
		sArea.Format(_T("%.2f"), dArea);
		Acad::ErrorStatus es = DBHelper::AddXRecord(blankId, _T("cloud_area"), sArea);
		CString cloudInf = _T("空白区域");
		es = DBHelper::AddXRecord(blankId, _T("cloud"), cloudInf);
		es = DBHelper::AddXRecord(blankId, _T("实体"), cloudInf);
		blankIds.append(blankId);
	}
	if (!blankIds.isEmpty())
	{
		return true;
	}
	else
	{
		sMsg = _T("生成空白区云线失败！");
		return false;
	}
}

double COperaMultiSchemeShow::getPicAttributeValue(std::map<std::string, double>& picAttributedata, const std::string& picAttributeTage)
{
	//通过key找value
	if (picAttributedata.count(picAttributeTage) > 0)
		return picAttributedata[picAttributeTage];

	return 0;
}

void COperaMultiSchemeShow::checkLaneDimPosition(const AcDbObjectIdArray& laneDimIds, const AcDbObjectIdArray& arrowIds, AcDbDatabase *pDb /*= acdbCurDwg()*/)
{
	if (laneDimIds.isEmpty())
	{
		return;
	}
	m_linesel.init(arrowIds, pDb);
	for (int i = 0; i < laneDimIds.length(); i++)
	{
		AcDbEntity* pEntity = NULL;
		if (acdbOpenAcDbEntity(pEntity, laneDimIds[i], kForWrite) != eOk)							//这里是可读
		{
			continue;
		}
		if (pEntity->isKindOf(AcDbDimension::desc()))
		{
			//对标注进行操作
			AcDbDimension *pDim = AcDbDimension::cast(pEntity);
			AcGePoint3dArray p;//装取标注的5个点
			pDim->getStretchPoints(p);
			//测试看数据用
			AcGePoint3d pt0 = p[0];
			AcGePoint3d pt1 = p[1];
			AcGePoint3d pt2 = p[2];
			AcGePoint3d pt3 = p[3];
			AcGePoint3d pt4 = p[4];
			
			AcGeVector3d vec = pt1 - pt3;
			AcGeVector3d unit3dVec = vec.normalize();
			AcGeVector2d unitvecMove(unit3dVec.x, unit3dVec.y);
			AcGePoint2d startPt(pt2.x, pt2.y);
			AcGePoint2d endPt(pt3.x, pt3.y);
			startPt.transformBy(unitvecMove * 10);
			endPt.transformBy(unitvecMove * 10);

			AcDbObjectIdArray Ids = m_linesel.select(startPt, endPt);
			AcGeMatrix3d mat;
			mat.setTranslation(500*unit3dVec);
			if (Ids.length()>0)
			{
				pDim->transformBy(mat);
			}
			int stop = 0;
		}

		pEntity->close();
	}
}

CString COperaMultiSchemeShow::getCurTimeNum()
{
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
	return sNum;
}

REG_CMD_P(COperaMultiSchemeShow, BGY, MultiSchemeShow);//多方案排布展示