// DlgFirePoolSet.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "DlgFirePoolSet.h"
#include "afxdialogex.h"
#include "DBHelper.h"
#include "EquipmentroomTool.h"
#include "DlgVillageSet.h"
#include "DlgFirePumpHouseSet.h"

// CDlgFirePoolSet �Ի���
CWnd* CDlgFirePoolSet::parent_dlg = NULL;
CString CDlgFirePoolSet::ms_sStoreyHeightText;
CString CDlgFirePoolSet::ms_sPorportionText;
bool CDlgFirePoolSet::ms_bConsider;
bool CDlgFirePoolSet::ms_bMunicipalWaterSupply;

IMPLEMENT_DYNAMIC(CDlgFirePoolSet, CAcUiDialog)

void CDlgFirePoolSet::setConsiderState(bool& bConsider)
{
	ms_bConsider = bConsider;
}

void CDlgFirePoolSet::setWaterSupplyState(bool& bWaterSupply)
{
	ms_bMunicipalWaterSupply = bWaterSupply;
}

CDlgFirePoolSet::CDlgFirePoolSet(CWnd* pParent /*=NULL*/)
	: CAcUiDialog(CDlgFirePoolSet::IDD, pParent)
{

}

CDlgFirePoolSet::~CDlgFirePoolSet()
{
}

static void __smartLog(bool*& data, bool isDataValid)
{
	if (isDataValid)
	{
		bool end = (*data);
		if (!end)
			CParkingLog::AddLogA("DEBUG_��֧�ֳ���ִ�еĲ���", 0, "CDlgFirePoolSet::DoDataExchange");
		delete data;
	}
}

void CDlgFirePoolSet::DoDataExchange(CDataExchange* pDX)
{
	Smart<bool*> end(new bool(false), __smartLog);
	CAcUiDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_STOREYHEIGHT, m_editStoreyHeight);
	DDX_Control(pDX, IDC_EDIT_FP_PROPORTION, m_editFPProportion);
	DDX_Control(pDX, IDC_FP_CONSIDERCHECK, m_checkConsider);
	DDX_Control(pDX, IDC_FP_SUPPLYCHECK, m_checkSupply);
	*(end()) = true;
}

//��ģ̬���ڱ��ֽ���(2/3)
LRESULT CDlgFirePoolSet::OnKeepFocus(WPARAM, LPARAM)
{
	return TRUE;
}

BEGIN_MESSAGE_MAP(CDlgFirePoolSet, CAcUiDialog)
	ON_MESSAGE(WM_ACAD_KEEPFOCUS, &CDlgFirePoolSet::OnKeepFocus)//��ģ̬���ڱ��ֽ���(3/3)
	ON_WM_SIZE()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDOK, &CDlgFirePoolSet::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CDlgFirePoolSet::OnBnClickedCancel)
END_MESSAGE_MAP()


// CDlgFirePoolSet ��Ϣ�������


void CDlgFirePoolSet::OnBnClickedOk()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CString sStoreyHeight;
	m_editStoreyHeight.GetWindowText(sStoreyHeight);
	CString sPorportion;
	m_editFPProportion.GetWindowText(sPorportion);
	if (sPorportion.IsEmpty() || sStoreyHeight.IsEmpty())
	{
		acedAlert(_T("\n��߻���¥��ռ�Ȳ�������Ϊ�գ�"));
		return;
	}
	double dStoreyHeight = _tstof(sStoreyHeight.GetString());
	double dPorportion = _tstof(sPorportion.GetString());
	bool isConsider = false;
	if (m_checkConsider.GetCheck()==1)
	{
		isConsider = true;
	}
	else
	{
		isConsider = false;
	}
	bool isMunicipalWaterSupply = false;
	if (1 == m_checkSupply.GetCheck())
	{
		isMunicipalWaterSupply = true;
	}
	else
	{
		isMunicipalWaterSupply = false;
	}
	CAcUiDialog::OnOK();
	Doc_Locker doc_locker;
	acedGetAcadDwgView()->SetFocus();
	firePoolSetOpera(dStoreyHeight, dPorportion, isConsider, isMunicipalWaterSupply);
	parent_dlg->ShowWindow(SW_SHOW);
	ms_sStoreyHeightText = sStoreyHeight;
	ms_sPorportionText = sPorportion;
	ms_bConsider = isConsider;
	ms_bMunicipalWaterSupply = isMunicipalWaterSupply;
	CDlgVillageSet::setConsiderState(ms_bConsider);
	CDlgFirePumpHouseSet::setWaterSupplyState(ms_bMunicipalWaterSupply);
}


void CDlgFirePoolSet::OnBnClickedCancel()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CAcUiDialog::OnCancel();
	parent_dlg->ShowWindow(SW_SHOW);
}


BOOL CDlgFirePoolSet::OnInitDialog()
{
	CAcUiDialog::OnInitDialog();

	DlgHelper::AdjustPosition(this, DlgHelper::TOP_LEFT);

	m_editStoreyHeight.SetWindowText(ms_sStoreyHeightText);
	m_editFPProportion.SetWindowText(ms_sPorportionText);
	if (ms_bConsider)
	{
		m_checkConsider.SetCheck(1);
	}
	if (ms_bMunicipalWaterSupply)
	{
		m_checkSupply.SetCheck(1);
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
				  // �쳣: OCX ����ҳӦ���� FALSE
}

void CDlgFirePoolSet::firePoolSetOpera(double& dStoreyHeight, double& dPorportion, bool& isConsider, bool& isMunicipalWaterSupply)
{
	double FirePoolS = 0;
	if (isConsider)
	{
		FirePoolS = ((40 * 3.6 * 2 + 20 * 3.6 * 2 + 80 * 3.6 * 1.5) / (dStoreyHeight - 2.3)) * 1000000;
	}
	else
	{
		FirePoolS = ((40 * 3.6 * 2 + 20 * 3.6 * 2 + 30 * 3.6 * 1) / (dStoreyHeight - 2.3)) * 1000000;
	}
	if (isMunicipalWaterSupply)
	{
		FirePoolS = FirePoolS - 288000000;
		if (FirePoolS<0)
		{
			acedAlert(_T("\n�����߲�������"));
			return;
		}
	}
	CEquipmentroomTool::layerSet();
	double FirePoolsideLength = 0;
	double limitLength = 0;
	if (dPorportion < 1 || dPorportion>100)
	{
		dPorportion = 100;
	}
	double dnewArea = FirePoolS*((dPorportion) / 100);
	if (dnewArea == 0)
		return;
	AcDbObjectIdArray FirePooljigUseIds = CEquipmentroomTool::createArea(dnewArea, _T("����ˮ��"), FirePoolsideLength, limitLength);
	CEquipmentroomTool::setEntToLayer(FirePooljigUseIds);
	CEquipmentroomTool::jigShow(FirePooljigUseIds, FirePoolsideLength);
}