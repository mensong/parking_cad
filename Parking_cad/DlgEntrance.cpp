// (C) Copyright 2002-2007 by Autodesk, Inc. 
//
// Permission to use, copy, modify, and distribute this software in
// object code form for any purpose and without fee is hereby granted, 
// provided that the above copyright notice appears in all copies and 
// that both that copyright notice and the limited warranty and
// restricted rights notice below appear in all supporting 
// documentation.
//
// AUTODESK PROVIDES THIS PROGRAM "AS IS" AND WITH ALL FAULTS. 
// AUTODESK SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTY OF
// MERCHANTABILITY OR FITNESS FOR A PARTICULAR USE.  AUTODESK, INC. 
// DOES NOT WARRANT THAT THE OPERATION OF THE PROGRAM WILL BE
// UNINTERRUPTED OR ERROR FREE.
//
// Use, duplication, or disclosure by the U.S. Government is subject to 
// restrictions set forth in FAR 52.227-19 (Commercial Computer
// Software - Restricted Rights) and DFAR 252.227-7013(c)(1)(ii)
// (Rights in Technical Data and Computer Software), as applicable.
//

//-----------------------------------------------------------------------------
//----- DlgEntrance.cpp : Implementation of CDlgEntrance
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "resource.h"
#include "DlgEntrance.h"
#include "DBHelper.h"
#include <json/json.h>
#include "ModulesManager.h"
#include "Convertor.h"
#include "RTreeEx.h"
#include "GeHelper.h"
#include "EquipmentroomTool.h"
#include "LibcurlHttp.h"
#include "ParkingLog.h"

CString CDlgEntrance::ms_sBasementHeight;
CString CDlgEntrance::ms_sEntranceWidth;
//-----------------------------------------------------------------------------
IMPLEMENT_DYNAMIC (CDlgEntrance, CAcUiDialog)

BEGIN_MESSAGE_MAP(CDlgEntrance, CAcUiDialog)
	ON_MESSAGE(WM_ACAD_KEEPFOCUS, OnAcadKeepFocus)
	ON_WM_SIZE()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDOK, &CDlgEntrance::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON_GETENTRANCEPL, &CDlgEntrance::OnBnClickedButtonGetentrancepl)
END_MESSAGE_MAP()

//-----------------------------------------------------------------------------
CDlgEntrance::CDlgEntrance (CWnd *pParent /*=NULL*/, HINSTANCE hInstance /*=NULL*/) : CAcUiDialog (CDlgEntrance::IDD, pParent, hInstance) {
}

static void __smartLog(bool*& data, bool isDataValid)
{
	if (isDataValid)
	{
		bool end = (*data);
		if (!end)
			CParkingLog::AddLogA("DEBUG_��֧�ֳ���ִ�еĲ���", 0, "CDlgEntrance::DoDataExchange");
		delete data;
	}
}

//-----------------------------------------------------------------------------
void CDlgEntrance::DoDataExchange (CDataExchange *pDX) {
	Smart<bool*> end(new bool(false), __smartLog);
	CAcUiDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BUTTON_GETENTRANCEPL, m_btn_getEntrancePline);
	DDX_Control(pDX, IDC_EDIT_ENTRANCEDATA, m_show_entranceData);
	DDX_Control(pDX, IDC_EDIT_BASEMENT_HEIGHT, m_edit_basementHeight);
	DDX_Control(pDX, IDC_EDIT_ENTRANCE_WIDTH, m_edit_entranceWidth);
	*(end()) = true;
}

//-----------------------------------------------------------------------------
//----- Needed for modeless dialogs to keep focus.
//----- Return FALSE to not keep the focus, return TRUE to keep the focus
LRESULT CDlgEntrance::OnAcadKeepFocus (WPARAM, LPARAM) {
	return (TRUE) ;
}

BOOL CDlgEntrance::OnInitDialog()
{
	CAcUiDialog::OnInitDialog();

	DlgHelper::AdjustPosition(this, DlgHelper::TOP_LEFT);

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	m_edit_basementHeight.SetWindowText(ms_sBasementHeight);
	m_edit_entranceWidth.SetWindowText(ms_sEntranceWidth);
	return TRUE;
}


void CDlgEntrance::OnBnClickedOk()
{
	if (m_targetId.isNull())
	{
		acedAlert(_T("û��ѡ���������ɳ���ڵĶ��߶�!"));
		return;
	}
	CString sBasementHeight;
	m_edit_basementHeight.GetWindowText(sBasementHeight);
	if (sBasementHeight.IsEmpty())
	{
		acedAlert(_T("û����������Ҹ߶ȵĲ�����Ϣ!"));
		return;
	}
	CString sEtranceWidth;
	m_edit_entranceWidth.GetWindowText(sEtranceWidth);
	if (sEtranceWidth.IsEmpty())
	{
		acedAlert(_T("û���������ڿ�ȵĲ�����Ϣ!"));
		return;
	}
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	double dBasementHeight = _tstof(sBasementHeight.GetString());
	double dEtranceWidth = _tstof(sEtranceWidth.GetString());
	Doc_Locker doc_locker;
	CEquipmentroomTool::creatLayerByjson("lane_center_line_and_driving_direction");
	CEquipmentroomTool::creatLayerByjson("entrance");
	creatEntrance(dBasementHeight, dEtranceWidth);

	std::vector<AcDbEntity*> blockEnts;
	for (int i = 0; i < m_addBlockIds.length(); i++)
	{
		AcDbEntity *pEnt = NULL;
		acdbOpenObject(pEnt, m_addBlockIds[i], AcDb::kForWrite);
		//�ж��Զ���ʵ�������
		if (pEnt == NULL)
			continue;
		blockEnts.push_back(pEnt);
	}
	if (blockEnts.size() < 1)
		return;

	CString sBlockName = _T("entrance");
	int iCount = 0;
	while (CEquipmentroomTool::hasNameOfBlock(sBlockName))
	{
		iCount++;
		CString sCount;
		sCount.Format(_T("%d"), iCount);
		sBlockName += sCount;
	}
	Acad::ErrorStatus es = DBHelper::CreateBlock(sBlockName, blockEnts);
	if (es != eOk)
	{
		acutPrintf(_T("\n���������ͼ��ʧ�ܣ�"));
		return;
	}
	for (int j = 0; j < blockEnts.size(); j++)
	{
		blockEnts[j]->erase();
		blockEnts[j]->close();
	}
	AcDbObjectId blockId;
	DBHelper::InsertBlkRef(blockId, sBlockName, AcGePoint3d::kOrigin);
	CString sEntranceLayer(CEquipmentroomTool::getLayerName("entrance").c_str());
	es = DBHelper::AddXRecord(blockId, _T("ʵ��"), _T("�����"));
	CEquipmentroomTool::setEntToLayer(blockId, sEntranceLayer);

	CAcUiDialog::OnOK();
	ms_sBasementHeight = sBasementHeight;
	ms_sEntranceWidth = sEtranceWidth;
}


void CDlgEntrance::OnBnClickedButtonGetentrancepl()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	Doc_Locker doc_locker;
	HideDialogHolder holder(this);
	ads_name ename; ads_point pt;
	AcGePoint2dArray GetretreatlinePts;//װȡȥ���ص���Ч��
	if (acedEntSel(_T("\n��ѡ���������ɳ���ڵĶ����ʵ��:"), ename, pt) != RTNORM)
	{
		return;
	}
	acutPrintf(_T("\n"));
	AcDbObjectId id;
	acdbGetObjectId(id, ename);
	AcDbEntity *pEnt;
	//���������Ҫ�жϲ����ɹ����
	acdbOpenObject(pEnt, id, AcDb::kForRead);
	std::vector<AcGePoint2d> allPoints;//�õ������е�
	if (pEnt->isKindOf(AcDbPolyline::desc()))
	{
		AcDbVoidPtrArray entsTempArray;
		AcDbPolyline *pPline = AcDbPolyline::cast(pEnt);
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
				AcGePoint3dArray result = GeHelper::CalcArcFittingPoints(arc, 3);
				for (int x = 0; x < result.length(); x++)
				{
					AcGePoint2d onArcpoint(result[x].x, result[x].y);
					allPoints.push_back(onArcpoint);
				}
			}
		}
		for (int x = 0; x < allPoints.size(); x++)
		{
			if (GetretreatlinePts.contains(allPoints[x]))
			{
				continue;
			}
			GetretreatlinePts.append(allPoints[x]);
		}
	}
	pEnt->close();
	CString sEntranceData;
	for (int yy = 0; yy < GetretreatlinePts.length(); yy++)
	{
		CString tempStr_X;
		CString tempStr_Y;
		tempStr_X.Format(_T("%.2f"), GetretreatlinePts[yy].x);
		tempStr_Y.Format(_T("%.2f"), GetretreatlinePts[yy].y);

		sEntranceData += CString(_T("(")) + tempStr_X + _T(",") + tempStr_Y + _T(")");
		m_show_entranceData.SetWindowText(sEntranceData);
	}
	m_targetId = id;
}

