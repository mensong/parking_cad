#include "stdafx.h"
#include "OperaZoomAxisNumber.h"
#include "OperaAxleNetMaking.h"
#include "EquipmentroomTool.h"
#include "CommonFuntion.h"
#include "MinimumRectangle.h"
#include "DBHelper.h"
#include <algorithm>
#include "DlgZoomAxisNum.h"

class CDlgZoomAxisNum* COperaZoomAxisNumber::ms_zoomAxisNumDlg = NULL;

REG_CMD(COperaZoomAxisNumber, BGY, COZSN);


COperaZoomAxisNumber::COperaZoomAxisNumber()
{
}


COperaZoomAxisNumber::~COperaZoomAxisNumber()
{
}

void COperaZoomAxisNumber::Start()
{
	if (!ms_zoomAxisNumDlg)
	{
		CAcModuleResourceOverride resOverride;//资源定位
		ms_zoomAxisNumDlg = new CDlgZoomAxisNum(acedGetAcadDwgView());
		ms_zoomAxisNumDlg->Create(CDlgZoomAxisNum::IDD, acedGetAcadDwgView());
	}
	ms_zoomAxisNumDlg->ShowWindow(SW_SHOW);
}

bool COperaZoomAxisNumber::axisNumZoomOpera(double& scaleValue)
{
	//获取配置文件中轴线所在图层
	CString sAxisLayerName(CEquipmentroomTool::getLayerName("parking_axis").c_str());
	AcDbObjectIdArray axisIds = DBHelper::GetEntitiesByLayerName(sAxisLayerName);
	if (axisIds.length() == 0)
	{
		acutPrintf(_T("\n未获取到轴线信息"));
		return false;
	}

	//删除轴号
	CString sAadAxleNumLayerName(CEquipmentroomTool::getLayerName("axis_dimensions").c_str());
	COperaZoomAxisNumber::deleteEntitys(sAadAxleNumLayerName);

	//CString sAxisLayer(CEquipmentroomTool::getLayerName("parking_axis").c_str());
	//const ACHAR *layername = sAxisLayer;
	////轴线所在图层
	//std::vector<std::vector<AcDbObjectId>> idsvec = batchStorageAxis(LineIds);
	//for (int i=0; i<idsvec.size(); ++i)
	//{
	//	for (int k = 0; k < idsvec[i].size(); ++k)
	//	{
	//		AcDbEntity *pEnt = NULL;
	//		if(acdbOpenObject(pEnt, idsvec[i][k], AcDb::kForWrite)!=eOk)
	//			continue;
	//		pEnt->setColorIndex(i);
	//		pEnt->close();
	//	}
	//}
	//return;

	std::vector<std::vector<AcDbObjectId>> idsvec = batchStorageAxis(axisIds);
	for (int idsvecNum = 0; idsvecNum < idsvec.size(); ++idsvecNum)
	{
		AcDbObjectIdArray LineIds;
		for (int iNum = 0; iNum < idsvec[idsvecNum].size(); ++iNum)
			LineIds.append(idsvec[idsvecNum][iNum]);

		//分批存储轴线,将平行的轴线存储在容器中。
		std::vector<std::vector<AcDbObjectId>> outputId;
		CCommonFuntion::BatchLine(LineIds, outputId, 1);

		for (int i = 0; i < outputId.size(); ++i)
		{
			AcDbObjectIdArray lineids;
			for (int k = 0; k < outputId[i].size(); ++k)
				lineids.append(outputId[i][k]);

			AcGePoint3dArray pts = COperaZoomAxisNumber::getLinesPoints(lineids);
			AcGePoint3dArray fectanglepts = CMinimumRectangle::getMinRact(pts);
			if (fectanglepts.length() != 4)
				continue;

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
			for (int soridsNum = 0; soridsNum < sortIds.length(); ++soridsNum)
			{
				if (acdbOpenObject(pEnt, sortIds[soridsNum], AcDb::kForRead) != eOk)
					continue;
				else
					break;
			}

			AcDbLine *line = AcDbLine::cast(pEnt);
			AcGePoint3d startpt;
			AcGePoint3d endptpt;
			line->getStartPoint(startpt);
			line->getEndPoint(endptpt);
			if (line)
				line->close();
			pEnt->close();

			AcGePoint3d lefdownPt = fectanglepts[0];
			AcGePoint3d rigthdownPt = fectanglepts[1];
			AcGePoint3d rigthupPt = fectanglepts[2];
			AcGePoint3d lefupPt = fectanglepts[3];

			if (CCommonFuntion::IsParallel(lefdownPt, rigthdownPt, startpt, endptpt, 1))
			{
				//轴号标注

				AcDbObjectIdArray getids;
				int len = 0;
				do
				{
					getids = COperaAxleNetMaking::inserAadAxleNum(sortIds, rigthdownPt, rigthupPt, scaleValue);
					len = sortIds.length();

					sortIds.removeAll();
					sortIds = getids;
				} while (getids.length() >= 2 && getids.length() != len && COperaAxleNetMaking::getChangPoints(getids, rigthdownPt, rigthupPt) == 1);

			}
			else
			{
				AcDbObjectIdArray getids;
				int len = 0;
				do
				{
					getids = COperaAxleNetMaking::inserAadAxleNum(sortIds, lefdownPt, rigthdownPt, scaleValue);
					len = sortIds.length();

					sortIds.removeAll();
					sortIds = getids;
				} while (getids.length() >= 2 && getids.length() != len && COperaAxleNetMaking::getChangPoints(getids, lefdownPt, rigthdownPt) == 1);

			}

		}
	}
	return true;
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

std::vector<std::vector<AcDbObjectId>> COperaZoomAxisNumber::batchStorageAxis(AcDbObjectIdArray& AxisIds)
{
	std::vector<std::vector<AcDbObjectId>> outidsVec;

	for (int i = 0; i<AxisIds.length(); ++i)
	{
		AcDbEntity *pEnt = NULL;
		if (acdbOpenObject(pEnt, AxisIds[i], AcDb::kForRead) != eOk)
			continue;
		AcDbObjectId id_1 = AxisIds[i];

		for (int k = i + 1; k<AxisIds.length(); ++k)
		{
			AcDbEntity *pTempEnt = NULL;
			if (acdbOpenObject(pTempEnt, AxisIds[k], AcDb::kForRead) != eOk)
				continue;
			AcDbObjectId id_2 = AxisIds[k];

			AcGePoint3dArray interwithpoints;
			pEnt->intersectWith(pTempEnt, AcDb::kOnBothOperands, interwithpoints);
			if (interwithpoints.length() <= 0)
			{
				//不相交，不做处理
			}
			else
			{
				bool tag = true;
				for (int Vecnum = 0; Vecnum<outidsVec.size(); ++Vecnum)
				{
					//如果id_1是容器中
					if (std::find(outidsVec[Vecnum].begin(), outidsVec[Vecnum].end(), id_1) != outidsVec[Vecnum].end())
					{
						//如果id_2不在容器中，则加入id_2到容器；如果id_2在容器中，则不做处理
						if (std::find(outidsVec[Vecnum].begin(), outidsVec[Vecnum].end(), id_2) == outidsVec[Vecnum].end())
							outidsVec[Vecnum].push_back(id_2);
						tag = false;
						break;
					}
					else if (std::find(outidsVec[Vecnum].begin(), outidsVec[Vecnum].end(), id_2) != outidsVec[Vecnum].end())
					{
						//如果id_1不在容器中，而id_2在容器中，则加入id_1
						outidsVec[Vecnum].push_back(id_1);
						tag = false;
						break;
					}

				}

				//如果id_1和id_2都不在容器中，则加入到新建容器中
				if (tag)
				{
					std::vector<AcDbObjectId> tempvec;
					tempvec.push_back(id_1);
					tempvec.push_back(id_2);
					outidsVec.push_back(tempvec);
				}
			}

			if (pTempEnt)
				pTempEnt->close();
		}

		if (pEnt)
			pEnt->close();
	}

	return outidsVec;
}
