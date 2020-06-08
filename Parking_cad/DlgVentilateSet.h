#pragma once
#include "resource.h"
#include "afxwin.h"

// CDlgVentilateSet 对话框

class CDlgVentilateSet : public CAcUiDialog
{
	DECLARE_DYNAMIC(CDlgVentilateSet)

public:
	CDlgVentilateSet(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgVentilateSet();
	// 对话框数据
	enum {
		IDD = IDD_DLG_VENTILATE
	};
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	LRESULT OnKeepFocus(WPARAM, LPARAM);//非模态窗口保持焦点(1/3)
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
};
