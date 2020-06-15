#pragma once
#include "resource.h"
#include "afxwin.h"
#include "MyEdit.h"


// CDlgLivingWaterTankSet �Ի���

class CDlgLivingWaterTankSet : public CAcUiDialog
{
	DECLARE_DYNAMIC(CDlgLivingWaterTankSet)

public:
	CDlgLivingWaterTankSet(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgLivingWaterTankSet();

// �Ի�������

	enum { IDD = IDD_DLG_LIVEWATERTANK };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	LRESULT OnKeepFocus(WPARAM, LPARAM);//��ģ̬���ڱ��ֽ���(1/3)
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	// С������
	CEdit m_houseHoldsEdit;
	// ��¥��ռ�ȱ༭��
	CMyEdit m_porportionText;
	void livingWaterTankOpera(int& iHouseHolds, double& dPorportion);
	static CWnd* parent_dlg;
	static CString ms_sHouseHoldsText;
	static CString ms_sPorportionText;
	virtual BOOL OnInitDialog();
};
