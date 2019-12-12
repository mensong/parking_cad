#include "stdafx.h"
#include <acedads.h>
#include <vector>
#include "LineSelect.h"
#include <algorithm>
#include "DBHelper.h"
#include "CommonFuntion.h"
#ifndef PI
#define PI 3.1415926535898
#endif
CCommonFuntion::CCommonFuntion()
{
}


CCommonFuntion::~CCommonFuntion()
{
}
AcDbObjectIdArray CCommonFuntion::Select_Ent(AcGePoint3d& centerpoint)
{
	AcDbObjectIdArray entIds;
	AcDbObjectIdArray outIds;
	if (!CCommonFuntion::PromptSelectEnts(_T("��ѡ��ʵ��"), AcDbEntity::desc(), entIds))
		return outIds;
	AcDbEntity *pEnt = NULL;
	AcDbExtents cloneblockExt;
	for (int i = 0; i < entIds.length(); i++)
	{
		if (Acad::eOk != acdbOpenObject(pEnt, entIds[i], AcDb::kForRead))
			continue;

		AcDbExtents extens;
		pEnt->bounds(extens);
		cloneblockExt.addExt(extens);
		outIds.append(entIds[i]);

		pEnt->close();
	}

	double x = (cloneblockExt.minPoint().x + cloneblockExt.maxPoint().x) / 2;
	double y = (cloneblockExt.minPoint().y + cloneblockExt.maxPoint().y) / 2;
	double z = (cloneblockExt.minPoint().z + cloneblockExt.maxPoint().z) / 2;
	centerpoint = AcGePoint3d(x, y, z);

	return outIds;

}

bool CCommonFuntion::PromptSelectEnts(const TCHAR* prompt, AcRxClass* classDesc, AcDbObjectIdArray &entIds)
{

	std::vector<AcRxClass*> vecClassDesc;
	vecClassDesc.push_back(classDesc);

	return PromptSelectEnts(prompt, vecClassDesc, entIds);
}

bool CCommonFuntion::PromptSelectEnts(const TCHAR* prompt, const std::vector<AcRxClass*> &classDescs, AcDbObjectIdArray &entIds)
{
	// ��ʼ��
	entIds.setLogicalLength(0);

	// ��ʾ�û�ѡ��ʵ��
	ads_name sset;
	int result = 0;
	acutPrintf(prompt);
	result = acedSSGet(NULL, NULL, NULL, NULL, sset);

	if (result != RTNORM)
	{
		return false;
	}

	// ȡ��ѡ�񼯵ĳ���
	long length = 0;
	acedSSLength(sset, reinterpret_cast<Adesk::Int32*>(&length));

	// ����ѡ��
	Acad::ErrorStatus es;
	AcDbEntity* pEnt = NULL;
	for (long i = 0; i < length; i++)
	{
		ads_name ename;
		acedSSName(sset, i, ename);

		AcDbObjectId curEntId;
		es = acdbGetObjectId(curEntId, ename);
		if (es != Acad::eOk)
		{
			continue;
		}

		// ��ʵ��
		es = acdbOpenObject(pEnt, curEntId, AcDb::kForRead);
		if (es != Acad::eOk)
		{
			continue;
		}

		// �жϵ�ǰʵ���Ƿ���ָ����ʵ������
		for (int j = 0; j < (int)classDescs.size(); j++)
		{
			if (pEnt->isKindOf(classDescs[j]))
			{
				entIds.append(pEnt->objectId());
				break;
			}
		}

		pEnt->close();	// �ر�ʵ��
	}
	acedSSFree(sset);	// �ͷ�ѡ��

	return (entIds.length() > 0);
}

void CCommonFuntion::BatchStorageEnt(AcDbObjectIdArray& inputId, std::vector<std::vector<AcDbObjectId>>& outputId)
{
	AcDbEntity *pEnt = NULL;
	AcDbEntity *tempEnt = NULL;
	if (inputId.length() == 1)
	{
		std::vector<AcDbObjectId> tempcompare;
		tempcompare.push_back(inputId[0]);
		outputId.push_back(tempcompare);
	}
	else
	{
		for (int i = 0; i < inputId.length(); i++)
		{
			bool tag = true;

			if (Acad::eOk != acdbOpenObject(pEnt, inputId[i], AcDb::kForRead))
				continue;

			for (int j = 0; j < inputId.length(); j++)
			{

				std::vector<AcDbObjectId> tempcompare;
				if (inputId[i] == inputId[j])
					continue;
				if (Acad::eOk != acdbOpenObject(tempEnt, inputId[j], AcDb::kForRead))
					continue;

				AcGePoint3dArray intersectPoints;
				tempEnt->intersectWith(pEnt, AcDb::kOnBothOperands, intersectPoints);
				if (intersectPoints.length() > 0)
				{
					tag = false;
					bool ent_tag = true;
					int ent_num = 0;
					for (int k = 0; k < outputId.size(); k++)
					{
						if (std::find(outputId[k].begin(), outputId[k].end(), inputId[i]) != outputId[k].end())//����ʵ��ID
						{

							ent_tag = false;
							ent_num = k;
							break;
						}
					}

					bool tement_tag = true;
					int tempent_num = 0;
					for (int k = 0; k < outputId.size(); k++)
					{
						if (std::find(outputId[k].begin(), outputId[k].end(), inputId[j]) != outputId[k].end())//����ʵ��ID
						{

							tement_tag = false;
							tempent_num = k;
							break;
						}
					}

					if (ent_tag)
					{
						if (tement_tag)
						{
							tempcompare.push_back(inputId[i]);
							tempcompare.push_back(inputId[j]);
							outputId.push_back(tempcompare);
						}
						else
							outputId[tempent_num].push_back(inputId[i]);
					}
					else
					{
						if (tement_tag)
							outputId[ent_num].push_back(inputId[j]);
						else
						{
							if (tempent_num != ent_num)
							{
								for (int Num = 0; Num < outputId[tempent_num].size(); Num++)
								{
									outputId[ent_num].push_back(outputId[tempent_num][Num]);
								}
								std::remove(outputId.begin(), outputId.end(), outputId[tempent_num]);
							}
						}
					}
				}
				if (tempEnt)
					tempEnt->close();
			}
			if (tag)
			{
				std::vector<AcDbObjectId> tempvector;
				tempvector.push_back(inputId[i]);
				outputId.push_back(tempvector);
			}

			if (pEnt)
				pEnt->close();
		}
	}
}

int CCommonFuntion::charIsInstring(const wchar_t findedChar, const wchar_t* theChars)
{
	int i = 0;
	while (*theChars != '\0')
	{
		if (findedChar == *theChars)
		{
			return i;
			break;
		}
		++theChars;
		++i;
	}
	return -1;

}

