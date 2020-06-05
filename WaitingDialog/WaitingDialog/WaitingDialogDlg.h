
// WaitingDialogDlg.h : 头文件
//

#pragma once
#include "afxcmn.h"
#include "afxwin.h"
#include "MyListBox.h"


// CWaitingDialogDlg 对话框
class CWaitingDialogDlg 
	: public CDialogEx
{
// 构造
public:
	CWaitingDialogDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_WAITINGDIALOG_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg BOOL OnCopyData(CWnd* pWnd, COPYDATASTRUCT* pCopyDataStruct);
	afx_msg LRESULT OnReset(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMyClose(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnGetPos(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnGetRange(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnGetId(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnSetShowTime(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()

public:
	LONG m_id;
	BOOL m_showTime;

	CProgressCtrl m_progressCtrl;
	CStatic m_staPrecent;
	CStatic m_staMsg;
	CMyListBox m_listLog;
};
