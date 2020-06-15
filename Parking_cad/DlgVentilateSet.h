#pragma once
#include "resource.h"
#include "afxwin.h"
#include "MyEdit.h"

// CDlgVentilateSet �Ի���

class CDlgVentilateSet : public CAcUiDialog
{
	DECLARE_DYNAMIC(CDlgVentilateSet)

public:
	CDlgVentilateSet(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgVentilateSet();
	// �Ի�������
	enum {
		IDD = IDD_DLG_VENTILATE
	};
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	LRESULT OnKeepFocus(WPARAM, LPARAM);//��ģ̬���ڱ��ֽ���(1/3)
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	virtual BOOL OnInitDialog();
	// �ؿ�������༭��
	CMyEdit m_totalAreaText;
	// ͨ���豸����¥��ռ��
	CMyEdit m_porportionText;
	void ventilateOpera(double& dTotalArea, double& dPorportion,int& roomCount);
	static CWnd* parent_dlg;
	static CString ms_sTotalAreaText;
	static CString ms_sPorportionText;
	afx_msg void OnBnClickedCancel();
};
