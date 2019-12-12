#include "stdafx.h"
#include "OperaAddFrame.h"
#include "DBHelper.h"
#include "DlgAddFrame.h"
#include "GeHelper.h"

std::string COperaAddFrame::ms_strText;

COperaAddFrame::COperaAddFrame()
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

	double SPF1area = getPloyLineArea(ids, _T("设备房"));

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
		std::map<std::string, double> picAttributedata;
		//测试数据
		picAttributedata["SP"] = 4865;
		picAttributedata["SPT"] = 3740;
		picAttributedata["SPF"] = 1125;
		picAttributedata["SPF1"] = 210;
		picAttributedata["SPF2"] = 450;
		picAttributedata["SPF3"] = 460;
		picAttributedata["CP"] = 132;
		picAttributedata["JSPC"] = 25;
		picAttributedata["SPC"] = 33;
		picAttributedata["H"] = 3.55;
		picAttributedata["HT"] = 1;

		std::string sPicAttributeText = setPicAttributeData(SPF1area, picAttributedata);

		dlg.setBlockInserPoint(sPicAttributeText);
		
		AcDbPolyline* pFrame = new AcDbPolyline;
		pFrame->addVertexAt(0, dlg.mBigFramept0);
		pFrame->addVertexAt(1, dlg.mBigFramept1);
		pFrame->addVertexAt(2, dlg.mBigFramept2);
		pFrame->addVertexAt(3, dlg.mBigFramept3);
		pFrame->setClosed(Adesk::kTrue);
		pFrame->setColorIndex(255);

		AcGePoint2d centerpt = AcGePoint2d((dlg.mBigFramept0.x + dlg.mBigFramept2.x) / 2, (dlg.mBigFramept0.y + dlg.mBigFramept2.y) / 2);

		AcDbPolyline* pOutermostFrame = new AcDbPolyline;
		pOutermostFrame->addVertexAt(0, GetChangePoint(centerpt, dlg.mBigFramept0));
		pOutermostFrame->addVertexAt(1, GetChangePoint(centerpt, dlg.mBigFramept1));
		pOutermostFrame->addVertexAt(2, GetChangePoint(centerpt, dlg.mBigFramept2));
		pOutermostFrame->addVertexAt(3, GetChangePoint(centerpt, dlg.mBigFramept3));
		pOutermostFrame->setClosed(Adesk::kTrue);
		pOutermostFrame->setColorIndex(255);

		std::vector<AcDbEntity*> vcEnts;
		vcEnts.push_back(pFrame);
		vcEnts.push_back(pOutermostFrame);
		DBHelper::CreateBlock(_T("图框"), vcEnts);
		if (pFrame)
			pFrame->close();
		if (pOutermostFrame)
			pOutermostFrame->close();

		AcDbObjectId idEnt;
		DBHelper::InsertBlkRef(idEnt, _T("图框"), AcGePoint3d(0, 0, 0));

	}
}

AcGePoint2d COperaAddFrame::GetChangePoint(AcGePoint2d& centerpt, AcGePoint2d& changpt)
{
	AcGeVector2d vec(changpt - centerpt);
	vec.normalize();
	AcGePoint2d pt = changpt;
	pt.transformBy(vec * 4000);
	return pt;
}

std::vector<AcGePoint2dArray> COperaAddFrame::getPlinePointForLayer(std::vector<AcDbObjectId>& inputIds, const AcString& layerNameofEnty)
{
	std::vector<AcGePoint2dArray> outputPoints;
	
	for (int i = 0; i < inputIds.size(); i++)
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

			if(onePlinePts.length()<=1)
				continue;
			
			onePlinePts.append(onePlinePts[0]);
			outputPoints.push_back(onePlinePts);
		}
		pEntity->close();
	}
	return outputPoints;
}

double COperaAddFrame::getPloyLineArea(std::vector<AcDbObjectId>& inputIds, const AcString& layerNameofEnty)
{
	std::vector<AcGePoint2dArray> vectPoints = getPlinePointForLayer(inputIds, layerNameofEnty);

	double totalarea = 0;
	for (int i=0; i<vectPoints.size(); i++)
	{
		double area = GeHelper::CalcPolygonArea(vectPoints[i]);

		totalarea += area;
	}

	return totalarea;

}

std::string COperaAddFrame::setPicAttributeData(double SPF1value, std::map<std::string, double>& picAttributedata)
{
	//"SP=4865|SPT=3740|SPF=1125|SPF1=210|SPF2=450|SPF3=463|CP=132|JSPC=25|SPC=33|H=3.55|HT=1"
	double SPvalue = getPicAttributeValue(picAttributedata, "SP");
	double SPF2value = getPicAttributeValue(picAttributedata, "SPF2");
	double SPF3value = getPicAttributeValue(picAttributedata, "SPF3");
	double SPF4value = getPicAttributeValue(picAttributedata, "SPF4");
	double SPF5value = getPicAttributeValue(picAttributedata, "SPF5");
	double CPvalue = getPicAttributeValue(picAttributedata, "CP");
	double Hvalue = getPicAttributeValue(picAttributedata, "H");
	double HTvalue = getPicAttributeValue(picAttributedata, "HT");

	double SPFvalue = (SPF1value/1000000) + SPF2value + SPF3value + SPF4value + SPF5value;
	double SPTvalue = SPvalue - SPFvalue;
	double JSPCvalue = SPTvalue / CPvalue;
	double SPCvalue = (SPTvalue + (SPF1value / 1000000) + SPF2value) / CPvalue;

	std::string sAttributeData = setStringData(SPvalue, "SP") + "|" + setStringData(SPTvalue, "SPT") + "|"
		                       + setStringData(SPFvalue, "SPF") + "|" + setStringData((SPF1value / 1000000), "SPF1") + "|"
		                       + setStringData(SPF2value, "SPF2") + "|" + setStringData(SPF3value, "SPF3") + "|"
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

double COperaAddFrame::getPicAttributeValue(std::map<std::string,double>& picAttributedata, const std::string& picAttributeTage)
{
	//通过key找value
	if (picAttributedata.count(picAttributeTage) > 0)
		return picAttributedata[picAttributeTage];

	return 0;
}

void COperaAddFrame::setTextStr(const std::string& strText)
{
	ms_strText = strText;
}

REG_CMD(COperaAddFrame, BGY, AddFrame);//增加图框