#pragma once
#include "resource.h"
#include "afxcmn.h"
#include <vector>
#include "afxwin.h"

struct _SortStrLen
{
	bool operator()(const std::string& _Left, const std::string& _Right) const
	{
		if (_Left.size() != _Right.size())
			return (_Left.size() > _Right.size());
		
		return _Left < _Right;
	}
};

// CDlgResetEntityLayer �Ի���

class CDlgResetEntityLayer : public CAcUiDialog
{
	DECLARE_DYNAMIC(CDlgResetEntityLayer)

public:
	CDlgResetEntityLayer(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgResetEntityLayer();

// �Ի�������
	enum { IDD = IDD_DIALOG_RESETENTITYLAYER };


protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	afx_msg LRESULT OnAcadKeepFocus(WPARAM, LPARAM);
	virtual BOOL OnInitDialog();
	virtual BOOL DestroyWindow();
	virtual void OnCancel() override;
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	DECLARE_MESSAGE_MAP()
			
private:
	int standardlayersNum;
	std::vector<std::string> vecTargetlayer;

public:
	bool readParkingConfig();
	bool readConfig(std::map<std::string, std::string>& keywordmap);
	void getAllLayers(std::map<std::string, std::string>& keywordmap, std::map<std::string, std::string>& outkeywordmap, std::vector<CString>& vecMismatchlayer);
	bool setLayerofEntity(const AcString& setlayername, AcDbObjectId& entityId);
	void createLayer(CString& layername, AcDbDatabase *pDb = acdbCurDwg());
	AcDbObjectIdArray GetAllEntityId(const TCHAR* layername, AcDbDatabase *pDb = acdbCurDwg());
	void deleteLayer(CString& layername, AcDbDatabase *pDb = acdbCurDwg());
	int setCombox(CString& text);
	void hideCombox();

public:
	afx_msg void OnNMClickList1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedButtonTrue();
	afx_msg void OnCbnSelchangeComboxCreateid();
	CListCtrl m_list;
	CComboBox m_comBox;
	int m_ComItem; // Ҫ����Combo Box ����   
	int m_ComSubItem; //Ҫ����Combo Box���� Ϊ���������������,���������bool����Ҳһ��  
	bool  ComneedSave;	
	CStatic m_standardlayersNum;
};
