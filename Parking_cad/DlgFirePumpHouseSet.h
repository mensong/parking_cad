#pragma once
#include "resource.h"
#include "afxwin.h"
#include "MyEdit.h"

// CDlgFirePumpHouseSet 对话框

class CDlgFirePumpHouseSet : public CAcUiDialog
{
	DECLARE_DYNAMIC(CDlgFirePumpHouseSet)

public:
	CDlgFirePumpHouseSet(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgFirePumpHouseSet();

// 对话框数据
	enum { IDD = IDD_DLG_FIREPUMPHOUSE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	LRESULT OnKeepFocus(WPARAM, LPARAM);//非模态窗口保持焦点(1/3)
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	// 消防泵房塔楼外占比
	CMyEdit m_porportionText;
	// 室外消防栓是否由市政供水复选框
	CButton m_checkWaterSuppply;
	virtual BOOL OnInitDialog();
	void villageSetOpera(double& dPorportion, bool& isMunicipalWaterSupply);
	static CWnd* parent_dlg;
	static CString ms_sPorportionText;
	static bool ms_bMunicipalWaterSupply;
	static void setWaterSupplyState(bool& bWaterSupply);
};
