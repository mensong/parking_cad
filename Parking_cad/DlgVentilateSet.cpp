// DlgVentilateSet.cpp : 实现文件
//

#include "stdafx.h"
#include "DlgVentilateSet.h"
#include "afxdialogex.h"
#include "DBHelper.h"


// CDlgVentilateSet 对话框

IMPLEMENT_DYNAMIC(CDlgVentilateSet, CAcUiDialog)

CDlgVentilateSet::CDlgVentilateSet(CWnd* pParent /*=NULL*/)
	: CAcUiDialog(CDlgVentilateSet::IDD, pParent)
{

}

CDlgVentilateSet::~CDlgVentilateSet()
{
}

void CDlgVentilateSet::DoDataExchange(CDataExchange* pDX)
{
	CAcUiDialog::DoDataExchange(pDX);
}

//非模态窗口保持焦点(2/3)
LRESULT CDlgVentilateSet::OnKeepFocus(WPARAM, LPARAM)
{
	return TRUE;
}

BEGIN_MESSAGE_MAP(CDlgVentilateSet, CAcUiDialog)
	ON_MESSAGE(WM_ACAD_KEEPFOCUS, &CDlgVentilateSet::OnKeepFocus)//非模态窗口保持焦点(3/3)
	ON_WM_SIZE()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDOK, &CDlgVentilateSet::OnBnClickedOk)
END_MESSAGE_MAP()



// CDlgVentilateSet 消息处理程序


void CDlgVentilateSet::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	CAcUiDialog::OnOK();
	Doc_Locker doc_locker;
	DBHelper::CallCADCommand(_T("VentilationEquipmentroomSet "));
}
