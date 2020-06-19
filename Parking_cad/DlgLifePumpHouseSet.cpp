// DlgLifePumpHouseSet.cpp : 实现文件
//

#include "stdafx.h"
#include "DlgLifePumpHouseSet.h"
#include "afxdialogex.h"
#include "EquipmentroomTool.h"
#include "DBHelper.h"

// CDlgLifePumpHouseSet 对话框
#define LifePumpHouse 80000000.0//生活泵房面积
CWnd* CDlgLifePumpHouseSet::parent_dlg = NULL;
CString CDlgLifePumpHouseSet::ms_sPorportionText;

IMPLEMENT_DYNAMIC(CDlgLifePumpHouseSet, CAcUiDialog)

CDlgLifePumpHouseSet::CDlgLifePumpHouseSet(CWnd* pParent /*=NULL*/)
	: CAcUiDialog(CDlgLifePumpHouseSet::IDD, pParent)
{

}

CDlgLifePumpHouseSet::~CDlgLifePumpHouseSet()
{
}

static void __smartLog(bool*& data, bool isDataValid)
{
	if (isDataValid)
	{
		bool end = (*data);
		if (!end)
			CParkingLog::AddLogA("DEBUG_不支持尝试执行的操作", 0, "CDlgLifePumpHouseSet::DoDataExchange");
		delete data;
	}
}

void CDlgLifePumpHouseSet::DoDataExchange(CDataExchange* pDX)
{
	Smart<bool*> end(new bool(false), __smartLog);
	CAcUiDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_LPH_PRO, m_textProportion);
	*(end()) = true;
}

//非模态窗口保持焦点(2/3)
LRESULT CDlgLifePumpHouseSet::OnKeepFocus(WPARAM, LPARAM)
{
	return TRUE;
}

BEGIN_MESSAGE_MAP(CDlgLifePumpHouseSet, CAcUiDialog)
	ON_MESSAGE(WM_ACAD_KEEPFOCUS, &CDlgLifePumpHouseSet::OnKeepFocus)//非模态窗口保持焦点(3/3)
	ON_WM_SIZE()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDOK, &CDlgLifePumpHouseSet::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CDlgLifePumpHouseSet::OnBnClickedCancel)
END_MESSAGE_MAP()


// CDlgLifePumpHouseSet 消息处理程序


void CDlgLifePumpHouseSet::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	CString sPorportion;
	m_textProportion.GetWindowText(sPorportion);
	double dPorportion = _tstof(sPorportion.GetString());
	if (sPorportion.IsEmpty())
	{
		acedAlert(_T("\n塔楼外占比为空！"));
		return;
	}
	CAcUiDialog::OnOK();
	Doc_Locker doc_locker;
	acedGetAcadDwgView()->SetFocus();
	lifePumpHouseSetOpera(dPorportion);
	parent_dlg->ShowWindow(SW_SHOW);
	ms_sPorportionText = sPorportion;
}


void CDlgLifePumpHouseSet::OnBnClickedCancel()
{
	// TODO: 在此添加控件通知处理程序代码
	CAcUiDialog::OnOK();
	parent_dlg->ShowWindow(SW_SHOW);
}

void CDlgLifePumpHouseSet::lifePumpHouseSetOpera(double& dPorportion)
{
	if (dPorportion < 1 || dPorportion>100)
	{
		dPorportion = 100;
	}
	CEquipmentroomTool::layerSet();
	double LifepumpAreaSideLength = 0;
	double limitLength = 4000;
	double dnewArea = LifePumpHouse*((dPorportion) / 100);
	AcDbObjectIdArray LifepumpAreaJigUseIds = CEquipmentroomTool::createArea(dnewArea, _T("生活泵房"), LifepumpAreaSideLength, limitLength);
	CEquipmentroomTool::setEntToLayer(LifepumpAreaJigUseIds);
	CEquipmentroomTool::jigShow(LifepumpAreaJigUseIds, LifepumpAreaSideLength);
}

BOOL CDlgLifePumpHouseSet::OnInitDialog()
{
	CAcUiDialog::OnInitDialog();

	DlgHelper::AdjustPosition(this, DlgHelper::TOP_LEFT);

	// TODO:  在此添加额外的初始化
	m_textProportion.SetWindowText(ms_sPorportionText);
	return TRUE; 
}
