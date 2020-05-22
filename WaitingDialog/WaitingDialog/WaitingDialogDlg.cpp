
// WaitingDialogDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "WaitingDialog.h"
#include "WaitingDialogDlg.h"
#include "afxdialogex.h"
#include "WaitingDialogDef.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

extern CWaitingDialogApp theApp;

// CWaitingDialogDlg 对话框


CWaitingDialogDlg::CWaitingDialogDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CWaitingDialogDlg::IDD, pParent)
	, m_showTime(TRUE)
{
	//AfxMessageBox(theApp.m_lpCmdLine);
	m_id = _ttol(theApp.m_lpCmdLine);

	WNDCLASS wc;
	// 获取窗口类信息。MFC默认的所有对话框的窗口类名为 #32770
	::GetClassInfo(AfxGetInstanceHandle(), _T("#32770"), &wc);
	// 改变窗口类名
	wc.lpszClassName = _T("WaitingDialog");
	// 注册新窗口类，使程序能使用它
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
	ON_MESSAGE(WM_WD_GETPOS, OnGetPos)
	ON_MESSAGE(WM_WD_GETRANGE, OnGetRange)
	ON_MESSAGE(WM_WD_GETID, OnGetId)
	ON_MESSAGE(WM_WD_SETSHOWTIME, OnSetShowTime)
END_MESSAGE_MAP()


// CWaitingDialogDlg 消息处理程序

BOOL CWaitingDialogDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	this->SetWindowText(_T(""));
	m_staMsg.SetWindowText(_T(""));
	m_staPrecent.SetWindowText(_T(""));

	ModifyStyleEx(WS_EX_APPWINDOW,WS_EX_TOOLWINDOW);//从任务栏中去掉.
	::SetWindowPos(this->GetSafeHwnd(), HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);//置顶
	this->CenterWindow();

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

BOOL CWaitingDialogDlg::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN && (pMsg->wParam == VK_ESCAPE || pMsg->wParam == VK_RETURN))
		return TRUE;

	return __super::PreTranslateMessage(pMsg);
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CWaitingDialogDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
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

		if (pMsg->curPos < 0)
			pMsg->curPos = m_progressCtrl.GetPos() + 1;
		m_progressCtrl.SetPos(pMsg->curPos);

		CString sPrecent;
		sPrecent.Format(_T("%d/%d"), pMsg->curPos, maxRange);
		m_staPrecent.SetWindowText(sPrecent);

		CString oldMsg;
		m_staMsg.GetWindowText(oldMsg);

		CString sMsg;
		if (m_showTime == TRUE)
		{
			SYSTEMTIME st;
			CString sTime;
			GetLocalTime(&st);
			sTime.Format(_T("%.2d-%.2d %.2d:%.2d:%.2d"),
				st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
			sMsg.Format(_T("[%s]%s"), sTime.GetString(), pMsg->msg);
		}
		else
		{
			sMsg = pMsg->msg;
		}
		
		m_staMsg.SetWindowText(sMsg);

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

LRESULT CWaitingDialogDlg::OnGetPos(WPARAM wParam, LPARAM lParam)
{
	return m_progressCtrl.GetPos();
}

LRESULT CWaitingDialogDlg::OnGetRange(WPARAM wParam, LPARAM lParam)
{
	int mi = 0, ma = 0;
	m_progressCtrl.GetRange(mi, ma);

	if ((int)lParam == 1)
		return mi;
	if ((int)lParam == 2)
		return ma;

	return -1;
}

LRESULT CWaitingDialogDlg::OnGetId(WPARAM wParam, LPARAM lParam)
{
	return m_id;
}

LRESULT CWaitingDialogDlg::OnSetShowTime(WPARAM wParam, LPARAM lParam)
{
	m_showTime = (BOOL)lParam;
	return 0;
}
