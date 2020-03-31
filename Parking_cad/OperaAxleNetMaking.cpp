#include "stdafx.h"
#include "OperaAxleNetMaking.h"
#include <tchar.h>
#include "CommonFuntion.h"
#include <algorithm>
#include "GeHelper.h"
#include "EquipmentroomTool.h"
#include "DBHelper.h"
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

void COperaAxleNetMaking::Start()
{
	CString sAxisLayer(CEquipmentroomTool::getLayerName("parking_axis").c_str());
	const ACHAR *layername = sAxisLayer;//轴线所在图层
	
	AcDbObjectIdArray LineIds = ms_axisIds;
	if (LineIds.length()==0)
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

		if(Ids.length()<=0)
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
		//	COperaAxleNetMaking::inserAadAxleNum(sortIds, lefupPt, lefdownPt, 1, 1);

			CCommonFuntion::creatLaneGridDimensionsDimStyle(_T("车道轴网尺寸标注样式"));

			//这里设置轴网标注所在图层
			CString setlayernameofAXSI(CEquipmentroomTool::getLayerName("axis_dimensions").c_str());
			CEquipmentroomTool::creatLayerByjson("axis_dimensions");
			//CCommonFuntion::setLayer(setlayernameofAXSI, 255);

			//初次处理四边的轴线，标注
			std::vector<AcDbObjectId> outIds_1;
			COperaAxleNetMaking::InitialProcessing(setlayernameofAXSI,sortIds, lefupPt, lefdownPt, outIds_1);
			std::vector<AcDbObjectId> outIds_2;
			COperaAxleNetMaking::InitialProcessing(setlayernameofAXSI,sortIds, rigthdownPt, rigthupPt, outIds_2);

			//还没处理的轴线
			AcDbObjectIdArray specaildealIds = FilterIds(sortIds, outIds_1, outIds_2);

			//对还没处理的轴线，进行阈值判断来标志
			AcDbObjectIdArray specialoutputIds_1;
			if (specaildealIds.length() >= 2)
				SpecialDeal(setlayernameofAXSI,specaildealIds, lefupPt, lefdownPt, rigthdownPt, rigthupPt,layername);

		
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

		
			AcDbObjectIdArray sortIds;
			for (int m = 0; m < distancevet.size(); m++)
			{
				for (int j = 0; j < LDstructVet.size(); j++)
				{
					if (LDstructVet[j].distance == distancevet[m])
						sortIds.append(LDstructVet[j].Lineid);
				}
			}

			/*AcDbObjectIdArray sortIds;
			for (int k = tempsortIds.length() - 1; k >= 0; --k)
				sortIds.append(tempsortIds[k]);
			COperaAxleNetMaking::inserAadAxleNum(sortIds, rigthupPt, lefupPt, 2, -1);*/

			CCommonFuntion::creatLaneGridDimensionsDimStyle(_T("车道轴网尺寸标注样式"));

			//这里设置轴网标注所在图层
			CString setlayernameofAXSI(CEquipmentroomTool::getLayerName("axis_dimensions").c_str());
			CEquipmentroomTool::creatLayerByjson("axis_dimensions");
			//CCommonFuntion::setLayer(setlayernameofAXSI,0);

			std::vector<AcDbObjectId> outIds_1;
			COperaAxleNetMaking::InitialProcessing(setlayernameofAXSI,sortIds, rigthupPt, lefupPt, outIds_1);
			std::vector<AcDbObjectId> outIds_2;
			COperaAxleNetMaking::InitialProcessing(setlayernameofAXSI,sortIds, lefdownPt, rigthdownPt, outIds_2);

			AcDbObjectIdArray specaildealIds = FilterIds(sortIds, outIds_1, outIds_2);
			
			AcDbObjectIdArray specialoutputIds_1;
			if (specaildealIds.length() >= 2)
				SpecialDeal(setlayernameofAXSI,specaildealIds, rigthupPt, lefupPt, lefdownPt, rigthdownPt, layername);
		}
	}
}

