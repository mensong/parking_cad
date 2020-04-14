#include "stdafx.h"
#include "OperaZoomAxisNumber.h"
#include "OperaAxleNetMaking.h"
#include "EquipmentroomTool.h"
#include "CommonFuntion.h"
#include "MinimumRectangle.h"
#include "DBHelper.h"

REG_CMD(COperaZoomAxisNumber, BGY, COZSN);


COperaZoomAxisNumber::COperaZoomAxisNumber()
{
}


COperaZoomAxisNumber::~COperaZoomAxisNumber()
{
}

void COperaZoomAxisNumber::Start()
{
	double zoomcoefficient;//�������ϵ��
	if (acedGetReal(_T("���������ű���:"), &zoomcoefficient) != RTNORM)
		return;

	//��ȡ�����ļ�����������ͼ��
	CString sAxisLayerName(CEquipmentroomTool::getLayerName("parking_axis").c_str());
	AcDbObjectIdArray LineIds = DBHelper::GetEntitiesByLayerName(sAxisLayerName);
	if (LineIds.length() == 0)
	{
		acutPrintf(_T("\nδ��ȡ��������Ϣ"));
		return;
	}

	//ɾ�����
	CString sAadAxleNumLayerName(CEquipmentroomTool::getLayerName("axis_dimensions").c_str());
	COperaZoomAxisNumber::deleteEntitys(sAadAxleNumLayerName);
	 
	CString sAxisLayer(CEquipmentroomTool::getLayerName("parking_axis").c_str());
	const ACHAR *layername = sAxisLayer;//��������ͼ��

	//�����洢����,��ƽ�е����ߴ洢�������С�
	std::vector<std::vector<AcDbObjectId>> outputId;
	CCommonFuntion::BatchLine(LineIds, outputId);

	for (int i = 0; i<outputId.size(); ++i)
	{
		AcDbObjectIdArray lineids;
		for (int k = 0; k < outputId[i].size(); ++k)
			lineids.append(outputId[i][k]);

		AcGePoint3dArray pts = COperaZoomAxisNumber::getLinesPoints(lineids);
		AcGePoint3dArray fectanglepts = CMinimumRectangle::getMinRact(pts);

		if (fectanglepts.length() != 4)
			continue;

		//�����ӽṹ�����ʽ���洢�����븨����֮��ľ�������ߵ�id,Ϊ��������ߵ�������׼��
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

		//�Ծ�������
		std::sort(distancevet.begin(), distancevet.end());

		//��������õľ��룬���ṹ�����Ӧ��id������˳�������sortids���������ߵ����������
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
		acdbOpenObject(pEnt, sortIds[0], AcDb::kForRead);
		AcDbLine *line = AcDbLine::cast(pEnt);
		AcGePoint3d startpt;
		AcGePoint3d endptpt;
		line->getStartPoint(startpt);
		line->getEndPoint(endptpt);
		if (line)
			line->close();
		pEnt->close();

		if (CCommonFuntion::IsParallel(fectanglepts[0], fectanglepts[1], startpt, endptpt))
		{
				//��ű�ע
				COperaAxleNetMaking::inserAadAxleNum(sortIds, fectanglepts[1], fectanglepts[2], zoomcoefficient, -1);				
		}
		else
		{
			AcDbObjectIdArray tempsortIds;
			for (int k = sortIds.length() - 1; k >= 0; --k)
				tempsortIds.append(sortIds[k]);

			//��ű�ע
			COperaAxleNetMaking::inserAadAxleNum(sortIds, fectanglepts[1], fectanglepts[2], zoomcoefficient, -1);
		}
	}
}


AcGePoint3dArray COperaZoomAxisNumber::getLinesPoints(AcDbObjectIdArray& LineIds)
{
	AcGePoint3dArray pts;
	for (int i = 0; i < LineIds.length(); ++i)
	{
		AcDbEntity* pEnt = NULL;
		if (acdbOpenObject(pEnt, LineIds[i], AcDb::kForRead) != eOk)
			continue;

		AcDbLine *pLine = AcDbLine::cast(pEnt);
		AcGePoint3d startpt = pLine->startPoint();
		AcGePoint3d endpt = pLine->endPoint();
		pts.append(startpt);
		pts.append(endpt);

		if (pLine)
			pLine->close();
		if (pEnt)
			pEnt->close();
	}

	return pts;
}

double COperaZoomAxisNumber::getMinDis(AcGePoint3d& pt, AcDbObjectId& lineid)
{
	double dis = 0;
	AcDbEntity* pEnt = NULL;
	if (acdbOpenObject(pEnt, lineid, AcDb::kForRead) != eOk)
		return dis;

	AcDbLine *pLine = AcDbLine::cast(pEnt);
	AcGePoint3d startpt = pLine->startPoint();
	AcGePoint3d endpt = pLine->endPoint();

	if (pLine)
		pLine->close();
	if (pEnt)
		pEnt->close();

	if (pt.distanceTo(startpt) <= pt.distanceTo(endpt))
		dis = pt.distanceTo(startpt);
	else
		dis = pt.distanceTo(endpt);

	return dis;

}

void COperaZoomAxisNumber::deleteEntitys(const AcString& layername)
{
	AcDbObjectIdArray ids = DBHelper::GetEntitiesByLayerName(layername);

	AcDbEntity *pEnt = NULL;
	for (int i = 0; i < ids.length(); i++)
	{
		if (acdbOpenObject(pEnt, ids[i], AcDb::kForWrite) != eOk)
			continue;

		pEnt->erase();
		if (pEnt)
			pEnt->close();
	}
}
