#include "stdafx.h"
#include "OperaAxleNetMaking.h"
#include <tchar.h>
#include "CommonFuntion.h"
#include <algorithm>
#include "GeHelper.h"
#include "EquipmentroomTool.h"
#include "DBHelper.h"
#include "OperaZoomAxisNumber.h"
#include "MinimumRectangle.h"
#ifndef PI
#define PI 3.1415926535898
#endif


REG_CMD_P(COperaAxleNetMaking, BGY, ANM);

AcDbObjectIdArray COperaAxleNetMaking::ms_axisIds;

COperaAxleNetMaking::COperaAxleNetMaking(const AcString& group, const AcString& cmd, const AcString& alias, Adesk::Int32 cmdFlag)
	: CIOperaLog(group, cmd, alias, cmdFlag)
{
}


COperaAxleNetMaking::~COperaAxleNetMaking()
{
}

#if 0
void COperaAxleNetMaking::Start()
{
	CString sAxisLayer(CEquipmentroomTool::getLayerName("parking_axis").c_str());
	const ACHAR *layername = sAxisLayer;//轴线所在图层

	AcDbObjectIdArray LineIds = ms_axisIds;
	if (LineIds.length() == 0)
	{
		acutPrintf(_T("\n未获取到轴线信息"));
		return;
	}

	//分批存储轴线
	std::vector<std::vector<AcDbObjectId>> outputId;
	CCommonFuntion::BatchLine(LineIds, outputId);

	for (int i = 0; i < outputId.size(); i++)
	{
		AcDbObjectIdArray Ids;
		for (int k = 0; k < outputId[i].size(); k++)
			Ids.append(outputId[i][k]);

		if (Ids.length() <= 0)
			continue;

		AcDbExtents exts = CCommonFuntion::GetBigExtents(Ids);

		AcGePoint3d lefdownPt = exts.minPoint();
		AcGePoint3d lefupPt = AcGePoint3d(exts.minPoint().x, exts.maxPoint().y, 0);
		AcGePoint3d rigthupPt = exts.maxPoint();
		AcGePoint3d rigthdownPt = AcGePoint3d(exts.maxPoint().x, exts.minPoint().y, 0);

		AcDbEntity *pEnt = NULL;
		acdbOpenObject(pEnt, Ids[0], AcDb::kForRead);
		AcDbLine *line = AcDbLine::cast(pEnt);
		AcGePoint3d startpt;
		AcGePoint3d endptpt;
		line->getStartPoint(startpt);
		line->getEndPoint(endptpt);
		if (line)
			line->close();
		pEnt->close();

		if (CCommonFuntion::IsParallel(lefupPt, rigthupPt, startpt, endptpt))
		{
			AcDbLine *Line = new AcDbLine(lefupPt, rigthupPt);
			AcDbObjectId Lineid = CCommonFuntion::PostToModelSpace(Line);
			if (Line)
				Line->close();

			//容器加结构体的形式来存储轴线与辅助线之间的距离和轴线的id,为后面对轴线的排序做准备
			std::vector<double> distancevet;
			std::vector<LineDistance> LDstructVet;
			for (int m = 0; m < Ids.length(); m++)
			{
				double dis = CCommonFuntion::GetLineDistance(Lineid, Ids[m]);
				LineDistance LDstruct;
				LDstruct.Lineid = Ids[m];
				LDstruct.distance = dis;
				LDstructVet.push_back(LDstruct);
				distancevet.push_back(dis);
			}

			//删除辅助线
			CCommonFuntion::DeleteEnt(Lineid);

			//对距离排序
			std::sort(distancevet.begin(), distancevet.end());

			//按照排序好的距离，将结构体里对应的id，按照顺序存续在sortids，这样轴线的排序已完成
			AcDbObjectIdArray sortIds;
			for (int m = 0; m < distancevet.size(); m++)
			{
				for (int j = 0; j < LDstructVet.size(); j++)
				{
					if (LDstructVet[j].distance == distancevet[m])
					{
						sortIds.append(LDstructVet[j].Lineid);
						break;
					}
				}
			}

			//轴号标注
			COperaAxleNetMaking::inserAadAxleNum(sortIds, lefupPt, lefdownPt, 1, 1);

			CCommonFuntion::creatLaneGridDimensionsDimStyle(_T("车道轴网尺寸标注样式"));

			//这里设置轴网标注所在图层
			CString setlayernameofAXSI(CEquipmentroomTool::getLayerName("axis_dimensions").c_str());
			CEquipmentroomTool::creatLayerByjson("axis_dimensions", pDb);
			//CCommonFuntion::setLayer(setlayernameofAXSI, 255);

			//初次处理四边的轴线，标注
			std::vector<AcDbObjectId> outIds_1;
			COperaAxleNetMaking::InitialProcessing(setlayernameofAXSI, sortIds, lefupPt, lefdownPt, outIds_1);
			std::vector<AcDbObjectId> outIds_2;
			COperaAxleNetMaking::InitialProcessing(setlayernameofAXSI, sortIds, rigthdownPt, rigthupPt, outIds_2);

			//还没处理的轴线
			AcDbObjectIdArray specaildealIds = FilterIds(sortIds, outIds_1, outIds_2);

			//对还没处理的轴线，进行阈值判断来标志
			AcDbObjectIdArray specialoutputIds_1;
			if (specaildealIds.length() >= 2)
				SpecialDeal(setlayernameofAXSI, specaildealIds, lefupPt, lefdownPt, rigthdownPt, rigthupPt, layername);


		}
		else
		{
			AcDbLine *Line = new AcDbLine(lefupPt, lefdownPt);
			AcDbObjectId Lineid = CCommonFuntion::PostToModelSpace(Line);
			AcDbObjectIdArray LeftIds;

			std::vector<double> distancevet;
			std::vector<LineDistance> LDstructVet;

			for (int m = 0; m < Ids.length(); m++)
			{
				double dis = CCommonFuntion::GetLineDistance(Lineid, Ids[m]);
				LineDistance LDstruct;
				LDstruct.Lineid = Ids[m];
				LDstruct.distance = dis;
				LDstructVet.push_back(LDstruct);
				distancevet.push_back(dis);
			}
			if (Line)
				Line->close();
			CCommonFuntion::DeleteEnt(Lineid);

			std::sort(distancevet.begin(), distancevet.end());


			AcDbObjectIdArray tempsortIds;
			for (int m = 0; m < distancevet.size(); m++)
			{
				for (int j = 0; j < LDstructVet.size(); j++)
				{
					if (LDstructVet[j].distance == distancevet[m])
						tempsortIds.append(LDstructVet[j].Lineid);
				}
			}

			AcDbObjectIdArray sortIds;
			for (int k = tempsortIds.length() - 1; k >= 0; --k)
				sortIds.append(tempsortIds[k]);
			COperaAxleNetMaking::inserAadAxleNum(sortIds, rigthupPt, lefupPt, 2, -1);

			CCommonFuntion::creatLaneGridDimensionsDimStyle(_T("车道轴网尺寸标注样式"));

			//这里设置轴网标注所在图层
			CString setlayernameofAXSI(CEquipmentroomTool::getLayerName("axis_dimensions").c_str());
			CEquipmentroomTool::creatLayerByjson("axis_dimensions", pDb);
			//CCommonFuntion::setLayer(setlayernameofAXSI,0);

			std::vector<AcDbObjectId> outIds_1;
			COperaAxleNetMaking::InitialProcessing(setlayernameofAXSI, sortIds, rigthupPt, lefupPt, outIds_1);
			std::vector<AcDbObjectId> outIds_2;
			COperaAxleNetMaking::InitialProcessing(setlayernameofAXSI, sortIds, lefdownPt, rigthdownPt, outIds_2);

			AcDbObjectIdArray specaildealIds = FilterIds(sortIds, outIds_1, outIds_2);

			AcDbObjectIdArray specialoutputIds_1;
			if (specaildealIds.length() >= 2)
				SpecialDeal(setlayernameofAXSI, specaildealIds, rigthupPt, lefupPt, lefdownPt, rigthdownPt, layername);
		}
	}
}

