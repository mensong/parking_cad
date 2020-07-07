#pragma once
#include "resource.h"
#include "afxcmn.h"
#include <vector>

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
	DECLARE_MESSAGE_MAP()
			
private:
	std::vector<std::string> vecTargetlayer;

public:
	bool readConfig(std::map<std::string, std::string>& keywordmap);
	void getAllLayers(std::map<std::string, std::string>& keywordmap, std::map<std::string, std::string>& outkeywordmap, std::vector<CString>& vecMismatchlayer);
	bool setLayerofEntity(const AcString& setlayername, AcDbObjectId& entityId);
	void createLayer(CString& layername, AcDbDatabase *pDb = acdbCurDwg());
	AcDbObjectIdArray GetAllEntityId(const TCHAR* layername, AcDbDatabase *pDb = acdbCurDwg());
	void deleteLayer(CString& layername, AcDbDatabase *pDb = acdbCurDwg());

public:
	CListCtrl m_list;

	int e_Item;    //�ձ༭����  
	int e_SubItem; //�ձ༭���� 
	CComboBox m_comBox;//������Ԫ�������б����
	bool haveccomboboxcreate;//��־�����б���Ѿ�������
	void createCcombobox(NM_LISTVIEW *pEditCtrl, CComboBox *createccomboboxobj, int &Item, int &SubItem, bool &havecreat);//������Ԫ�������б����
	void distroyCcombobox(CListCtrl *list, CComboBox* distroyccomboboxobj, int &Item, int &SubItem);//���ٵ�Ԫ�������б��
	afx_msg void OnNMClickList1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedButtonTrue();
	afx_msg void OnKillfocusCcomboBox();//��̬���������б��ʧȥ������Ӧ����
};