bool CCommonFuntion::string1_In_string2(const wchar_t* chars1, const wchar_t* chars2)
{
	int indexOfFirstLetter = charIsInstring(*chars1, chars2);
	if (indexOfFirstLetter != -1)
	{
		while (*chars1 != '\0'&& *++chars1 == chars2[++indexOfFirstLetter])
		{
		}
		if (*chars1 == '\0')
			return TRUE;
		else
		{
			return FALSE;
		}
	}
	else
		return FALSE;
}

void CCommonFuntion::SaveEntInfo(AcDbObjectIdArray& inputIds, AcGePoint2dArray& points)
{
	int linesnum = 0;
	int polylinesnum = 0;
	int circlesnum = 0;
	int arcsnum = 0;

	AcDbEntity* pEnty = NULL;

	for (int i = 0; i < inputIds.length(); i++)
	{
		if (Acad::eOk != acdbOpenObject(pEnty, inputIds[i], AcDb::kForRead))
			continue;
		if (pEnty->isKindOf(AcDbLine::desc()))
		{
			AcDbLine *line = AcDbLine::cast(pEnty);
			AcGePoint3d star = line->startPoint();
			AcGePoint3d end = line->endPoint();
			points.append(AcGePoint2d(star.x, star.y));
			points.append(AcGePoint2d(end.x, end.y));


		}
		else if (pEnty->isKindOf(AcDbPolyline::desc()))
		{
			std::vector<AcGePoint3d> temp;
			AcDbPolyline *polyline = AcDbPolyline::cast(pEnty);
			int n = polyline->numVerts();
			for (int j = 0; j < n; j++) {
				AcGePoint3d pt;
				polyline->getPointAt(j, pt);
				points.append(AcGePoint2d(pt.x, pt.y));
			}

		}
		else if (pEnty->isKindOf(AcDbCircle::desc()))
		{
			AcDbCircle *pCircle = (AcDbCircle *)pEnty;

			AcGePoint3d pCen = pCircle->center();
			points.append(AcGePoint2d(pCen.x, pCen.y));

		}
		else if (pEnty->isKindOf(AcDbArc::desc()))
		{
			AcDbArc *arc = AcDbArc::cast(pEnty);
			AcGePoint3d arccenter = arc->center();
			AcGePoint3d startpt;
			AcGePoint3d endpt;
			arc->getStartPoint(startpt);
			arc->getEndPoint(endpt);
			points.append(AcGePoint2d(arccenter.x, arccenter.y));
			points.append(AcGePoint2d(startpt.x, startpt.y));
			points.append(AcGePoint2d(endpt.x, endpt.y));
		}
		if (pEnty)
			pEnty->close();
	}
}

void CCommonFuntion::DealEnt(AcDbEntity* pEnt, AcGePoint3dArray& intersectPoints)
{
	if (pEnt->isKindOf(AcDbLine::desc()))
	{
		AcDbLine *LinEnt = AcDbLine::cast(pEnt);
		AcGePoint3d starpoint = LinEnt->startPoint();
		AcGePoint3d endpoint = LinEnt->endPoint();
		if (starpoint.distanceTo(intersectPoints[0]) <= endpoint.distanceTo(intersectPoints[0]))
		{
			LinEnt->setStartPoint(intersectPoints[0]);
		}
		else
		{
			LinEnt->setEndPoint(intersectPoints[0]);
		}
	}
	else if (pEnt->isKindOf(AcDbPolyline::desc()))
	{
		AcDbPolyline *pPolyline = AcDbPolyline::cast(pEnt);
		int num = pPolyline->numVerts();
		AcGePoint3d starpoint;
		AcGePoint3d endpoint;
		pPolyline->getStartPoint(starpoint);
		pPolyline->getEndPoint(endpoint);
		Acad::ErrorStatus es;
		if (starpoint.distanceTo(intersectPoints[0]) < endpoint.distanceTo(intersectPoints[0]))
			es = pPolyline->setPointAt(0, AcGePoint2d(intersectPoints[0].x, intersectPoints[0].y));
		else
			es = pPolyline->setPointAt(num - 1, AcGePoint2d(intersectPoints[0].x, intersectPoints[0].y));
	}
	else if (pEnt->isKindOf(AcDbArc::desc()))
	{
		AcDbArc *pARC = AcDbArc::cast(pEnt);
		AcGePoint3d starpoint;
		AcGePoint3d endpoint;
		pARC->getStartPoint(starpoint);
		pARC->getEndPoint(endpoint);
		AcGePoint3d centerpoint = pARC->center();
		AcDbAttribute* pAttrib = NULL;
		if (starpoint.distanceTo(intersectPoints[0]) < endpoint.distanceTo(intersectPoints[0]))
		{
			AcGeVector2d changVec = AcGeVector2d(intersectPoints[0].x - centerpoint.x, intersectPoints[0].y - centerpoint.y);
			double startAngle = changVec.angle();
			pARC->setStartAngle(startAngle);
		}
		else
		{
			AcGeVector2d changVec = AcGeVector2d(intersectPoints[0].x - centerpoint.x, intersectPoints[0].y - centerpoint.y);
			double endAngle = changVec.angle();
			pARC->setEndAngle(endAngle);
		}


	}
}

AcDbObjectIdArray CCommonFuntion::GetAllEntityId(const TCHAR* layername)
{
	AcDbObjectIdArray entIds;
	bool bFilterlayer = false;
	AcDbObjectId layerId;
	//��ȡָ��ͼ�����ID
	if (layername != NULL)
	{
		AcDbLayerTable *pLayerTbl = NULL;
		acdbHostApplicationServices()->workingDatabase()->getSymbolTable(pLayerTbl, AcDb::kForRead);
		if (!pLayerTbl->has(layername))
		{
			pLayerTbl->close();
			return entIds;
		}
		pLayerTbl->getAt(layername, layerId);
		pLayerTbl->close();
		bFilterlayer = true;
	}
	//��ÿ��
	AcDbBlockTable *pBlkTbl = NULL;
	acdbHostApplicationServices()->workingDatabase()->getSymbolTable(pBlkTbl, AcDb::kForRead);
	//����¼
	AcDbBlockTableRecord *pBlkTblRcd = NULL;
	pBlkTbl->getAt(ACDB_MODEL_SPACE, pBlkTblRcd, AcDb::kForRead);
	pBlkTbl->close();
	//���������������η���ģ�Ϳռ��е�ÿһ��ʵ��
	AcDbBlockTableRecordIterator *it = NULL;
	pBlkTblRcd->newIterator(it);
	for (it->start(); !it->done(); it->step())
	{
		AcDbEntity *pEnt = NULL;
		Acad::ErrorStatus es = it->getEntity(pEnt, AcDb::kForRead);
		if (es == Acad::eOk)
		{
			if (bFilterlayer)//����ͼ��
			{
				if (pEnt->layerId() == layerId)
				{
					entIds.append(pEnt->objectId());
				}
			}
			else
			{
				//    entIds.append(pEnt->objectId());
				pEnt->close();
			}
		}
		if (pEnt)
			pEnt->close();
	}
	delete it;
	pBlkTblRcd->close();
	return entIds;
}

