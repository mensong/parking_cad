// DlgVillageSet.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "DlgVillageSet.h"
#include "afxdialogex.h"
#include "EquipmentroomTool.h"
#include "DBHelper.h"
#include "DlgFirePoolSet.h"


// CDlgVillageSet �Ի���
CWnd* CDlgVillageSet::parent_dlg = NULL;
CString CDlgVillageSet::ms_sTotalAreaText;
CString CDlgVillageSet::ms_sPorportionText;
bool CDlgVillageSet::ms_bConsider;
IMPLEMENT_DYNAMIC(CDlgVillageSet, CAcUiDialog)

void CDlgVillageSet::setConsiderState(bool& bConsider)
{
	ms_bConsider = bConsider;
}

CDlgVillageSet::CDlgVillageSet(CWnd* pParent /*=NULL*/)
	: CAcUiDialog(CDlgVillageSet::IDD, pParent)
{

}

CDlgVillageSet::~CDlgVillageSet()
{
}

static void __smartLog(bool*& data, bool isDataValid)
{
	if (isDataValid)
	{
		bool end = (*data);
		if (!end)
			CParkingLog::AddLogA("DEBUG_��֧�ֳ���ִ�еĲ���", 0, "CDlgVillageSet::DoDataExchange");
		delete data;
	}
}

void CDlgVillageSet::DoDataExchange(CDataExchange* pDX)
{
	Smart<bool*> end(new bool(false), __smartLog);
	CAcUiDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_TOTALAREA, m_totalAreaText);
	DDX_Control(pDX, IDC_EDIT_VILL_PROPORTION, m_porportionText);
	DDX_Control(pDX, IDC_VILLAGE_CHECK, m_considerCheck);
	*(end()) = true;
}
//��ģ̬���ڱ��ֽ���(2/3)
LRESULT CDlgVillageSet::OnKeepFocus(WPARAM, LPARAM)
{
	return TRUE;
}

BEGIN_MESSAGE_MAP(CDlgVillageSet, CAcUiDialog)
	ON_MESSAGE(WM_ACAD_KEEPFOCUS, &CDlgVillageSet::OnKeepFocus)//��ģ̬���ڱ��ֽ���(3/3)
	ON_WM_SIZE()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDOK, &CDlgVillageSet::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CDlgVillageSet::OnBnClickedCancel)
END_MESSAGE_MAP()


// CDlgVillageSet ��Ϣ�������


void CDlgVillageSet::OnBnClickedOk()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CString sTotalArea;
	m_totalAreaText.GetWindowText(sTotalArea);
	CString sPorportion;
	m_porportionText.GetWindowText(sPorportion);
	double dTotalArea = _tstof(sTotalArea.GetString());
	double dPorportion = _tstof(sPorportion.GetString());
	bool isConsider = false;
	if (1 == m_considerCheck.GetCheck())
	{
		isConsider = true;
	}
	else
	{
		isConsider = false;
	}
	if (sTotalArea.IsEmpty() || sPorportion.IsEmpty())
	{
		acedAlert(_T("\n�ؿ����������¥��ռ��Ϊ�գ�"));
		return;
	}
	CAcUiDialog::OnOK();
	Doc_Locker doc_locker;
	acedGetAcadDwgView()->SetFocus();
	villageSetOpera(dTotalArea, dPorportion, isConsider);
	parent_dlg->ShowWindow(SW_SHOW);
	ms_sTotalAreaText = sTotalArea;
	ms_sPorportionText = sPorportion;
	ms_bConsider = isConsider;
	CDlgFirePoolSet::setConsiderState(ms_bConsider);
}


void CDlgVillageSet::OnBnClickedCancel()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CAcUiDialog::OnOK();
	parent_dlg->ShowWindow(SW_SHOW);
}


BOOL CDlgVillageSet::OnInitDialog()
{
	CAcUiDialog::OnInitDialog();
	m_totalAreaText.SetWindowText(ms_sTotalAreaText);
	m_porportionText.SetWindowText(ms_sPorportionText);
	if (ms_bConsider)
	{
		m_considerCheck.SetCheck(1);
	}
	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	CenterWindow();
	return TRUE;  // return TRUE unless you set the focus to a control
				  // �쳣: OCX ����ҳӦ���� FALSE
}

void CDlgVillageSet::villageSetOpera(double& dTotalArea, double& dPorportion, bool& isConsider)
{
	double distributionRoomS = 0;
	if (isConsider)
	{
		distributionRoomS = (dTotalArea / 100000) * 550000000;
	}
	else
	{
		distributionRoomS = (dTotalArea / 100000) * 450000000;
	}
	if (dPorportion < 1 || dPorportion>100)
	{
		dPorportion = 100;
	}
	double scaleArea = distributionRoomS*((dPorportion) / 100);
	CEquipmentroomTool::layerSet();
	double sideLength = 0;
	double limitLength = 250000;
	AcDbObjectIdArray jigUseIds = CEquipmentroomTool::createArea(scaleArea, _T("��緿"), sideLength, limitLength, false);
	CEquipmentroomTool::setEntToLayer(jigUseIds);
	CEquipmentroomTool::jigShow(jigUseIds, sideLength);
}
