#pragma once
#include "resource.h"
#include "afxwin.h"
#include "MyEdit.h"

// CDlgFirePumpHouseSet �Ի���

class CDlgFirePumpHouseSet : public CAcUiDialog
{
	DECLARE_DYNAMIC(CDlgFirePumpHouseSet)

public:
	CDlgFirePumpHouseSet(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgFirePumpHouseSet();

// �Ի�������
	enum { IDD = IDD_DLG_FIREPUMPHOUSE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	LRESULT OnKeepFocus(WPARAM, LPARAM);//��ģ̬���ڱ��ֽ���(1/3)
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	// �����÷���¥��ռ��
	CMyEdit m_porportionText;
	// ��������˨�Ƿ���������ˮ��ѡ��
	CButton m_checkWaterSuppply;
	virtual BOOL OnInitDialog();
	void villageSetOpera(double& dPorportion, bool& isMunicipalWaterSupply);
	static CWnd* parent_dlg;
	static CString ms_sPorportionText;
	static bool ms_bMunicipalWaterSupply;
	static void setWaterSupplyState(bool& bWaterSupply);
};
