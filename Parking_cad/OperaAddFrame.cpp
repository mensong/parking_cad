#include "stdafx.h"
#include "OperaAddFrame.h"
#include "DBHelper.h"
#include "DlgAddFrame.h"
#include "GeHelper.h"
#include "EquipmentroomTool.h"

std::map<std::string, double> COperaAddFrame::ms_mapTableData;
CString COperaAddFrame::ms_sOutLineLayerName;


COperaAddFrame::COperaAddFrame(const AcString& group, const AcString& cmd, const AcString& alias, Adesk::Int32 cmdFlag)
	: CIOperaLog(group, cmd, alias, cmdFlag)
{
}

COperaAddFrame::~COperaAddFrame()
{
}

void COperaAddFrame::Start()
{
	std::vector<AcDbObjectId> ids = DBHelper::SelectEntityPrompt(_T("\n选择要生成图框的实体:"));
	if (ids.size() < 1)
		return;
	CString sParkingsLayer(CEquipmentroomTool::getLayerName("ordinary_parking").c_str());
	CString sEquipmentroomLayer(CEquipmentroomTool::getLayerName("equipmentroom").c_str());
	CString sCoreWall(CEquipmentroomTool::getLayerName("core_wall").c_str());
	CString sScope(CEquipmentroomTool::getLayerName("rangeline").c_str());
	double SPF1area = getPloyLineArea(ids, sEquipmentroomLayer);
	double CPvalue = getNumberOfCars(ids, sParkingsLayer);
	double SPvalue = getPloyLineArea(ids, sScope);
	/*---------------------------------------------------*/
	std::vector<AcGePoint2dArray> parkingsPoints;
	std::map<AcDbObjectId, AcGePoint2dArray> parkIdAndPts;
	CEquipmentroomTool::getParkingExtentPts(parkingsPoints,ids,sParkingsLayer, parkIdAndPts);
	std::vector<AcGePoint2dArray> equipmentPoints = getPlinePointForLayer(ids, sEquipmentroomLayer);
	if (ms_sOutLineLayerName.IsEmpty())
	{
		for (int size=0; size<ids.size();size++)
		{
			AcString sOutLineLayerName;
			DBHelper::GetXRecord(ids[size], _T("外轮廓图层名"), sOutLineLayerName);
			if (!sOutLineLayerName.isEmpty())
			{
				ms_sOutLineLayerName = sOutLineLayerName.constPtr();
				break;
			}
		}
	}
	std::vector<AcGePoint2dArray> outLinePoints = getPlinePointForLayer(ids, ms_sOutLineLayerName,false);
	double dOutLineArea = getPloyLineArea(ids, ms_sOutLineLayerName,false);
	double dCoreWallArea = getPloyLineArea(ids, sCoreWall);
	double dIntersectWithParkingArea = getIntersectionArea(outLinePoints, parkingsPoints);
	double dIntersectWithEquipmentaArea = getIntersectionArea(outLinePoints,equipmentPoints);
	double SPF2area = dOutLineArea - dCoreWallArea - dIntersectWithParkingArea - dIntersectWithEquipmentaArea;
	/*-----------------------------------------------------*/
	AcDbExtents extFrame;
	for (int i = 0; i < ids.size(); ++i)
	{
		AcDbExtents ext;
		Acad::ErrorStatus es = DBHelper::GetEntityExtents(ext, ids[i]);
		if (Acad::eOk == es)
			extFrame.addExt(ext);
	}

	CAcModuleResourceOverride resOverride;//资源定位
	CDlgAddFrame dlg;
	dlg.setContextExtents(extFrame);
	INT_PTR res = dlg.DoModal();
	if (res == IDOK)
	{
		dlg.setBigFramePoints();
		//"SP=4865|SPT=3740|SPF=1125|SPF1=210|SPF2=450|SPF3=463|CP=132|JSPC=25|SPC=33|H=3.55|HT=1"
		std::map<std::string, double> picAttributedata = ms_mapTableData;
		//测试数据
		/*picAttributedata["SP"] = 4865;
		picAttributedata["SPT"] = 3740;
		picAttributedata["SPF"] = 1125;
		picAttributedata["SPF1"] = 210;
		picAttributedata["SPF2"] = 450;
		picAttributedata["SPF3"] = 460;
		picAttributedata["CP"] = 132;
		picAttributedata["JSPC"] = 25;
		picAttributedata["SPC"] = 33;
		picAttributedata["H"] = 3.55;
		picAttributedata["HT"] = 1;*/
		std::string sPicAttributeText = setPicAttributeData(SPvalue, SPF1area, CPvalue, SPF2area, picAttributedata, ids);

		dlg.setBlockInserPoint(sPicAttributeText);
		std::vector<AcDbEntity*> vcEnts;
		AcDbPolyline* pFrame = new AcDbPolyline;
		pFrame->addVertexAt(0, dlg.mBigFramept0);
		pFrame->addVertexAt(1, dlg.mBigFramept1);
		pFrame->addVertexAt(2, dlg.mBigFramept2);
		pFrame->addVertexAt(3, dlg.mBigFramept3);
		pFrame->setClosed(Adesk::kTrue);
		pFrame->setColorIndex(255);
		vcEnts.push_back(pFrame);

		AcGePoint2d centerpt = AcGePoint2d((dlg.mBigFramept0.x + dlg.mBigFramept2.x) / 2, (dlg.mBigFramept0.y + dlg.mBigFramept2.y) / 2);

		AcDbPolyline* pOutermostFrame = new AcDbPolyline;
		pOutermostFrame->addVertexAt(0, GetChangePoint(centerpt, dlg.mBigFramept0));
		pOutermostFrame->addVertexAt(1, GetChangePoint(centerpt, dlg.mBigFramept1));
		pOutermostFrame->addVertexAt(2, GetChangePoint(centerpt, dlg.mBigFramept2));
		pOutermostFrame->addVertexAt(3, GetChangePoint(centerpt, dlg.mBigFramept3));
		pOutermostFrame->setClosed(Adesk::kTrue);
		pOutermostFrame->setColorIndex(255);
		vcEnts.push_back(pOutermostFrame);

		CString setblockname;
		if (!isHasBlockName(_T("图框"), setblockname))
			setblockname = _T("图框1");
		
		if (Acad::eOk != DBHelper::CreateBlock(setblockname, vcEnts))
		{
			acutPrintf(_T("\n创建图框图块失败！"));
		}
		
		for (int i=0; i<vcEnts.size(); ++i)
		{
			delete vcEnts[i];
		}

		AcDbObjectId idEnt;
		if (Acad::eOk == DBHelper::InsertBlkRef(idEnt, setblockname, AcGePoint3d(0, 0, 0)))
		{
			CString strPictureframeLayer(CEquipmentroomTool::getLayerName("pictureframe").c_str());
			CEquipmentroomTool::creatLayerByjson("pictureframe");
			CEquipmentroomTool::setEntToLayer(idEnt, strPictureframeLayer);
			//dlg.setBlokcLayer(strPictureframeLayer, idEnt);
		}
		else
		{
			acutPrintf(_T("\n生成图框失败"));
		}
	}
}

