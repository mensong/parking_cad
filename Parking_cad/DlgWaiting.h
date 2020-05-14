#pragma once
#include "resource.h"
#include "PictureEx.h"
#include "afxwin.h"

// CDlgWaiting 对话框

class CDlgWaiting : public CAcUiDialog
{
	DECLARE_DYNAMIC(CDlgWaiting)

public:
	static void Show(bool bShow = true);
	static void Destroy();
	static void setUuid(const std::string& uuid, const bool& useV1, const bool& useManyShow);
	static void setGetUrlPortOne(const std::string& strGeturlPortone);
	static void setGetUrlPortTwo(const std::string& strGeturlPorttwo);

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
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	DECLARE_MESSAGE_MAP()

public:
	static std::string ms_uuid;//从界面类获取到回传的uuid的接口
	static bool ms_bUseV1;
	static bool ms_bUseManyShow;
	static std::string ms_strGeturlPortone;//从初始化类取到配置文件中端口1的IP地址接口
	static std::string ms_strGeturlPorttwo;//从初始化类取到配置文件中端口2的IP地址接
	CPictureEx m_ctrlGif;
	CStatic m_staStatusText;
	bool m_bIsReady;
	CString m_sStatus;
	int getStatus(std::string& josn, std::string& sMsg, CString& sIndex);
	void setLayerClose(const CString& layerName);
	int getJsonForLocal(std::string& json, std::string& sMsg, CString& sIndex, int& iCount);
};
