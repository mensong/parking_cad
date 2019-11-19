#pragma once
#include "afxwin.h"
#include "resource.h"
#include "CadPreviewCtrl.h"

// CDlgAddFrame �Ի���

class CDlgAddFrame : public CAcUiDialog
{
	DECLARE_DYNAMIC(CDlgAddFrame)

public:
	CDlgAddFrame(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgAddFrame();

// �Ի�������
	enum { IDD = IDD_DLG_ADD_FRAME };

protected:
	void refreshPreview();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()

protected:
	CEdit m_editFrameLen;
	CEdit m_editFrameWidth;
	CCadPreviewCtrl m_staPreview;

public:
	afx_msg void OnBnClickedRadA0();
	afx_msg void OnBnClickedRadA1();
	afx_msg void OnBnClickedRadA2();
	afx_msg void OnBnClickedRadA3();
	afx_msg void OnBnClickedRadA4();
	afx_msg void OnBnClickedOk();
};
