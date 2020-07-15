#pragma once
#include "resource.h"
#include "MyEdit.h"

// CDlgDimensionAdjustment �Ի���

class CDlgDimensionAdjustment : public CAcUiDialog
{
	DECLARE_DYNAMIC(CDlgDimensionAdjustment)

public:
	CDlgDimensionAdjustment(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgDimensionAdjustment();

// �Ի�������
	enum { IDD = IDD_DIALOG_DIMADJUSTMENT };	

protected:
	static void __stdcall delayDownClick(WPARAM wp, LPARAM lp, void* anyVal);
	static void __stdcall delayUpClick(WPARAM wp, LPARAM lp, void* anyVal);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	virtual BOOL OnInitDialog();
	virtual BOOL DestroyWindow();
	virtual void OnCancel() override;
	LRESULT OnKeepFocus(WPARAM, LPARAM);//��ģ̬���ڱ��ֽ���(1/3)
	DECLARE_MESSAGE_MAP()

public:
	// ��С��ť
	CButton m_btn_NumDown;
	// ��ŷŴ�
	CButton m_btn_NumUp;
	// ��ǰ����ֵչʾ
	CMyEdit m_show_ScaleValue;
	// ����ֵ�༭��
	CMyEdit m_edit_StepValue;
	static CString ms_sStepValue;
	double getCurrentScale();
	static bool setCurrentScale(double scalenum);
	afx_msg void OnBnClickedButtonNumUp();
	afx_msg void OnBnClickedButtonNumDown();
};