#endif

void COperaAxleNetMaking::Start()
{
	//AcDbObjectIdArray LineIds = ms_axisIds;
	COperaZoomAxisNumber::deleteEntitys(_T("A-标注-尺寸标注"));
	COperaZoomAxisNumber::deleteEntitys(_T("A-标注-轴号"));
	AcDbObjectIdArray LineIds = CCommonFuntion::GetAllEntityId(_T("A-轴线-车库"));
	if (LineIds.length() == 0)
	{
		acutPrintf(_T("\n未获取到轴线信息"));
		return;
	}

	int tagnum = 0;
	int addtagnum = 0;
	int num = 0;//横向编号
	CString tagvalue;//轴号属性值
	CString csAddtagvalue = _T("");
	std::map<AcDbObjectId, AcString> AxisNumberMap;
	for (int i = 0; i < LineIds.length(); ++i)
	{
		COperaAxleNetMaking::LongitudinalNumbering(tagnum, addtagnum, tagvalue, csAddtagvalue);
		AxisNumberMap[LineIds[i]] = tagvalue;
	}

	COperaAxleNetMaking::drawAxisNumber(LineIds, AxisNumberMap);
}

void COperaAxleNetMaking::drawAxisNumber(AcDbObjectIdArray& LineIds, std::map<AcDbObjectId, AcString>& AxisNumberMap, AcDbDatabase *pDb /*= acdbCurDwg()*/)
{
	for (int i = 0; i < LineIds.length(); ++i)
	{
		AcString keyvalue = COperaAxleNetMaking::getTagvaluefromMap(AxisNumberMap, LineIds[i]).constPtr();
		DBHelper::AddXRecord(LineIds[i], _T("轴号"), keyvalue);

		CString batchNum;

	}

	CString sAxisLayer(CEquipmentroomTool::getLayerName("parking_axis").c_str());
	const ACHAR *layername = sAxisLayer;//轴线所在图层

	//分批存储轴线
	std::vector<std::vector<AcDbObjectId>> outputId;
	CCommonFuntion::BatchLine(LineIds, outputId, 1);

	CCommonFuntion::creatLaneGridDimensionsDimStyle(_T("车道轴网尺寸标注样式"), pDb);

	for (int i = 0; i < outputId.size(); i++)
	{
		AcDbObjectIdArray lineids;
		for (int k = 0; k < outputId[i].size(); ++k)
			lineids.append(outputId[i][k]);

		AcGePoint3dArray pts = COperaZoomAxisNumber::getLinesPoints(lineids);
		AcGePoint3dArray fectanglepts = CMinimumRectangle::getMinRact(pts);
		if (fectanglepts.length() != 4)
			continue;

		acutPrintf(_T("\n最小矩形："));
		for (int kk = 0; kk < fectanglepts.length(); ++kk)
		{
			double x = fectanglepts[kk].x;
			double y = fectanglepts[kk].y;
			double z = fectanglepts[kk].z;

		}

		//容器加结构体的形式来存储轴线与辅助线之间的距离和轴线的id,为后面对轴线的排序做准备
		std::vector<double> distancevet;
		std::vector<LineDistance> LDstructVet;
		for (int m = 0; m < lineids.length(); m++)
		{
			double dis = COperaZoomAxisNumber::getMinDis(fectanglepts[0], lineids[m]);
			LineDistance LDstruct;
			LDstruct.Lineid = lineids[m];
			LDstruct.distance = dis;
			LDstructVet.push_back(LDstruct);
			distancevet.push_back(dis);
		}

		//对距离排序
		std::sort(distancevet.begin(), distancevet.end());

		//按照排序好的距离，将结构体里对应的id，按照顺序存续在sortids，这样轴线的排序已完成
		AcDbObjectIdArray sortIds;
		for (int m = 0; m < distancevet.size(); m++)
		{
			for (int j = 0; j < LDstructVet.size(); j++)
			{
				if (LDstructVet[j].distance == distancevet[m])
				{
					sortIds.append(LDstructVet[j].Lineid);
					break;
				}
			}
		}

		AcDbEntity *pEnt = NULL;
		AcGePoint3d startpt;
		AcGePoint3d endptpt;
		for (int tempnum = 0; tempnum < sortIds.length(); tempnum++)
		{
			if (acdbOpenObject(pEnt, sortIds[tempnum], AcDb::kForRead) != eOk)
				continue;
			AcDbLine *line = AcDbLine::cast(pEnt);
			line->getStartPoint(startpt);
			line->getEndPoint(endptpt);
			if (line)
				line->close();
			break;
		}
		if (pEnt)
			pEnt->close();

		AcGePoint3d lefdownPt = fectanglepts[0];
		AcGePoint3d rigthdownPt = fectanglepts[1];
		AcGePoint3d rigthupPt = fectanglepts[2];
		AcGePoint3d lefupPt = fectanglepts[3];

		//这里设置轴网标注所在图层
		CString setlayernameofAXSI(CEquipmentroomTool::getLayerName("dimensions_dimensiontext").c_str());
		CEquipmentroomTool::creatLayerByjson("dimensions_dimensiontext", pDb);

		if (CCommonFuntion::IsParallel(lefdownPt, rigthdownPt, startpt, endptpt, 1))
		{
			AcDbObjectIdArray tempsoritIds = sortIds;

			//轴号标注
			AcDbObjectIdArray getids;
			int len = 0;
			do
			{
				getids = COperaAxleNetMaking::inserAadAxleNum(sortIds, rigthdownPt, rigthupPt, 1, pDb);
				len = sortIds.length();

				sortIds.removeAll();
				sortIds = getids;
			} while (getids.length() >= 2 && getids.length() != len && COperaAxleNetMaking::getChangPoints(getids, rigthdownPt, rigthupPt) == 1);
			

			sortIds.removeAll();
			sortIds = tempsoritIds;
			AcDbObjectIdArray specaildealIds;
			getChangPoints(sortIds, lefdownPt, rigthdownPt, rigthupPt, lefupPt);
			do 
			{
			
			//初次处理四边的轴线，标注
			std::vector<AcDbObjectId> outIds_1;
			COperaAxleNetMaking::InitialProcessing(setlayernameofAXSI, sortIds, rigthdownPt, rigthupPt, outIds_1, pDb);
			//std::vector<AcDbObjectId> outIds_2;
			//COperaAxleNetMaking::InitialProcessing(setlayernameofAXSI, sortIds, lefdownPt, lefupPt, outIds_2, pDb);
			len = sortIds.length();

			//还没处理的轴线
			specaildealIds = FilterIds(sortIds, outIds_1);
			sortIds.removeAll();
			sortIds = specaildealIds;
			} while (specaildealIds.length() >= 2 && specaildealIds.length() != len && getChangPoints(specaildealIds, rigthdownPt, rigthupPt)==1);
		
			sortIds.removeAll();
			sortIds = tempsoritIds;
			getChangPoints(sortIds, lefdownPt, rigthdownPt, rigthupPt, lefupPt);
			do
			{

				//初次处理四边的轴线，标注
				//std::vector<AcDbObjectId> outIds_1;
				//COperaAxleNetMaking::InitialProcessing(setlayernameofAXSI, sortIds, rigthdownPt, rigthupPt, outIds_1, pDb);
				std::vector<AcDbObjectId> outIds_2;
				COperaAxleNetMaking::InitialProcessing(setlayernameofAXSI, sortIds, lefdownPt, lefupPt, outIds_2, pDb);
				len = sortIds.length();

				//还没处理的轴线
				specaildealIds = FilterIds(sortIds, outIds_2);
				sortIds.removeAll();
				sortIds = specaildealIds;
			} while (specaildealIds.length() >= 2 && specaildealIds.length() != len && getChangPoints(specaildealIds, lefdownPt, lefupPt) == 1);


			//对还没处理的轴线，进行阈值判断来标志
			//AcDbObjectIdArray specialoutputIds_1;
			/*if (specaildealIds.length() >= 2)
				SpecialDeal(setlayernameofAXSI, specaildealIds, rigthdownPt, rigthupPt, lefdownPt, lefupPt, layername, pDb);*/
		}
		else
		{
			AcDbObjectIdArray tempsoritIds = sortIds;
			AcDbObjectIdArray getids;
			int len = 0;
			do
			{
				getids = COperaAxleNetMaking::inserAadAxleNum(sortIds, lefdownPt, rigthdownPt, 1, pDb);
				len = sortIds.length();

				sortIds.removeAll();
				sortIds = getids;
			} while (getids.length() >= 2 && getids.length() != len && COperaAxleNetMaking::getChangPoints(getids, lefdownPt, rigthdownPt) == 1);
			
			
			sortIds.removeAll();
			sortIds = tempsoritIds;
			AcDbObjectIdArray specaildealIds;
			getChangPoints(sortIds, lefdownPt, rigthdownPt, rigthupPt, lefupPt);
			do 
			{
			
			std::vector<AcDbObjectId> outIds_1;
			COperaAxleNetMaking::InitialProcessing(setlayernameofAXSI, sortIds, lefdownPt, rigthdownPt, outIds_1, pDb);
			//std::vector<AcDbObjectId> outIds_2;
		//	COperaAxleNetMaking::InitialProcessing(setlayernameofAXSI, sortIds, rigthupPt, lefupPt, outIds_2, pDb);
			len = sortIds.length();

			specaildealIds = FilterIds(sortIds, outIds_1);
			sortIds.removeAll();
			sortIds = specaildealIds;
			} while (specaildealIds.length() >= 2 && specaildealIds.length()!= len && getChangPoints(specaildealIds, lefdownPt, rigthdownPt) == 1);
			
			sortIds.removeAll();
			sortIds = tempsoritIds;
			getChangPoints(sortIds, lefdownPt, rigthdownPt, rigthupPt, lefupPt);
			do
			{

				//std::vector<AcDbObjectId> outIds_1;
				//COperaAxleNetMaking::InitialProcessing(setlayernameofAXSI, sortIds, lefdownPt, rigthdownPt, outIds_1, pDb);
				std::vector<AcDbObjectId> outIds_2;
				COperaAxleNetMaking::InitialProcessing(setlayernameofAXSI, sortIds, rigthupPt, lefupPt, outIds_2, pDb);
				len = sortIds.length();

				specaildealIds = FilterIds(sortIds, outIds_2);
				sortIds.removeAll();
				sortIds = specaildealIds;
			} while (specaildealIds.length() >= 2 && specaildealIds.length() != len && getChangPoints(specaildealIds, rigthupPt, lefupPt) == 1);


			/*AcDbObjectIdArray specialoutputIds_1;
			if (specaildealIds.length() >= 2)
				SpecialDeal(setlayernameofAXSI, specaildealIds, lefdownPt, rigthdownPt, rigthupPt, lefupPt, layername, pDb);*/
		}
	}
}

