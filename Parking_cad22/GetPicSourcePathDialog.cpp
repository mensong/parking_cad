// GetPicSourcePathDialog.cpp : 实现文件
//

#include "stdafx.h"
#include "GetPicSourcePathDialog.h"
#include "afxdialogex.h"
#include "..\Parking_cad\resource.h"


// CGetPicSourcePathDialog 对话框

IMPLEMENT_DYNAMIC(CGetPicSourcePathDialog, CDialogEx)

CGetPicSourcePathDialog::CGetPicSourcePathDialog(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOG_MAPSIGNPATH, pParent)
	, mPicSourcePath(_T(""))
{

}

CGetPicSourcePathDialog::~CGetPicSourcePathDialog()
{
}

void CGetPicSourcePathDialog::OnClose()
{
	CDialogEx::OnClose();

	DestroyWindow();
}

BOOL CGetPicSourcePathDialog::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	UpdateData(TRUE);

	return true;
}

void CGetPicSourcePathDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_PICPATH, mPicSourcePath);
}


BEGIN_MESSAGE_MAP(CGetPicSourcePathDialog, CDialogEx)
END_MESSAGE_MAP()


// CGetPicSourcePathDialog 消息处理程序
