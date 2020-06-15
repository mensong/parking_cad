// DlgLivingWaterTankSet.cpp : 实现文件
//

#include "stdafx.h"
#include "DlgLivingWaterTankSet.h"
#include "afxdialogex.h"
#include "DBHelper.h"
#include "EquipmentroomTool.h"

// CDlgLivingWaterTankSet 对话框
CWnd* CDlgLivingWaterTankSet::parent_dlg = NULL;
CString CDlgLivingWaterTankSet::ms_sHouseHoldsText;
CString CDlgLivingWaterTankSet::ms_sPorportionText;
IMPLEMENT_DYNAMIC(CDlgLivingWaterTankSet, CAcUiDialog)

CDlgLivingWaterTankSet::CDlgLivingWaterTankSet(CWnd* pParent /*=NULL*/)
	: CAcUiDialog(IDD_DLG_LIVEWATERTANK, pParent)
{

}

CDlgLivingWaterTankSet::~CDlgLivingWaterTankSet()
{
}

static void __smartLog(bool*& data, bool isDataValid)
{
	if (isDataValid)
	{
		bool end = (*data);
		if (!end)
			CParkingLog::AddLogA("DEBUG_不支持尝试执行的操作", 0, "CDlgLivingWaterTankSet::DoDataExchange");
		delete data;
	}
}

void CDlgLivingWaterTankSet::DoDataExchange(CDataExchange* pDX)
{
	Smart<bool*> end(new bool(false), __smartLog);
	CAcUiDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_HOUSEHOLDS, m_houseHoldsEdit);
	DDX_Control(pDX, IDC_EDIT_LWTPRO, m_porportionText);
	*(end()) = true;
}

//非模态窗口保持焦点(2/3)
LRESULT CDlgLivingWaterTankSet::OnKeepFocus(WPARAM, LPARAM)
{
	return TRUE;
}

BEGIN_MESSAGE_MAP(CDlgLivingWaterTankSet, CAcUiDialog)
	ON_MESSAGE(WM_ACAD_KEEPFOCUS, &CDlgLivingWaterTankSet::OnKeepFocus)//非模态窗口保持焦点(3/3)
	ON_WM_SIZE()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDOK, &CDlgLivingWaterTankSet::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CDlgLivingWaterTankSet::OnBnClickedCancel)
END_MESSAGE_MAP()


// CDlgLivingWaterTankSet 消息处理程序


void CDlgLivingWaterTankSet::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	CString sHouseHolds;
	m_houseHoldsEdit.GetWindowText(sHouseHolds);
	CString sPorportion;
	m_porportionText.GetWindowText(sPorportion);
	int iHouseHolds = _tstoi(sHouseHolds.GetString());
	double dPorportion = _tstof(sPorportion.GetString());
	if (sHouseHolds.IsEmpty() || sPorportion.IsEmpty())
	{
		acedAlert(_T("\n小区户数或塔楼外占比输入为空！"));
		return;
	}
	CAcUiDialog::OnOK();
	Doc_Locker doc_locker;
	acedGetAcadDwgView()->SetFocus();
	livingWaterTankOpera(iHouseHolds, dPorportion);
	parent_dlg->ShowWindow(SW_SHOW);
	ms_sHouseHoldsText = sHouseHolds;
	ms_sPorportionText = sPorportion;
}


void CDlgLivingWaterTankSet::OnBnClickedCancel()
{
	// TODO: 在此添加控件通知处理程序代码
	CAcUiDialog::OnCancel();
	parent_dlg->ShowWindow(SW_SHOW);
}

void CDlgLivingWaterTankSet::livingWaterTankOpera(int& iHouseHolds, double& dPorportion)
{
	double LivingWaterTank = ((iHouseHolds * 3.2 * 0.25 * 0.2) * 2.0 / 1.9) * 1000000;
	CEquipmentroomTool::layerSet();
	double LivingWaterTankSideLength = 0;
	double limitLength = 0;
	if (dPorportion < 1 || dPorportion>100)
	{
		dPorportion = 100;
	}
	double scaleArea = LivingWaterTank*((dPorportion) / 100);
	AcDbObjectIdArray LivingWaterTankJigUseIds = CEquipmentroomTool::createArea(scaleArea, _T("生活水箱"), LivingWaterTankSideLength, limitLength);
	CEquipmentroomTool::setEntToLayer(LivingWaterTankJigUseIds);
	CEquipmentroomTool::jigShow(LivingWaterTankJigUseIds, LivingWaterTankSideLength);
}

BOOL CDlgLivingWaterTankSet::OnInitDialog()
{
	CAcUiDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
	m_houseHoldsEdit.SetWindowText(ms_sHouseHoldsText);
	m_porportionText.SetWindowText(ms_sPorportionText);
	// TODO:  在此添加额外的初始化
	CenterWindow();

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}
