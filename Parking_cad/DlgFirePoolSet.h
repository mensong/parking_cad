#pragma once
#include "resource.h"
#include "afxwin.h"
#include "MyEdit.h"

// CDlgFirePoolSet 对话框

class CDlgFirePoolSet : public CAcUiDialog
{
	DECLARE_DYNAMIC(CDlgFirePoolSet)

public:
	CDlgFirePoolSet(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgFirePoolSet();

// 对话框数据
	enum {
		IDD = IDD_DLG_FIREPOOL
	};

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	LRESULT OnKeepFocus(WPARAM, LPARAM);//非模态窗口保持焦点(1/3)
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	virtual BOOL OnInitDialog();
	void firePoolSetOpera(double& dStoreyHeight, double& dPorportion, bool& isConsider, bool& isMunicipalWaterSupply);
	static CWnd* parent_dlg;
	static CString ms_sStoreyHeightText;
	static CString ms_sPorportionText;
	static bool ms_bConsider;
	static bool ms_bMunicipalWaterSupply;
	static void setConsiderState(bool& bConsider);
	static void setWaterSupplyState(bool& bWaterSupply);
	// 层高编辑框
	CMyEdit m_editStoreyHeight;
	// 消防水池设备房塔楼外占比
	CMyEdit m_editFPProportion;
	// 充电桩考虑
	CButton m_checkConsider;
	// 室外消防栓供水确认
	CButton m_checkSupply;
};
