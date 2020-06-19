// DlgFirePumpHouseSet.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "DlgFirePumpHouseSet.h"
#include "afxdialogex.h"
#include "EquipmentroomTool.h"
#include "DBHelper.h"
#include "DlgFirePoolSet.h"
#define FirePumpHouse 120000000.0//�����÷�
// CDlgFirePumpHouseSet �Ի���
CWnd* CDlgFirePumpHouseSet::parent_dlg = NULL;
CString CDlgFirePumpHouseSet::ms_sPorportionText;
bool CDlgFirePumpHouseSet::ms_bMunicipalWaterSupply;

IMPLEMENT_DYNAMIC(CDlgFirePumpHouseSet, CAcUiDialog)

void CDlgFirePumpHouseSet::setWaterSupplyState(bool& bWaterSupply)
{
	ms_bMunicipalWaterSupply = bWaterSupply;
}

CDlgFirePumpHouseSet::CDlgFirePumpHouseSet(CWnd* pParent /*=NULL*/)
	: CAcUiDialog(CDlgFirePumpHouseSet::IDD, pParent)
{

}

CDlgFirePumpHouseSet::~CDlgFirePumpHouseSet()
{
}

static void __smartLog(bool*& data, bool isDataValid)
{
	if (isDataValid)
	{
		bool end = (*data);
		if (!end)
			CParkingLog::AddLogA("DEBUG_��֧�ֳ���ִ�еĲ���", 0, "CDlgFirePumpHouseSet::DoDataExchange");
		delete data;
	}
}

void CDlgFirePumpHouseSet::DoDataExchange(CDataExchange* pDX)
{
	Smart<bool*> end(new bool(false), __smartLog);
	CAcUiDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_LPH_PRO, m_porportionText);
	DDX_Control(pDX, IDC_FIREPUMP_CHECK, m_checkWaterSuppply);
	*(end()) = true;
}

//��ģ̬���ڱ��ֽ���(2/3)
LRESULT CDlgFirePumpHouseSet::OnKeepFocus(WPARAM, LPARAM)
{
	return TRUE;
}

BEGIN_MESSAGE_MAP(CDlgFirePumpHouseSet, CAcUiDialog)
	ON_MESSAGE(WM_ACAD_KEEPFOCUS, &CDlgFirePumpHouseSet::OnKeepFocus)//��ģ̬���ڱ��ֽ���(3/3)
	ON_WM_SIZE()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDOK, &CDlgFirePumpHouseSet::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CDlgFirePumpHouseSet::OnBnClickedCancel)
END_MESSAGE_MAP()


// CDlgFirePumpHouseSet ��Ϣ�������


void CDlgFirePumpHouseSet::OnBnClickedOk()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CString sPorportion;
	m_porportionText.GetWindowText(sPorportion);
	double dPorportion = _tstof(sPorportion.GetString());
	bool isMunicipalWaterSupply = false;
	if (1 == m_checkWaterSuppply.GetCheck())
	{
		isMunicipalWaterSupply = true;
	}
	else
	{
		isMunicipalWaterSupply = false;
	}
	if (sPorportion.IsEmpty())
	{
		acedAlert(_T("\n��¥��ռ��Ϊ�գ�"));
		return;
	}
	CAcUiDialog::OnOK();
	Doc_Locker doc_locker;
	acedGetAcadDwgView()->SetFocus();
	villageSetOpera(dPorportion, isMunicipalWaterSupply);
	parent_dlg->ShowWindow(SW_SHOW);
	ms_sPorportionText = sPorportion;
	ms_bMunicipalWaterSupply = isMunicipalWaterSupply;
	CDlgFirePoolSet::setWaterSupplyState(ms_bMunicipalWaterSupply);
}


void CDlgFirePumpHouseSet::OnBnClickedCancel()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CAcUiDialog::OnCancel();
	parent_dlg->ShowWindow(SW_SHOW);
}


BOOL CDlgFirePumpHouseSet::OnInitDialog()
{
	CAcUiDialog::OnInitDialog();

	DlgHelper::AdjustPosition(this, DlgHelper::TOP_LEFT);

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	m_porportionText.SetWindowText(ms_sPorportionText);
	if (ms_bMunicipalWaterSupply)
	{
		m_checkWaterSuppply.SetCheck(1);
	}
	return TRUE; 
}

void CDlgFirePumpHouseSet::villageSetOpera(double& dPorportion, bool& isMunicipalWaterSupply)
{
	double dfirePumpHouseArea = FirePumpHouse;
	if (isMunicipalWaterSupply)
	{
		dfirePumpHouseArea = FirePumpHouse - 40000000;
	}
	CEquipmentroomTool::layerSet();
	double FirePumpHouseSideLength = 0;
	double limitLength = 5500;
	if (dPorportion < 1 || dPorportion>100)
	{
		dPorportion = 100;
	}
	double dnewArea = dfirePumpHouseArea*((dPorportion) / 100);
	if (dnewArea == 0)
		return;
	AcDbObjectIdArray FirePumpHouseJigUseIds = CEquipmentroomTool::createArea(dnewArea, _T("�����÷�"), FirePumpHouseSideLength, limitLength);
	CEquipmentroomTool::setEntToLayer(FirePumpHouseJigUseIds);
	CEquipmentroomTool::jigShow(FirePumpHouseJigUseIds, FirePumpHouseSideLength);
}
