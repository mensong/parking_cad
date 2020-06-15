#pragma once
#include "resource.h"
#include "afxwin.h"
#include "MyEdit.h"

// CDlgZoomAxisNum 对话框

class CDlgZoomAxisNum : public CAcUiDialog
{
	DECLARE_DYNAMIC(CDlgZoomAxisNum)

public:
	CDlgZoomAxisNum(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgZoomAxisNum();

// 对话框数据

	enum { IDD = IDD_DLG_AXISNUMZOOM };

protected:
	static void __stdcall delayDownClick(WPARAM wp, LPARAM lp, void* anyVal);
	static void __stdcall delayUpClick(WPARAM wp, LPARAM lp, void* anyVal);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	LRESULT OnKeepFocus(WPARAM, LPARAM);//非模态窗口保持焦点(1/3)
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonAxisnumUp();
	afx_msg void OnBnClickedButtonAxisnumDown();
	virtual BOOL OnInitDialog();
	// 缩小按钮
	CButton m_btn_AxisNumDown;
	// 轴号放大
	CButton m_btn_AxisNumUp;
	// 当前缩放值展示
	CMyEdit m_show_ScaleValue;
	// 步进值编辑框
	CMyEdit m_edit_StepValue;
	static CString ms_sStepValue;
	double getCurrentScale();
	afx_msg void OnEnKillfocusShowScalevalue();
};
