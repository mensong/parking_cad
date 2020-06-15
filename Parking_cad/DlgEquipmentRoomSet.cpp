#include "StdAfx.h"
#include "DlgEquipmentRoomSet.h"
#include "DBHelper.h"
#include "GeHelper.h"
#include "EquipmentroomTool.h"
#include "OperaEquipmentRoomSet.h"
#include "DlgVentilateSet.h"
#include "DlgVillageSet.h"
#include "DlgLifePumpHouseSet.h"
#include "DlgLivingWaterTankSet.h"
#include "DlgFirePumpHouseSet.h"
#include "DlgFirePoolSet.h"

class CDlgVentilateSet* CDlgEquipmentRoomSet::ms_ventilateDlg = NULL;
class CDlgVillageSet* CDlgEquipmentRoomSet::ms_villageDlg = NULL;
class CDlgLifePumpHouseSet* CDlgEquipmentRoomSet::ms_lifePumpHouseDlg = NULL;
class CDlgLivingWaterTankSet* CDlgEquipmentRoomSet::ms_livingWaterTankDlg = NULL;
class CDlgFirePumpHouseSet* CDlgEquipmentRoomSet::ms_firePumpHouseDlg = NULL;
class CDlgFirePoolSet* CDlgEquipmentRoomSet::ms_firePoolDlg = NULL;
//-----------------------------------------------------------------------------
IMPLEMENT_DYNAMIC (CDlgEquipmentRoomSet, CAcUiDialog)

BEGIN_MESSAGE_MAP(CDlgEquipmentRoomSet, CAcUiDialog)
	ON_MESSAGE(WM_ACAD_KEEPFOCUS, OnAcadKeepFocus)
	ON_BN_CLICKED(IDC_BUTTON_VENTILATE, &CDlgEquipmentRoomSet::OnBnClickedButtonVentilate)
	ON_BN_CLICKED(IDC_BUTTON_VILLAGE_SET, &CDlgEquipmentRoomSet::OnBnClickedButtonVillageSet)
	ON_BN_CLICKED(IDC_BUTTON_LIFEPUMPHOUSE_SET, &CDlgEquipmentRoomSet::OnBnClickedButtonLifepumphouseSet)
	ON_BN_CLICKED(IDC_BUTTON_LIVINGWATERTANK_SET, &CDlgEquipmentRoomSet::OnBnClickedButtonLivingwatertankSet)
	ON_BN_CLICKED(IDC_BUTTON_FIREPUMPHOUSE_SET, &CDlgEquipmentRoomSet::OnBnClickedButtonFirepumphouseSet)
	ON_BN_CLICKED(IDC_BUTTON_FIREPOOL_SET, &CDlgEquipmentRoomSet::OnBnClickedButtonFirepoolSet)
	ON_BN_CLICKED(IDC_BUTTON_CUSTOM, &CDlgEquipmentRoomSet::OnBnClickedButtonCustom)
END_MESSAGE_MAP()

//-----------------------------------------------------------------------------
CDlgEquipmentRoomSet::CDlgEquipmentRoomSet (CWnd *pParent /*=NULL*/, HINSTANCE hInstance /*=NULL*/) : CAcUiDialog (CDlgEquipmentRoomSet::IDD, pParent, hInstance) {
}
 
static void __smartLog(bool*& data, bool isDataValid)
{
	if (isDataValid)
	{
		bool end = (*data);
		if (!end)
			CParkingLog::AddLogA("DEBUG_不支持尝试执行的操作", 0, "CDlgEquipmentRoomSet::DoDataExchange");
		delete data;
	}
}

//-----------------------------------------------------------------------------
void CDlgEquipmentRoomSet::DoDataExchange (CDataExchange *pDX) {
	Smart<bool*> end(new bool(false), __smartLog);
	CAcUiDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BUTTON_VENTILATE, m_btnVentilate);
	*(end()) = true;
}

//-----------------------------------------------------------------------------
//----- Needed for modeless dialogs to keep focus.
//----- Return FALSE to not keep the focus, return TRUE to keep the focus
LRESULT CDlgEquipmentRoomSet::OnAcadKeepFocus (WPARAM, LPARAM) {
	return (TRUE) ;
}


