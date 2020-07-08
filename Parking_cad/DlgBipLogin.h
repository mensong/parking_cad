#pragma once
#include "resource.h"
#include "afxwin.h"

// CDlgBipLogin 对话框

class CDlgBipLogin 
	: public CAcUiDialog
{
	DECLARE_DYNAMIC(CDlgBipLogin)

public:
	CDlgBipLogin(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgBipLogin();

	enum {
		IDD = IDD_DLG_BIP_LOGIN
	};

	bool loginSuccess;
	CString userName;
	CString bipId;
	CString groupUdid;
	int allow;
	CString descr;
	CString regTime;
	CString lastSigninTime;
	int signinCount;

protected:
	std::string m_get_userUrl;
	std::string m_add_userUrl;

private:
	std::string _getSavePwdFilePath();
	void _savePwd();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()

public:
	HICON m_hIcon;//图标
	afx_msg void OnBnClickedOk();
	CEdit m_editUser;
	CEdit m_editPassword;
	CButton m_chkSavePwd;
	afx_msg void OnBnClickedCancel();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
};