void COperaAxleNetMaking::InitialProcessing(const CString& layerNameofAXSI,AcDbObjectIdArray& sortIds, AcGePoint3d& inputstartpt, AcGePoint3d& inputendpt, std::vector<AcDbObjectId>& outputIds)
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
			CCommonFuntion::DrowDimaligned(layerNameofAXSI,interwithpoints[0], interwithpointstemp[0]);
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
	if(outputIds.size()>=3)
	COperaAxleNetMaking::DrowBigDimaligned(layerNameofAXSI,outputIds, inputstartpt, inputendpt);
}

void COperaAxleNetMaking::DrowBigDimaligned(const CString& layerNameofAXSI,std::vector<AcDbObjectId>& inputids, AcGePoint3d& inputstartpt, AcGePoint3d& inputendpt)
{
	AcDbEntity *pEnt = NULL;
	AcDbEntity *pTempEnt = NULL;
	AcDbLine *TempLine = new AcDbLine(inputstartpt, inputendpt);

	AcGeVector2d targetvect = AcGeVector2d(inputendpt.x - inputstartpt.x,inputendpt.y-inputstartpt.y);
	double angle = ((int)((targetvect.angle()) * 100000000 + 0.5)) / 100000000.0;

	std::vector<AcDbObjectId> bigdimalignedids;
	acdbOpenObject(pEnt, inputids[0], AcDb::kForRead);

	AcGePoint3dArray interwithpoints;
	pEnt->intersectWith(TempLine, AcDb::kOnBothOperands, interwithpoints);

	int num = inputids.size() - 1;
	acdbOpenObject(pTempEnt, inputids[num], AcDb::kForRead);

	AcGePoint3dArray interwithpointstemp;
	pTempEnt->intersectWith(TempLine, AcDb::kOnBothOperands, interwithpointstemp);

	AcGeVector3d tempVec = AcGeVector3d(interwithpoints[0] - interwithpointstemp[0]);

	AcGeVector3d Linevec;
	double pi = 3.14159265;
	if(angle== ((int)((3*pi / 2) * 100000000 + 0.5)) / 100000000.0)
	     Linevec = tempVec.rotateBy(ARX_PI / 2, AcGeVector3d(0, 0, 1));
	else if(angle== ((int)(pi * 100000000 + 0.5)) / 100000000.0)
		Linevec = tempVec.rotateBy(3*ARX_PI / 2, AcGeVector3d(0, 0, 1));
	else if(angle== ((int)((pi / 2) * 100000000 + 0.5)) / 100000000.0)
		Linevec = tempVec.rotateBy(3*ARX_PI / 2, AcGeVector3d(0, 0, 1));
	else if(angle==0)
		Linevec = tempVec.rotateBy(ARX_PI / 2, AcGeVector3d(0, 0, 1));

	Linevec.normalize();
	AcGePoint3d Pt1 = interwithpoints[0].transformBy(Linevec * 500);
	AcGePoint3d Pt2 = interwithpointstemp[0].transformBy(Linevec * 500);

	AcGeVector3d vec = AcGeVector3d(Pt2 - Pt1);
	CCommonFuntion::DrowDimaligned(layerNameofAXSI,Pt1, Pt2);


	if (pTempEnt)
		pTempEnt->close();
	if (pEnt)
		pEnt->close();
	if (TempLine)
		TempLine->close();
}