BOOL CDlgEquipmentRoomSet::OnInitDialog()
{
	if (FALSE == CAcUiDialog::OnInitDialog())
		return FALSE;

	//窗口置于cad窗口左侧 Add by: limingsheng
	CRect rcCad;
	acedGetAcadDwgView()->GetWindowRect(&rcCad);
	CRect rc;
	this->GetWindowRect(&rc);
	LONG width = rc.Width();
	LONG height = rc.Height();
	rc.left = rcCad.left;
	rc.right = rc.left + width;
	rc.top = rcCad.top;
	rc.bottom = rc.top + height;
	this->MoveWindow(rc);

	return TRUE;
}

void CDlgEquipmentRoomSet::OnBnClickedButtonVentilate()
{
	// TODO: 在此添加控件通知处理程序代码
	//HideDialogHolder holder(this);
	CDlgVentilateSet::parent_dlg = COperaEquipmentRoomSet::ms_eqdlg;
	COperaEquipmentRoomSet::ms_eqdlg->ShowWindow(SW_HIDE);
	CAcModuleResourceOverride resOverride;
	ms_ventilateDlg = new CDlgVentilateSet(acedGetAcadDwgView());
	ms_ventilateDlg->Create(CDlgVentilateSet::IDD, acedGetAcadDwgView());
	ms_ventilateDlg->ShowWindow(SW_SHOW);
	//DBHelper::CallCADCommand(_T("VentilationEquipmentroomSet "));
}


void CDlgEquipmentRoomSet::OnBnClickedButtonVillageSet()
{
	// TODO: 在此添加控件通知处理程序代码
	//HideDialogHolder holder(this);
	CDlgVillageSet::parent_dlg = COperaEquipmentRoomSet::ms_eqdlg;
	COperaEquipmentRoomSet::ms_eqdlg->ShowWindow(SW_HIDE);
	CAcModuleResourceOverride resOverride;
	ms_villageDlg = new CDlgVillageSet(acedGetAcadDwgView());
	ms_villageDlg->Create(CDlgVillageSet::IDD, acedGetAcadDwgView());
	ms_villageDlg->ShowWindow(SW_SHOW);
	//Doc_Locker doc_locker;
	//DBHelper::CallCADCommand(_T("VillageSet "));
}


void CDlgEquipmentRoomSet::OnBnClickedButtonLifepumphouseSet()
{
	// TODO: 在此添加控件通知处理程序代码
	//HideDialogHolder holder(this);
	CDlgLifePumpHouseSet::parent_dlg = COperaEquipmentRoomSet::ms_eqdlg;
	COperaEquipmentRoomSet::ms_eqdlg->ShowWindow(SW_HIDE);
	CAcModuleResourceOverride resOverride;
	ms_lifePumpHouseDlg = new CDlgLifePumpHouseSet(acedGetAcadDwgView());
	ms_lifePumpHouseDlg->Create(CDlgLifePumpHouseSet::IDD, acedGetAcadDwgView());
	ms_lifePumpHouseDlg->ShowWindow(SW_SHOW);
	//Doc_Locker doc_locker;
	//DBHelper::CallCADCommand(_T("LifePumpHouseSet "));
}


void CDlgEquipmentRoomSet::OnBnClickedButtonLivingwatertankSet()
{
	// TODO: 在此添加控件通知处理程序代码
	//HideDialogHolder holder(this);
	CDlgLivingWaterTankSet::parent_dlg = COperaEquipmentRoomSet::ms_eqdlg;
	COperaEquipmentRoomSet::ms_eqdlg->ShowWindow(SW_HIDE);
	CAcModuleResourceOverride resOverride;
	ms_livingWaterTankDlg = new CDlgLivingWaterTankSet(acedGetAcadDwgView());
	ms_livingWaterTankDlg->Create(CDlgLivingWaterTankSet::IDD, acedGetAcadDwgView());
	ms_livingWaterTankDlg->ShowWindow(SW_SHOW);
	//Doc_Locker doc_locker;
	//DBHelper::CallCADCommand(_T("LivingwatertankSet "));
}


