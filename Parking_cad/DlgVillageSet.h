#pragma once
#include "resource.h"
#include "afxwin.h"
#include "MyEdit.h"

// CDlgVillageSet �Ի���

class CDlgVillageSet : public CAcUiDialog
{
	DECLARE_DYNAMIC(CDlgVillageSet)

public:
	CDlgVillageSet(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgVillageSet();
	// �Ի�������
	enum {
		IDD = IDD_DLG_VILLAGE
	};

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	LRESULT OnKeepFocus(WPARAM, LPARAM);//��ģ̬���ڱ��ֽ���(1/3)
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	static CWnd* parent_dlg;
	virtual BOOL OnInitDialog();
	// С������� 
	CMyEdit m_totalAreaText;
	// ��¥��ռ��
	CMyEdit m_porportionText;
	// �Ƿ��ǳ��׮
	CButton m_considerCheck;
	void villageSetOpera(double& dTotalArea, double& dPorportion, bool& isConsider);
	static CString ms_sTotalAreaText;
	static CString ms_sPorportionText;
	static bool ms_bConsider;
	static void setConsiderState(bool& bConsider);
};
