// DlgLifePumpHouseSet.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "DlgLifePumpHouseSet.h"
#include "afxdialogex.h"
#include "EquipmentroomTool.h"
#include "DBHelper.h"

// CDlgLifePumpHouseSet �Ի���
#define LifePumpHouse 80000000.0//����÷����
CWnd* CDlgLifePumpHouseSet::parent_dlg = NULL;
CString CDlgLifePumpHouseSet::ms_sPorportionText;

IMPLEMENT_DYNAMIC(CDlgLifePumpHouseSet, CAcUiDialog)

CDlgLifePumpHouseSet::CDlgLifePumpHouseSet(CWnd* pParent /*=NULL*/)
	: CAcUiDialog(CDlgLifePumpHouseSet::IDD, pParent)
{

}

CDlgLifePumpHouseSet::~CDlgLifePumpHouseSet()
{
}

static void __smartLog(bool*& data, bool isDataValid)
{
	if (isDataValid)
	{
		bool end = (*data);
		if (!end)
			CParkingLog::AddLogA("DEBUG_��֧�ֳ���ִ�еĲ���", 0, "CDlgLifePumpHouseSet::DoDataExchange");
		delete data;
	}
}

void CDlgLifePumpHouseSet::DoDataExchange(CDataExchange* pDX)
{
	Smart<bool*> end(new bool(false), __smartLog);
	CAcUiDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_LPH_PRO, m_textProportion);
	*(end()) = true;
}

//��ģ̬���ڱ��ֽ���(2/3)
LRESULT CDlgLifePumpHouseSet::OnKeepFocus(WPARAM, LPARAM)
{
	return TRUE;
}

BEGIN_MESSAGE_MAP(CDlgLifePumpHouseSet, CAcUiDialog)
	ON_MESSAGE(WM_ACAD_KEEPFOCUS, &CDlgLifePumpHouseSet::OnKeepFocus)//��ģ̬���ڱ��ֽ���(3/3)
	ON_WM_SIZE()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDOK, &CDlgLifePumpHouseSet::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CDlgLifePumpHouseSet::OnBnClickedCancel)
END_MESSAGE_MAP()


// CDlgLifePumpHouseSet ��Ϣ�������


void CDlgLifePumpHouseSet::OnBnClickedOk()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CString sPorportion;
	m_textProportion.GetWindowText(sPorportion);
	double dPorportion = _tstof(sPorportion.GetString());
	if (sPorportion.IsEmpty())
	{
		acedAlert(_T("\n��¥��ռ��Ϊ�գ�"));
		return;
	}
	CAcUiDialog::OnOK();
	Doc_Locker doc_locker;
	acedGetAcadDwgView()->SetFocus();
	lifePumpHouseSetOpera(dPorportion);
	parent_dlg->ShowWindow(SW_SHOW);
	ms_sPorportionText = sPorportion;
}


void CDlgLifePumpHouseSet::OnBnClickedCancel()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CAcUiDialog::OnOK();
	parent_dlg->ShowWindow(SW_SHOW);
}

void CDlgLifePumpHouseSet::lifePumpHouseSetOpera(double& dPorportion)
{
	if (dPorportion < 1 || dPorportion>100)
	{
		dPorportion = 100;
	}
	CEquipmentroomTool::layerSet();
	double LifepumpAreaSideLength = 0;
	double limitLength = 4000;
	double dnewArea = LifePumpHouse*((dPorportion) / 100);
	AcDbObjectIdArray LifepumpAreaJigUseIds = CEquipmentroomTool::createArea(dnewArea, _T("����÷�"), LifepumpAreaSideLength, limitLength);
	CEquipmentroomTool::setEntToLayer(LifepumpAreaJigUseIds);
	CEquipmentroomTool::jigShow(LifepumpAreaJigUseIds, LifepumpAreaSideLength);
}

BOOL CDlgLifePumpHouseSet::OnInitDialog()
{
	CAcUiDialog::OnInitDialog();

	DlgHelper::AdjustPosition(this, DlgHelper::TOP_LEFT);

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	m_textProportion.SetWindowText(ms_sPorportionText);
	return TRUE; 
}