void CDlgEquipmentRoomSet::OnBnClickedButtonFirepumphouseSet()
{
	// TODO: 在此添加控件通知处理程序代码
	//HideDialogHolder holder(this);
	CDlgFirePumpHouseSet::parent_dlg = COperaEquipmentRoomSet::ms_eqdlg;
	COperaEquipmentRoomSet::ms_eqdlg->ShowWindow(SW_HIDE);
	CAcModuleResourceOverride resOverride;
	ms_firePumpHouseDlg = new CDlgFirePumpHouseSet(acedGetAcadDwgView());
	ms_firePumpHouseDlg->Create(CDlgFirePumpHouseSet::IDD, acedGetAcadDwgView());
	ms_firePumpHouseDlg->ShowWindow(SW_SHOW);
	//Doc_Locker doc_locker;
	//DBHelper::CallCADCommand(_T("FirePumpHouseSet "));
}


void CDlgEquipmentRoomSet::OnBnClickedButtonFirepoolSet()
{
	// TODO: 在此添加控件通知处理程序代码
	//HideDialogHolder holder(this);
	CDlgFirePoolSet::parent_dlg = COperaEquipmentRoomSet::ms_eqdlg;
	COperaEquipmentRoomSet::ms_eqdlg->ShowWindow(SW_HIDE);
	CAcModuleResourceOverride resOverride;
	ms_firePoolDlg = new CDlgFirePoolSet(acedGetAcadDwgView());
	ms_firePoolDlg->Create(CDlgFirePoolSet::IDD, acedGetAcadDwgView());
	ms_firePoolDlg->ShowWindow(SW_SHOW);
	//Doc_Locker doc_locker;
	//DBHelper::CallCADCommand(_T("FirePoolSet "));
}