bool CCommonFuntion::IsOnLine(AcGePoint2d& pt1, AcGePoint2d& pt2, AcGePoint2d& pt3)
{
	AcGeVector3d vec1 = AcGeVector3d(pt2.x - pt1.x, pt2.y - pt1.y, 0);
	AcGeVector3d vec2 = AcGeVector3d(pt3.x - pt1.x, pt3.y - pt1.y, 0);
	double pi = 3.14159265;// 35897931;
						   //double angle = vec1.angleTo(vec2);
	double angle = ((int)((vec1.angleTo(vec2)) * 100000000 + 0.5)) / 100000000.0;
	if (angle == pi || angle == 0)
		return true;
	else
		return false;
}

bool CCommonFuntion::IsOnLine(AcGePoint3d& pt1, AcGePoint3d& pt2, AcGePoint3d& pt3)
{
	AcGeVector3d vec1 = AcGeVector3d(pt2.x - pt1.x, pt2.y - pt1.y, 0);
	AcGeVector3d vec2 = AcGeVector3d(pt3.x - pt1.x, pt3.y - pt1.y, 0);
	double pi = 3.14159265;// 35897931;
						   //double angle = vec1.angleTo(vec2);
	double angle = ((int)((vec1.angleTo(vec2)) * 100000000 + 0.5)) / 100000000.0;
	if (angle == pi || angle == 0)
		return true;
	else
		return false;
}

ACHAR* CCommonFuntion::ChartoACHAR(const char* src)
{
	size_t srcsize = strlen(src) + 1;
	size_t newsize = srcsize;
	size_t convertedChars = 0;
	wchar_t *wcstring;
	wcstring = new wchar_t[newsize];
	mbstowcs_s(&convertedChars, wcstring, srcsize, src, _TRUNCATE);

	return wcstring;
}

char* CCommonFuntion::ACHARtoChar(const ACHAR* src)
{

	int iSize;
	char* pszMultiByte;
	//���ؽ����ַ������軺�����Ĵ�С���Ѿ������ַ���β��'\0'
	iSize = WideCharToMultiByte(CP_ACP, 0, src, -1, NULL, 0, NULL, NULL); //iSize =wcslen(pwsUnicode)+1=6
	pszMultiByte = (char*)malloc(iSize * sizeof(char)); //����Ҫ pszMultiByte = (char*)malloc(iSize*sizeof(char)+1);
	WideCharToMultiByte(CP_ACP, 0, src, -1, pszMultiByte, iSize, NULL, NULL);
	return pszMultiByte;
}
AcDbObjectId CCommonFuntion::PostToModelSpace(AcDbEntity* pEnt)
{
	// ���ָ�����ָ��
	AcDbBlockTable *pBlockTable = NULL;
	//workingDatabase()�ܹ����һ��ָ��ǰ���ͼ�����ݿ��ָ�룬
	acdbHostApplicationServices()->workingDatabase()->getBlockTable(pBlockTable, AcDb::kForRead);
	// ���ָ���ض��Ŀ���¼��ģ�Ϳռ䣩��ָ��
	AcDbBlockTableRecord *pBlockTableRecord = NULL;
	pBlockTable->getAt(ACDB_MODEL_SPACE, pBlockTableRecord, AcDb::kForWrite);

	AcDbObjectId entId;
	// ��AcDbLine��Ķ�����ӵ�����¼��
	pBlockTableRecord->appendAcDbEntity(entId, pEnt);

	// �ر�ͼ�����ݿ�ĸ��ֶ���
	pBlockTable->close();
	pBlockTableRecord->close();
	pEnt->close();

	return entId;
}

void CCommonFuntion::GetEntPointofinBlock()
{
	//���´���ͨ��ѡ��һ������գ������ڵ�Բ���������ת��Ϊ��Բ�ڵ�ǰdwg�е����겢������൱��ը������պ��ٶ�ȡԲ���������Ч��
	ads_name ObjName;
	ads_point pnt;
	AcDbObjectId objId;
	if (RTNORM != acedEntSel(_T(""), ObjName, pnt))
	{
		return;
	}

	acdbGetObjectId(objId, ObjName);
	AcDbEntity *pEnt = NULL;
	acdbOpenObject(pEnt, objId, AcDb::kForRead);
	if (pEnt->isKindOf(AcDbBlockReference::desc()))
	{
		AcDbBlockReference *pBlkRef = AcDbBlockReference::cast(pEnt);
		AcGeMatrix3d mat = pBlkRef->blockTransform();

		AcDbBlockTableRecordPointer BlkTBRec_Ptr(pBlkRef->blockTableRecord(), AcDb::kForRead);
		if (Acad::eOk == BlkTBRec_Ptr.openStatus())
		{
			AcDbBlockTableRecordIterator *pIt = NULL;
			BlkTBRec_Ptr->newIterator(pIt);
			for (pIt->start(); !pIt->done(); pIt->step())
			{
				AcDbEntity *pEntItem = NULL;
				pIt->getEntity(pEntItem, AcDb::kForRead);
				if (pEntItem->isKindOf(AcDbCircle::desc()))
				{
					AcDbCircle *pCircle = AcDbCircle::cast(pEntItem);
					AcGePoint3d transPnt = mat * pCircle->center(); //���任����Ӧ�õ�����Ŀ���λ���ϣ��������ڵ�ǰdwgͼ�е�λ��	
					acutPrintf(_T("����λ�ã�%f,%f\n"), pCircle->center().x, pCircle->center().y);
					acutPrintf(_T("�ڵ�ǰͼ�е�λ��%f,%f\n"), transPnt.x, transPnt.y);
				}
				pEntItem->close();
			}
			delete pIt;
		}
	}
	pEnt->close();

}

void CCommonFuntion::DrowPloyLine(AcGePoint2dArray& inputpoints)
{
	if (inputpoints.length() < 3)
		return;
	AcDbPolyline *pPolyLine = new AcDbPolyline(3);
	for (int i = 0; i < inputpoints.length(); i++)
	{

		pPolyLine->addVertexAt(i, inputpoints.at(i), 0, 1.0, 1.0);


	}

	AcDbBlockTable *pBlockTable = NULL;
	acdbHostApplicationServices()->workingDatabase()->getBlockTable(pBlockTable, AcDb::kForRead);
	AcDbBlockTableRecord *pBlockTableRocord;
	pBlockTable->getAt(ACDB_MODEL_SPACE, pBlockTableRocord, AcDb::kForWrite);
	AcDbObjectId lineId;
	pBlockTableRocord->appendAcDbEntity(lineId, pPolyLine);
	pBlockTable->close();
	pBlockTableRocord->close();
	pPolyLine->close();
}