//bool COperaAddFrame::isHasBlockName(const AcString& blockname, AcString& outblockname)
//{
//	AcDbBlockTable *pBlockTable = NULL;
//	Acad::ErrorStatus es = acdbHostApplicationServices()->workingDatabase()->getBlockTable(pBlockTable, AcDb::kForRead);
//	if (Acad::eOk != es)
//	{
//		return es;
//	}
//
//	for (int i=1; i<100; i++)
//	{
//		CString tempblockname;
//		tempblockname.Format(_T("%s%d"),blockname,i);
//		if (Adesk::kTrue == pBlockTable->has(tempblockname))
//		   continue;
//
//		outblockname = tempblockname;
//
//		if (pBlockTable)
//			pBlockTable->close();
//
//		return true;
//	}
//	
//	return false;
//}

bool COperaAddFrame::isHasBlockName(const CString& blockname, CString& outblockname)
{
	AcDbBlockTable *pBlockTable = NULL;
	Acad::ErrorStatus es = acdbHostApplicationServices()->workingDatabase()->getBlockTable(pBlockTable, AcDb::kForRead);
	if (Acad::eOk != es)
	{
		return false;
	}

	for (int i=1; i<100; i++)
	{
		CString tempblockname;
		CString strCount;
		strCount.Format(_T("%d"),i);
		tempblockname = blockname + strCount;
		if (Adesk::kTrue == pBlockTable->has(tempblockname))
			continue;

		outblockname = tempblockname;

		if (pBlockTable)
			pBlockTable->close();
		return true;
	}
	return false;
}

