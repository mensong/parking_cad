#include "stdafx.h"
#include "OperaAxleNetMaking.h"
#include <tchar.h>
#include "CommonFuntion.h"
#include <algorithm>
#include "GeHelper.h"

REG_CMD(COperaAxleNetMaking, BGY, ANM);

AcDbObjectIdArray COperaAxleNetMaking::ms_axisIds;

COperaAxleNetMaking::COperaAxleNetMaking()
{
}


COperaAxleNetMaking::~COperaAxleNetMaking()
{
}

void COperaAxleNetMaking::Start()
{

	ACHAR *layername = _T("axis");//轴线所在图层，这里写死只是为了测试

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

			CCommonFuntion::creatLaneGridDimensionsDimStyle(_T("车道轴网尺寸标注样式"));

			CString setlayernameofAXSI = _T("轴网标注");//这里设置轴网标注所在图层，为硬代码，可根据需要进行更改
			CCommonFuntion::setLayer(setlayernameofAXSI, 255);

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

			CCommonFuntion::creatLaneGridDimensionsDimStyle(_T("车道轴网尺寸标注样式"));

			CString setlayernameofAXSI = _T("轴网标注");//这里设置轴网标注所在图层，为硬代码，可根据需要进行更改
			CCommonFuntion::setLayer(setlayernameofAXSI,0);

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
