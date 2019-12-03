#pragma once
#include "afxwin.h"
#include "resource.h"
#include <vector>
//-----------------------------------------------------------------------------
class CDlgEquipmentRoomSet : public CAcUiDialog 
{
	DECLARE_DYNAMIC (CDlgEquipmentRoomSet)

public:
	CDlgEquipmentRoomSet (CWnd *pParent =NULL, HINSTANCE hInstance =NULL) ;

	enum { IDD = IDD_DLG_ADD_EQUIPMENT_ROOM} ;

protected:
	virtual void DoDataExchange (CDataExchange *pDX) ;
	afx_msg LRESULT OnAcadKeepFocus (WPARAM, LPARAM) ;

	DECLARE_MESSAGE_MAP()
private:
	// 通风按钮
	CButton m_btnVentilate;
public:
	afx_msg void OnBnClickedButtonVentilate();
	afx_msg void OnBnClickedButtonVillageSet();
	afx_msg void OnBnClickedButtonLifepumphouseSet();
	afx_msg void OnBnClickedButtonLivingwatertankSet();
	afx_msg void OnBnClickedButtonFirepumphouseSet();
	afx_msg void OnBnClickedButtonFirepoolSet();
	afx_msg void OnBnClickedButtonCustom();
	std::vector<AcGePoint2dArray> allPartitionPts;//分区点组

} ;
