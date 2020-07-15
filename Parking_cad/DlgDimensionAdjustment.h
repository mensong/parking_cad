#pragma once
#include "resource.h"
#include "MyEdit.h"

// CDlgDimensionAdjustment 对话框

class CDlgDimensionAdjustment : public CAcUiDialog
{
	DECLARE_DYNAMIC(CDlgDimensionAdjustment)

public:
	CDlgDimensionAdjustment(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgDimensionAdjustment();

// 对话框数据
	enum { IDD = IDD_DIALOG_DIMADJUSTMENT };	

protected:
	static void __stdcall delayDownClick(WPARAM wp, LPARAM lp, void* anyVal);
	static void __stdcall delayUpClick(WPARAM wp, LPARAM lp, void* anyVal);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	virtual BOOL DestroyWindow();
	virtual void OnCancel() override;
	LRESULT OnKeepFocus(WPARAM, LPARAM);//非模态窗口保持焦点(1/3)
	DECLARE_MESSAGE_MAP()

public:
	// 缩小按钮
	CButton m_btn_NumDown;
	// 轴号放大
	CButton m_btn_NumUp;
	// 当前缩放值展示
	CMyEdit m_show_ScaleValue;
	// 步进值编辑框
	CMyEdit m_edit_StepValue;
	static CString ms_sStepValue;
	double getCurrentScale();
	static bool setCurrentScale(double scalenum);
	afx_msg void OnBnClickedButtonNumUp();
	afx_msg void OnBnClickedButtonNumDown();
};