void CDlgEntrance::creatEntrance(double& dBasementHeight, double& dEntranceWidth)
{
	AcDbObjectIdArray useIds = CDlgEntrance::explodeEnty(m_targetId);//��ʵ�����ը������
	if (useIds.isEmpty())
	{
		return;
	}
	if (!addDim(useIds, dBasementHeight, dEntranceWidth))
	{
		actrTransactionManager->abortTransaction();
		return;
	}

	AcDbEntity *pEnt = NULL;
	AcDbEntity *tempEnt = NULL;
	AcDbObjectIdArray tempIds;
	AcDbObjectIdArray insortIds;

	//���û�ѡ���ʵ�壬��������Ϊ���ݣ�����洢
	std::vector<std::vector<AcDbObjectId>> operaIds;
	CDlgEntrance::BatchStorageEnt(useIds, operaIds);
	double changdistance = (dEntranceWidth * 1000) / 2;//��������
	for (int i = 0; i < operaIds.size(); i++)
	{
		if (operaIds[i].empty())
			continue;
		//���������������Ա����������ߣ����������㴦��
		/*
		*    ____________
		*    ������������       --> ____________
		*    ��������-|��| |	    ������������
		              |  | |		��������-   | |
		              |  | |		         |  | |
		              |  | |				 |  | |
		              |  | |				 |  | |
		*									 |  | |
		*/
		AcDbObjectIdArray GuideIds;//GuideIdsΪ���߳�����id
		CDlgEntrance::GenerateGuides(changdistance, operaIds[i], GuideIds);
		//��һ������
		/*
		*   ____________		   ____________
		*   ������������      --> ������������ |
		*   -������-   | |        -������-   | |
		               |  | |  	  	  	  |  | |
		               |  | |  	  		  |  | |
		               |  | |  			  |  | |
		               |  | |  			  |  | |
		*
		*/
		double inputdistance = changdistance;
		CDlgEntrance::MultipleCycles(inputdistance, GuideIds);
		
		std::vector<AcGePoint3dArray> allLinePts;
		for (int count = 0; count<GuideIds.length(); count++)
		{
			AcGePoint3dArray linePts;
			AcDbEntity *pEnt = NULL;
			if (Acad::eOk != acdbOpenObject(pEnt, GuideIds[count], AcDb::kForRead))
				continue;
			if (pEnt->isKindOf(AcDbLine::desc()))
			{
				AcDbLine *pLine = AcDbLine::cast(pEnt);
				AcGePoint3d startpoint = pLine->startPoint();
				AcGePoint3d endpoint = pLine->endPoint();
				linePts.append(startpoint);
				linePts.append(endpoint);
				int oo = 0;
			}
			allLinePts.push_back(linePts);
		}


		for (int num = 0; num < operaIds[i].size(); num++)
			GuideIds.append(operaIds[i][num]);

		//���з�մ���
		/*
		*    ____________	    | ____________
		*   ������������ |  --> |������������ |
		*   -������-   | |      |-������-   | |
		               |  | |       	    | | |
		               |  | |               | | |
		               |  | |               | | |
		               |  | |               | | |
		*                                   -----
		*/
		CDlgEntrance::ConnectionPoint(GuideIds);
		CString sEntranceLayer(CEquipmentroomTool::getLayerName("entrance").c_str());
		AcDbObjectIdArray useforGetPtsID;
		for (int g = 0; g<GuideIds.length(); g++)
		{
			//CEquipmentroomTool::setEntToLayer(GuideIds[g], sEntranceLayer);
			if (useIds.contains(GuideIds[g]))
			{
				continue;
			}
			useforGetPtsID.append(GuideIds[g]);
		}
		int count = useforGetPtsID.length();
		addWideDim(dEntranceWidth);
		CDlgEntrance::creatPlinePoints(useforGetPtsID);
		CDlgEntrance::changeLine2Polyline(useforGetPtsID);
	}
}

AcDbObjectIdArray CDlgEntrance::explodeEnty(AcDbObjectId& entId)
{
	AcDbObjectIdArray returnIds;
	AcDbEntity *pEnt = NULL;
	if (Acad::eOk != acdbOpenObject(pEnt, entId, AcDb::kForWrite))
		return returnIds;
	if (pEnt->isKindOf(AcDbPolyline::desc()))
	{
		//ը��
		AcDbVoidPtrArray ents;
		pEnt->explode(ents);
		//ѭ������ը����ʵ��
		for (int i = 0; i < ents.length(); ++i)
		{
			AcDbEntity* pSubEnt = (AcDbEntity*)ents[i];
			AcDbEntity* entity = AcDbEntity::cast(pSubEnt);
			AcDbObjectId entId;
			DBHelper::AppendToDatabase(entId, entity);

			returnIds.append(entId);
			entity->close();
		}
		pEnt->erase();
	}
	if (pEnt)
		pEnt->close();
	return returnIds;
}

bool CDlgEntrance::addDim(const AcDbObjectIdArray entIds, const double  dHeight, const double dWidth)
{
	CString sEntranceLayer(CEquipmentroomTool::getLayerName("entrance").c_str());
	CString sLaneLayer(CEquipmentroomTool::getLayerName("lane_center_line_and_driving_direction").c_str());
	for (int i = 0; i < entIds.length(); i++)
	{
		AcDbEntity *pEnt = NULL;
		if (acdbOpenObject(pEnt, entIds[i], AcDb::kForRead) != eOk)
			continue;

		if (pEnt->isKindOf(AcDbArc::desc()))
		{
			AcDbArc *pArc = AcDbArc::cast(pEnt);

			AcGeCircArc3d pGeArc(
				pArc->center(),
				pArc->normal(),
				pArc->normal().perpVector(),
				pArc->radius(),
				pArc->startAngle(),
				pArc->endAngle());
			AcGePoint3dArray result = GeHelper::CalcArcFittingPoints(pGeArc, 1);
			AcGePoint3d endPt;
			AcGePoint3d startPt;
			pArc->getEndPoint(endPt);
			pArc->getStartPoint(startPt);
			AcGePoint3d centerPt = pArc->center();
			//AcDbObjectId EquipmentId = pEnt->objectId();
			//EquipmentIds.append(EquipmentId);
			AcGeVector3d endPtMoveVec = endPt - centerPt;
			AcGeVector3d unitEndVec = endPtMoveVec.normalize();
			//endPt.transformBy(unitEndVec * 1000);
			AcGeVector3d startPtMoveVec = startPt - centerPt;
			AcGeVector3d unitStartVec = startPtMoveVec.normalize();
			AcGePoint3d onArcPt = result[1];
			//startPt.transformBy(unitStartVec * 1000);
			onArcPt.transformBy(unitStartVec * 1000);

			AcDbBlockTable *pBlockTable;//������ָ��
			acdbHostApplicationServices()->workingDatabase()
				->getSymbolTable(pBlockTable, AcDb::kForRead);
			AcDbBlockTableRecord *pBlockTableRecord;
			pBlockTable->getAt(ACDB_MODEL_SPACE, pBlockTableRecord,
				AcDb::kForWrite);
			pBlockTable->close();
			AcDbArcDimension *pDim1 = new AcDbArcDimension(centerPt, startPt, endPt, onArcPt);//AcDbRadialDimension;
			double showLength = (2400 * 2 + (dHeight * 1000 - 280) / 0.12) / 1000;
			//double realLength = showLength * 1000000;
			CString sEntranceLength;
			//sEntranceLength.Format(_T("%.1f"), showLength);
			sEntranceLength.Format(_T("%.2f"), showLength);
			pDim1->setDimensionText(sEntranceLength);
			//pDim1->setDimtix(0);//���ñ�ע����ʼ�ջ����ڳߴ����֮��
			pDim1->setDimtxt(500);
			pDim1->setDimexo(0);
			pDim1->setDimexe(300);
			AcCmColor suiceng;
			suiceng.setColorIndex(3);
			pDim1->setDimclrd(suiceng);//Ϊ�ߴ��ߡ���ͷ�ͱ�ע����ָ����ɫ��0Ϊ��ͼ��
			pDim1->setDimclre(suiceng);//Ϊ�ߴ����ָ����ɫ������ɫ������������Ч����ɫ���
			AcCmColor textcolor;
			textcolor.setColorIndex(7);
			pDim1->setDimclrt(textcolor);
			AcDbObjectId Id;
			pBlockTableRecord->appendAcDbEntity(Id, pDim1);
			pBlockTableRecord->close();
			pDim1->close();
			CEquipmentroomTool::setEntToLayer(Id, sEntranceLayer);
			m_addBlockIds.append(Id);
		}
		else if (pEnt->isKindOf(AcDbLine::desc()))
		{
			AcDbLine *pLine = AcDbLine::cast(pEnt);
			AcGePoint3d endPt;
			AcGePoint3d startPt;
			pLine->getEndPoint(endPt);
			pLine->getStartPoint(startPt);

			//AcGePoint3d centerpt = AcGePoint3d((startPt.x + endPt.x) / 2, (startPt.y + endPt.y) / 2, 0);

			AcGeVector3d tempVec = AcGeVector3d(startPt - endPt);
			AcGeVector3d Linevec = tempVec.rotateBy(ARX_PI / 2, AcGeVector3d(0, 0, 1));
			Linevec.normalize();
			//startPt.transformBy(Linevec * 50);
			//endPt.transformBy(Linevec * 50);
			AcGePoint3d Pt1 = startPt;
			Pt1.transformBy(Linevec * 1000);
			double showLength = (3600 * 2 + (dHeight * 1000 - 540) / 0.15) / 1000;
			//double realLength = showLength * 1000000;
			CString sEntranceLength;
			//sEntranceLength.Format(_T("%.1f"), showLength);
			sEntranceLength.Format(_T("%.2f"), showLength);
			AcDbObjectId dimId = CDlgEntrance::creatDim(startPt, endPt, Pt1, sEntranceLength);
			CEquipmentroomTool::setEntToLayer(dimId, sEntranceLayer);
			m_addBlockIds.append(dimId);
		}
		pEnt->close();
		m_addBlockIds.append(entIds[i]);
		CEquipmentroomTool::setEntToLayer(entIds[i], sLaneLayer);

	}
	return true;
}

