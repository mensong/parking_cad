#pragma once
#include "resource.h"
#include "afxwin.h"
#include "MyEdit.h"

// CDlgLifePumpHouseSet �Ի���

class CDlgLifePumpHouseSet : public CAcUiDialog
{
	DECLARE_DYNAMIC(CDlgLifePumpHouseSet)

public:
	CDlgLifePumpHouseSet(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgLifePumpHouseSet();

// �Ի�������
	enum {
		IDD = IDD_DLG_LIFEPUMPHOUSE
	};

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	LRESULT OnKeepFocus(WPARAM, LPARAM);//��ģ̬���ڱ��ֽ���(1/3)
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	// ��¥��ռ��
	CMyEdit m_textProportion;
	static CWnd* parent_dlg;
	static CString ms_sPorportionText;
	void lifePumpHouseSetOpera(double& dPorportion);
	virtual BOOL OnInitDialog();
};
