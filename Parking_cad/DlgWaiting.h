#pragma once
#include "resource.h"
#include "PictureEx.h"

// CDlgWaiting �Ի���

class CDlgWaiting : public CAcUiDialog
{
	DECLARE_DYNAMIC(CDlgWaiting)

public:
	static void Show(bool bShow = true);
	static void Destroy();
	static void setUuid(const std::string& uuid);

public:
	CDlgWaiting(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgWaiting();

// �Ի�������
	enum {IDD = IDD_DLG_WAITTING_SHOW_RES};

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//virtual void OnOK() {}
	virtual void OnCancel() {}
	DECLARE_MESSAGE_MAP()

public:
	CPictureEx m_ctrlGif;
	bool m_bIsReady;
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	int getStatus(std::string& josn, CString& sMsg);
	
	static std::string ms_uuid;//�ӽ������ȡ���ش���uuid�Ľӿ�
	void parkingShow(AcGePoint2d& parkingShowPt, double& parkingShowRotation);
	void axisShow(AcGePoint2dArray& axisPts);
	void laneShow(AcGePoint2dArray& lanePts);
	void scopeShow(AcGePoint2dArray& park_columnPts);
	void pillarShow(AcGePoint2dArray& onePillarPts);

	bool layerSet(CString layerName,int layerColor);
	void setAxisLayerClose();
};