//�½���ע������ʽ�޸�
AcDbObjectId CDlgEntrance::creatDim(const AcGePoint3d& pt1, const AcGePoint3d& pt2, const AcGePoint3d& pt3, const CString sLegth)
{
	CString str = _T("����ڱ�ע");
	creatDimStyle(str);
	AcDbObjectId dimStyleId;
	////��õ�ǰͼ�εı�ע��ʽ��  
	AcDbDimStyleTable* pDimStyleTbl;
	acdbHostApplicationServices()->workingDatabase()->getDimStyleTable(pDimStyleTbl, AcDb::kForRead);
	if (pDimStyleTbl->has(str))
		pDimStyleTbl->getAt(str, dimStyleId);
	else
	{
		pDimStyleTbl->close();
		return dimStyleId;
	}
	pDimStyleTbl->close();
	AcDbAlignedDimension *pnewdim = new AcDbAlignedDimension(pt1, pt2, pt3, NULL, dimStyleId);//AcDbRadialDimension;
	pnewdim->setDimensionText(sLegth);
	pnewdim->setDimtxt(500);//��ע���ֵĸ߶�
	pnewdim->setDimtix(0);//���ñ�ע����ʼ�ջ����ڳߴ����֮��
	pnewdim->setDimtmove(1);
	AcCmColor textcolor;
	textcolor.setColorIndex(7);
	pnewdim->setDimclrt(textcolor);
	AcDbObjectId dimId;
	DBHelper::AppendToDatabase(dimId, pnewdim);
	if (pnewdim)
		pnewdim->close();
	return dimId;
}

void CDlgEntrance::creatDimStyle(const CString &styleName)
{
	// ��õ�ǰͼ�εı�ע��ʽ��
	AcDbDimStyleTable *pDimStyleTbl = NULL;
	acdbHostApplicationServices()->workingDatabase()->getDimStyleTable(pDimStyleTbl, AcDb::kForWrite);
	if (pDimStyleTbl->has(styleName))
	{
		pDimStyleTbl->close();//�Ѿ�����
		return;
	}

	// �����µı�ע��ʽ���¼
	AcDbDimStyleTableRecord *pDimStyleTblRcd = NULL;
	pDimStyleTblRcd = new AcDbDimStyleTableRecord();

	// ���ñ�ע��ʽ������
	pDimStyleTblRcd->setName(styleName); // ��ʽ����
	pDimStyleTblRcd->setDimasz(1);//���ü�ͷ��С
	pDimStyleTblRcd->setDimzin(8);//ʮ����С����ʾʱ�����ƺ�����
	pDimStyleTblRcd->setDimblk(_T("_OPEN"));//���ü�ͷ����״Ϊ�������
	pDimStyleTblRcd->setDimexe(300);//���óߴ���߳����ߴ��߾���Ϊ400
	pDimStyleTblRcd->setDimexo(0);//���óߴ���ߵ����ƫ����Ϊ300
	pDimStyleTblRcd->setDimtxt(800);//�������ָ߶�
	pDimStyleTblRcd->setDimtad(1);//��������λ��-��ֱΪ�Ϸ���ˮƽĬ��Ϊ���У���������
	pDimStyleTblRcd->setDimgap(50);//��������λ��-�ӳߴ��ߵ�ƫ����
	pDimStyleTblRcd->setDimtih(0);
	pDimStyleTblRcd->setDimtoh(0);//�������ֶ���Ϊ����ߴ��߶���
	pDimStyleTblRcd->setDimtix(0);//���ñ�ע����ʼ�ջ����ڳߴ����֮��
	pDimStyleTblRcd->setDimtofl(1);//��ʹ��ͷ�����ڲ�����֮�⣬�ߴ���Ҳ�������ڲ�����֮��

	AcCmColor suiceng;
	suiceng.setColorIndex(3);
	pDimStyleTblRcd->setDimclrd(suiceng);//Ϊ�ߴ��ߡ���ͷ�ͱ�ע����ָ����ɫ��0Ϊ��ͼ��
	pDimStyleTblRcd->setDimclre(suiceng);//Ϊ�ߴ����ָ����ɫ������ɫ������������Ч����ɫ���

										 // ����ע��ʽ���¼��ӵ���ע��ʽ����
	pDimStyleTbl->add(pDimStyleTblRcd);
	pDimStyleTblRcd->close();
	pDimStyleTbl->close();
}

void CDlgEntrance::DealIntersectEnt(AcDbObjectIdArray& inputIds)
{
	std::vector<AcGePoint3d> points;
	AcDbEntity *pEnt = NULL;
	AcDbEntity *tempEnt = NULL;
	for (int k = 0; k < inputIds.length(); k++)
	{
		if (Acad::eOk != acdbOpenObject(pEnt, inputIds[k], AcDb::kForWrite))
			continue;

		for (int j = 0; j < inputIds.length(); j++)
		{
			if (inputIds[k] == inputIds[j])
				continue;
			if (Acad::eOk != acdbOpenObject(tempEnt, inputIds[j], AcDb::kForWrite))
				continue;

			AcGePoint3dArray intersectPoints;
			tempEnt->intersectWith(pEnt, AcDb::kOnBothOperands, intersectPoints);
			if (intersectPoints.length() > 0)
			{
				AcGePoint3d pt = intersectPoints[0];
				if (std::find(points.begin(), points.end(), pt) == points.end())//����
				{
					
					if (DealEnt(pEnt, pt))
						if(DealEnt(tempEnt, pt))
							points.push_back(pt);		
				}

			}
			if (tempEnt)
				tempEnt->close();
		}
		if (pEnt)
			pEnt->close();
	}
}