void COperaAxleNetMaking::InitialProcessing(const CString& layerNameofAXSI, AcDbObjectIdArray& sortIds, AcGePoint3d& inputstartpt, AcGePoint3d& inputendpt
	, std::vector<AcDbObjectId>& outputIds, AcDbDatabase *pDb /*= acdbCurDwg()*/)
{

	AcDbEntity *pEnt = NULL;
	AcDbEntity *pTempEnt = NULL;
	AcDbLine *TempLine = new AcDbLine(inputstartpt, inputendpt);

	for (int m = 0; m < sortIds.length() - 1; m++)
	{
		if (Acad::eOk != acdbOpenObject(pEnt, sortIds[m], AcDb::kForRead))
			continue;

		AcGePoint3dArray interwithpoints;
		pEnt->intersectWith(TempLine, AcDb::kOnBothOperands, interwithpoints);
		if (interwithpoints.length() <= 0)
		{
			pEnt->close();
			continue;
		}

		for (int num = m + 1; num < sortIds.length(); num++)
		{
			if (Acad::eOk != acdbOpenObject(pTempEnt, sortIds[num], AcDb::kForRead))
				continue;

			AcGePoint3dArray interwithpointstemp;
			pTempEnt->intersectWith(TempLine, AcDb::kOnBothOperands, interwithpointstemp);
			if (interwithpointstemp.length() <= 0)
			{
				pTempEnt->close();
				continue;
			}

			AcGeVector3d vec = AcGeVector3d(interwithpointstemp[0] - interwithpoints[0]);

			CCommonFuntion::DrowDimaligned(layerNameofAXSI, interwithpoints[0], interwithpointstemp[0], pDb);
			if (std::find(outputIds.begin(), outputIds.end(), sortIds[m]) == outputIds.end())
				outputIds.push_back(sortIds[m]);
			if (std::find(outputIds.begin(), outputIds.end(), sortIds[num]) == outputIds.end())
				outputIds.push_back(sortIds[num]);
			if (pTempEnt)
				pTempEnt->close();
			break;
		}
		if (pEnt)
			pEnt->close();

	}
	if (TempLine)
		TempLine->close();
	if (outputIds.size() >= 3)
		COperaAxleNetMaking::DrowBigDimaligned(layerNameofAXSI, outputIds, inputstartpt, inputendpt, pDb);
}

