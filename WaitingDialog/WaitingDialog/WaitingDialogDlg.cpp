
// WaitingDialogDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "WaitingDialog.h"
#include "WaitingDialogDlg.h"
#include "afxdialogex.h"
#include "WaitingDialogDef.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CWaitingDialogDlg �Ի���


CWaitingDialogDlg::CWaitingDialogDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CWaitingDialogDlg::IDD, pParent)
{
	WNDCLASS wc;
	// ��ȡ��������Ϣ��MFCĬ�ϵ����жԻ���Ĵ�������Ϊ #32770
	::GetClassInfo(AfxGetInstanceHandle(), _T("#32770"), &wc);
	// �ı䴰������
	wc.lpszClassName = _T("WaitingDialog");
	// ע���´����࣬ʹ������ʹ����
	BOOL b = AfxRegisterClass(&wc);

	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CWaitingDialogDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PROGRESS, m_progressCtrl);
	DDX_Control(pDX, IDC_STA_PRECENT, m_staPrecent);
	DDX_Control(pDX, IDC_STA_MSG, m_staMsg);
	DDX_Control(pDX, IDC_LIST_LOG, m_listLog);
}

BEGIN_MESSAGE_MAP(CWaitingDialogDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_COPYDATA()
	ON_MESSAGE(WM_WD_RESET, OnReset)
	ON_MESSAGE(WM_WD_CLOSE, OnMyClose)
END_MESSAGE_MAP()


// CWaitingDialogDlg ��Ϣ��������

BOOL CWaitingDialogDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ����Ӷ���ĳ�ʼ������
	this->SetWindowText(_T(""));
	m_staMsg.SetWindowText(_T(""));

	ModifyStyleEx(WS_EX_APPWINDOW,WS_EX_TOOLWINDOW);//����������ȥ��.
	::SetWindowPos(this->GetSafeHwnd(), HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);//�ö�
	this->CenterWindow();

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

BOOL CWaitingDialogDlg::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN && (pMsg->wParam == VK_ESCAPE || pMsg->wParam == VK_RETURN))
		return TRUE;

	return __super::PreTranslateMessage(pMsg);
}

// �����Ի���������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CWaitingDialogDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CWaitingDialogDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



BOOL CWaitingDialogDlg::OnCopyData(CWnd* pWnd, COPYDATASTRUCT* pCopyDataStruct)
{
	if (pCopyDataStruct->cbData == sizeof(WD_RANGE))
	{
		WD_RANGE* pRange = (WD_RANGE*)pCopyDataStruct->lpData;
		m_progressCtrl.SetRange32(pRange->minRange, pRange->maxRange);
		return TRUE;
	}
	else if (pCopyDataStruct->cbData == sizeof(WD_MSG))
	{
		WD_MSG* pMsg = (WD_MSG*)pCopyDataStruct->lpData;
		int minRange = 0, maxRange = 100;
		m_progressCtrl.GetRange(minRange, maxRange);

		m_progressCtrl.SetPos(pMsg->curPos);

		CString sPrecent;
		sPrecent.Format(_T("%d/%d"), pMsg->curPos, maxRange);
		m_staPrecent.SetWindowText(sPrecent);

		CString oldMsg;
		m_staMsg.GetWindowText(oldMsg);
		m_staMsg.SetWindowText(pMsg->msg);

		if (!oldMsg.IsEmpty())
		{
			m_listLog.InsertString(0, oldMsg);
			if (m_listLog.GetCount() > 0)
				m_listLog.SetCurSel(0);
		}

		return TRUE;
	}
	else if (pCopyDataStruct->cbData == sizeof(WD_SETTITLE))
	{
		WD_SETTITLE* pTitle = (WD_SETTITLE*)pCopyDataStruct->lpData;
		this->SetWindowText(pTitle->title);
		return TRUE;
	}

	return CDialogEx::OnCopyData(pWnd, pCopyDataStruct);
}

LRESULT CWaitingDialogDlg::OnReset(WPARAM wParam, LPARAM lParam)
{
	m_progressCtrl.SetRange32(0, 100);
	m_progressCtrl.SetPos(0);
	m_staPrecent.SetWindowText(_T("0/100"));
	m_staMsg.SetWindowText(_T(""));
	m_listLog.ResetContent();
	return 0;
}

LRESULT CWaitingDialogDlg::OnMyClose(WPARAM wParam, LPARAM lParam)
{
	this->DestroyWindow();
	return 0;
}