AcDbObjectIdArray COperaAxleNetMaking::FilterIds(AcDbObjectIdArray& inputids, std::vector<AcDbObjectId>& inputids1,std::vector<AcDbObjectId>& inputids2)
{
	AcDbObjectIdArray specaildealIds;
	for (int num = 0; num < inputids.length(); num++)
	{
		if (std::find(inputids1.begin(), inputids1.end(), inputids[num]) == inputids1.end() && find(inputids2.begin(), inputids2.end(), inputids[num]) == inputids2.end())
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

void COperaAxleNetMaking::SpecialDeal(const CString& layerNameofAXSI,AcDbObjectIdArray& inputIds, AcGePoint3d& inputstartpt, AcGePoint3d& inputendpt, AcGePoint3d& inputSecondstartpt, AcGePoint3d& inputSecondendpt, const ACHAR *LineLayerName)
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
					CCommonFuntion::DrowDimaligned(layerNameofAXSI,selectLinept2, selectLinept1);
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
			COperaAxleNetMaking::DrowBigDimaligned(layerNameofAXSI,tempvector, point1, point2);
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
		AcGeMatrix3d xform;
		xform.setToScaling(scaleFactor, basept);
		pEnt->transformBy(xform);

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

void COperaAxleNetMaking::inserAadAxleNum(AcDbObjectIdArray& sortIds, AcGePoint3d& inputstartpt, AcGePoint3d& inputendpt, int casenum, int plusorminus/*=1*/)
{
	CEquipmentroomTool::creatLayerByjson("axis_dimensions");//创建轴号所在图层

	CString sBlockName = _T("_AxleNumber");
	std::set<AcString> setBlockNames;
	setBlockNames.insert(sBlockName);
	AcString filepath = DBHelper::GetArxDir() + _T("template.dwg");
	bool es = DBHelper::ImportBlkDef(filepath, setBlockNames);
	if (!es)
		return;

	std::map<AcString, AcString> mpAttr;
	mpAttr[_T("A")] = _T("A");
	AcDbObjectId blockId;
	DBHelper::InsertBlkRefWithAttribute(blockId, sBlockName, AcGePoint3d(0, 0, 0), mpAttr);

	// 获得当前视图
	AcDbViewTableRecord *view = DBHelper::GetCurrentView();
	double h = view->height();
	double w = view->width();
	if (view)
		view->close();
	double scaleFactor = 60 * (w / h);//缩放比例
	double circleradius = COperaAxleNetMaking::getBlockRadius(blockId, scaleFactor, AcGePoint3d(0, 0, 0));
	double dis = scaleFactor*sin(45 * PI / 180);//折线导致增加的移动距离

	AcDbObjectId LineId = COperaAxleNetMaking::DrowLine(inputstartpt, inputendpt);

	int tagnum = 0;
	int addtagnum = 0;
	int num = 0;//横向编号
	CString tagvalue;//轴号属性值
	CString csAddtagvalue = _T("");
	for (int i = sortIds.length() - 1; i >= 0; --i)
	{
		//避免中间短轴线参与编号
		if (CCommonFuntion::GetLineDistance(sortIds[i], LineId) >= 2)
			continue;

		switch (casenum)
		{
		case 1:
			COperaAxleNetMaking::LongitudinalNumbering(tagnum, addtagnum, tagvalue, csAddtagvalue);
			break;
		case 2:
			num++;
			tagvalue.Format(_T("%d"), num);
			break;
		default:
			acutPrintf(_T("选项值输入错误!"));
		}


		if (i == 0 && i != sortIds.length() - 1)
		{
			if (CCommonFuntion::GetLineDistance(sortIds[i], sortIds[i + 1]) <= circleradius * 2 + dis)
			{
				//获取轴线起始点和终止点
				AcGePoint3d startpt;
				AcGePoint3d endpt;
				COperaAxleNetMaking::getLinePoint(sortIds[i], startpt, endpt);

				AcGePoint3d movept1, movept2;
				AcGeVector3d moveagvec1, moveagvec2;
				COperaAxleNetMaking::creatOtherGuideLine(startpt, endpt, movept1, moveagvec1, movept2, moveagvec2, scaleFactor, -plusorminus);

				//对插入的块参照进行移动
				COperaAxleNetMaking::dealBlock(sBlockName, movept1, moveagvec1, movept2, moveagvec2, scaleFactor, circleradius, tagvalue);
			}
			else
			{
				//获取轴线起始点和终止点
				AcGePoint3d startpt;
				AcGePoint3d endpt;
				COperaAxleNetMaking::getLinePoint(sortIds[i], startpt, endpt);

				//对插入的块参照进行移动
				COperaAxleNetMaking::dealBlock(sBlockName, startpt, AcGeVector3d(startpt - endpt), endpt, AcGeVector3d(endpt - startpt), scaleFactor * 20, circleradius, tagvalue);
			}
			continue;
		}

		//判断两轴线的距离是否大于一个圆直径+圆可能移动的距离（轴号块直接与直线相连）
		if (CCommonFuntion::GetLineDistance(sortIds[i], sortIds[i - 1]) <= circleradius * 2 + dis)
		{
			//如果该轴线与前一个轴线和后一个轴线的距离都小于circleradius * 2 + dis，则轴号不发生偏折。
			if (i != sortIds.length() - 1 && CCommonFuntion::GetLineDistance(sortIds[i], sortIds[i + 1]) <= circleradius * 2 + dis)
			{

				//获取轴线起始点和终止点
				AcGePoint3d startpt;
				AcGePoint3d endpt;
				COperaAxleNetMaking::getLinePoint(sortIds[i], startpt, endpt);

				//对插入的块参照进行移动
				COperaAxleNetMaking::dealBlock(sBlockName, startpt, AcGeVector3d(startpt - endpt), endpt, AcGeVector3d(endpt - startpt)
					, scaleFactor * 20, circleradius, tagvalue);

			}
			else
			{
				//获取轴线起始点和终止点
				AcGePoint3d startpt;
				AcGePoint3d endpt;
				COperaAxleNetMaking::getLinePoint(sortIds[i], startpt, endpt);

				AcGePoint3d movept1, movept2;
				AcGeVector3d moveagvec1, moveagvec2;
				COperaAxleNetMaking::creatOtherGuideLine(startpt, endpt, movept1, moveagvec1, movept2, moveagvec2, scaleFactor, plusorminus);

				//对插入的块参照进行移动
				COperaAxleNetMaking::dealBlock(sBlockName, movept1, moveagvec1, movept2, moveagvec2, scaleFactor, circleradius, tagvalue);
			}
		}
		else if (CCommonFuntion::GetLineDistance(sortIds[i], sortIds[i - 1]) > circleradius * 2 + dis)
		{
			if (i != sortIds.length() - 1 && CCommonFuntion::GetLineDistance(sortIds[i], sortIds[i + 1]) <= circleradius * 2 + dis)
			{
				//获取轴线起始点和终止点
				AcGePoint3d startpt;
				AcGePoint3d endpt;
				COperaAxleNetMaking::getLinePoint(sortIds[i], startpt, endpt);

				AcGePoint3d movept1, movept2;
				AcGeVector3d moveagvec1, moveagvec2;
				COperaAxleNetMaking::creatOtherGuideLine(startpt, endpt, movept1, moveagvec1, movept2, moveagvec2, scaleFactor, -plusorminus);
				//对插入的块参照进行移动
				COperaAxleNetMaking::dealBlock(sBlockName, movept1, moveagvec1, movept2, moveagvec2, scaleFactor, circleradius, tagvalue);
			}
			else
			{
				//获取轴线起始点和终止点
				AcGePoint3d startpt;
				AcGePoint3d endpt;
				COperaAxleNetMaking::getLinePoint(sortIds[i], startpt, endpt);

				//对插入的块参照进行移动
				COperaAxleNetMaking::dealBlock(sBlockName, startpt, AcGeVector3d(startpt - endpt), endpt, AcGeVector3d(endpt - startpt), scaleFactor * 20, circleradius, tagvalue);
			}
		}
	}

	CCommonFuntion::DeleteEnt(blockId);
	CCommonFuntion::DeleteEnt(LineId);
}


void COperaAxleNetMaking::inserBlockRec(const AcString& sBlockName, AcGePoint3d& basept,
	AcGeVector3d& moveagvec, double circleradius, const AcString& tagvalue)
{
	AcGePoint3d movept = basept;
	moveagvec.normalize();
	AcGePoint3d inserpt = movept.transformBy(moveagvec*circleradius);

	std::map<AcString, AcString> mpAttr;
	mpAttr[_T("A")] = tagvalue;
	AcDbObjectId blockId;
	DBHelper::InsertBlkRefWithAttribute(blockId, sBlockName, inserpt, mpAttr);
	CString sAadAxleNumLayerName(CEquipmentroomTool::getLayerName("axis_dimensions").c_str());
	CCommonFuntion::setEntityLayer(sAadAxleNumLayerName,blockId);
	//uint16_t colornum = 255;
	//COperaAxleNetMaking::setEntColor(blockId, colornum);

	AcDbViewTableRecord *view = DBHelper::GetCurrentView();
	double h = view->height();
	double w = view->width();
	if (view)
		view->close();
	double scaleFactor = 60 * (w / h);//缩放比例

	COperaAxleNetMaking::getBlockRadius(blockId, scaleFactor, inserpt);

}

void COperaAxleNetMaking::dealBlock(const AcString& sBlockName, AcGePoint3d& startpt, AcGeVector3d& moveagvec1
	, AcGePoint3d& endpt, AcGeVector3d& moveagvec2, double scaleFactor, double circleradius, const AcString& tagvalue)
{
	//起始点引出一条直线，并连接轴号块
	AcGePoint3d movept1 = COperaAxleNetMaking::getChangPoint(startpt, moveagvec1, scaleFactor);
	AcDbObjectId GuideLineId1 = COperaAxleNetMaking::DrowLine(startpt, movept1);
	CString sAadAxleNumLayerName(CEquipmentroomTool::getLayerName("axis_dimensions").c_str());
	COperaAxleNetMaking::setEntColor(GuideLineId1, 3);
	CCommonFuntion::setEntityLayer(sAadAxleNumLayerName, GuideLineId1);
	COperaAxleNetMaking::inserBlockRec(sBlockName, movept1, moveagvec1, circleradius, tagvalue);

	//终止点引出一条直线，并连接轴号块
	AcGePoint3d movept2 = COperaAxleNetMaking::getChangPoint(endpt, moveagvec2, scaleFactor);
	AcDbObjectId GuideLineId2 = COperaAxleNetMaking::DrowLine(endpt, movept2);
	COperaAxleNetMaking::setEntColor(GuideLineId2, 3);
	CCommonFuntion::setEntityLayer(sAadAxleNumLayerName, GuideLineId2);
	COperaAxleNetMaking::inserBlockRec(sBlockName, movept2, moveagvec2, circleradius, tagvalue);
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

void COperaAxleNetMaking::creatOtherGuideLine(AcGePoint3d& startpt, AcGePoint3d& endpt, AcGePoint3d& movept1
	, AcGeVector3d& moveagvec1, AcGePoint3d& movept2, AcGeVector3d& moveagvec2, double scaleFactor, int plusorminus/*=1*/)
{
	moveagvec1 = AcGeVector3d(startpt - endpt);
	//起始点引出一条直线，并连接轴号块
	movept1 = COperaAxleNetMaking::getChangPoint(startpt, moveagvec1, scaleFactor * 20);
	AcDbObjectId GuideLineId1 = COperaAxleNetMaking::DrowLine(startpt, movept1);
	COperaAxleNetMaking::setEntColor(GuideLineId1, 3);
	CString sAadAxleNumLayerName(CEquipmentroomTool::getLayerName("axis_dimensions").c_str());
	CCommonFuntion::setEntityLayer(sAadAxleNumLayerName, GuideLineId1);
	moveagvec1.rotateBy(plusorminus*PI / 4, AcGeVector3d(0, 0, 1));

	moveagvec2 = AcGeVector3d(endpt - startpt);
	//起始点引出一条直线，并连接轴号块
	movept2 = COperaAxleNetMaking::getChangPoint(endpt, moveagvec2, scaleFactor * 20);
	AcDbObjectId GuideLineId2 = COperaAxleNetMaking::DrowLine(endpt, movept2);
	COperaAxleNetMaking::setEntColor(GuideLineId2, 3);
	CCommonFuntion::setEntityLayer(sAadAxleNumLayerName, GuideLineId2);
	moveagvec2.rotateBy(-(PI / 4)*plusorminus, AcGeVector3d(0, 0, 1));
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

AcDbObjectId COperaAxleNetMaking::DrowLine(AcGePoint3d& starpt, AcGePoint3d& endpt)
{
	AcDbLine *pLine = new AcDbLine(starpt, endpt);

	//获得指向块表的指针
	AcDbBlockTable *pBlockTable;
	acdbHostApplicationServices()->workingDatabase()->getBlockTable(pBlockTable, AcDb::kForRead);

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