bool CDlgEntrance::DealEnt(AcDbEntity* pEnt, AcGePoint3d& intersectPoint)
{
	if (pEnt->isKindOf(AcDbLine::desc()))
	{
		AcDbLine *LinEnt = AcDbLine::cast(pEnt);
		AcGePoint3d starpoint = LinEnt->startPoint();
		AcGePoint3d endpoint = LinEnt->endPoint();
		if (starpoint.distanceTo(intersectPoint) <= endpoint.distanceTo(intersectPoint))
		{
			LinEnt->setStartPoint(intersectPoint);
		}
		else
		{
			LinEnt->setEndPoint(intersectPoint);
		}
		
	}
	else if (pEnt->isKindOf(AcDbArc::desc()))
	{
		AcDbArc *pARC = AcDbArc::cast(pEnt);
		AcGePoint3d starpoint;
		AcGePoint3d endpoint;
		pARC->getStartPoint(starpoint);
		pARC->getEndPoint(endpoint);
		AcGePoint3d centerpoint = pARC->center();
		if (starpoint.distanceTo(endpoint) == centerpoint.distanceTo(endpoint))
			return false;
		if (starpoint.distanceTo(intersectPoint) < endpoint.distanceTo(intersectPoint))
		{
			AcGeVector2d changVec = AcGeVector2d(intersectPoint.x - centerpoint.x, intersectPoint.y - centerpoint.y);
			pARC->setStartAngle(changVec.angle());
		}
		else
		{
			AcGeVector2d changVec = AcGeVector2d(intersectPoint.x - centerpoint.x, intersectPoint.y - centerpoint.y);
			pARC->setEndAngle(changVec.angle());
		}
	}
	return true;
}