void CDlgEquipmentRoomSet::OnBnClickedButtonCustom()
{
	// TODO: 在此添加控件通知处理程序代码
	HideDialogHolder holder(this);
	Doc_Locker doc_locker;
	CEquipmentroomTool::layerSet(_T("0"), 7);
	CEquipmentroomTool::layerSet();//设置图层
	std::vector<AcDbEntity*> vctEquipmentEnt;
	ads_name ssname;
	ads_name ent;
	AcDbObjectIdArray arrPlineIds;
	//获取选择集
	acedPrompt(_T("\n请选择自定义设备房的多线段实体："));
	struct resbuf *rb; // 结果缓冲区链表
	rb = acutBuildList(RTDXF0, _T("LWPOLYLINE"), RTNONE);
	// 选择复合要求的实体
	acedSSGet(NULL, NULL, NULL, rb, ssname);
	//获取选择集的长度
	Adesk::Int32 len = 0;
	int nRes = acedSSLength(ssname, &len);
	if (RTNORM == nRes)
	{
		//遍历选择集
		for (int i = 0; i < len; i++)
		{
			//获取实体名
			int nRes = acedSSName(ssname, i, ent);
			if (nRes != RTNORM)
				continue;
			//根据实体名得到ID，然后打开自定义实体
			AcDbObjectId id;
			acdbGetObjectId(id, ent);
			if (!id.isValid())
				continue;
			arrPlineIds.append(id);
			AcDbEntity *pEnt = NULL;
			acdbOpenObject(pEnt, id, AcDb::kForWrite);
			//判断自定义实体的类型
			if (pEnt == NULL)
				continue;
			vctEquipmentEnt.push_back(pEnt);
		}
	}
	//释放选择集和结果缓存区
	acutRelRb(rb);
	acedSSFree(ssname);
	if (vctEquipmentEnt.size() < 1)
		return;
	//拿到用户选择多线段实体指针
	AcDbObjectIdArray EquipmentIds;
	for (int i = 0; i < vctEquipmentEnt.size(); i++) 
	{
		if (vctEquipmentEnt[i]->isKindOf(AcDbPolyline::desc()))
		{
			AcDbObjectId EquipmentId = vctEquipmentEnt[i]->objectId();
			EquipmentIds.append(EquipmentId);
			std::vector<AcGePoint2d> allPoints;//得到的所有点
			AcDbVoidPtrArray entsTempArray;
			AcDbPolyline *pPline = AcDbPolyline::cast(vctEquipmentEnt[i]);
			bool bClosed = pPline->isClosed();
			AcGeLineSeg2d line;
			AcGeCircArc3d arc;
			int n = pPline->numVerts();
			for (int j = 0; j < n; j++)
			{
				if (pPline->segType(j) == AcDbPolyline::kLine)
				{
					pPline->getLineSegAt(j, line);
					AcGePoint2d startPoint;
					AcGePoint2d endPoint;
					startPoint = line.startPoint();
					endPoint = line.endPoint();
					allPoints.push_back(startPoint);
					allPoints.push_back(endPoint);
				}
				else if (pPline->segType(j) == AcDbPolyline::kArc)
				{
					pPline->getArcSegAt(j, arc);
					AcGePoint3dArray result = GeHelper::CalcArcFittingPoints(arc, 16);
					for (int x = 0; x < result.length(); x++)
					{
						AcGePoint2d onArcpoint(result[x].x, result[x].y);
						allPoints.push_back(onArcpoint);
					}
				}
			}

			AcGePoint2dArray onePlinePts;//装取去完重的有效点
			AcGeDoubleArray bulges;
			for (int x = 0; x < allPoints.size(); x++)
			{
				if (onePlinePts.contains(allPoints[x]))
				{
					continue;
				}
				onePlinePts.append(allPoints[x]);
				bulges.append(0);
			} 
			//检查闭合
			if (onePlinePts.length() > 1 && pPline->isClosed() ||
				(bClosed && onePlinePts.length() > 1 && onePlinePts[onePlinePts.length() - 1] != onePlinePts[0]))
			{
				onePlinePts.append(onePlinePts[0]);
				bulges.append(0);
			}
			double dArea = GeHelper::CalcPolygonArea(onePlinePts);
			AcGePoint2d plineCenterPt = GeHelper::CalcPolylineFocusPoint(onePlinePts);
			AcGePoint3d tempPt2dto3d(plineCenterPt.x, plineCenterPt.y,10);
			double transArea = dArea / 1000000;
			CString sArea;
			sArea.Format(_T("%.1f"), transArea);
			CString sShowText = _T("该区域面积为：") + sArea+_T("㎡");
			
			allPartitionPts.push_back(onePlinePts);
			AcGeDoubleArray test;
			onePlinePts.append(onePlinePts[0]);
			CEquipmentroomTool::CreateHatch( _T("ANSI31"), onePlinePts, test);

			//AcDbObjectId textId = CEquipmentroomTool::CreateText(tempPt2dto3d, sShowText, dArea / 10000);
			AcDbExtents boundaryOfBlk;
			if (pPline->bounds(boundaryOfBlk) != TRUE)
			{
				acutPrintf(_T("获取设备房的边界信息失败！"));
				pPline->close();
				vctEquipmentEnt[i]->close();
				continue;
			}
			AcGePoint3d positionOfBlkRefMax = boundaryOfBlk.maxPoint();
			AcGePoint3d positionOfBlkRefMin = boundaryOfBlk.minPoint();
			AcGePoint2d ptLeftBottom(positionOfBlkRefMin.x, positionOfBlkRefMin.y);
			AcGePoint2d ptRightBottom(positionOfBlkRefMax.x, positionOfBlkRefMin.y);
			AcGePoint2d ptRightTop(positionOfBlkRefMax.x, positionOfBlkRefMax.y);
			AcGePoint2d ptLeftTop(positionOfBlkRefMin.x, positionOfBlkRefMax.y);
			double textheight = (ptLeftBottom.distanceTo(ptLeftTop))/6;
			double textWidth = ptLeftBottom.distanceTo(ptRightBottom)/3*2;

			CString  textStyleName = _T("设备房用字体");
			CEquipmentroomTool::creatTextStyle(textStyleName);
			AcDbObjectId textStyleId = DBHelper::GetTextStyle(textStyleName);
			AcDbObjectId textId = CEquipmentroomTool::CreateMText(tempPt2dto3d, sShowText, textheight, textWidth, textStyleId);
			CEquipmentroomTool::textMove(tempPt2dto3d, textId);
			pPline->close();
			EquipmentIds.append(textId);
		}
		vctEquipmentEnt[i]->close();
	}
	CEquipmentroomTool::setEntToLayer(EquipmentIds);
	CEquipmentroomTool::layerSet(_T("0"), 7);//操作完成回到零图层
}

