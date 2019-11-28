#include "StdAfx.h"
#include "DlgEquipmentRoomSet.h"
#include "DBHelper.h"
//-----------------------------------------------------------------------------
IMPLEMENT_DYNAMIC (CDlgEquipmentRoomSet, CAcUiDialog)

BEGIN_MESSAGE_MAP(CDlgEquipmentRoomSet, CAcUiDialog)
	ON_MESSAGE(WM_ACAD_KEEPFOCUS, OnAcadKeepFocus)
	ON_BN_CLICKED(IDC_BUTTON_VENTILATE, &CDlgEquipmentRoomSet::OnBnClickedButtonVentilate)
	ON_BN_CLICKED(IDC_BUTTON_VILLAGE_SET, &CDlgEquipmentRoomSet::OnBnClickedButtonVillageSet)
	ON_BN_CLICKED(IDC_BUTTON_LIFEPUMPHOUSE_SET, &CDlgEquipmentRoomSet::OnBnClickedButtonLifepumphouseSet)
	ON_BN_CLICKED(IDC_BUTTON_LIVINGWATERTANK_SET, &CDlgEquipmentRoomSet::OnBnClickedButtonLivingwatertankSet)
	ON_BN_CLICKED(IDC_BUTTON_FIREPUMPHOUSE_SET, &CDlgEquipmentRoomSet::OnBnClickedButtonFirepumphouseSet)
	ON_BN_CLICKED(IDC_BUTTON_FIREPOOL_SET, &CDlgEquipmentRoomSet::OnBnClickedButtonFirepoolSet)
END_MESSAGE_MAP()

//-----------------------------------------------------------------------------
CDlgEquipmentRoomSet::CDlgEquipmentRoomSet (CWnd *pParent /*=NULL*/, HINSTANCE hInstance /*=NULL*/) : CAcUiDialog (CDlgEquipmentRoomSet::IDD, pParent, hInstance) {
}

//-----------------------------------------------------------------------------
void CDlgEquipmentRoomSet::DoDataExchange (CDataExchange *pDX) {
	CAcUiDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BUTTON_VENTILATE, m_btnVentilate);
}

//-----------------------------------------------------------------------------
//----- Needed for modeless dialogs to keep focus.
//----- Return FALSE to not keep the focus, return TRUE to keep the focus
LRESULT CDlgEquipmentRoomSet::OnAcadKeepFocus (WPARAM, LPARAM) {
	return (TRUE) ;
}


void CDlgEquipmentRoomSet::OnBnClickedButtonVentilate()
{
	// TODO: 在此添加控件通知处理程序代码
	HideDialogHolder holder(this);
	Doc_Locker doc_locker;
	DBHelper::CallCADCommand(_T("VentilationEquipmentroomSet "));
}


void CDlgEquipmentRoomSet::OnBnClickedButtonVillageSet()
{
	// TODO: 在此添加控件通知处理程序代码
	HideDialogHolder holder(this);
	Doc_Locker doc_locker;
	DBHelper::CallCADCommand(_T("VillageSet "));
}


void CDlgEquipmentRoomSet::OnBnClickedButtonLifepumphouseSet()
{
	// TODO: 在此添加控件通知处理程序代码
	HideDialogHolder holder(this);
	Doc_Locker doc_locker;
	DBHelper::CallCADCommand(_T("LifePumpHouseSet "));
}


void CDlgEquipmentRoomSet::OnBnClickedButtonLivingwatertankSet()
{
	// TODO: 在此添加控件通知处理程序代码
	HideDialogHolder holder(this);
	Doc_Locker doc_locker;
	DBHelper::CallCADCommand(_T("FirePumpHouseSet "));
}


void CDlgEquipmentRoomSet::OnBnClickedButtonFirepumphouseSet()
{
	// TODO: 在此添加控件通知处理程序代码
	HideDialogHolder holder(this);
	Doc_Locker doc_locker;
	DBHelper::CallCADCommand(_T("FirePumpHouseSet "));
}


void CDlgEquipmentRoomSet::OnBnClickedButtonFirepoolSet()
{
	// TODO: 在此添加控件通知处理程序代码
	HideDialogHolder holder(this);
	Doc_Locker doc_locker;
	DBHelper::CallCADCommand(_T("FirePoolSet "));
}