void COperaAxleNetMaking::DrowBigDimaligned(const CString& layerNameofAXSI, std::vector<AcDbObjectId>& inputids, AcGePoint3d& inputstartpt, AcGePoint3d& inputendpt, AcDbDatabase *pDb /*= acdbCurDwg()*/)
{
	AcDbEntity *pEnt = NULL;
	AcDbEntity *pTempEnt = NULL;
	AcDbLine *TempLine = new AcDbLine(inputstartpt, inputendpt);

	AcGeVector2d targetvect = AcGeVector2d(inputendpt.x - inputstartpt.x, inputendpt.y - inputstartpt.y);
	double angle = ((int)((targetvect.angle()) * 100000000 + 0.5)) / 100000000.0;

	do
	{
		std::vector<AcDbObjectId> bigdimalignedids;
		if (acdbOpenObject(pEnt, inputids[0], AcDb::kForRead) != eOk)
			break;

		AcGePoint3dArray interwithpoints;
		pEnt->intersectWith(TempLine, AcDb::kOnBothOperands, interwithpoints);
		if (interwithpoints.length() <= 0)
			break;

		int num = inputids.size() - 1;
		if (acdbOpenObject(pTempEnt, inputids[num], AcDb::kForRead) != eOk)
			break;

		AcGePoint3dArray interwithpointstemp;
		pTempEnt->intersectWith(TempLine, AcDb::kOnBothOperands, interwithpointstemp);
		if (interwithpointstemp.length() <= 0)
			break;
		//根据辅助线确定最外围标注偏移
		AcGePoint3d pt;
		if (pTempEnt->isKindOf(AcDbLine::desc()))
		{
			AcDbLine *pLine = AcDbLine::cast(pTempEnt);
			AcGePoint3d startpt = pLine->startPoint();
			AcGePoint3d endpt = pLine->endPoint();
			if (startpt.distanceTo(interwithpointstemp[0]) <= endpt.distanceTo(interwithpointstemp[0]))
				pt = endpt;
			else	
				pt = startpt;
			
		}
		AcGeVector3d Linevec = (interwithpointstemp[0] - pt);
		Linevec.normalize();
		AcGePoint3d Pt1 = interwithpoints[0].transformBy(Linevec * 1500);
		AcGePoint3d Pt2 = interwithpointstemp[0].transformBy(Linevec * 1500);

		AcGeVector3d vec = AcGeVector3d(Pt2 - Pt1);
		CCommonFuntion::DrowDimaligned(layerNameofAXSI, Pt1, Pt2, pDb);
	} while (0);

	if (pTempEnt)
		pTempEnt->close();
	if (pEnt)
		pEnt->close();
	if (TempLine)
		TempLine->close();
}

AcDbObjectIdArray COperaAxleNetMaking::FilterIds(AcDbObjectIdArray& inputids, std::vector<AcDbObjectId>& inputids1, std::vector<AcDbObjectId>& inputids2)
{
	AcDbObjectIdArray specaildealIds;
	for (int num = 0; num < inputids.length(); num++)
	{
		if (std::find(inputids1.begin(), inputids1.end(), inputids[num]) != inputids1.end() && find(inputids2.begin(), inputids2.end(), inputids[num]) != inputids2.end())
			continue;
		else
			specaildealIds.append(inputids[num]);
	}
	return specaildealIds;
}

AcDbObjectIdArray COperaAxleNetMaking::FilterIds(AcDbObjectIdArray& inputtotalids, std::vector<AcDbObjectId>& inputids)
{
	AcDbObjectIdArray specaildealIds;
	for (int num = 0; num < inputtotalids.length(); num++)
	{
		if (std::find(inputids.begin(), inputids.end(), inputtotalids[num]) == inputids.end())
			specaildealIds.append(inputtotalids[num]);
	}
	return specaildealIds;
}

void COperaAxleNetMaking::AxisLineInfo(AcGePoint3d& inputstartpt, AcGePoint3d& inputendpt, AcGePoint3d& inputSecondstartpt, AcGePoint3d& inputSecondendpt, AcGeVector3d& projectvect_90, AcGeVector3d& projectvect_180,
	AcGePoint3d& startpt, AcGePoint3d& endpt, AcGePoint3d& outpt1, AcGePoint3d& outpt2, AcGePoint3d& outpoint)
{
	double startdis = CCommonFuntion::PointToSegDist(inputstartpt, inputendpt, startpt);
	double enddis = CCommonFuntion::PointToSegDist(inputstartpt, inputendpt, endpt);

	if (startdis <= enddis)
	{
		if (startdis <= CCommonFuntion::PointToSegDist(inputSecondstartpt, inputSecondendpt, endpt))
		{
			outpt1 = startpt;
			outpt1.transformBy(projectvect_90);
			outpt2 = startpt;
			outpt2.transformBy(projectvect_180);
			outpoint = startpt;
		}
		else
		{
			outpt1 = endpt;
			outpt1.transformBy(projectvect_90);
			outpt2 = endpt;
			outpt2.transformBy(projectvect_180);
			outpoint = endpt;
		}



	}
	else
	{
		if (enddis <= CCommonFuntion::PointToSegDist(inputSecondstartpt, inputSecondendpt, startpt))
		{
			outpt1 = endpt;
			outpt1.transformBy(projectvect_90);
			outpt2 = endpt;
			outpt2.transformBy(projectvect_180);
			outpoint = endpt;
		}
		else
		{
			outpt1 = startpt;
			outpt1.transformBy(projectvect_90);
			outpt2 = startpt;
			outpt2.transformBy(projectvect_180);
			outpoint = startpt;
		}
	}
}

