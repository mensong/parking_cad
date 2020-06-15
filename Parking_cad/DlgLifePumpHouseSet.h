#pragma once
#include "resource.h"
#include "afxwin.h"
#include "MyEdit.h"

// CDlgLifePumpHouseSet 对话框

class CDlgLifePumpHouseSet : public CAcUiDialog
{
	DECLARE_DYNAMIC(CDlgLifePumpHouseSet)

public:
	CDlgLifePumpHouseSet(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgLifePumpHouseSet();

// 对话框数据
	enum {
		IDD = IDD_DLG_LIFEPUMPHOUSE
	};

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	LRESULT OnKeepFocus(WPARAM, LPARAM);//非模态窗口保持焦点(1/3)
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	// 塔楼外占比
	CMyEdit m_textProportion;
	static CWnd* parent_dlg;
	static CString ms_sPorportionText;
	void lifePumpHouseSetOpera(double& dPorportion);
	virtual BOOL OnInitDialog();
};
