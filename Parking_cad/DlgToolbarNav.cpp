// DlgToolbarNav.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "DlgToolbarNav.h"

// CDlgToolbarNav �Ի���

IMPLEMENT_DYNAMIC(CDlgToolbarNav, CDlgArxTools)

CDlgToolbarNav::CDlgToolbarNav(CWnd* pParent /*=NULL*/)
	: CDlgArxTools(IDD_DLG_TOOLBAR, pParent)
{

}

CDlgToolbarNav::~CDlgToolbarNav()
{
}

void CDlgToolbarNav::DoDataExchange(CDataExchange* pDX)
{
	CDlgArxTools::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE_NAV, m_tree);
}

BOOL CDlgToolbarNav::OnInitDialog()
{
	if (CDlgArxTools::OnInitDialog() == FALSE)
		return FALSE;

	m_tree.EnableRootBk(TRUE);
	CPaintDC dc(this);
	m_tree.SetBackgroudColor(dc.GetBkColor());
	m_tree.ModifyStyle(0, TVS_SHOWSELALWAYS, 0);

	HTREEITEM hRoot;
	//
	hRoot = m_tree.InsertItem(_T("����"), TVI_ROOT, TVI_LAST);
	m_tree.SetItemData(hRoot, -1);
	HTREEITEM hChild = m_tree.InsertItem(_T(" ͼ������"), hRoot, TVI_LAST);
	m_tree.SetItemData(hChild, m_commands.size());
	m_commands.push_back(_T("SetConfig "));

	m_tree.Expand(hRoot, TVE_EXPAND);

	//
	hRoot = m_tree.InsertItem(_T("���ܵؿⷽ��"), TVI_ROOT, TVI_LAST);
	m_tree.SetItemData(hRoot, -1);
	hChild = m_tree.InsertItem(_T(" �豸������"), hRoot, TVI_LAST);
	m_tree.SetItemData(hChild, m_commands.size());
	m_commands.push_back(_T("EquipmentRoomSet "));
	hChild = m_tree.InsertItem(_T(" ����Ͳ����"), hRoot, TVI_LAST);
	m_tree.SetItemData(hChild, m_commands.size());
	m_commands.push_back(_T("CoreWall "));
	hChild = m_tree.InsertItem(_T(" AI����"), hRoot, TVI_LAST);
	m_tree.SetItemData(hChild, m_commands.size());
	m_commands.push_back(_T("ParkingSpaceShow "));
	hChild = m_tree.InsertItem(_T(" ��������"), hRoot, TVI_LAST);
	m_tree.SetItemData(hChild, m_commands.size());
	m_commands.push_back(_T("AxisShowOrHide "));

	m_tree.Expand(hRoot, TVE_EXPAND);

	//
	hRoot = m_tree.InsertItem(_T("��ƹ���"), TVI_ROOT, TVI_LAST);
	m_tree.SetItemData(hRoot, -1);
	hChild = m_tree.InsertItem(_T(" ���������"), hRoot, TVI_LAST);
	m_tree.SetItemData(hChild, m_commands.size());
	m_commands.push_back(_T("SetEntranceData "));
	hChild = m_tree.InsertItem(_T(" ��λ�Ϲ����"), hRoot, TVI_LAST);
	m_tree.SetItemData(hChild, m_commands.size());
	m_commands.push_back(_T("BlockList "));
	hChild = m_tree.InsertItem(_T(" ͼֽ�쳣���"), hRoot, TVI_LAST);
	m_tree.SetItemData(hChild, m_commands.size());
	m_commands.push_back(_T("Check "));
	hChild = m_tree.InsertItem(_T(" �������"), hRoot, TVI_LAST);
	m_tree.SetItemData(hChild, m_commands.size());
	m_commands.push_back(_T("COZSN "));
	hChild = m_tree.InsertItem(_T(" ����ָ�����ͼ��"), hRoot, TVI_LAST);
	m_tree.SetItemData(hChild, m_commands.size());
	m_commands.push_back(_T("AddFrame "));

	m_tree.Expand(hRoot, TVE_EXPAND);

	//
	hRoot = m_tree.InsertItem(_T("��׼¥��"), TVI_ROOT, TVI_LAST);
	m_tree.SetItemData(hRoot, -1);
	hChild = m_tree.InsertItem(_T(" ��׼¥�Ϳ�"), hRoot, TVI_LAST);
	m_tree.SetItemData(hChild, m_commands.size());
	m_commands.push_back(_T("BGYBL "));

	m_tree.Expand(hRoot, TVE_EXPAND);

	return TRUE;
}

BEGIN_MESSAGE_MAP(CDlgToolbarNav, CDlgArxTools)
	ON_WM_SIZE()
	ON_NOTIFY(NM_CLICK, IDC_TREE_NAV, &CDlgToolbarNav::OnNMClickTreeNav)
END_MESSAGE_MAP()


// CDlgToolbarNav ��Ϣ��������


void CDlgToolbarNav::OnSize(UINT nType, int cx, int cy)
{
	CDlgArxTools::OnSize(nType, cx, cy);

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

	acedGetAcadDwgView()->SetFocus();
	DBHelper::CallCADCommand(m_commands[i]);

	*pResult = 0;
}