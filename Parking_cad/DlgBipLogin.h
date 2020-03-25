#pragma once
#include "resource.h"
#include "afxwin.h"

// CDlgBipLogin �Ի���

class CDlgBipLogin : public CAcUiDialog
{
	DECLARE_DYNAMIC(CDlgBipLogin)

public:
	CDlgBipLogin(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgBipLogin();

	enum {
		IDD = IDD_DLG_BIP_LOGIN
	};

	bool loginSuccess;
	CString userName;
	CString bipId;

private:
	std::string _getSavePwdFilePath();
	void _savePwd();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnBnClickedOk();
	CEdit m_editUser;
	CEdit m_editPassword;
	CButton m_chkSavePwd;
	afx_msg void OnBnClickedCancel();
};
