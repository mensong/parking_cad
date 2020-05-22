
// WaitingDialogDlg.h : ͷ�ļ�
//

#pragma once
#include "afxcmn.h"
#include "afxwin.h"
#include "MyListBox.h"


// CWaitingDialogDlg �Ի���
class CWaitingDialogDlg 
	: public CDialogEx
{
// ����
public:
	CWaitingDialogDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_WAITINGDIALOG_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
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