void COperaAxleNetMaking::SpecialDeal(const CString& layerNameofAXSI, AcDbObjectIdArray& inputIds, AcGePoint3d& inputstartpt
	, AcGePoint3d& inputendpt, AcGePoint3d& inputSecondstartpt, AcGePoint3d& inputSecondendpt, const ACHAR *LineLayerName, AcDbDatabase *pDb /*= acdbCurDwg()*/)
{
	AcDbEntity *pEnt = NULL;
	AcDbEntity *pTempEnt = NULL;

	AcGeVector3d vect = AcGeVector3d(inputendpt - inputstartpt);
	AcGeVector3d projectvect_90 = vect.rotateBy(ARX_PI, AcGeVector3d(0, 0, 1));
	vect = AcGeVector3d(inputendpt - inputstartpt);
	AcGeVector3d projectvect_180 = vect.rotateBy(2 * ARX_PI, AcGeVector3d(0, 0, 1));

	std::vector<std::vector<AcDbObjectId>> outputids;

	for (int i = 0; i < inputIds.length(); i++)
	{

		if (Acad::eOk != acdbOpenObject(pEnt, inputIds[i], AcDb::kForRead))
			continue;

		std::vector<AcDbObjectId> tempvector;
		tempvector.push_back(inputIds[i]);

		AcDbLine *pEntLine = AcDbLine::cast(pEnt);
		AcGePoint3d startpt;
		AcGePoint3d endpt;
		pEntLine->getStartPoint(startpt);
		pEntLine->getEndPoint(endpt);

		AcGePoint3d selectLinept1;
		AcGePoint3d pt1;
		AcGePoint3d pt2;
		AxisLineInfo(inputstartpt, inputendpt, inputSecondstartpt, inputSecondendpt, projectvect_90, projectvect_180,
			startpt, endpt, pt1, pt2, selectLinept1);

		AcGePoint3d point1 = selectLinept1;
		AcGeLineSeg2d lineAssist(AcGePoint2d(pt1.x, pt1.y), AcGePoint2d(pt2.x, pt2.y));
		AcGePoint3d point2;

		for (int m = i + 1; m < inputIds.length(); m++)
		{
			if (Acad::eOk != acdbOpenObject(pTempEnt, inputIds[m], AcDb::kForRead))
				continue;

			AcDbLine *pTempEntLine = AcDbLine::cast(pTempEnt);
			AcGePoint3d tempstartpt;
			AcGePoint3d tempendpt;
			pTempEntLine->getStartPoint(tempstartpt);
			pTempEntLine->getEndPoint(tempendpt);

			AcGeLineSeg2d TemplineAssist(AcGePoint2d(tempstartpt.x, tempstartpt.y), AcGePoint2d(tempendpt.x, tempendpt.y));

			AcGeTol tol;
			tol.setEqualPoint(150);
			AcGePoint2d getpt;

			if (lineAssist.intersectWith(TemplineAssist, getpt, tol))
			{
				bool tag = true;
				for (int vectnum = 0; vectnum < outputids.size(); vectnum++)
				{
					if (std::find(outputids[vectnum].begin(), outputids[vectnum].end(), inputIds[i]) != outputids[vectnum].end())
					{
						if (std::find(outputids[vectnum].begin(), outputids[vectnum].end(), inputIds[m]) != outputids[vectnum].end())
						{
							tag = false;
							break;
						}
					}
					if (tag)
						break;
				}
				if (tag)
				{
					tempvector.push_back(inputIds[m]);
				}

				AcGePoint3d selectLinept2 = AcGePoint3d(getpt.x, getpt.y, 0);
				point2 = selectLinept2;
				if (CCommonFuntion::IsIntersectEnt(LineLayerName, selectLinept1, selectLinept2, AcDbLine::desc()))
				{
					pTempEnt->close();
					continue;

				}
				else
				{
					AcGeVector3d Dimalignedvec = AcGeVector3d(selectLinept1 - selectLinept2);
					CCommonFuntion::DrowDimaligned(layerNameofAXSI, selectLinept2, selectLinept1, pDb);
				}
			}
			else
			{
				pTempEnt->close();
				continue;
			}

			if (pTempEnt)
				pTempEnt->close();
		}

		if (tempvector.size() >= 3)
		{
			outputids.push_back(tempvector);
			COperaAxleNetMaking::DrowBigDimaligned(layerNameofAXSI, tempvector, point1, point2, pDb);
		}

		if (pEnt)
			pEnt->close();
	}
}

void COperaAxleNetMaking::setAxisIds(AcDbObjectIdArray axisIds)
{
	ms_axisIds = axisIds;
}

double COperaAxleNetMaking::getBlockRadius(AcDbObjectId& blockid, double scaleFactor, AcGePoint3d& basept)
{
	AcDbEntity *pEnt = NULL;
	double circleradius = 0;
	if (acdbOpenObject(pEnt, blockid, AcDb::kForWrite) == eOk)
	{
		//AcGeMatrix3d xform;
		//xform.setToScaling(scaleFactor, basept);
		//pEnt->transformBy(xform);

		AcDbVoidPtrArray pExps;
		AcDbEntity *pExpEnt = NULL;
		if (pEnt->explode(pExps) == Acad::eOk)
		{
			for (int i = 0; i < pExps.length(); i++)
			{
				pExpEnt = (AcDbEntity*)pExps[i];
				if (pExpEnt->isKindOf(AcDbCircle::desc()))
				{
					AcDbCircle *pcircle = AcDbCircle::cast(pExpEnt);
					circleradius = pcircle->radius();

					if (pcircle)
						pcircle->close();
					if (pExpEnt)
						pExpEnt->close();
					if (pEnt)
						pEnt->close();

					return circleradius;
				}
			}
		}

		if (pEnt)
			pEnt->close();
	}
	return circleradius;
}

void COperaAxleNetMaking::getLinePoint(AcDbObjectId& lineid, AcGePoint3d& startpt, AcGePoint3d& endpt)
{
	AcDbEntity *pEnt = NULL;
	if (acdbOpenObject(pEnt, lineid, AcDb::kForRead) == eOk)
	{
		AcDbLine *pLine = AcDbLine::cast(pEnt);
		startpt = pLine->startPoint();
		endpt = pLine->endPoint();
		if (pLine)
			pLine->close();
	}

	if (pEnt)
		pEnt->close();
}

AcGePoint3d COperaAxleNetMaking::getChangPoint(AcGePoint3d& basept, AcGeVector3d& moveagvec, double movedis)
{
	AcGePoint3d temppt = basept;
	moveagvec.normalize();
	temppt.transformBy(moveagvec*movedis);
	return temppt;
}

