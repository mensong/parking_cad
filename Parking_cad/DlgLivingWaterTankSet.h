#pragma once
#include "resource.h"
#include "afxwin.h"
#include "MyEdit.h"


// CDlgLivingWaterTankSet 对话框

class CDlgLivingWaterTankSet : public CAcUiDialog
{
	DECLARE_DYNAMIC(CDlgLivingWaterTankSet)

public:
	CDlgLivingWaterTankSet(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgLivingWaterTankSet();

// 对话框数据

	enum { IDD = IDD_DLG_LIVEWATERTANK };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	LRESULT OnKeepFocus(WPARAM, LPARAM);//非模态窗口保持焦点(1/3)
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	// 小区户数
	CEdit m_houseHoldsEdit;
	// 塔楼外占比编辑框
	CMyEdit m_porportionText;
	void livingWaterTankOpera(int& iHouseHolds, double& dPorportion);
	static CWnd* parent_dlg;
	static CString ms_sHouseHoldsText;
	static CString ms_sPorportionText;
	virtual BOOL OnInitDialog();
};
