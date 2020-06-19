#pragma once
#include "resource.h"
#include "afxcmn.h"
#include "TreeCtrlBT.h"

// CDlgToolbarNav 对话框

class CDlgToolbarNav 
	: public CAcUiDialog
{
	DECLARE_DYNAMIC(CDlgToolbarNav)

public:
	CDlgToolbarNav(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgToolbarNav();

	enum { IDD = IDD_DLG_TOOLBAR };

protected:
	static void __stdcall ExecCommand(WPARAM wp, LPARAM lp, void* anyVal);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	virtual void OnOK() override;
	virtual void OnCancel() override;
	afx_msg void OnSize(UINT nType, int cx, int cy);
	DECLARE_MESSAGE_MAP()

public:
	CTreeCtrlBT m_tree;
	std::vector<CString> m_commands;
	afx_msg void OnNMClickTreeNav(NMHDR *pNMHDR, LRESULT *pResult);
};