AcGePoint2d COperaAddFrame::GetChangePoint(AcGePoint2d& centerpt, AcGePoint2d& changpt)
{
	AcGeVector2d vec(changpt - centerpt);
	vec.normalize();
	AcGePoint2d pt = changpt;
	pt.transformBy(vec * 4000);
	return pt;
}

std::vector<AcGePoint2dArray> COperaAddFrame::getPlinePointForLayer(std::vector<AcDbObjectId>& inputIds, const AcString& layerNameofEnty, bool isCreat/* = true*/)
{
	std::vector<AcGePoint2dArray> outputPoints;

	for (int i = 0; i < inputIds.size(); i++)
	{
		if (isCreat)
		{
			AcString usedFor;
			DBHelper::GetXRecord(inputIds[i], _T("实体"), usedFor);
			if (!usedFor.isEmpty())
			{
				AcDbEntity* pEntity = NULL;
				if (acdbOpenAcDbEntity(pEntity, inputIds[i], kForRead) != eOk)
					continue;

				CString layername = pEntity->layer();
				if (layername.Compare(layerNameofEnty) != 0)
				{
					if (pEntity)
						pEntity->close();
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

					if (onePlinePts.length() <= 1)
						continue;

					onePlinePts.append(onePlinePts[0]);
					outputPoints.push_back(onePlinePts);
				}
				pEntity->close();
			}
		}
		else
		{
			AcDbEntity* pEntity = NULL;
			if (acdbOpenAcDbEntity(pEntity, inputIds[i], kForRead) != eOk)
				continue;

			CString layername = pEntity->layer();
			if (layername.Compare(layerNameofEnty) != 0)
			{
				if (pEntity)
					pEntity->close();
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

				if (onePlinePts.length() <= 1)
					continue;

				onePlinePts.append(onePlinePts[0]);
				outputPoints.push_back(onePlinePts);
			}
			pEntity->close();
		}
		
	}
	return outputPoints;
}

double COperaAddFrame::getPloyLineArea(std::vector<AcDbObjectId>& inputIds, const AcString& layerNameofEnty, bool isCreat/* = true*/)
{
	std::vector<AcGePoint2dArray> vectPoints;
	if (isCreat)
	{
		vectPoints = getPlinePointForLayer(inputIds, layerNameofEnty,true);
	}
	else
	{
		vectPoints = getPlinePointForLayer(inputIds, layerNameofEnty,false);
	}
	double totalarea = 0;
	for (int i = 0; i < vectPoints.size(); i++)
	{
		double area = GeHelper::CalcPolygonArea(vectPoints[i]);

		totalarea += area;
	}

	return totalarea;

}

double COperaAddFrame::getNumberOfCars(std::vector<AcDbObjectId>& inputIds, const AcString& layerNameofEnty)
{
	int num = 0;
	AcDbBlockReference* pBlkRef = NULL;

	for (int i = 0; i < inputIds.size(); i++)
	{
		
		if (acdbOpenObject(pBlkRef, inputIds[i], kForRead) != eOk)
			continue;

		CString layername = pBlkRef->layer();
		if (layername.Compare(layerNameofEnty) == 0)
		{
			num++;

			if (pBlkRef)
				pBlkRef->close();
			continue;
		}
		else
		{
			if (pBlkRef)
				pBlkRef->close();
		}
	}

	return (double)num;
}

