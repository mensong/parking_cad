// DlgVentilateSet.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "DlgVentilateSet.h"
#include "afxdialogex.h"
#include "DBHelper.h"
#include "EquipmentroomTool.h"


// CDlgVentilateSet �Ի���
#define VentilationroomArea 15000000.0//ͨ���豸�����
CWnd* CDlgVentilateSet::parent_dlg = NULL;
CString CDlgVentilateSet::ms_sTotalAreaText;
CString CDlgVentilateSet::ms_sPorportionText;
IMPLEMENT_DYNAMIC(CDlgVentilateSet, CAcUiDialog)

CDlgVentilateSet::CDlgVentilateSet(CWnd* pParent /*=NULL*/)
	: CAcUiDialog(CDlgVentilateSet::IDD, pParent)
{

}

CDlgVentilateSet::~CDlgVentilateSet()
{
}

static void __smartLog(bool*& data, bool isDataValid)
{
	if (isDataValid)
	{
		bool end = (*data);
		if (!end)
			CParkingLog::AddLogA("DEBUG_��֧�ֳ���ִ�еĲ���", 0, "CDlgVentilateSet::DoDataExchange");
		delete data;
	}
}

void CDlgVentilateSet::DoDataExchange(CDataExchange* pDX)
{
	Smart<bool*> end(new bool(false), __smartLog);
	CAcUiDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, m_totalAreaText);
	DDX_Control(pDX, IDC_EDIT2, m_porportionText);
	*(end()) = true;
}

//��ģ̬���ڱ��ֽ���(2/3)
LRESULT CDlgVentilateSet::OnKeepFocus(WPARAM, LPARAM)
{
	return TRUE;
}

BEGIN_MESSAGE_MAP(CDlgVentilateSet, CAcUiDialog)
	ON_MESSAGE(WM_ACAD_KEEPFOCUS, &CDlgVentilateSet::OnKeepFocus)//��ģ̬���ڱ��ֽ���(3/3)
	ON_WM_SIZE()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDOK, &CDlgVentilateSet::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CDlgVentilateSet::OnBnClickedCancel)
END_MESSAGE_MAP()



// CDlgVentilateSet ��Ϣ�������

void CDlgVentilateSet::OnBnClickedOk()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	
	CString sTotalArea;
	m_totalAreaText.GetWindowText(sTotalArea);
	CString sPorportion;
	m_porportionText.GetWindowText(sPorportion);
	double dTotalArea = _tstof(sTotalArea.GetString());
	double dPorportion = _tstof(sPorportion.GetString());
	if (sTotalArea.IsEmpty() || sPorportion.IsEmpty())
	{
		acedAlert(_T("\n�ؿ����������¥��ռ��Ϊ�գ�"));
		return;
	}
	int roomCount = dTotalArea / 4000;
	if (roomCount == 0)
	{
		acedAlert(_T("\n���������С��4000��"));
		return;
	}
	CAcUiDialog::OnOK();
	Doc_Locker doc_locker;
	//HideDialogHolder holder(this);
	acedGetAcadDwgView()->SetFocus();
	ventilateOpera(dTotalArea,dPorportion,roomCount);
	//DBHelper::CallCADCommand(_T("VentilationEquipmentroomSet "));
	parent_dlg->ShowWindow(SW_SHOW);
	ms_sTotalAreaText = sTotalArea;
	ms_sPorportionText = sPorportion;
}


BOOL CDlgVentilateSet::OnInitDialog()
{
	CAcUiDialog::OnInitDialog();

	m_totalAreaText.SetWindowText(ms_sTotalAreaText);
	m_porportionText.SetWindowText(ms_sPorportionText);
	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	CenterWindow();

	return TRUE;  // return TRUE unless you set the focus to a control
				  // �쳣: OCX ����ҳӦ���� FALSE
}

void CDlgVentilateSet::ventilateOpera(double& dTotalArea, double& dPorportion,int& roomCount)
{
	//ͨ���豸��
	if (dPorportion < 1 || dPorportion>100)
	{
		dPorportion = 100;
	}
	double scaleArea = VentilationroomArea*((dPorportion) / 100);
	for (int i = 0; i < roomCount; i++)
	{
		CString count;
		count.Format(_T("%d"), i + 1);
		CString area1Name = _T("����") + count;
		CString area2Name = _T("����") + count;

		CEquipmentroomTool::layerSet();
		double windareaSideLength = 0;
		double limitLength = 3500;
		AcDbObjectIdArray windareaJigUseIds = CEquipmentroomTool::createArea(scaleArea, area1Name, windareaSideLength, limitLength);
		CEquipmentroomTool::setEntToLayer(windareaJigUseIds);
		if (!CEquipmentroomTool::jigShow(windareaJigUseIds, windareaSideLength))
		{
			break;
		}
		AcDbObjectIdArray windarea1JigUseIds = CEquipmentroomTool::createArea(scaleArea, area2Name, windareaSideLength, limitLength);
		CEquipmentroomTool::setEntToLayer(windarea1JigUseIds);
		if (!CEquipmentroomTool::jigShow(windarea1JigUseIds, windareaSideLength))
		{
			break;
		}
		AcDbObjectIdArray windarea2JigUseIds = CEquipmentroomTool::createArea(scaleArea, area2Name, windareaSideLength, limitLength);
		CEquipmentroomTool::setEntToLayer(windarea2JigUseIds);
		if (!CEquipmentroomTool::jigShow(windarea2JigUseIds, windareaSideLength))
		{
			break;
		}
	}
	acutPrintf(_T("\n"));
}


void CDlgVentilateSet::OnBnClickedCancel()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CAcUiDialog::OnOK();
	parent_dlg->ShowWindow(SW_SHOW);
}
