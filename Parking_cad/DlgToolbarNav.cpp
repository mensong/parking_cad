// DlgToolbarNav.cpp : 实现文件
//

#include "stdafx.h"
#include "DlgToolbarNav.h"

// CDlgToolbarNav 对话框

IMPLEMENT_DYNAMIC(CDlgToolbarNav, CAcUiDialog)

CDlgToolbarNav::CDlgToolbarNav(CWnd* pParent /*=NULL*/)
	: CAcUiDialog(IDD_DLG_TOOLBAR, pParent)
{

}

CDlgToolbarNav::~CDlgToolbarNav()
{
}

void CDlgToolbarNav::DoDataExchange(CDataExchange* pDX)
{
	CAcUiDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE_NAV, m_tree);
}

BOOL CDlgToolbarNav::OnInitDialog()
{
	if (CAcUiDialog::OnInitDialog() == FALSE)
		return FALSE;

	m_tree.EnableRootBk(TRUE);
	m_tree.SetBackgroudColor(GetSysColor(COLOR_3DFACE));
	m_tree.ModifyStyle(0, TVS_SHOWSELALWAYS, 0);

	HTREEITEM hRoot;
	//
	hRoot = m_tree.InsertItem(_T("设置"), TVI_ROOT, TVI_LAST);
	m_tree.SetItemData(hRoot, -1);
	HTREEITEM hChild = m_tree.InsertItem(_T(" 图层设置"), hRoot, TVI_LAST);
	m_tree.SetItemData(hChild, m_commands.size());
	m_commands.push_back(_T("SetConfig "));
	m_tree.Expand(hRoot, TVE_EXPAND);

	//
	hRoot = m_tree.InsertItem(_T("智能地库方案"), TVI_ROOT, TVI_LAST);
	m_tree.SetItemData(hRoot, -1);
	hChild = m_tree.InsertItem(_T(" 设备房定义"), hRoot, TVI_LAST);
	m_tree.SetItemData(hChild, m_commands.size());
	m_commands.push_back(_T("EquipmentRoomSet "));
	hChild = m_tree.InsertItem(_T(" 核心筒定义"), hRoot, TVI_LAST);
	m_tree.SetItemData(hChild, m_commands.size());
	m_commands.push_back(_T("CoreWall "));
	hChild = m_tree.InsertItem(_T(" AI计算"), hRoot, TVI_LAST);
	m_tree.SetItemData(hChild, m_commands.size());
	m_commands.push_back(_T("ParkingSpaceShow "));	
	m_tree.Expand(hRoot, TVE_EXPAND);

	//
	hRoot = m_tree.InsertItem(_T("设计工具"), TVI_ROOT, TVI_LAST);
	m_tree.SetItemData(hRoot, -1);
	hChild = m_tree.InsertItem(_T(" 轴网显隐"), hRoot, TVI_LAST);
	m_tree.SetItemData(hChild, m_commands.size());
	m_commands.push_back(_T("AxisShowOrHide "));
	hChild = m_tree.InsertItem(_T(" 轴号缩放"), hRoot, TVI_LAST);
	m_tree.SetItemData(hChild, m_commands.size());
	m_commands.push_back(_T("COZSN "));
	hChild = m_tree.InsertItem(_T(" 出入口生成"), hRoot, TVI_LAST);
	m_tree.SetItemData(hChild, m_commands.size());
	m_commands.push_back(_T("SetEntranceData "));
	hChild = m_tree.InsertItem(_T(" 图纸异常检测"), hRoot, TVI_LAST);
	m_tree.SetItemData(hChild, m_commands.size());
	m_commands.push_back(_T("Check "));
	hChild = m_tree.InsertItem(_T(" 车位合规测算"), hRoot, TVI_LAST);
	m_tree.SetItemData(hChild, m_commands.size());
	m_commands.push_back(_T("BlockList "));
	hChild = m_tree.InsertItem(_T(" 添加指标表及图框"), hRoot, TVI_LAST);
	m_tree.SetItemData(hChild, m_commands.size());
	m_commands.push_back(_T("AddFrame "));
	m_tree.Expand(hRoot, TVE_EXPAND);

	//
	hRoot = m_tree.InsertItem(_T("标准楼型"), TVI_ROOT, TVI_LAST);
	m_tree.SetItemData(hRoot, -1);
	hChild = m_tree.InsertItem(_T(" 标准楼型库"), hRoot, TVI_LAST);
	m_tree.SetItemData(hChild, m_commands.size());
	m_commands.push_back(_T("BGYBL "));
	m_tree.Expand(hRoot, TVE_EXPAND);

	return TRUE;
}

BEGIN_MESSAGE_MAP(CDlgToolbarNav, CAcUiDialog)
	ON_WM_SIZE()
	ON_NOTIFY(NM_CLICK, IDC_TREE_NAV, &CDlgToolbarNav::OnNMClickTreeNav)
END_MESSAGE_MAP()


// CDlgToolbarNav 消息处理程序


void CDlgToolbarNav::OnSize(UINT nType, int cx, int cy)
{
	CAcUiDialog::OnSize(nType, cx, cy);

	if (::IsWindow(m_tree.GetSafeHwnd()))
	{
		CRect rc;
		GetClientRect(rc);
		m_tree.MoveWindow(rc);
	}
}

void CDlgToolbarNav::OnOK()
{
	
}

void CDlgToolbarNav::OnCancel()
{
	
}

void CDlgToolbarNav::OnNMClickTreeNav(NMHDR *pNMHDR, LRESULT *pResult)
{
	HTREEITEM hItem = m_tree.getMouseItemBT();
	if (!hItem)
	{
		*pResult = 0;
		return;
	}

	DWORD_PTR i = m_tree.GetItemData(hItem);
	if (i < 0 || m_commands.size() <= i)
	{
		*pResult = 0;
		return;
	}

	SetDelayExecute(ExecCommand, 0, 0, &m_commands[i], 200, true);
		
	*pResult = 0;
}

void __stdcall CDlgToolbarNav::ExecCommand(WPARAM wp, LPARAM lp, void* anyVal)
{
	const CString* pCmd = (const CString*)anyVal;

	acedGetAcadDwgView()->SetFocus();
	DBHelper::CallCADCommand(*pCmd);
}
