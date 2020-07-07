#pragma once
#include "resource.h"
#include "afxcmn.h"
#include <vector>

// CDlgResetEntityLayer 对话框

class CDlgResetEntityLayer : public CAcUiDialog
{
	DECLARE_DYNAMIC(CDlgResetEntityLayer)

public:
	CDlgResetEntityLayer(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgResetEntityLayer();

// 对话框数据
	enum { IDD = IDD_DIALOG_RESETENTITYLAYER };


protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
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

	int e_Item;    //刚编辑的行  
	int e_SubItem; //刚编辑的列 
	CComboBox m_comBox;//生产单元格下拉列表对象
	bool haveccomboboxcreate;//标志下拉列表框已经被创建
	void createCcombobox(NM_LISTVIEW *pEditCtrl, CComboBox *createccomboboxobj, int &Item, int &SubItem, bool &havecreat);//创建单元格下拉列表框函数
	void distroyCcombobox(CListCtrl *list, CComboBox* distroyccomboboxobj, int &Item, int &SubItem);//销毁单元格下拉列表框
	afx_msg void OnNMClickList1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedButtonTrue();
	afx_msg void OnKillfocusCcomboBox();//动态生成下拉列表框失去焦点响应函数
};