AcDbObjectIdArray COperaAxleNetMaking::inserAadAxleNum(AcDbObjectIdArray& sortIds, AcGePoint3d& inputstartpt, AcGePoint3d& inputendpt, double zoomcoefficient /*= 1*/, AcDbDatabase *pDb /*= acdbCurDwg()*/)
{
	AcDbObjectIdArray outids;
	CEquipmentroomTool::creatLayerByjson("axis_dimensions", pDb);//创建轴号所在图层

	CString sBlockName = _T("_AxleNumber");
	if (!CEquipmentroomTool::hasNameOfBlock(sBlockName))
	{
		std::set<AcString> setBlockNames;
		setBlockNames.insert(sBlockName);
		AcString filepath = (GetUserDir() + _T("template.dwg")).GetString();
		if (DBHelper::ImportBlkDef(filepath, setBlockNames, pDb) != Acad::eOk)
		{
			return outids;
		}
	}
	
		
	double scaleFactor = 150 * zoomcoefficient;// 折线长度
	double circleradius = 450 * zoomcoefficient;//圆半径
	double dis = (scaleFactor + circleradius)*sin(45 * PI / 180);//折线导致增加的移动距离

	AcDbObjectId LineId = COperaAxleNetMaking::DrowLine(inputstartpt, inputendpt, pDb);

	//int tagnum = 0;
	//int addtagnum = 0;
	//int num = 0;//横向编号
	//CString tagvalue;//轴号属性值
	//CString csAddtagvalue = _T("");
	//std::map<AcDbObjectId, AcString> AxisNumberMap;
	//for (int i = sortIds.length() - 1; i >= 0; --i)
	//{
	//	COperaAxleNetMaking::LongitudinalNumbering(tagnum, addtagnum, tagvalue, csAddtagvalue);
	//	AxisNumberMap[sortIds[i]] = tagvalue;
	//}

	CString tagvalue;//轴号属性值
	for (int i = sortIds.length() - 1; i >= 0; --i)
	{
		//避免中间短轴线参与编号
		if (CCommonFuntion::GetLineDistance(sortIds[i], LineId) >= 2)
		{
			outids.append(sortIds[i]);
			continue;
		}
			

		//tagvalue = COperaAxleNetMaking::getTagvaluefromMap(AxisNumberMap, sortIds[i]).constPtr();
		AcString keyvalue;
		DBHelper::GetXRecord(sortIds[i], _T("轴号"), keyvalue);
		tagvalue = DBHelper::ACHARAcString(keyvalue);

		if (i == sortIds.length() - 1 && i != 0)
		{
			if (CCommonFuntion::GetLineDistance(sortIds[i], sortIds[i - 1]) <= circleradius * 2 + dis)
			{
				COperaAxleNetMaking::_currency(sortIds[i], sortIds[i - 1], sBlockName, scaleFactor, circleradius, tagvalue, pDb);

			}
			else
			{
				COperaAxleNetMaking::currency(sortIds[i], sBlockName, scaleFactor, circleradius, tagvalue, pDb);
			}
		}
		else if (i == 0 && i != sortIds.length() - 1)
		{
			if (CCommonFuntion::GetLineDistance(sortIds[i], sortIds[i + 1]) <= circleradius * 2 + dis)
			{
				COperaAxleNetMaking::_currency(sortIds[i], sortIds[i + 1], sBlockName, scaleFactor, circleradius, tagvalue, pDb);
			}
			else
			{

				COperaAxleNetMaking::currency(sortIds[i], sBlockName, scaleFactor, circleradius, tagvalue, pDb);
			}
		}
		else if (CCommonFuntion::GetLineDistance(sortIds[i], sortIds[i + 1]) <= circleradius * 2 + dis)
		{
			if (CCommonFuntion::GetLineDistance(sortIds[i], sortIds[i - 1]) <= circleradius * 2 + dis)
			{
				COperaAxleNetMaking::currency(sortIds[i], sBlockName, scaleFactor, circleradius, tagvalue, pDb);

			}
			else
			{
				COperaAxleNetMaking::_currency(sortIds[i], sortIds[i + 1], sBlockName, scaleFactor, circleradius, tagvalue, pDb);
			}
		}
		else if (CCommonFuntion::GetLineDistance(sortIds[i], sortIds[i - 1]) <= circleradius * 2 + dis)
		{
			COperaAxleNetMaking::_currency(sortIds[i], sortIds[i - 1], sBlockName, scaleFactor, circleradius, tagvalue, pDb);
		}
		else
		{
			COperaAxleNetMaking::currency(sortIds[i], sBlockName, scaleFactor, circleradius, tagvalue, pDb);
		}

	}
	CCommonFuntion::DeleteEnt(LineId);

	return outids;
}


AcDbObjectId COperaAxleNetMaking::inserBlockRec(const AcString& sBlockName, AcGePoint3d& basept,
	AcGeVector3d& moveagvec, double circleradius, const AcString& tagvalue, AcDbDatabase *pDb /*= acdbCurDwg()*/)
{
	AcGePoint3d movept = basept;
	moveagvec.normalize();
	AcGePoint3d inserpt = movept.transformBy(moveagvec*circleradius);

	std::map<AcString, AcString> mpAttr;
	mpAttr[_T("A")] = tagvalue;
	AcDbObjectId blockId;
	DBHelper::InsertBlkRefWithAttribute(blockId, sBlockName, inserpt, mpAttr, 0, pDb);


	AcDbBlockReference* pRef = NULL;
	if (Acad::eOk == acdbOpenObject(pRef, blockId, AcDb::kForRead))
	{
		AcDbObjectIterator *pIter = pRef->attributeIterator();
		//无论图块是否有属性,迭代器一般不会为NULL
		if (NULL == pIter)
		{
			acutPrintf(_T("\n获取属性迭代器失败!"));
			return blockId;
		}
		//设置判断是否能获取到属性
		bool bIsAttribBlock = false;
		for (pIter->start(); !pIter->done(); pIter->step())
		{
			AcDbObjectId attribId = pIter->objectId();
			AcDbObjectPointer<AcDbAttribute> pAttrib(attribId, AcDb::kForWrite);
			if (Acad::eOk != pAttrib.openStatus())
			{
				continue;
			}

			AcString pStr = pAttrib->textString();
			int len = pStr.length();

			pAttrib->setWidthFactor((circleradius * 2) / (857.51293*len));


			//Acad::ErrorStatus es = pAttrib->setWidthFactor(len);
			//pAttrib->setHorizontalMode(AcDb::TextHorzMode::kTextFit);
			//pAttrib->setVerticalMode(AcDb::TextVertMode::kTextVertMid);
			////es = pAttrib->setJustification(AcDbText::kTextAlignmentFit);			
			//es = pAttrib->adjustAlignment(pDb);

			//方式二
			//ACHAR *szTag = pAttrib->tag();
			//ACHAR *szValue = pAttrib->textString();
			//if (szTag != NULL && szValue != NULL)
			//{
			//	acutPrintf(_T("\n属性名: %s 属性值: %s"), szTag, szValue);
			//}
			//else
			//{
			//	acutPrintf(_T("\n获取属性值失败!"), szTag, szValue);
			//}
			//if (szTag != NULL)
			//{
			//	acutDelString(szTag);
			//}
			//if (szValue != NULL)
			//{
			//	acutDelString(szValue);
			//}
			//获取到属性,设置标志
			bIsAttribBlock = true;
		}
		//释放迭代器
		delete pIter;


		pRef->close();
	}

	CString sAadAxleNumLayerName(CEquipmentroomTool::getLayerName("axis_dimensions").c_str());
	CCommonFuntion::setEntityLayer(sAadAxleNumLayerName, blockId, pDb);

	AcDbEntity *pEnt = NULL;
	if (acdbOpenObject(pEnt, blockId, AcDb::kForWrite) == eOk)
	{
		AcGeMatrix3d xform;
		xform.setToScaling(circleradius / 450, inserpt);
		pEnt->transformBy(xform);

		if (pEnt)
			pEnt->close();
	}

	return blockId;
}