AcDbObjectIdArray CCommonFuntion::DrowLine(AcGePoint3dArray& inputpoints)
{
	AcDbObjectIdArray ids;
	if (inputpoints.length() < 2)
		return ids;
	for (int i = 0; i < inputpoints.length(); i++)
	{

		if (i == inputpoints.length() - 1)
		{
			AcDbLine *pPline = new AcDbLine(inputpoints[i], inputpoints[0]);
			AcDbObjectId ID = CCommonFuntion::PostToModelSpace(pPline);
			if (pPline)
				pPline->close();
			ids.append(ID);
		}
		else
		{
			AcDbLine *pPline = new AcDbLine(inputpoints[i], inputpoints[i + 1]);
			AcDbObjectId ID = CCommonFuntion::PostToModelSpace(pPline);
			if (pPline)
				pPline->close();
			ids.append(ID);
		}

	}
	return ids;
}

const double _dTol = 0.0001;

template<> UINT AFXAPI HashKey<AcGePoint3d>(AcGePoint3d key)
{
	CString sPoint;
	sPoint.Format(_T("%f,%f,%f"), key.x, key.y, key.z);//��int���͵ı�����ת��CString����

	UINT iHash = (NULL == &key) ? 0 : HashKey((LPCSTR)sPoint.GetBuffer());
	return iHash;
}
template<> BOOL AFXAPI CompareElements<AcGePoint3d, AcGePoint3d>
	(const AcGePoint3d* pElement1, const AcGePoint3d* pElement2)
{
	if ((pElement1 == NULL) || (pElement2 == NULL))
		return false;

	AcGeTol gTol;
	gTol.setEqualPoint(_dTol); // Point comparison tolerance
	return (pElement1->isEqualTo(*pElement2, gTol));
}
	void CCommonFuntion::getlinepoint(AcDbObjectIdArray& lineIds, CMap<AcGePoint3d, AcGePoint3d, AcDbObjectIdArray, AcDbObjectIdArray&> &mapLines)
	{
		acdbTransactionManager->startTransaction();
		for (int i = 0; i < lineIds.length() - 1; i++)
		{
			AcDbLine* pLineA = NULL;
			if (acdbTransactionManager->getObject((AcDbObject*&)pLineA, lineIds[i], AcDb::kForRead) == Acad::eOk)
			{
				for (int j = i + 1; j < lineIds.length(); j++)
				{
					AcDbLine* pLineB = NULL;
					if (acdbTransactionManager->getObject((AcDbObject*&)pLineB, lineIds[j], AcDb::kForRead) == Acad::eOk)
					{
						AcGePoint3dArray arrPts;
						if (pLineA->intersectWith(pLineB, AcDb::kOnBothOperands, arrPts) == Acad::eOk)
						{
							if (arrPts.length() > 0)
							{
								for (int p = 0; p < arrPts.length(); p++)
								{
									AcDbObjectIdArray arrExist;
									if (mapLines.Lookup(arrPts[p], arrExist) == TRUE)
									{
										// Existing point...
										if (arrExist.contains(pLineA->objectId()) == false)
											mapLines[arrPts[p]].append(pLineA->objectId());

										if (arrExist.contains(pLineB->objectId()) == false)
											mapLines[arrPts[p]].append(pLineB->objectId());
									}
									else
									{
										// New point...
										AcDbObjectIdArray arrNewEnts;
										arrNewEnts.append(pLineA->objectId());
										arrNewEnts.append(pLineB->objectId());
										mapLines.SetAt(arrPts[p], arrNewEnts);
									}
								}
							}
						}
					}
				}
			}
		}

		acdbTransactionManager->endTransaction();

		if (mapLines.GetCount() == 0)
		{
			acutPrintf(_T("����û�з��ֽ���!\n"));
			return;
		}
	}

	void CCommonFuntion::GetCornerPoint(AcGePoint3dArray& inputptArr, AcGePoint3d& lefdownPt, AcGePoint3d& lefupPt, AcGePoint3d& rigthupPt, AcGePoint3d& rigthdownPt)
	{

		if (inputptArr.length() < 4)
			return;
		AcGePoint3d tempPoint = inputptArr[0];
		for (int i = 1; i < inputptArr.length(); i++)
		{
			if (inputptArr[i].x <= tempPoint.x&&inputptArr[i].y <= tempPoint.y)
				tempPoint = inputptArr[i];
		}
		lefdownPt = tempPoint;

		tempPoint = inputptArr[0];
		for (int i = 1; i < inputptArr.length(); i++)
		{
			if (inputptArr[i].x <= tempPoint.x&&inputptArr[i].y >= tempPoint.y)
				tempPoint = inputptArr[i];
		}
		lefupPt = tempPoint;

		tempPoint = inputptArr[0];
		for (int i = 1; i < inputptArr.length(); i++)
		{
			if (inputptArr[i].x >= tempPoint.x&&inputptArr[i].y >= tempPoint.y)
				tempPoint = inputptArr[i];
		}
		rigthupPt = tempPoint;

		tempPoint = inputptArr[0];
		for (int i = 1; i < inputptArr.length(); i++)
		{
			if (inputptArr[i].x >= tempPoint.x&&inputptArr[i].y <= tempPoint.y)
				tempPoint = inputptArr[i];
		}
		rigthdownPt = tempPoint;

	}

	void CCommonFuntion::DrowDimaligned(const AcString& setLayerName,AcGePoint3d& point1, AcGePoint3d& point2)
	{
		AcGePoint3d pt1 = point1;
		AcGePoint3d pt2 = point2;
		double movedata = pt1.distanceTo(pt2);
		
		AcGePoint3d centerpt = AcGePoint3d((pt1.x + pt2.x) / 2, (pt1.y + pt2.y) / 2, 0);

		AcGeVector3d tempVec = AcGeVector3d(pt1 - pt2);
		AcGeVector3d Linevec = tempVec.rotateBy(PI / 2, AcGeVector3d(0, 0, 1));
		Linevec.normalize();
		AcGePoint3d Pt1 = pt1.transformBy(Linevec * 500);
		AcGePoint3d Pt2 = pt2.transformBy(Linevec * 500);

		CString disText;
		int iDistance = ceil(movedata);
		disText.Format(_T("%d"), iDistance);
		
		CreateDimAligned(setLayerName,Pt1, Pt2, centerpt,disText);
	}

	void CCommonFuntion::GetAllLineIntersectpoints(AcDbObjectIdArray& inputIds, AcGePoint3dArray& outptArr)
	{
		AcDbDatabase *pDb = acdbHostApplicationServices()->workingDatabase();
		AcDbBlockTableRecordPointer pBTR(acdbSymUtil()->blockModelSpaceId(pDb), AcDb::kForWrite);

		CMap<AcGePoint3d, AcGePoint3d, AcDbObjectIdArray, AcDbObjectIdArray&> mapLines;
		CCommonFuntion::getlinepoint(inputIds, mapLines);
		POSITION pos = mapLines.GetStartPosition();
		while (pos)
		{
			AcGePoint3d ptKey(0, 0, 0);
			AcDbObjectIdArray arrEnts;
			mapLines.GetNextAssoc(pos, ptKey, arrEnts);

			AcDbPoint* ptEnt = new AcDbPoint(ptKey);
			AcDbObjectId idPointEnt;
			pBTR->appendAcDbEntity(idPointEnt, ptEnt);
			ptEnt->close();

			CString sEnts;
			for (int e = 0; e < arrEnts.length(); e++)
			{
				ACHAR pBuff[255] = _T("");
				arrEnts[e].handle().getIntoAsciiBuffer(pBuff);
				CString sBuff;
				sBuff.Format((e == arrEnts.length() - 1) ? _T("%s") : _T("%s,"), pBuff);
				sEnts += sBuff;
			}

			CString sPromptReport;
			sPromptReport.Format(_T("Point (%.4f, %.4f, %.4f) - Entities [%s]\n"), ptKey.x, ptKey.y, ptKey.z, sEnts);
			//acutPrintf(sPromptReport);
			AcGePoint3d tempPt = AcGePoint3d(ptKey.x, ptKey.y, ptKey.z);
			outptArr.append(tempPt);

		}
	}

	AcDbObjectId CCommonFuntion::CreateDimAligned(const AcString& setLayerName, const AcGePoint3d& pt1, const AcGePoint3d& pt2, const AcGePoint3d& ptLine, const ACHAR* dimText)
	{
		/* �����ע��AcDbAlignedDimension��:
		��һ��������xLine1Point����һ���ߴ�߽��ߵ���㣻�ڶ���������xLine2Point���ڶ����ߴ�߽��ߵ���㣺
		������������dimLinePoint��ͨ���ߴ��ߵ�һ�㣻���ĸ�������dimText ����ע���֣������������dimStyle ����ʽ��*/
		AcDbAlignedDimension *pDim = new AcDbAlignedDimension(pt1, pt2, ptLine, dimText, AcDbObjectId::kNull);
		AcDbObjectId dimensionId = CCommonFuntion::PostToModelSpace(pDim);

		// ���Ѿ������ı�ע�������ֵ�λ�ý����޸�
		AcDbEntity *pEnt = NULL;
		Acad::ErrorStatus es = acdbOpenAcDbEntity(pEnt, dimensionId, AcDb::kForWrite);
		if (es != eOk)
			return dimensionId;

		pEnt->setLayer(setLayerName);

		AcDbAlignedDimension *pDimension = AcDbAlignedDimension::cast(pEnt);
		if (pDimension != NULL)
		{
			// �ƶ�����λ��ǰ���������ֺͳߴ����ƶ�ʱ�Ĺ�ϵ������ָ��Ϊ���ߴ��߲����������ֺͳߴ���֮��Ӽ�ͷ��
			pDimension->setDimtmove(1);

			pDimension->setDimblk(_T("_OPEN"));//���ü�ͷ����״Ϊ�������
			pDimension->setDimexe(5);//���óߴ���߳����ߴ��߾���Ϊ400
			pDimension->setDimexo(0);//���óߴ���ߵ����ƫ����Ϊ300
									 //pDimension->setDimtad(50);//����λ�ڱ�ע�ߵ��Ϸ�
			pDimension->setDimtix(0);//���ñ�ע����ʼ�ջ����ڳߴ����֮��
			pDimension->setDimtxt(5);//��ע���ֵĸ߶�
			pDimension->setDimdec(2);
			pDimension->setDimasz(1);//��ͷ����
			pDimension->setDimlfac(1);//��������
			AcCmColor suiceng;
			suiceng.setColorIndex(3);
			pDimension->setDimclrd(suiceng);//Ϊ�ߴ��ߡ���ͷ�ͱ�ע����ָ����ɫ��0Ϊ��ͼ��
			pDimension->setDimclre(suiceng);//Ϊ�ߴ����ָ����ɫ������ɫ������������Ч����ɫ���

		}
		pEnt->close();
		return dimensionId;
	}

	//�������ֱ������������һ�����λ�ã�
	AcGePoint3d CCommonFuntion::RelativePoint(const AcGePoint3d& pt, double x, double y)
	{
		AcGePoint3d ptReturn(pt.x + x, pt.y + y, pt.z);
		return ptReturn;
	}

	double CCommonFuntion::GetLineLenth(AcDbObjectId& inputId)
	{
		AcDbEntity *pEnt = NULL;
		if (Acad::eOk != acdbOpenObject(pEnt, inputId, AcDb::kForRead))
			return 0;
		AcDbLine *Line = AcDbLine::cast(pEnt);
		AcGePoint3d startpoint;
		Line->getStartPoint(startpoint);
		AcGePoint3d endpoint;
		Line->getEndPoint(endpoint);
		double distance = startpoint.distanceTo(endpoint);
		if (pEnt)
			pEnt->close();
		if (Line)
			Line->close();
		return distance;



	}

	void CCommonFuntion::DeleteEnt(AcDbObjectIdArray& inputIds)
	{
		for (int i = 0; i < inputIds.length(); i++)
		{
			DeleteEnt(inputIds[i]);
		}


	}

	void CCommonFuntion::DeleteEnt(AcDbObjectId& inputId)
	{
		AcDbEntity *pEnt = NULL;
		if (Acad::eOk != acdbOpenObject(pEnt, inputId, AcDb::kForWrite))
			return;
		pEnt->erase();
		if (pEnt)
			pEnt->close();

	}

	void CCommonFuntion::BatchLine(AcDbObjectIdArray& inputId, std::vector<std::vector<AcDbObjectId>>& outputId, double tol /*= 0.0*/)
	{
		AcDbEntity *pEnt = NULL;
		AcDbEntity *tempEnt = NULL;
		if (inputId.length() == 1)
		{
			std::vector<AcDbObjectId> tempcompare;
			tempcompare.push_back(inputId[0]);
			outputId.push_back(tempcompare);
		}
		else
		{
			for (int i = 0; i < inputId.length(); i++)
			{
				bool tag = true;

				if (Acad::eOk != acdbOpenObject(pEnt, inputId[i], AcDb::kForRead))
					continue;
				AcDbLine *Line1 = AcDbLine::cast(pEnt);
				AcGePoint3d startpt1;
				AcGePoint3d endpt1;
				Line1->getStartPoint(startpt1);
				Line1->getEndPoint(endpt1);
				if (Line1)
					Line1->close();
				for (int j = 0; j < inputId.length(); j++)
				{

					std::vector<AcDbObjectId> tempcompare;
					if (inputId[i] == inputId[j])
						continue;
					if (Acad::eOk != acdbOpenObject(tempEnt, inputId[j], AcDb::kForRead))
						continue;

					AcDbLine *Line2 = AcDbLine::cast(tempEnt);
					AcGePoint3d startpt2;
					AcGePoint3d endpt2;
					Line2->getStartPoint(startpt2);
					Line2->getEndPoint(endpt2);
					if (Line2)
						Line2->close();

					/*	AcGePoint3dArray intersectPoints;
						tempEnt->intersectWith(pEnt, AcDb::kOnBothOperands, intersectPoints);*/
					
					if (IsParallel(startpt1, endpt1, startpt2, endpt2, tol))
					{
						tag = false;
						bool ent_tag = true;
						int ent_num = 0;
						for (int k = 0; k < outputId.size(); k++)
						{
							if (std::find(outputId[k].begin(), outputId[k].end(), inputId[i]) != outputId[k].end())//����ʵ��ID
							{

								ent_tag = false;
								ent_num = k;
								break;
							}
						}

						bool tement_tag = true;
						int tempent_num = 0;
						for (int k = 0; k < outputId.size(); k++)
						{
							if (std::find(outputId[k].begin(), outputId[k].end(), inputId[j]) != outputId[k].end())//����ʵ��ID
							{

								tement_tag = false;
								tempent_num = k;
								break;
							}
						}

						if (ent_tag)
						{
							if (tement_tag)
							{
								tempcompare.push_back(inputId[i]);
								tempcompare.push_back(inputId[j]);
								outputId.push_back(tempcompare);
							}
							else
								outputId[tempent_num].push_back(inputId[i]);
						}
						else
						{
							if (tement_tag)
								outputId[ent_num].push_back(inputId[j]);
							else
							{
								if (tempent_num != ent_num)
								{
									for (int Num = 0; Num < outputId[tempent_num].size(); Num++)
									{
										outputId[ent_num].push_back(outputId[tempent_num][Num]);
									}
									std::remove(outputId.begin(), outputId.end(), outputId[tempent_num]);
								}
							}
						}
					}
					if (tempEnt)
						tempEnt->close();
				}
				if (tag)
				{
					std::vector<AcDbObjectId> tempvector;
					tempvector.push_back(inputId[i]);
					outputId.push_back(tempvector);
				}

				if (pEnt)
					pEnt->close();
			}
		}
	}

	bool CCommonFuntion::IsParallel(AcGePoint3d& pt1, AcGePoint3d& pt2, AcGePoint3d& pt3, AcGePoint3d& pt4, double tol/* = 0.0*/)
	{
		
		double x1 = pt1.x;
		double y1 = pt1.y;
		double x2 = pt2.x;
		double y2 = pt2.y;
		double x3 = pt3.x;
		double y3 = pt3.y;
		double x4 = pt4.x;
		double y4 = pt4.y;
		double ax = x2 - x1;//vector of line 1
		double ay = y2 - y1;
		double bx = x4 - x3;//vector of line 2
		double by = y4 - y3;
		double dvalue = (ax*by - ay*bx) / (ax*bx + ay*by);
		if (abs(dvalue) <= tol)
			return true;
		else
			return false;
	}

	AcDbExtents CCommonFuntion::GetBigExtents(AcDbObjectIdArray& inputId)
	{
		AcDbExtents extents;
		for (int i = 0; i < inputId.length(); i++)
		{
			AcDbExtents tempextents;
			DBHelper::GetEntityExtents(tempextents,inputId[i]);
			extents.addExt(tempextents);
		}

		return extents;
	}

	bool CCommonFuntion::IsIntersectEnt(const ACHAR *LineLayerName, AcGePoint3d& pt1, AcGePoint3d& pt2, AcRxClass* classDesc)
	{
		LineSelect ls;
		std::set<AcRxClass*> filter;
		filter.insert(classDesc);
		ls.init(&filter);

		AcGeVector3d vec = AcGeVector3d(pt1 - pt2);
		AcGeVector3d projectvec = vec.normalize();

		AcGePoint3d startpoint = pt1;
		startpoint.transformBy(-1 * projectvec * 10);
		AcGePoint3d endpoint = pt2;
		endpoint.transformBy(projectvec * 10);

		//����֮��ѡ��ʵ��
		AcDbObjectIdArray outIds;
		outIds = ls.select(AcGePoint2d(startpoint[X], startpoint[Y]), AcGePoint2d(endpoint[X], endpoint[Y]));

		for (int i = 0; i < outIds.length(); i++)
		{
			
			if (IsEntInLayer(outIds[i], LineLayerName))	
				return true;		
		}
		return false;
	}

	void CCommonFuntion::GetALLTextStyle(std::vector<CString>& textStyle)
	{
		textStyle.clear();
		AcDbTextStyleTable *pTextStyleTbl = NULL;
		acdbHostApplicationServices()->workingDatabase()->getSymbolTable(pTextStyleTbl, AcDb::kForRead);
		AcDbTextStyleTableIterator *pIt = NULL;
		pTextStyleTbl->newIterator(pIt);
		for (; !pIt->done(); pIt->step())
		{
			AcDbTextStyleTableRecord *pRcd = NULL;
			if (pIt->getRecord(pRcd, AcDb::kForRead) == Acad::eOk)
			{
				TCHAR *szName = NULL;
				pRcd->getName(szName);
				if (_tcslen(szName) > 0)
				{
					textStyle.push_back(szName);

				}

				acutDelString(szName);
				pRcd->close();

			}
		}
		delete pIt;
		pTextStyleTbl->close();
	}

	void CCommonFuntion::Split(const std::string& src, const std::string& separator, std::vector<std::string>& dest)
	{
		//����1��Ҫ�ָ���ַ���������2����Ϊ�ָ������ַ�������3����ŷָ����ַ�����vector����

		std::string str = src;
		std::string substring;
		std::string::size_type start = 0, index;
		dest.clear();
		index = str.find_first_of(separator, start);
		do
		{
			if (index != std::string::npos)
			{
				substring = str.substr(start, index - start);
				dest.push_back(substring);
				start = index + separator.size();
				index = str.find(separator, start);
				if (start == std::string::npos) break;
			}
		} while (index != std::string::npos);

		//the last part
		substring = str.substr(start);
		dest.push_back(substring);
	}

	void CCommonFuntion::Split(const std::string& src, const std::string& separator, std::string& dest)
	{
		//����1��Ҫ�ָ���ַ���������2����Ϊ�ָ������ַ�������3����ŷָ����ַ�����

		std::string str = src;
		std::string substring;
		std::string::size_type start = 0, index;
		index = str.find_first_of(separator, start);
		do
		{
			if (index != std::string::npos)
			{
				substring = str.substr(start, index - start);
				dest += "\n" + substring;
				start = index + separator.size();
				index = str.find(separator, start);
				if (start == std::string::npos) break;
			}
		} while (index != std::string::npos);

		//the last part
		substring = str.substr(start);
		dest += "\n" + substring;
	}

	bool CCommonFuntion::is_str_utf8(const char* str)
	{
		unsigned int nBytes = 0;//UFT8����1-6���ֽڱ���,ASCII��һ���ֽ�
		unsigned char chr = *str;
		bool bAllAscii = true;
		for (unsigned int i = 0; str[i] != '\0'; ++i)
		{
			chr = *(str + i);
			//�ж��Ƿ�ASCII����,�������,˵���п�����UTF8,ASCII��7λ����,���λ���Ϊ0,0xxxxxxx
			if (nBytes == 0 && (chr & 0x80) != 0) {
				bAllAscii = false;
			}
			if (nBytes == 0) {
				//�������ASCII��,Ӧ���Ƕ��ֽڷ�,�����ֽ���
				if (chr >= 0x80) {
					if (chr >= 0xFC && chr <= 0xFD) {
						nBytes = 6;
					}
					else if (chr >= 0xF8) {
						nBytes = 5;
					}
					else if (chr >= 0xF0) {
						nBytes = 4;
					}
					else if (chr >= 0xE0) {
						nBytes = 3;
					}
					else if (chr >= 0xC0) {
						nBytes = 2;
					}
					else {
						return false;
					}
					nBytes--;
				}
			}
			else {
				//���ֽڷ��ķ����ֽ�,ӦΪ 10xxxxxx
				if ((chr & 0xC0) != 0x80) {
					return false;
				}
				//����Ϊ��Ϊֹ
				nBytes--;
			}
		}
		//Υ��UTF8�������
		if (nBytes != 0) {
			return false;
		}
		if (bAllAscii) { //���ȫ������ASCII, Ҳ��UTF8
			return true;
		}
		return true;
	}

	bool CCommonFuntion::is_str_gbk(const char* str)
	{
		unsigned int nBytes = 0;//GBK����1-2���ֽڱ���,�������� ,Ӣ��һ��
		unsigned char chr = *str;
		bool bAllAscii = true; //���ȫ������ASCII,
		for (unsigned int i = 0; str[i] != '\0'; ++i) {
			chr = *(str + i);
			if ((chr & 0x80) != 0 && nBytes == 0) {// �ж��Ƿ�ASCII����,�������,˵���п�����GBK
				bAllAscii = false;
			}
			if (nBytes == 0) {
				if (chr >= 0x80) {
					if (chr >= 0x81 && chr <= 0xFE) {
						nBytes = +2;
					}
					else {
						return false;
					}
					nBytes--;
				}
			}
			else {
				if (chr < 0x40 || chr>0xFE) {
					return false;
				}
				nBytes--;
			}//else end
		}
		if (nBytes != 0) {   //Υ������
			return false;
		}
		if (bAllAscii) { //���ȫ������ASCII, Ҳ��GBK
			return true;
		}
		return true;
	}

	bool CCommonFuntion::InserBlockFormDWG(const CString& dwgblockname, const CString& dwgpath, AcGePoint3d& inserpoint, AcDbObjectId& outinserblockid)
	{
		//��ȡָ�����ⲿͼֽ�е����ݿ�
		AcDbDatabase *pDwg = new AcDbDatabase(Adesk::kFalse); // �ⲿͼ�����ݿ�
		if (Acad::eOk != pDwg->readDwgFile(dwgpath))
		{
			acedAlert(_T("��ȡDWG�ļ�ʧ��!"));
			return false;
		}

		//�ж��ڵ�ǰͼֽ���Ƿ��Ѿ�������
		AcDbDatabase *pCurDb = acdbHostApplicationServices()->workingDatabase();
		AcDbBlockTable *pBlkTab;
		AcDbObjectId blockId;
		pCurDb->getBlockTable(pBlkTab, AcDb::kForRead);

		//��ǰͼֽ��δ����ÿ�
		if (!pBlkTab->has(dwgblockname))
		{
			pBlkTab->close();

			//��ȡ�ⲿ���ݿ��иÿ�������ʵ���id
			AcDbObjectIdArray objIds;
			getIdsByDwgBlkName(pDwg, dwgblockname, objIds);//�ú��������Լ���д�����ں������

														//���ⲿ���ݿ��ж�ӦID��ʵ��д�뵽pDb���ݿ��У��ò�����������ѡ�����������Ҫ������ʵ��

			AcDbDatabase *pDb = NULL;
			pDwg->wblock(pDb, objIds, AcGePoint3d(0, 0, 0));

			//��pDb�е�����ʵ����뵽��ǰͼ����
			Acad::ErrorStatus es = pCurDb->insert(blockId, dwgblockname, pDb, Adesk::kFalse);
			if (Acad::eOk != es)
			{
				acutPrintf(_T("\n��������"));
				return false;
			}
			delete pDb;

		}
		//��ǰͼֽ���Ѿ������˸ÿ�
		else
		{
			pBlkTab->getAt(dwgblockname, blockId);
		}
		pBlkTab->close();
		//���������
		AcDbBlockReference *pBlkRef = new AcDbBlockReference(inserpoint, blockId);

		//���ģ�Ϳռ����¼������������
		AcDbBlockTable *pBlockTab;
		acdbHostApplicationServices()->workingDatabase()->getBlockTable(pBlockTab, AcDb::kForRead);
		AcDbBlockTableRecord *pBlockTabRcd;
		pBlockTab->getAt(ACDB_MODEL_SPACE, pBlockTabRcd, AcDb::kForWrite);
		pBlockTab->close();
		pBlockTabRcd->appendAcDbEntity(pBlkRef);

		//�������
		AcDbBlockTableRecord *pBlkDefRcd;
		acdbOpenObject(pBlkDefRcd, blockId, AcDb::kForRead);
		if (pBlkDefRcd->hasAttributeDefinitions())
		{

			//��������¼
			AcDbBlockTableRecordIterator *pItr;
			pBlkDefRcd->newIterator(pItr);
			for (pItr->start(); !pItr->done(); pItr->step())
			{

				//��������ԣ�����뵽Ҫ��������Զ�����
				AcDbEntity *pEnt;
				pItr->getEntity(pEnt, AcDb::kForRead);
				AcDbAttributeDefinition *pAttDef = AcDbAttributeDefinition::cast(pEnt);
				if (pAttDef != NULL)
				{
					AcDbAttribute *pAtt = new AcDbAttribute();
					pAtt->setPropertiesFrom(pAttDef);
					pAtt->setInvisible(pAttDef->isInvisible());
					pAtt->setHorizontalMode(pAttDef->horizontalMode());
					pAtt->setVerticalMode(pAttDef->verticalMode());

					AcGePoint3d basePt = pAttDef->position();

					AcGePoint3d movept = inserpoint;
					AcGeVector3d vec = AcGeVector3d(basePt - inserpoint);
					movept.transformBy(vec);

					basePt.x += movept.x;
					basePt.y += movept.y;
					basePt.z += movept.z;
					pAtt->setPosition(basePt);

					pAtt->setHeight(pAttDef->height());
					pAtt->setTextStyle(pAttDef->textStyle());

					//�������Ե�Tag��TextString

					pAtt->setTag(pAttDef->tag());
					pAtt->setTextString(pAttDef->textString());
					pAtt->setFieldLength(pAttDef->fieldLength());

					//�����Զ���׷�ӵ��������

					pBlkRef->appendAttribute(pAtt);
					pAtt->close();
					pEnt->close();
				}
			}
			delete pItr;
		}
		pBlkDefRcd->close();
		pBlockTabRcd->close();
		pBlkRef->close();

		return true;

	}

	int CCommonFuntion::getIdsByDwgBlkName(AcDbDatabase *pDwg, CString strBlkName, AcDbObjectIdArray &objIds)
	{

		//��ָ���鶨��Ŀ���¼
		AcDbBlockTable *pBlkTbl = NULL;
		pDwg->getBlockTable(pBlkTbl, AcDb::kForRead);
		AcDbBlockTableRecord* pBlkTblRcd;
		Acad::ErrorStatus es = pBlkTbl->getAt(strBlkName, pBlkTblRcd, AcDb::kForRead);
		pBlkTbl->close();
		if (es != Acad::eOk)
		{
			return RTNONE;
		}

		//��������¼�е�ʵ��
		AcDbBlockTableRecordIterator* pItr;
		pBlkTblRcd->newIterator(pItr);
		AcDbEntity* pEnt = NULL;

		for (pItr->start(); !pItr->done(); pItr->step())
		{
			Acad::ErrorStatus es = pItr->getEntity(pEnt, AcDb::kForWrite);
			if (Acad::eOk == es)
			{

				//��ʵ��Id���뵽Id������
				objIds.append(pEnt->objectId());
			}
			pEnt->close();
		}
		delete pItr;
		pBlkTblRcd->close();

		return RTNORM;
	}

	bool CCommonFuntion::IsEntInLayer(AcDbObjectId& EntId, const ACHAR* layername)
	{
		AcDbEntity *pEnt = NULL;
		if (Acad::eOk != acdbOpenObject(pEnt, EntId, AcDb::kForRead))
			return false;
		ACHAR *LayerName = pEnt->layer();
		if (wcscmp(LayerName, layername) == 0)
		{
			pEnt->close();
			return true;

		}
		return false;
	}

	double CCommonFuntion::GetLineDistance(AcDbObjectId& Line1, AcDbObjectId& Line2)
	{
		AcGeLineSeg2d geLine1 = GetGeLineObj(Line1);
		AcGeLineSeg2d geLine2 = GetGeLineObj(Line2);

		//���㲢�������֮�����̾���
		double distance = geLine1.distanceTo(geLine2);
		return distance;
	}

	//���������ʵ��ID�����ͬ������AcGeLineSeg2d����
	AcGeLineSeg2d CCommonFuntion::GetGeLineObj(AcDbObjectId lineId)
	{
		AcGeLineSeg2d geLine; //AcGeLineSeg2d:�ڶ�ά�ռ��б�ʾһ���н���߶Ρ�
		AcDbLine *pLine = NULL;
		if (acdbOpenObject(pLine, lineId, AcDb::kForRead) == Acad::eOk)
			//ָ��pLineָ��򿪵Ķ���lineId
		{
			geLine.set(ToPoint2d(pLine->startPoint()),
				ToPoint2d(pLine->endPoint()));
			pLine->close();
		}
		return geLine;
	}

	// ��ά�����ά��֮���ת��
	AcGePoint2d CCommonFuntion::ToPoint2d(const AcGePoint3d &point3d)
	{
		return AcGePoint2d(point3d.x, point3d.y);
	}

	bool CCommonFuntion::PathernMatch(char *pat, char *str)
	{
		char *s = NULL;
		char *p = NULL;
		bool star = false;
		bool bBreak = false;
		do
		{
			bBreak = false;
			for (s = str, p = pat; *s; ++s, ++p)
			{
				switch (*p)
				{
				case '?':
					break;
				case '*':
					star = true; //����*ƥ���
					str = s;
					pat = p;
					if (!*++pat)
						return true;
					bBreak = true; //�˳�ѭ��
					break;
				default:
					if (*s != *p)
					{
						if (!star)
							return false;
						str++;
						bBreak = true;
					}
					break;
				}
				if (bBreak) //�˳�ѭ�� ���¿�ʼѭ��
					break;
			}
			if (bBreak == false)
			{
				if (*p == '*')
					++p;
				return (!*p);
			}
		} while (true);
	}

	double CCommonFuntion::distance(AcGePoint3d& p, AcGePoint3d& p1) 
	{
		return hypot(p.x - p1.x, p.y - p1.y);
	}

	double CCommonFuntion::PointToSegDist(AcGePoint3d& linept1, AcGePoint3d& linept2, AcGePoint3d& pt)
	{

		double ans = 0;
		double a, b, c;
		a = distance(linept1, linept2);
		b = distance(linept1, pt);
		c = distance(linept2, pt);
		if (abs(a-(c + b))<=0.1)
		{
			//�����߶���
			ans = 0;
			return ans;
		}
		if (a <= 0.00001)
		{
			//�����߶Σ���һ����
			ans = b;
			return ans;
		}
		if (c*c >= a*a + b*b)
		{
			//���ֱ�������λ�۽������Σ�p1Ϊֱ�ǻ�۽�
			ans = b;
			return ans;
		}
		if (b * b >= a * a + c * c)
		{
			// ���ֱ�������λ�۽������Σ�p2Ϊֱ�ǻ�۽�
			ans = c;
			return ans;
		}
		// �����������Σ����������εĸ�
		double p0 = (a + b + c) / 2;// ���ܳ�
		double s = sqrt(p0 * (p0 - a) * (p0 - b) * (p0 - c));// ���׹�ʽ�����
		ans = 2 * s / a;// ���ص㵽�ߵľ��루���������������ʽ��ߣ�
		return ans;
	}

	void CCommonFuntion::setLayer(const CString& layerName, const int& layerColor)
	{
		// ��õ�ǰͼ�εĲ��
		AcDbLayerTable *pLayerTbl = NULL;
		acdbHostApplicationServices()->workingDatabase()->getLayerTable(pLayerTbl, AcDb::kForWrite);

		// �Ƿ��Ѿ�����ָ���Ĳ���¼
		if (pLayerTbl->has(layerName))
		{
			pLayerTbl->close();
			return;
		}

		// �����µĲ���¼
		AcDbLayerTableRecord *pLayerTblRcd = new AcDbLayerTableRecord();
		pLayerTblRcd->setName(layerName);

		// ������ɫ,����������ԣ����͵ȣ�����ȱʡֵ
		AcCmColor color;
		color.setColorIndex(layerColor);
		pLayerTblRcd->setColor(color);

		// ���½��Ĳ���¼��ӵ������
		AcDbObjectId layerTblRcdId;
		pLayerTbl->add(layerTblRcdId, pLayerTblRcd);
		acdbHostApplicationServices()->workingDatabase()->setClayer(layerTblRcdId);
		pLayerTblRcd->close();
		pLayerTbl->close();
	}