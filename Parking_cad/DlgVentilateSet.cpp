// DlgVentilateSet.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "DlgVentilateSet.h"
#include "afxdialogex.h"
#include "DBHelper.h"


// CDlgVentilateSet �Ի���

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

//��ģ̬���ڱ��ֽ���(2/3)
LRESULT CDlgVentilateSet::OnKeepFocus(WPARAM, LPARAM)
{
	return TRUE;
}

BEGIN_MESSAGE_MAP(CDlgVentilateSet, CAcUiDialog)
	ON_MESSAGE(WM_ACAD_KEEPFOCUS, &CDlgVentilateSet::OnKeepFocus)//��ģ̬���ڱ��ֽ���(3/3)
	ON_WM_SIZE()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDOK, &CDlgVentilateSet::OnBnClickedOk)
END_MESSAGE_MAP()



// CDlgVentilateSet ��Ϣ�������


void CDlgVentilateSet::OnBnClickedOk()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CAcUiDialog::OnOK();
	Doc_Locker doc_locker;
	DBHelper::CallCADCommand(_T("VentilationEquipmentroomSet "));
}
