#pragma once
#include "afxwin.h"
#include "resource.h"
#include "CadPreviewCtrl.h"

// CDlgAddFrame 对话框

class CDlgAddFrame : public CAcUiDialog
{
	DECLARE_DYNAMIC(CDlgAddFrame)

public:
	CDlgAddFrame(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgAddFrame();

// 对话框数据
	enum { IDD = IDD_DLG_ADD_FRAME };

	void setContextExtents(const AcDbExtents& ext);

protected:
	void refreshPreview();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	DECLARE_MESSAGE_MAP()

protected:
	CEdit m_editFrameLen;
	CEdit m_editFrameWidth;
	CCadPreviewCtrl m_staPreview;
	AcDbExtents m_extents;

public:
	afx_msg void OnBnClickedRadA0();
	afx_msg void OnBnClickedRadA1();
	afx_msg void OnBnClickedRadA2();
	afx_msg void OnBnClickedRadA3();
	afx_msg void OnBnClickedRadA4();
	afx_msg void OnBnClickedOk();
};