void COperaAxleNetMaking::dealBlock(const AcString& sBlockName, AcGePoint3d& startpt, AcGeVector3d& moveagvec1
	, AcGePoint3d& endpt, AcGeVector3d& moveagvec2, double scaleFactor, double circleradius, const AcString& tagvalue, AcDbDatabase *pDb /*= acdbCurDwg()*/)
{
	//起始点引出一条直线，并连接轴号块
	AcGePoint3d movept1 = COperaAxleNetMaking::getChangPoint(startpt, moveagvec1, scaleFactor);
	AcDbObjectId GuideLineId1 = COperaAxleNetMaking::DrowLine(startpt, movept1, pDb);
	CString sAadAxleNumLayerName(CEquipmentroomTool::getLayerName("axis_dimensions").c_str());
	COperaAxleNetMaking::setEntColor(GuideLineId1, 3);
	CCommonFuntion::setEntityLayer(sAadAxleNumLayerName, GuideLineId1, pDb);
	AcDbObjectId blockid_1 = COperaAxleNetMaking::inserBlockRec(sBlockName, movept1, moveagvec1, circleradius, tagvalue, pDb);
	//旋转轴号
	//rotateEntity(startpt, endpt, blockid_1, movept1, moveagvec1, circleradius,scaleFactor);
	

	//终止点引出一条直线，并连接轴号块
	AcGePoint3d movept2 = COperaAxleNetMaking::getChangPoint(endpt, moveagvec2, scaleFactor);
	AcDbObjectId GuideLineId2 = COperaAxleNetMaking::DrowLine(endpt, movept2, pDb);
	COperaAxleNetMaking::setEntColor(GuideLineId2, 3);
	CCommonFuntion::setEntityLayer(sAadAxleNumLayerName, GuideLineId2, pDb);
	AcDbObjectId blockid_2 = COperaAxleNetMaking::inserBlockRec(sBlockName, movept2, moveagvec2, circleradius, tagvalue, pDb);
	//旋转轴号
	//rotateEntity(endpt, startpt, blockid_2, movept2, moveagvec2, circleradius, scaleFactor);
}

void COperaAxleNetMaking::LongitudinalNumbering(int& tagnum, int& addtagnum, CString& tagvalue, CString& csAddtagvalue)
{
	tagvalue = (TCHAR)(tagnum + 'A');
	if (tagvalue.Compare(_T("I")) == 0 || tagvalue.Compare(_T("Z")) == 0)
	{
		if (tagvalue.Compare(_T("Z")) == 0)
		{
			csAddtagvalue = (TCHAR)(addtagnum + 'A');
			addtagnum++;
			tagnum = 0;
			tagvalue = (TCHAR)(tagnum + 'A');
			tagvalue = csAddtagvalue + tagvalue;
		}
		else
		{
			if (addtagnum == 0)
			{
				tagnum++;
				tagvalue = (TCHAR)(tagnum + 'A');
			}
			else
			{
				tagnum++;
				tagvalue = (TCHAR)(tagnum + 'A');
				tagvalue = csAddtagvalue + tagvalue;
			}

		}
	}
	else
		tagvalue = csAddtagvalue + tagvalue;
	tagnum++;
}

void COperaAxleNetMaking::setEntColor(AcDbObjectId& id, uint16_t colornum)
{
	AcDbEntity *pEnt = NULL;
	if (Acad::eOk != acdbOpenObject(pEnt, id, AcDb::kForWrite))
		return;
	pEnt->setColorIndex(colornum);
	if (pEnt)
		pEnt->close();
}

AcDbObjectId COperaAxleNetMaking::DrowLine(AcGePoint3d& starpt, AcGePoint3d& endpt, AcDbDatabase *pDb/* = acdbCurDwg()*/)
{
	AcDbLine *pLine = new AcDbLine(starpt, endpt);

	//获得指向块表的指针
	AcDbBlockTable *pBlockTable;
	pDb->getBlockTable(pBlockTable, AcDb::kForRead);

	//获得指向特定的块表记录(模型空间)的指针
	AcDbBlockTableRecord *pBlockTableRecord;
	pBlockTable->getAt(ACDB_MODEL_SPACE, pBlockTableRecord,
		AcDb::kForWrite);

	//将AcDbLine类的对象添加到块表记录中
	AcDbObjectId lineId;
	pBlockTableRecord->appendAcDbEntity(lineId, pLine);
	pLine->setColorIndex(1); //设置直线为红色

							 //关闭图形数据库的各种对象
	pBlockTable->close();
	pBlockTableRecord->close();
	pLine->close();
	return lineId;
}

AcString COperaAxleNetMaking::getTagvaluefromMap(std::map<AcDbObjectId, AcString>& AxisNumberMap, AcDbObjectId& id)
{
	//通过key找value
	if (AxisNumberMap.count(id) > 0)
		return AxisNumberMap[id];

	return _T("");
}

void COperaAxleNetMaking::currency(AcDbObjectId& lineid, const AcString& sBlockName, double scaleFactor, double circleradius,
	const AcString& tagvalue, AcDbDatabase *pDb/*= acdbCurDwg()*/)
{
	//获取轴线起始点和终止点
	AcGePoint3d startpt;
	AcGePoint3d endpt;
	COperaAxleNetMaking::getLinePoint(lineid, startpt, endpt);

	//对插入的块参照进行移动
	COperaAxleNetMaking::dealBlock(sBlockName, startpt, AcGeVector3d(startpt - endpt), endpt, AcGeVector3d(endpt - startpt)
		, scaleFactor * 20, circleradius, tagvalue, pDb);

}

void COperaAxleNetMaking::_currency(AcDbObjectId& lineid, AcDbObjectId& contrastId, const AcString& sBlockName, double scaleFactor, double circleradius, const AcString& tagvalue, AcDbDatabase *pDb /*= acdbCurDwg()*/)
{
	//获取轴线起始点和终止点
	AcGePoint3d startpt;
	AcGePoint3d endpt;
	COperaAxleNetMaking::getLinePoint(lineid, startpt, endpt);

	AcGePoint3d startpt_contrast;
	AcGePoint3d endpt_contrast;
	COperaAxleNetMaking::getLinePoint(contrastId, startpt_contrast, endpt_contrast);

	//为了判断方向一点
	AcGePoint3d pt_start, pt_end;
	if (startpt.distanceTo(startpt_contrast) <= startpt.distanceTo(endpt_contrast))
	{
		pt_start = startpt_contrast;
		pt_end = endpt_contrast;
	}
	else
	{
		pt_start = endpt_contrast;
		pt_end = startpt_contrast;
	}

	AcGeVector3d moveagvec1 = COperaAxleNetMaking::getAcgVec(startpt, AcGeVector3d(startpt - pt_start), AcGeVector3d(startpt - endpt), circleradius);
	AcGeVector3d moveagvec2 = COperaAxleNetMaking::getAcgVec(endpt, AcGeVector3d(endpt - pt_end), AcGeVector3d(endpt - startpt), circleradius);

	AcGePoint3d movept1;
	COperaAxleNetMaking::createGuideLine(startpt, AcGeVector3d(startpt - endpt), scaleFactor, movept1, pDb);
	AcGePoint3d movept2;
	COperaAxleNetMaking::createGuideLine(endpt, AcGeVector3d(endpt - startpt), scaleFactor, movept2, pDb);

	//对插入的块参照进行移动
	COperaAxleNetMaking::dealBlock(sBlockName, movept1, moveagvec1, movept2, moveagvec2, scaleFactor, circleradius, tagvalue, pDb);
}

