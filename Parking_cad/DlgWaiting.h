#pragma once
#include "resource.h"
#include "PictureEx.h"

// CDlgWaiting 对话框

class CDlgWaiting : public CAcUiDialog
{
	DECLARE_DYNAMIC(CDlgWaiting)

public:
	static void Show(bool bShow = true);
	static void Destroy();
	static void setUuid(const std::string& uuid);
	static void setGetUrl(const std::string& geturl);

public:
	CDlgWaiting(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgWaiting();

// 对话框数据
	enum {IDD = IDD_DLG_WAITTING_SHOW_RES};

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual void OnOK();
	virtual void OnCancel();
	DECLARE_MESSAGE_MAP()

public:
	CPictureEx m_ctrlGif;
	bool m_bIsReady;
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	int getStatus(std::string& josn, CString& sMsg);
	
	static std::string ms_uuid;//从界面类获取到回传的uuid的接口
	static std::string ms_geturl;//从初始化类取到配置文件中的IP地址接口
	void parkingShow(const AcGePoint2d& parkingShowPt, const double& parkingShowRotation);
	void axisShow(const AcGePoint2dArray& axisPts);
	void laneShow(const AcGePoint2dArray& lanePts);
	void scopeShow(const AcGePoint2dArray& park_columnPts);
	void pillarShow(const AcGePoint2dArray& onePillarPts);
	void arrowShow(const AcGePoint2dArray& oneArrowPts);

	bool layerSet(const CString& layerName,const int& layerColor);
	void setAxisLayerClose();
};
