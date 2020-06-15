#pragma once
#include "resource.h"
#include "afxwin.h"
#include "MyEdit.h"

// CDlgZoomAxisNum �Ի���

class CDlgZoomAxisNum : public CAcUiDialog
{
	DECLARE_DYNAMIC(CDlgZoomAxisNum)

public:
	CDlgZoomAxisNum(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgZoomAxisNum();

// �Ի�������

	enum { IDD = IDD_DLG_AXISNUMZOOM };

protected:
	static void __stdcall delayDownClick(WPARAM wp, LPARAM lp, void* anyVal);
	static void __stdcall delayUpClick(WPARAM wp, LPARAM lp, void* anyVal);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	LRESULT OnKeepFocus(WPARAM, LPARAM);//��ģ̬���ڱ��ֽ���(1/3)
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonAxisnumUp();
	afx_msg void OnBnClickedButtonAxisnumDown();
	virtual BOOL OnInitDialog();
	// ��С��ť
	CButton m_btn_AxisNumDown;
	// ��ŷŴ�
	CButton m_btn_AxisNumUp;
	// ��ǰ����ֵչʾ
	CMyEdit m_show_ScaleValue;
	// ����ֵ�༭��
	CMyEdit m_edit_StepValue;
	static CString ms_sStepValue;
	double getCurrentScale();
	afx_msg void OnEnKillfocusShowScalevalue();
};