AcGeVector3d COperaAxleNetMaking::getAcgVec(AcGePoint3d& inputPt, AcGeVector3d& inputVec1, AcGeVector3d& inputVec2, double circleradius)
{
	AcGePoint3d Movept = inputPt;
	AcGePoint3d maxpt = Movept.transformBy(inputVec1.normalize()*circleradius);
	Movept = inputPt;
	AcGePoint3d minpt = Movept.transformBy(inputVec2.normalize()*circleradius);
	AcGePoint3d centerpt = AcGePoint3d((minpt.x + maxpt.x) / 2, (minpt.y + maxpt.y) / 2, 0);
	AcGeVector3d moveagvec = AcGeVector3d(centerpt - inputPt);
	return moveagvec;
}

AcDbObjectId COperaAxleNetMaking::createGuideLine(AcGePoint3d& inputPt, AcGeVector3d& inputVec, double scaleFactor, AcGePoint3d& outpt, AcDbDatabase *pDb /*= acdbCurDwg()*/)
{
	//起始点引出一条直线，并连接轴号块
	AcGePoint3d movept = COperaAxleNetMaking::getChangPoint(inputPt, inputVec, scaleFactor * 20);
	outpt = movept;
	AcDbObjectId GuideLineId = COperaAxleNetMaking::DrowLine(inputPt, movept, pDb);
	COperaAxleNetMaking::setEntColor(GuideLineId, 3);
	CString sAadAxleNumLayerName(CEquipmentroomTool::getLayerName("axis_dimensions").c_str());
	CCommonFuntion::setEntityLayer(sAadAxleNumLayerName, GuideLineId, pDb);

	return GuideLineId;
}

void COperaAxleNetMaking::rotateEntity(AcGePoint3d& startpt, AcGePoint3d& endpt, AcDbObjectId& blockid, AcGePoint3d& movepoint, AcGeVector3d& vec, double circleradius, double scaleFactor)
{
	AcGePoint3d movept = movepoint;
	AcGeVector3d tempvec = vec;
	tempvec.normalize();
	AcGePoint3d inserpt = movept.transformBy(tempvec*circleradius);

	AcGePoint3d originpt = COperaAxleNetMaking::getChangPoint(startpt, AcGeVector3d(AcGePoint3d(startpt.x, startpt.y - 20, 0) - startpt), scaleFactor);
	AcGeVector3d vec_1 = AcGeVector3d(AcGePoint3d(startpt.x, startpt.y - 20, 0) - startpt);
	AcGeVector3d vec_2 = AcGeVector3d(startpt - endpt);
	AcGePoint3d pt = COperaAxleNetMaking::getChangPoint(startpt, vec_2, scaleFactor - 100);

	if (CCommonFuntion::isVerticalBothLines(pt, startpt, startpt, originpt, 1) || CCommonFuntion::IsOnLine(pt,startpt,originpt))
		return;

	int tag = CCommonFuntion::relationshipWithLinePosition(pt, startpt, originpt);

	double angle = vec_1.angleTo(vec_2);
	if (tag > 0)//左侧
	{
		if (CCommonFuntion::radianAngle(angle) >= 90)
			angle = vec_1.angleTo(-vec_2);
		else
			angle = -angle;
	}
	else if (tag < 0)//右侧
	{
		if (CCommonFuntion::radianAngle(angle) >= 90)
			angle = -(vec_1.angleTo(-vec_2));
		else
			angle = angle;
	}
	
	AcDbEntity *pEnt = NULL;
	if (acdbOpenObject(pEnt, blockid, AcDb::kForWrite) == eOk)
	{
		AcGeMatrix3d mat;
		mat.setToRotation(angle, AcGeVector3d::kZAxis, inserpt);
		pEnt->transformBy(mat);
		pEnt->close();
	}
}

bool COperaAxleNetMaking::getChangPoints(AcDbObjectIdArray& lineids, AcGePoint3d& piont1, AcGePoint3d& piont2)
{
	AcGePoint3dArray pts = COperaZoomAxisNumber::getLinesPoints(lineids);
	AcGePoint3dArray fectanglepts = CMinimumRectangle::getMinRact(pts);
	if (fectanglepts.length() != 4)
		return false;

	AcGePoint3d pt1 = fectanglepts[0];
	AcGePoint3d pt2;
	for (int i=1;i<fectanglepts.length();++i)
	{
		double dis1 = CCommonFuntion::PointToSegDist(piont1, piont2, pt1);
		double dis2 = CCommonFuntion::PointToSegDist(piont1, piont2, fectanglepts[i]);
		long tol = dis1 - dis2;
		if (tol >= 0 && tol <= 1)
			pt2 = fectanglepts[i];
		else if (tol >= 5) 
		{
			pt1 = fectanglepts[i];
		}
			
	}
	piont1 = pt1;
	piont2 = pt2;

	return true;
}

bool COperaAxleNetMaking::getChangPoints(AcDbObjectIdArray& lineids, AcGePoint3d& lefdownPt, AcGePoint3d& rigthdownPt, AcGePoint3d& rigthupPt, AcGePoint3d& lefupPt)
{
	AcGePoint3dArray pts = COperaZoomAxisNumber::getLinesPoints(lineids);
	AcGePoint3dArray fectanglepts = CMinimumRectangle::getMinRact(pts);
	if (fectanglepts.length() != 4)
		return false;

	/*if (lefdownPt == fectanglepts[0])
	lefdownPt = AcGePoint3d(0, 0, 0);
	else
	lefdownPt = fectanglepts[0];

	if (rigthdownPt == fectanglepts[1])
	rigthdownPt = AcGePoint3d(0, 0, 0);
	else
	rigthdownPt = fectanglepts[1];

	if (rigthupPt == fectanglepts[2])
	rigthupPt = AcGePoint3d(0, 0, 0);
	else
	rigthupPt = fectanglepts[2];

	if (lefupPt == fectanglepts[3])
	lefupPt = AcGePoint3d(0, 0, 0);
	else
	lefupPt = fectanglepts[3];*/

	

	lefdownPt = fectanglepts[0];
	rigthdownPt = fectanglepts[1];
	rigthupPt = fectanglepts[2];
	lefupPt = fectanglepts[3];

	return true;
}