void CDlgEntrance::BatchStorageEnt(AcDbObjectIdArray& inputId, std::vector<std::vector<AcDbObjectId>>& outputId)
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

			if (Acad::eOk != acdbOpenObject(pEnt, inputId[i], AcDb::kForWrite))
				continue;

			for (int j = 0; j < inputId.length(); j++)
			{

				std::vector<AcDbObjectId> tempcompare;
				if (inputId[i] == inputId[j])
					continue;
				if (Acad::eOk != acdbOpenObject(tempEnt, inputId[j], AcDb::kForRead))
					continue;

				//AcGePoint3dArray intersectPoints;
				//	tempEnt->intersectWith(pEnt, AcDb::kOnBothOperands, intersectPoints);
				if (CDlgEntrance::isIntersect(pEnt, tempEnt, 1))
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

bool CDlgEntrance::isIntersect(AcDbEntity* pEnt, AcDbEntity* pTempEnt, double tol)
{
	AcGePoint3d Entstartpt;
	AcGePoint3d Entendpt;
	CDlgEntrance::getpoint(pEnt, Entstartpt, Entendpt);

	AcGePoint3d TempEntstartpt;
	AcGePoint3d TempEntendpt;
	CDlgEntrance::getpoint(pTempEnt, TempEntstartpt, TempEntendpt);

	if (Entstartpt.distanceTo(TempEntstartpt) <= tol)
		return true;
	else if (Entstartpt.distanceTo(TempEntendpt) <= tol)
		return true;
	else if (Entendpt.distanceTo(TempEntstartpt) <= tol)
		return true;
	else if (Entendpt.distanceTo(TempEntendpt) <= tol)
		return true;

	return false;
}

void CDlgEntrance::getpoint(AcDbEntity* pEnt, AcGePoint3d& startpt, AcGePoint3d& endpt)
{
	if (pEnt->isKindOf(AcDbLine::desc()))
	{
		AcDbLine *pLine = AcDbLine::cast(pEnt);
		startpt = pLine->startPoint();
		endpt = pLine->endPoint();
		if (pLine)
			pLine->close();
	}
	else if (pEnt->isKindOf(AcDbArc::desc()))
	{
		AcDbArc *pArc = AcDbArc::cast(pEnt);
		pArc->getStartPoint(startpt);
		pArc->getEndPoint(endpt);
		if (pArc)
			pArc->close();
	}
}

void CDlgEntrance::GenerateGuides(double& changdistance, std::vector<AcDbObjectId>& operaIds, AcDbObjectIdArray& GuideIds)
{
	AcDbEntity *pEnt = NULL;
	AcDbEntity *tempEnt = NULL;

	for (int m = 0; m < operaIds.size(); m++)
	{
		if (Acad::eOk != acdbOpenObject(pEnt, operaIds[m], AcDb::kForWrite))
			continue;
		if (pEnt->isKindOf(AcDbLine::desc()))
		{
			AcDbLine *line = AcDbLine::cast(pEnt);
			AcGePoint3d starpt = line->startPoint();
			AcGePoint3d endpt = line->endPoint();

			AcGeVector3d vect = AcGeVector3d(endpt - starpt);
			AcGeVector3d projVect_90 = vect.rotateBy(ARX_PI / 2, AcGeVector3d(0, 0, 1));
			projVect_90.normalize();

			starpt.transformBy(projVect_90*changdistance);
			endpt.transformBy(projVect_90*changdistance);

			AcDbLine *pLine = new AcDbLine(starpt, endpt);

			AcDbObjectId lineId;
			DBHelper::AppendToDatabase(lineId, pLine);

			GuideIds.append(lineId);
			if (pLine)
				pLine->close();
		}
		else if (pEnt->isKindOf(AcDbArc::desc()))
		{
			AcDbArc *arc = AcDbArc::cast(pEnt);
			AcGePoint3d arccenter = arc->center();
			double radius = arc->radius();
			double startAngle = arc->startAngle();
			double endAngle = arc->endAngle();

			AcDbArc *pArc1 = new AcDbArc(arccenter, radius - changdistance, startAngle, endAngle);
			AcDbArc *pArc2 = new AcDbArc(arccenter, radius + changdistance, startAngle, endAngle);

			AcDbObjectId pArcid_1;
			DBHelper::AppendToDatabase(pArcid_1, pArc1);
			GuideIds.append(pArcid_1);
			AcDbObjectId pArcid_2;
			DBHelper::AppendToDatabase(pArcid_2, pArc2);
			GuideIds.append(pArcid_2);
			if (pArc1)
				pArc1->close();
			if (pArc2)
				pArc2->close();
		}

		if (pEnt)
			pEnt->close();
	}

	CDlgEntrance::DealIntersectEnt(GuideIds);

	for (int m = 0; m < operaIds.size(); m++)
	{
		if (Acad::eOk != acdbOpenObject(pEnt, operaIds[m], AcDb::kForWrite))
			continue;
		if (pEnt->isKindOf(AcDbLine::desc()))
		{
			AcDbLine *line = AcDbLine::cast(pEnt);
			AcGePoint3d starpt = line->startPoint();
			AcGePoint3d endpt = line->endPoint();

			AcGeVector3d vect = AcGeVector3d(endpt - starpt);
			AcGeVector3d projVect_180 = vect.rotateBy(3 * ARX_PI / 2, AcGeVector3d(0, 0, 1));

			projVect_180.normalize();

			starpt.transformBy(projVect_180*changdistance);
			endpt.transformBy(projVect_180*changdistance);

			AcDbLine *pLine1 = new AcDbLine(starpt, endpt);
			AcDbObjectId lineId_1;
			DBHelper::AppendToDatabase(lineId_1, pLine1);
			GuideIds.append(lineId_1);

			if (pLine1)
				pLine1->close();


		}
		if (pEnt)
			pEnt->close();
	}
	CDlgEntrance::DealIntersectEnt(GuideIds);
}

void CDlgEntrance::MultipleCycles(double& inputdistance, AcDbObjectIdArray& GuideIds)
{
	AcDbEntity *pEnt = NULL;
	AcDbEntity *tempEnt = NULL;

	std::vector<AcGePoint3d> points;
	std::vector<InfoStructLine> saveLineInfoVector;
	std::vector<InfoStructArc> saveArcInfoVector;
	std::vector<AcGePoint3d> inserpoint;
	std::vector<AcGePoint3d> inserpoint1;

	//�����ӳ�������������ǰ�Ķ˵���Ϣ�洢������Ϊ����������֮�󣬻ָ����ԱߵĶ˵㣬Ȼ����з�մ���
	CDlgEntrance::SpecialSaveEntInfo(inputdistance, GuideIds, saveLineInfoVector, saveArcInfoVector, inserpoint1);
	CDlgEntrance::DealIntersectEnt(GuideIds);

	//��Ϊ��һ���ӳ�������ʱ������һ���Կ��ԣ���Ҫ�ظ����Σ�����һ�������ʵ�ǲ��Ͻ��ģ���Ҫ�Ż���
	for (int i = 0; i < 2; i++)
	{
		std::vector<InfoStructLine> saveLineInfoVector1;
		std::vector<InfoStructArc> saveArcInfoVector1;
		CDlgEntrance::SpecialSaveEntInfo(inputdistance, GuideIds, saveLineInfoVector1, saveArcInfoVector1, inserpoint);
		CDlgEntrance::DealIntersectEnt(GuideIds);
	}

	//�ָ��˵�
#if 1 
	for (int i = 0; i < GuideIds.length(); i++)
	{
		if (Acad::eOk != acdbOpenObject(pEnt, GuideIds[i], AcDb::kForWrite))
			continue;

		if (pEnt->isKindOf(AcDbLine::desc()))
		{
			InfoStructLine tempStrcut;
			AcDbLine *pLine = AcDbLine::cast(pEnt);
			AcGePoint3d starpoint = pLine->startPoint();
			AcGePoint3d endpoint = pLine->endPoint();

			if (std::find(inserpoint.begin(), inserpoint.end(), starpoint) != inserpoint.end())//����
			{
				for (int j = 0; j < saveLineInfoVector.size(); j++)
				{
					if (saveLineInfoVector[j].entId == GuideIds[i])
					{
						AcGePoint3d temp = saveLineInfoVector[j].startpoint;
						pLine->setStartPoint(temp);
						break;
					}
				}
			}
			if (std::find(inserpoint.begin(), inserpoint.end(), endpoint) != inserpoint.end())
			{
				for (int j = 0; j < saveLineInfoVector.size(); j++)
				{
					if (saveLineInfoVector[j].entId == GuideIds[i])
					{
						AcGePoint3d temp = saveLineInfoVector[j].endpoint;
						pLine->setEndPoint(temp);
						break;
					}
				}
			}

			if (pLine)
				pLine->close();

		}
		else if (pEnt->isKindOf(AcDbPolyline::desc()))
		{

			AcDbPolyline *pPolyLine = AcDbPolyline::cast(pEnt);
			int num = pPolyLine->numVerts();
			AcGePoint3d startpoint;
			pPolyLine->getStartPoint(startpoint);
			AcGePoint3d endpoint;
			pPolyLine->getEndPoint(endpoint);

			if (std::find(inserpoint.begin(), inserpoint.end(), startpoint) != inserpoint.end())//����
			{
				for (int j = 0; j < saveLineInfoVector.size(); j++)
				{
					if (saveLineInfoVector[j].entId == GuideIds[i])
					{
						pPolyLine->setPointAt(0, AcGePoint2d(saveLineInfoVector[j].startpoint.x, saveLineInfoVector[j].startpoint.y));
						break;
					}
				}
			}
			if (std::find(inserpoint.begin(), inserpoint.end(), endpoint) != inserpoint.end())
			{
				for (int j = 0; j < saveLineInfoVector.size(); j++)
				{
					if (saveLineInfoVector[j].entId == GuideIds[i])
					{
						pPolyLine->setPointAt(num - 1, AcGePoint2d(saveLineInfoVector[j].endpoint.x, saveLineInfoVector[j].endpoint.y));
						break;
					}
				}
			}

			if (pPolyLine)
				pPolyLine->close();

		}
		else if (pEnt->isKindOf(AcDbArc::desc()))
		{

			AcDbArc *pArc = AcDbArc::cast(pEnt);

			AcGePoint3d startpoint;
			AcGePoint3d endpoint;
			pArc->getStartPoint(startpoint);
			pArc->getEndPoint(endpoint);

			if (std::find(inserpoint.begin(), inserpoint.end(), startpoint) != inserpoint.end())//����
			{
				for (int j = 0; j < saveArcInfoVector.size(); j++)
				{
					if (saveArcInfoVector[j].entId == GuideIds[i])
					{
						pArc->setStartAngle(saveArcInfoVector[j].startAngle);
						break;
					}
				}
			}
			if (std::find(inserpoint.begin(), inserpoint.end(), endpoint) != inserpoint.end())
			{
				for (int j = 0; j < saveArcInfoVector.size(); j++)
				{
					if (saveArcInfoVector[j].entId == GuideIds[i])
					{
						pArc->setEndAngle(saveArcInfoVector[j].endAngle);
						break;
					}
				}
			}
			if (pArc)
				pArc->close();
		}

		if (pEnt)
			pEnt->close();
	}
#endif
}

void CDlgEntrance::SpecialSaveEntInfo(double& movedistance, AcDbObjectIdArray& inputIds, std::vector<InfoStructLine>& saveLineInfoVector,
	std::vector<InfoStructArc>& saveArcInfoVector, std::vector<AcGePoint3d>& inserpoint)
{
	double changdistance = movedistance;
	std::vector<AcGePoint3d> points;

	//����ȡ������洢������points��
	for (int i = 0; i < inputIds.length(); i++)
	{
		AcDbEntity *pEnt = NULL;
		AcDbEntity *ptempEnt = NULL;
		if (Acad::eOk != acdbOpenObject(pEnt, inputIds[i], AcDb::kForWrite))
			continue;
		for (int k = 0; k < inputIds.length(); k++)
		{

			if (inputIds[k] == inputIds[i])
				continue;
			if (Acad::eOk != acdbOpenObject(ptempEnt, inputIds[k], AcDb::kForWrite))
				continue;
			AcGePoint3dArray intersectPoints;
			ptempEnt->intersectWith(pEnt, AcDb::kOnBothOperands, intersectPoints);
			if (intersectPoints.length() > 0)
			{
				if (std::find(points.begin(), points.end(), intersectPoints[0]) == points.end())
				{
					points.push_back(intersectPoints[0]);
				}
			}
			if (ptempEnt)
				ptempEnt->close();

		}
		if (pEnt)
			pEnt->close();
	}

	//���ʵ��Ķ˵�û����points�����Ҫ�����������
	for (int i = 0; i < inputIds.length(); i++)
	{
		AcDbEntity *pEnt = NULL;
		if (Acad::eOk != acdbOpenObject(pEnt, inputIds[i], AcDb::kForWrite))
			continue;

		if (pEnt->isKindOf(AcDbLine::desc()))
		{
			InfoStructLine tempStrcut;
			AcDbLine *pLine = AcDbLine::cast(pEnt);
			AcGePoint3d startpoint = pLine->startPoint();
			AcGePoint3d endpoint = pLine->endPoint();

			if (std::find(points.begin(), points.end(), startpoint) == points.end())
			{


				tempStrcut.startpoint = startpoint;
				AcGeVector3d vec = AcGeVector3d(startpoint - endpoint);
				vec.normalize();
				startpoint.transformBy(vec*changdistance);
				inserpoint.push_back(startpoint);
				pLine->setStartPoint(startpoint);

			}
			if (std::find(points.begin(), points.end(), endpoint) == points.end())
			{

				tempStrcut.endpoint = endpoint;
				AcGeVector3d vec = AcGeVector3d(endpoint - startpoint);
				vec.normalize();
				endpoint.transformBy(vec*changdistance);
				inserpoint.push_back(endpoint);
				pLine->setEndPoint(endpoint);

			}
			tempStrcut.entId = inputIds[i];
			saveLineInfoVector.push_back(tempStrcut);

			if (pLine)
				pLine->close();

		}
		else if (pEnt->isKindOf(AcDbArc::desc()))
		{

			InfoStructArc tempStrcut;
			AcDbArc *pArc = AcDbArc::cast(pEnt);

			double radius = pArc->radius();
			double startAngle = pArc->startAngle();
			double endAngle = pArc->endAngle();
			AcGePoint3d startpoint;
			AcGePoint3d endpoint;
			pArc->getStartPoint(startpoint);
			pArc->getEndPoint(endpoint);
			double changAngle = changdistance / radius;

			if (std::find(points.begin(), points.end(), startpoint) == points.end())
			{

				tempStrcut.startAngle = startAngle;
				double tempstartAnle = startAngle - changAngle;
				pArc->setStartAngle(tempstartAnle);
				AcGePoint3d startpt;
				pArc->getStartPoint(startpt);
				inserpoint.push_back(startpt);
			}
			if (std::find(points.begin(), points.end(), endpoint) == points.end())
			{
				tempStrcut.endAngle = endAngle;
				double tempendAngle = endAngle + changAngle;
				pArc->setEndAngle(tempendAngle);
				AcGePoint3d endpt;
				pArc->getEndPoint(endpt);
				inserpoint.push_back(endpt);
			}
			tempStrcut.entId = inputIds[i];
			saveArcInfoVector.push_back(tempStrcut);

			if (pArc)
				pArc->close();

		}

		if (pEnt)
			pEnt->close();
	}
}

void CDlgEntrance::ConnectionPoint(AcDbObjectIdArray& inputIds)
{
	std::vector<AcGePoint3d> points;
	AcGePoint2dArray dealpoints;

	for (int i = 0; i < inputIds.length(); i++)
	{
		AcDbEntity *pEnt = NULL;
		if (Acad::eOk != acdbOpenObject(pEnt, inputIds[i], AcDb::kForRead))
			continue;
		AcGeLineSeg3d *pGeLine=NULL;
		AcGeCircArc3d *pGeArc=NULL;
		bool flag = true;
		if (pEnt->isKindOf(AcDbLine::desc()))
		{
			AcDbLine* pLine = AcDbLine::cast(pEnt);
			pGeLine = new AcGeLineSeg3d(pLine->startPoint(), pLine->endPoint());
		}
		else if (pEnt->isKindOf(AcDbArc::desc()))
		{
			AcDbArc* pArc = AcDbArc::cast(pEnt);
			pGeArc = new AcGeCircArc3d(pArc->center(),pArc->normal(),pArc->normal().perpVector(),pArc->radius(),pArc->startAngle(),pArc->endAngle());
		}

		AcGeLineSeg3d *pTempGeLine = NULL;
		AcGeCircArc3d *pTempGeArc = NULL;
		for (int j = 0; j < inputIds.length(); j++)
		{
			if (inputIds[j] == inputIds[i])
				continue;
			AcDbEntity *tempEnt = NULL;
			if (Acad::eOk != acdbOpenObject(tempEnt, inputIds[j], AcDb::kForRead))
				continue;
			
			if (tempEnt->isKindOf(AcDbLine::desc()))
			{
				AcDbLine* pLine = AcDbLine::cast(tempEnt);
				pTempGeLine = new AcGeLineSeg3d(pLine->startPoint(), pLine->endPoint());

				
			}
			else if (tempEnt->isKindOf(AcDbArc::desc()))
			{
				AcDbArc* pArc = AcDbArc::cast(tempEnt);
				pTempGeArc = new AcGeCircArc3d(pArc->center(), pArc->normal(), pArc->normal().perpVector(), pArc->radius(), pArc->startAngle(), pArc->endAngle());

			}
			
			AcGeTol tol;
			tol.setEqualPoint(1);
			Adesk::Boolean bRec;
			AcGePoint3d intersectPoint1;
			AcGePoint3d intersectPoint2;
			if (pGeLine)
			{
				if (pTempGeLine)
				{
					bRec = pGeLine->intersectWith(*pTempGeLine, intersectPoint1,tol);
				}
				else if(pTempGeArc)
				{
					int num = 1;
					bRec = pTempGeArc->intersectWith(*pGeLine, num, intersectPoint1,intersectPoint2,tol);
					
				}
			}
			else if(pGeArc)
			{
				if (pTempGeLine)
				{
					int num = 1;
					bRec = pGeArc->intersectWith(*pTempGeLine, num, intersectPoint1, intersectPoint2, tol);
				}
				else if (pTempGeArc)
				{
					int num = 1;
					bRec =  pGeArc->intersectWith(*pTempGeArc, num, intersectPoint1, intersectPoint2, tol);
				}

			}
		
			if (bRec)
			{
				if (std::find(points.begin(), points.end(), intersectPoint1) == points.end())
				{
					points.push_back(intersectPoint1);
				}

			}
			if (tempEnt)
				tempEnt->close();
			if (pTempGeLine)
			{
				delete pTempGeLine;
				pTempGeLine = NULL;
			}
			if (pTempGeArc)
			{
				delete pTempGeArc;
				pTempGeArc = NULL;
			}
		}
		if (pEnt)
			pEnt->close();
		if (pGeLine)
		{
			delete pGeLine;
			pGeLine = NULL;
		}
		if (pGeArc)
		{
			delete pGeArc;
			pGeArc = NULL;
		}
	}

	for (int i = 0; i < inputIds.length(); i++)
	{

		AcDbEntity *pEnt = NULL;
		if (Acad::eOk != acdbOpenObject(pEnt, inputIds[i], AcDb::kForRead))
			continue;
		if (pEnt->isKindOf(AcDbLine::desc()))
		{
			InfoStructLine tempStrcut;
			AcDbLine *pLine = AcDbLine::cast(pEnt);
			AcGePoint3d startpoint = pLine->startPoint();
			AcGePoint3d endpoint = pLine->endPoint();
			AcGePoint3dArray pts;
			if (!findPoint(startpoint, endpoint, points, pts, 3))
			{
				for (int ptSizenum = 0; ptSizenum < pts.length(); ptSizenum++)
				{
					dealpoints.append(AcGePoint2d(pts[ptSizenum].x, pts[ptSizenum].y));
				}
			}	
			if (pLine)
				pLine->close();

		}
		else if (pEnt->isKindOf(AcDbPolyline::desc()))
		{

			AcDbPolyline *pPolyLine = AcDbPolyline::cast(pEnt);
			AcGePoint3d startpoint;
			pPolyLine->getStartPoint(startpoint);
			AcGePoint3d endpoint;
			pPolyLine->getEndPoint(endpoint);
			AcGePoint3dArray pts;
			if (!findPoint(startpoint, endpoint, points, pts, 3))
			{
				for (int ptSizenum = 0; ptSizenum < pts.length(); ptSizenum++)
				{
					dealpoints.append(AcGePoint2d(pts[ptSizenum].x, pts[ptSizenum].y));
				}
			}
			
			if (pPolyLine)
				pPolyLine->close();

		}
		else if (pEnt->isKindOf(AcDbArc::desc()))
		{

			AcDbArc *pArc = AcDbArc::cast(pEnt);
			AcGePoint3d startpoint;
			AcGePoint3d endpoint;
			pArc->getStartPoint(startpoint);
			pArc->getEndPoint(endpoint);
			AcGePoint3dArray pts;
			if (!findPoint(startpoint, endpoint, points, pts, 3))
			{
				for (int ptSizenum = 0; ptSizenum < pts.length(); ptSizenum++)
				{
					dealpoints.append(AcGePoint2d(pts[ptSizenum].x, pts[ptSizenum].y));
				}
			}
			if (pArc)
				pArc->close();
		}
		pEnt->close();
	}

	std::vector<AcGePoint2dArray> Pointsvect;
	std::vector<AcGePoint2d> comparepoints;
	for (int m = 0; m<dealpoints.length(); m++)
	{
		if (find(comparepoints.begin(), comparepoints.end(), dealpoints[m]) != comparepoints.end())
			continue;
		for (int i = 0; i < dealpoints.length(); i++)
		{
			if (dealpoints[m] == dealpoints[i])
				continue;
			if (find(comparepoints.begin(), comparepoints.end(), dealpoints[i]) != comparepoints.end())
				continue;

			for (int k = 0; k < dealpoints.length(); k++)
			{
				if (dealpoints[i] == dealpoints[k] || dealpoints[m] == dealpoints[k])
					continue;
				if (find(comparepoints.begin(), comparepoints.end(), dealpoints[k]) != comparepoints.end())
					continue;
				if (CDlgEntrance::IsOnLine(dealpoints[m], dealpoints[i], dealpoints[k]))
				{
					AcGePoint2dArray polyLinePt;
					polyLinePt.append(dealpoints[m]);
					polyLinePt.append(dealpoints[i]);
					polyLinePt.append(dealpoints[k]);
					Pointsvect.push_back(polyLinePt);
					comparepoints.push_back(dealpoints[m]);
					comparepoints.push_back(dealpoints[i]);
					comparepoints.push_back(dealpoints[k]);
					break;
				}
			}

		}
	}

	//ֱ����Բ�����е����
	if (dealpoints.length()>6)
	{
		AcGePoint2dArray Linepts;
		std::vector<AcGePoint2d> compareTemp;
		for (int i = 0; i<dealpoints.length(); i++)
		{
			if (find(comparepoints.begin(), comparepoints.end(), dealpoints[i]) != comparepoints.end())
				continue;
			Linepts.append(dealpoints[i]);
		}

		for (int i = 0; i < Linepts.length() - 1; i++)
		{
			if (find(compareTemp.begin(), compareTemp.end(), Linepts[i]) != compareTemp.end())
				continue;
			AcGePoint3d endpoint;
			double distdata = 0;
			for (int m = 0; m<Linepts.length(); m++)
			{
				if (Linepts[m] == Linepts[i])
					continue;
				if (find(compareTemp.begin(), compareTemp.end(), Linepts[m]) != compareTemp.end())
					continue;
				if (distdata == 0)
				{
					distdata = Linepts[i].distanceTo(Linepts[m]);
					if (Linepts.length() == 2)
						endpoint = AcGePoint3d(Linepts[m].x, Linepts[m].y, 0);

				}
				else if (Linepts[i].distanceTo(Linepts[m]) <= distdata)
				{
					distdata = Linepts[i].distanceTo(Linepts[m]);
					endpoint = AcGePoint3d(Linepts[m].x, Linepts[m].y, 0);
					compareTemp.push_back(Linepts[i]);
					compareTemp.push_back(Linepts[m]);
				}


			}
			AcDbLine *pPline = new AcDbLine(AcGePoint3d(Linepts[i].x, Linepts[i].y, 0), endpoint);
			AcDbObjectId plineId;
			DBHelper::AppendToDatabase(plineId, pPline);
			inputIds.append(plineId);
			m_widthLineIds.append(plineId);
			if (pPline)
				pPline->close();
		}
	}

	for (int num = 0; num < Pointsvect.size(); num++)
	{
		if (Pointsvect[num].length() == 3)
		{
			double dis1 = Pointsvect[num].at(0).distanceTo(Pointsvect[num].at(1));
			double dis2 = Pointsvect[num].at(0).distanceTo(Pointsvect[num].at(2));
			double dis3 = Pointsvect[num].at(1).distanceTo(Pointsvect[num].at(2));

			double arr[] = { dis1,dis2,dis3 };
			std::sort(arr, arr + 3);
			AcDbObjectId pPolyLineId;
			if (arr[2] == dis1)
			{
				AcDbLine *pPline = new AcDbLine(AcGePoint3d(Pointsvect[num].at(0).x, Pointsvect[num].at(0).y, 0), AcGePoint3d(Pointsvect[num].at(1).x
					, Pointsvect[num].at(1).y, 0));
				DBHelper::AppendToDatabase(pPolyLineId, pPline);
				if (pPline)
					pPline->close();
			}
			else if (arr[2] == dis2)
			{
				AcDbLine *pPline = new AcDbLine(AcGePoint3d(Pointsvect[num].at(0).x, Pointsvect[num].at(0).y, 0), AcGePoint3d(Pointsvect[num].at(2).x
					, Pointsvect[num].at(2).y, 0));
				DBHelper::AppendToDatabase(pPolyLineId, pPline);
				if (pPline)
					pPline->close();
			}
			else if (arr[2] == dis3)
			{
				AcDbLine *pPline = new AcDbLine(AcGePoint3d(Pointsvect[num].at(1).x, Pointsvect[num].at(1).y, 0), AcGePoint3d(Pointsvect[num].at(2).x
					, Pointsvect[num].at(2).y, 0));
				DBHelper::AppendToDatabase(pPolyLineId, pPline);
				if (pPline)
					pPline->close();
			}
			inputIds.append(pPolyLineId);
			m_widthLineIds.append(pPolyLineId);
		}

	}
}

bool CDlgEntrance::findPoint(AcGePoint3d& startpt, AcGePoint3d& endpt, std::vector<AcGePoint3d>& points, AcGePoint3dArray& outpts, int tol/*= 0 */)
{
	int num = 0;
	for (int i=0; i<points.size();i++)
	{
		if (startpt.distanceTo(points[i]) <= tol)
		{
			outpts.append(endpt);
			num++;
		}
		if (endpt.distanceTo(points[i]) <= tol)
		{
			outpts.append(startpt);
			num++;
		}
	}
	if (num >= 2)
		return true;

	if (num == 0)
	{
		outpts.append(startpt);
		outpts.append(endpt);
	}

	return false;
}

bool CDlgEntrance::IsOnLine(AcGePoint2d& pt1, AcGePoint2d& pt2, AcGePoint2d& pt3)
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

void CDlgEntrance::addWideDim(const double showWidth)
{
	CString sEntranceLayer(CEquipmentroomTool::getLayerName("entrance").c_str());
	if (m_widthLineIds.length() > 0)
	{
		AcDbEntity *pEnt = NULL;
		Acad::ErrorStatus es = acdbOpenObject(pEnt, m_widthLineIds[0], AcDb::kForRead);
		if (es != eOk)
			return;
		if (pEnt->isKindOf(AcDbLine::desc()))
		{
			AcDbLine *pLine = AcDbLine::cast(pEnt);
			AcGePoint3d endPt;
			AcGePoint3d startPt;
			pLine->getEndPoint(endPt);
			pLine->getStartPoint(startPt);
			AcGeVector3d tempVec = AcGeVector3d(startPt - endPt);
			AcGeVector3d Linevec = tempVec.rotateBy(ARX_PI / 2, AcGeVector3d(0, 0, 1));
			Linevec.normalize();
			AcGePoint3d Pt1 = startPt;
			Pt1.transformBy(Linevec * 1000);
			CString sEntranceLength;
			sEntranceLength.Format(_T("%.2f"), showWidth);
			AcDbObjectId dimId = CDlgEntrance::creatDim(startPt, endPt, Pt1, sEntranceLength);
			CEquipmentroomTool::setEntToLayer(dimId, sEntranceLayer);
			m_addBlockIds.append(dimId);
		}
		pEnt->close();
	}
}

void CDlgEntrance::creatPlinePoints(const AcDbObjectIdArray allLineIds)
{
	std::vector<AcGePoint2dArray> allLinePts;
	for (int m = 0; m < allLineIds.length(); m++)
	{
		AcDbEntity *pEnt = NULL;
		if (Acad::eOk != acdbOpenObject(pEnt, allLineIds[m], AcDb::kForRead))
			continue;
		AcGePoint2dArray linePts;
		if (pEnt->isKindOf(AcDbLine::desc()))
		{
			AcDbLine *line = AcDbLine::cast(pEnt);
			AcGePoint3d starpt = line->startPoint();
			AcGePoint3d endpt = line->endPoint();
			AcGePoint2d start2dPt(starpt.x, starpt.y);
			AcGePoint2d enf2dPt(endpt.x, endpt.y);
			linePts.append(start2dPt);
			linePts.append(enf2dPt);
		}
		else if (pEnt->isKindOf(AcDbArc::desc()))
		{
			AcDbArc *pArc = AcDbArc::cast(pEnt);
			AcGeCircArc3d pGeArc(
				pArc->center(),
				pArc->normal(),
				pArc->normal().perpVector(),
				pArc->radius(),
				pArc->startAngle(),
				pArc->endAngle());
			AcGePoint3dArray result = GeHelper::CalcArcFittingPoints(pGeArc, 18);
			for (int length = 0; length < result.length(); length++)
			{
				AcGePoint2d temp(result[length].x, result[length].y);
				linePts.append(temp);
			}
		}
		else if (pEnt->isKindOf(AcDbPolyline::desc()))
		{
			AcDbVoidPtrArray entsTempArray;
			AcDbPolyline *pPline = AcDbPolyline::cast(pEnt);
			AcGeLineSeg2d line;
			AcGeCircArc3d arc;
			int n = pPline->numVerts();
			for (int i = 0; i < n; i++)
			{
				AcGePoint2dArray linePoints;//�õ������е�
				if (pPline->segType(i) == AcDbPolyline::kLine)
				{
					pPline->getLineSegAt(i, line);
					AcGePoint2d startPoint;
					AcGePoint2d endPoint;
					startPoint = line.startPoint();
					endPoint = line.endPoint();
					linePoints.append(startPoint);
					linePoints.append(endPoint);
				}
				else if (pPline->segType(i) == AcDbPolyline::kArc)
				{
					pPline->getArcSegAt(i, arc);
					AcGePoint3dArray result = GeHelper::CalcArcFittingPoints(arc, 16);
					for (int x = 0; x < result.length(); x++)
					{
						AcGePoint2d onArcpoint(result[x].x, result[x].y);
						linePoints.append(onArcpoint);
					}
				}
				if (!linePoints.isEmpty())
				{
					allLinePts.push_back(linePoints);
				}
			}
		}
		if (!linePts.isEmpty())
		{
			allLinePts.push_back(linePts);
		}
		if (pEnt)
			pEnt->close();
	}

	AcGePoint2dArray resultPts;
	AcGePoint2d targetPts = allLinePts[0][0];
	AcGePoint2dArray nextUsedPts = allLinePts[0];
	AcGePoint2d plinePt;
	if (!CDlgEntrance::checkClosed(targetPts, allLinePts))
	{
		return;
	}
	resultPts.append(targetPts);
	AcGePoint2d firstPt = targetPts;
	bool flag = true;
	do
	{
		AcGePoint2d tempplinePt = CDlgEntrance::getPlineNextPoint(targetPts, nextUsedPts, allLinePts, resultPts);
		targetPts = tempplinePt;
		resultPts.append(tempplinePt);
		if (firstPt == tempplinePt || resultPts.length() > 100)
		{
			flag = false;
		}
	} while (flag);
	for (int o = 0; o < resultPts.length(); o++)
	{
		AcGePoint2d show = resultPts[o];
		int re = resultPts.length();
	}
	deleParkInEntrance(resultPts);
}

bool CDlgEntrance::checkClosed(const AcGePoint2d checkPt, const std::vector<AcGePoint2dArray> allLinePts)
{
	int count = 0;
	for (int i = 0; i < allLinePts.size(); i++)
	{
		for (int j = 0; j < allLinePts[i].length(); j++)
		{
			AcGePoint2d debugShow = allLinePts[i][j];
			if (checkPt == allLinePts[i][j])
			{
				count++;
			}
		}
	}
	if (count == 1 || count == 0)
	{
		return false;
	}
	else
	{
		return true;
	}
}

AcGePoint2d CDlgEntrance::getPlineNextPoint(const AcGePoint2d targetPt, AcGePoint2dArray &nextUsedPts,
	const std::vector<AcGePoint2dArray> allLinePts, AcGePoint2dArray &resultPts)
{
	for (int a = 0; a < allLinePts.size(); a++)
	{
		if (allLinePts[a] == nextUsedPts)
		{
			continue;
		}
		AcGePoint2d startPt = allLinePts[a][0];
		int size = allLinePts[a].length();
		AcGePoint2d endPt = allLinePts[a][size - 1];
		if (targetPt == startPt)
		{
			nextUsedPts = allLinePts[a];
			if (size > 2)
			{
				for (int v = 1; v < size - 1; v++)
				{
					resultPts.append(allLinePts[a][v]);
				}
			}
			return endPt;
		}
		else if (targetPt == endPt)
		{
			nextUsedPts = allLinePts[a];
			if (size > 2)
			{
				for (int v = 1; v < size - 1; v++)
				{
					resultPts.append(allLinePts[a][v]);
				}
			}
			return startPt;
		}
	}
	AcGePoint2d errorPt(0, 0);
	return errorPt;
}


void CDlgEntrance::deleParkInEntrance(const AcGePoint2dArray plinePts)
{
	CString parkingLayer(CEquipmentroomTool::getLayerName("ordinary_parking").c_str());
	AcDbObjectIdArray allParkingIds = DBHelper::GetEntitiesByLayerName(parkingLayer);
	std::vector<AcDbObjectId> parkIds;
	for (int i = 0; i < allParkingIds.length(); i++)
	{
		parkIds.push_back(allParkingIds[i]);
	}
	std::vector<AcGePoint2dArray> parkingsPoints;
	std::map<AcDbObjectId, AcGePoint2dArray> parkIdAndPts;
	CEquipmentroomTool::getParkingExtentPts(parkingsPoints, parkIds, parkingLayer, parkIdAndPts);
	for (int j = 0; j < parkIdAndPts.size(); j++)
	{
		std::vector<AcGePoint2dArray> polyIntersections;
		GeHelper::GetIntersectionOfTwoPolygon(plinePts, parkingsPoints[j], polyIntersections);
		if (polyIntersections.size() != 0)
		{
			for (int look = 0; look < parkingsPoints[j].length(); look++)
			{
				AcGePoint2d show = parkingsPoints[j][look];
				int ka = 0;
			}

			std::map<AcDbObjectId, AcGePoint2dArray>::iterator tempIter;
			for (tempIter = parkIdAndPts.begin(); tempIter != parkIdAndPts.end(); tempIter++)
			{
				if (tempIter->second == parkingsPoints[j])
				{
					AcDbObjectId deletId = tempIter->first;
					AcDbEntity* pEntity = NULL;
					if (acdbOpenAcDbEntity(pEntity, deletId, kForWrite) != eOk)							//�����ǿɶ�
					{
						acutPrintf(_T("��ȡ����ʵ��ָ��ʧ�ܣ�"));
						continue;
					}
					if (pEntity->isKindOf(AcDbBlockReference::desc()))
					{
						pEntity->erase();
					}
					pEntity->close();
				}
			}
		}
	}

}


void CDlgEntrance::changeLine2Polyline(AcDbObjectIdArray targetEntIds)
{
	AcDbEntity *pEnt = NULL;
	AcDbObjectIdArray changeColorIds;
	for (int i = 0; i < targetEntIds.length(); i++)
	{
		if (Acad::eOk != acdbOpenObject(pEnt, targetEntIds[i], AcDb::kForWrite))
			continue;
		if (pEnt->isKindOf(AcDbLine::desc()))
		{
			AcDbLine *pLine = AcDbLine::cast(pEnt);
			AcGePoint3d starpoint = pLine->startPoint();
			AcGePoint3d endpoint = pLine->endPoint();

			AcGePoint2d startpt2d(starpoint.x, starpoint.y);
			AcGePoint2d endpt2d(endpoint.x, endpoint.y);
			AcDbPolyline *pPoly = new AcDbPolyline(1);
			double width = 100;//�������߿�/�����������γɾ���
			pPoly->addVertexAt(0, startpt2d, 0, width, width);
			pPoly->addVertexAt(1, endpt2d, 0, width, width);
			pPoly->setClosed(false);
			AcDbObjectId lineId;
			DBHelper::AppendToDatabase(lineId, pPoly);
			pPoly->close();
			changeColorIds.append(lineId);
			pEnt->erase();
		}
		else if (pEnt->isKindOf(AcDbArc::desc()))
		{
			AcDbArc *pArc = AcDbArc::cast(pEnt);
			AcGeCircArc3d pGeArc(
				pArc->center(),
				pArc->normal(),
				pArc->normal().perpVector(),
				pArc->radius(),
				pArc->startAngle(),
				pArc->endAngle());
			AcGePoint3dArray result = GeHelper::CalcArcFittingPoints(pGeArc, 1);
			AcGePoint3d midArcPt = result[1];
			double radiu = pArc->radius();
			AcGePoint3d startPt;
			pArc->getStartPoint(startPt);
			AcGePoint3d endPt;
			pArc->getEndPoint(endPt);
			AcGePoint3d centerPt = pArc->center();
			AcGePoint3d midPt;
			midPt.x = (startPt.x + endPt.x) / 2;
			midPt.y = (startPt.y + endPt.y) / 2;
			midPt.z = 0;
			//double dis = midArcPt.distanceTo(midPt);
			double bowLength = midArcPt.distanceTo(midPt);
			double chordLength = startPt.distanceTo(endPt);
			double bulge = (2 * bowLength) / chordLength;
			AcDbPolyline *pPline = new AcDbPolyline(1);
			AcGePoint2d startpt2d(startPt.x, startPt.y);
			AcGePoint2d endpt2d(endPt.x, endPt.y);
			pPline->addVertexAt(0, startpt2d, bulge, 100, 100);
			pPline->addVertexAt(1, endpt2d, bulge, 100, 100);
			pPline->setClosed(false);
			AcDbObjectId polyId;
			DBHelper::AppendToDatabase(polyId, pPline);
			pPline->close();
			changeColorIds.append(polyId);
			pEnt->erase();
		}
		pEnt->close();
	}
	AcDbEntity *pchangeEnt = NULL;
	for (int size = 0; size < changeColorIds.length(); size++)
	{
		if (Acad::eOk != acdbOpenObject(pchangeEnt, changeColorIds[size], AcDb::kForWrite))
			continue;
		pchangeEnt->setColorIndex(8);
		pchangeEnt->close();
		m_addBlockIds.append(changeColorIds[size]);
	}
}
