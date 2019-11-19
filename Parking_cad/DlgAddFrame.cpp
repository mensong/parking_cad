// DlgAddFrame.cpp : 实现文件
//

#include "stdafx.h"
#include "DlgAddFrame.h"
#include "afxdialogex.h"
#include "DBHelper.h"


// CDlgAddFrame 对话框

IMPLEMENT_DYNAMIC(CDlgAddFrame, CAcUiDialog)

CDlgAddFrame::CDlgAddFrame(CWnd* pParent /*=NULL*/)
	: CAcUiDialog(CDlgAddFrame::IDD, pParent)
{

}

CDlgAddFrame::~CDlgAddFrame()
{
}

void CDlgAddFrame::refreshPreview()
{
	CString sLen;
	m_editFrameLen.GetWindowText(sLen);
	int nLen = _ttoi(sLen.GetString());
	CString sWidth;
	m_editFrameWidth.GetWindowText(sWidth);
	int nWidth = _ttoi(sWidth.GetString());

	if (nLen < 10 || nWidth < 10)
	{
		m_staPreview.Clear();
		return;
	}

	AcDbDatabase* pDb = new AcDbDatabase;
	AcDbPolyline* pFrame = new AcDbPolyline(4);
	pFrame->setPointAt(0, AcGePoint2d(0, 0));
	pFrame->setPointAt(1, AcGePoint2d(nLen, 0));
	pFrame->setPointAt(2, AcGePoint2d(nLen, nWidth));
	pFrame->setPointAt(3, AcGePoint2d(0, nWidth));
	DBHelper::AppendToDatabase(pFrame, pDb);
	pFrame->close();
	m_staPreview.Init(pDb);
}

void CDlgAddFrame::DoDataExchange(CDataExchange* pDX)
{
	CAcUiDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_FRAME_LENGTH, m_editFrameLen);
	DDX_Control(pDX, IDC_EDIT_FRAME_WIDTH, m_editFrameWidth);
	DDX_Control(pDX, IDC_STA_PREVIEW, m_staPreview);
}


BOOL CDlgAddFrame::OnInitDialog()
{
	if (!CAcUiDialog::OnInitDialog())
		return FALSE;

	((CButton*)GetDlgItem(IDC_RAD_A0))->SetCheck(TRUE);
	OnBnClickedRadA0();

	return TRUE;
}

BEGIN_MESSAGE_MAP(CDlgAddFrame, CAcUiDialog)
	ON_BN_CLICKED(IDC_RAD_A0, &CDlgAddFrame::OnBnClickedRadA0)
	ON_BN_CLICKED(IDC_RAD_A1, &CDlgAddFrame::OnBnClickedRadA1)
	ON_BN_CLICKED(IDC_RAD_A2, &CDlgAddFrame::OnBnClickedRadA2)
	ON_BN_CLICKED(IDC_RAD_A3, &CDlgAddFrame::OnBnClickedRadA3)
	ON_BN_CLICKED(IDC_RAD_A4, &CDlgAddFrame::OnBnClickedRadA4)
	ON_BN_CLICKED(IDOK, &CDlgAddFrame::OnBnClickedOk)
END_MESSAGE_MAP()


// CDlgAddFrame 消息处理程序


void CDlgAddFrame::OnBnClickedRadA0()
{
	m_editFrameLen.SetWindowText(_T("1189"));
	m_editFrameWidth.SetWindowText(_T("841"));
	refreshPreview();
}


void CDlgAddFrame::OnBnClickedRadA1()
{
	m_editFrameLen.SetWindowText(_T("841"));
	m_editFrameWidth.SetWindowText(_T("594"));
	refreshPreview();
}


void CDlgAddFrame::OnBnClickedRadA2()
{
	m_editFrameLen.SetWindowText(_T("594"));
	m_editFrameWidth.SetWindowText(_T("420"));
	refreshPreview();
}


void CDlgAddFrame::OnBnClickedRadA3()
{
	m_editFrameLen.SetWindowText(_T("420"));
	m_editFrameWidth.SetWindowText(_T("297"));
	refreshPreview();
}


void CDlgAddFrame::OnBnClickedRadA4()
{
	m_editFrameLen.SetWindowText(_T("297"));
	m_editFrameWidth.SetWindowText(_T("210"));
	refreshPreview();
}


void CDlgAddFrame::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	this->OnOK();
}
