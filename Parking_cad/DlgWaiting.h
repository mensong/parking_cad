#pragma once
#include "resource.h"
#include "PictureEx.h"
#include "afxwin.h"

// CDlgWaiting �Ի���

class CDlgWaiting : public CAcUiDialog
{
	DECLARE_DYNAMIC(CDlgWaiting)

public:
	static void Show(bool bShow = true);
	static void Destroy();
	static void setUuid(const std::string& uuid, const bool& useV1);
	static void setGetUrlPortOne(const std::string& strGeturlPortone);
	static void setGetUrlPortTwo(const std::string& strGeturlPorttwo);

public:
	CDlgWaiting(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgWaiting();

// �Ի�������
	enum {IDD = IDD_DLG_WAITTING_SHOW_RES};

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	DECLARE_MESSAGE_MAP()

public:
	CPictureEx m_ctrlGif;
	CStatic m_staStatusText;
	bool m_bIsReady;
	CString m_sStatus;	
	int getStatus(std::string& josn, std::string& sMsg,CString& sIndex);
	static std::string ms_uuid;//�ӽ������ȡ���ش���uuid�Ľӿ�
	static bool ms_bUseV1;
	static std::string ms_strGeturlPortone;//�ӳ�ʼ����ȡ�������ļ��ж˿�1��IP��ַ�ӿ�
	static std::string ms_strGeturlPorttwo;//�ӳ�ʼ����ȡ�������ļ��ж˿�2��IP��ַ��
	void parkingShow(const AcGePoint2d& parkingShowPt, const double& parkingShowRotation, const CString& blockName);
	void axisShow(const AcGePoint2dArray& axisPts);
	void laneShow(const AcGePoint2dArray& lanePts);
	void scopeShow(const AcGePoint2dArray& park_columnPts);
	void pillarShow(const AcGePoint2dArray& onePillarPts);
	void arrowShow(const AcGePoint2dArray& oneArrowPts);
	bool layerSet(const CString& layerName,const int& layerColor);
	void setAxisLayerClose();
	void creatNewParking(const double& dParkingLength,const double& dParkingWidth, CString& blockName);
	bool getDataforJson(const std::string& json, CString& sMsg);
};
