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

	//zhangzechi
	void setBigFramePoints();
	bool setBlockInserPoint(std::string& Textstr);
	void SetPoints(AcGePoint2d& pt1, AcGePoint2d& pt2, AcGePoint2d& pt3, AcGePoint2d& pt4
		, AcGePoint2d& pt5, AcGePoint2d& pt6, AcGePoint2d& pt7, AcGePoint2d& pt8);
	void setBlokcLayer(const AcString& setlayername, AcDbObjectId& entityId);

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

//zhangzechi 
public:
	AcGePoint2d mBigFramept0;
	AcGePoint2d mBigFramept1;
	AcGePoint2d mBigFramept2;
	AcGePoint2d mBigFramept3;

protected:
	AcGePoint3d mExtsCentepoint;
	AcGePoint3d mInserPicPoint;
	
	bool InpromDRenceFromDWG(const double& inputLen);
	bool IsDistanceAppoint(AcGePoint3d& pt1, AcGePoint3d& pt2, AcGePoint3d& pt3);
	void setExchangeButtonImg();

	double mBlockextentsLen;
	double mBlockextentsWidth;
	double mMultiple;
public:
	afx_msg void OnBnClickedButtonExchange();
	CString m_sFrameWidth;
	CString m_sFrameLen;
	CButton m_btnExchange;
};
