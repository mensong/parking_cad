#pragma once
#include "resource.h"
#include "afxwin.h"
#include "MyEdit.h"

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
	virtual BOOL OnInitDialog();
	// 地库总面积编辑框
	CMyEdit m_totalAreaText;
	// 通风设备房塔楼外占比
	CMyEdit m_porportionText;
	void ventilateOpera(double& dTotalArea, double& dPorportion,int& roomCount);
	static CWnd* parent_dlg;
	static CString ms_sTotalAreaText;
	static CString ms_sPorportionText;
	afx_msg void OnBnClickedCancel();
};
