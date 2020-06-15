#pragma once
#include "resource.h"
#include "afxwin.h"
#include "MyEdit.h"

// CDlgVillageSet 对话框

class CDlgVillageSet : public CAcUiDialog
{
	DECLARE_DYNAMIC(CDlgVillageSet)

public:
	CDlgVillageSet(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgVillageSet();
	// 对话框数据
	enum {
		IDD = IDD_DLG_VILLAGE
	};

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	LRESULT OnKeepFocus(WPARAM, LPARAM);//非模态窗口保持焦点(1/3)
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	static CWnd* parent_dlg;
	virtual BOOL OnInitDialog();
	// 小区总面积 
	CMyEdit m_totalAreaText;
	// 塔楼外占比
	CMyEdit m_porportionText;
	// 是否考虑充电桩
	CButton m_considerCheck;
	void villageSetOpera(double& dTotalArea, double& dPorportion, bool& isConsider);
	static CString ms_sTotalAreaText;
	static CString ms_sPorportionText;
	static bool ms_bConsider;
	static void setConsiderState(bool& bConsider);
};
