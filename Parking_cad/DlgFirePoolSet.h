#pragma once
#include "resource.h"
#include "afxwin.h"
#include "MyEdit.h"

// CDlgFirePoolSet �Ի���

class CDlgFirePoolSet : public CAcUiDialog
{
	DECLARE_DYNAMIC(CDlgFirePoolSet)

public:
	CDlgFirePoolSet(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgFirePoolSet();

// �Ի�������
	enum {
		IDD = IDD_DLG_FIREPOOL
	};

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	LRESULT OnKeepFocus(WPARAM, LPARAM);//��ģ̬���ڱ��ֽ���(1/3)
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
	// ��߱༭��
	CMyEdit m_editStoreyHeight;
	// ����ˮ���豸����¥��ռ��
	CMyEdit m_editFPProportion;
	// ���׮����
	CButton m_checkConsider;
	// ��������˨��ˮȷ��
	CButton m_checkSupply;
};