std::string COperaAddFrame::setPicAttributeData(double SPvalue, double SPF1value, double CPvalue, double SPF2value, std::map<std::string, double>& picAttributedata, std::vector<AcDbObjectId>& inputIds)
{
	double SPF3value = 0;
	double SPF4value = 0;
	double SPF5value = 0;
	double Hvalue = 0;
	double HTvalue = 0;
	for (int i = 0; i < inputIds.size(); i++)
	{
		AcString XRecordText;
		DBHelper::GetXRecord(inputIds[i], _T("实体"), XRecordText);
		if (XRecordText == _T("地库范围线"))
		{
			AcString sSPF3value;
			DBHelper::GetXRecord(inputIds[i], _T("SPF3"), sSPF3value);
			SPF3value = _tstof(sSPF3value);
			AcString sSPF4value;
			DBHelper::GetXRecord(inputIds[i], _T("SPF4"), sSPF4value);
			SPF4value = _tstof(sSPF4value);
			AcString sSPF5value;
			DBHelper::GetXRecord(inputIds[i], _T("SPF5"), sSPF5value);
			SPF5value = _tstof(sSPF5value);
			AcString sHvalue;
			DBHelper::GetXRecord(inputIds[i], _T("H"), sHvalue);
			Hvalue = _tstof(sHvalue);
			AcString sHTvalue;
			DBHelper::GetXRecord(inputIds[i], _T("HT"), sHTvalue);
			HTvalue = _tstof(sHTvalue);
		}
	}
	//double SPF3value = getPicAttributeValue(picAttributedata, "SPF3");
	//double SPF4value = getPicAttributeValue(picAttributedata, "SPF4");
	//double SPF5value = getPicAttributeValue(picAttributedata, "SPF5");
	//double Hvalue = getPicAttributeValue(picAttributedata, "H");
	//double HTvalue = getPicAttributeValue(picAttributedata, "HT");

	double SPFvalue = (SPF1value / 1000000) + (SPF2value / 1000000) + SPF3value + SPF4value + SPF5value;
	double SPTvalue = (SPvalue/ 1000000) - SPFvalue;
	double JSPCvalue = SPTvalue / CPvalue;
	double SPCvalue = (SPTvalue + (SPF1value / 1000000) + (SPF2value/ 1000000)) / CPvalue;

	std::string sAttributeData = setStringData(SPvalue/ 1000000, "SP") + "|" + setStringData(SPTvalue, "SPT") + "|"
		+ setStringData(SPFvalue, "SPF") + "|" + setStringData((SPF1value / 1000000), "SPF1") + "|"
		+ setStringData(SPF2value/ 1000000, "SPF2") + "|" + setStringData(SPF3value, "SPF3") + "|"
		+ setStringData(CPvalue, "CP") + "|" + setStringData(JSPCvalue, "JSPC") + "|"
		+ setStringData(SPCvalue, "SPC") + "|" + setStringData(Hvalue, "H") + "|" + setStringData(HTvalue, "HT");

	return sAttributeData;
}

std::string COperaAddFrame::setStringData(double inputValue, const std::string& Keydata)
{
	char strValue[100];
	memset(strValue, 0x00, sizeof(strValue));
	sprintf(strValue, "%s=%0.2f", Keydata, inputValue);

	return strValue;
}

double COperaAddFrame::getPicAttributeValue(std::map<std::string, double>& picAttributedata, const std::string& picAttributeTage)
{
	//通过key找value
	if (picAttributedata.count(picAttributeTage) > 0)
		return picAttributedata[picAttributeTage];

	return 0;
}


void COperaAddFrame::setTableDataMap(const std::map<std::string, double>& tableData)
{
	ms_mapTableData = tableData;
}

void COperaAddFrame::setOutLineLayerName(const CString& sOutLineLayerName)
{
	ms_sOutLineLayerName = sOutLineLayerName;
}

double COperaAddFrame::getIntersectionArea(const std::vector<AcGePoint2dArray>& targetPlinePts, const std::vector<AcGePoint2dArray>& usePlinePts)
{
	double dTotalArea = 0;
	for (int i=0; i<targetPlinePts.size();i++)
	{
		AcGePoint2dArray targetPts = targetPlinePts[i];
		double doneTargetAllIntersectArea = 0;
		for (int j=0; j<usePlinePts.size(); j++)
		{
			std::vector<AcGePoint2dArray> polyIntersections;
			GeHelper::GetIntersectionOfTwoPolygon(targetPts, usePlinePts[j], polyIntersections);
			if (polyIntersections.size()!=0)
			{
				double OnePlineAllIntersectArea = 0;
				for (int x=0; x<polyIntersections.size();x++)
				{
					 double dOneIntersectArea = GeHelper::CalcPolygonArea(polyIntersections[x]);
					 OnePlineAllIntersectArea += dOneIntersectArea;
				}
				doneTargetAllIntersectArea += OnePlineAllIntersectArea;
			}
		}
		dTotalArea += doneTargetAllIntersectArea;
	}
	return dTotalArea;
}

REG_CMD_P(COperaAddFrame, BGY, AddFrame);//增加图框