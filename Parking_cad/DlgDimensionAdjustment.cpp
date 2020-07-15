// DlgDimensionAdjustment.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "DlgDimensionAdjustment.h"
#include "afxdialogex.h"
#include "OperaDimensionAdjustment.h"

CString CDlgDimensionAdjustment::ms_sStepValue = _T("0.5");


// CDlgDimensionAdjustment �Ի���

IMPLEMENT_DYNAMIC(CDlgDimensionAdjustment, CAcUiDialog)

CDlgDimensionAdjustment::CDlgDimensionAdjustment(CWnd* pParent /*=NULL*/)
	: CAcUiDialog(IDD_DIALOG_DIMADJUSTMENT, pParent)
{

}

CDlgDimensionAdjustment::~CDlgDimensionAdjustment()
{
}

BOOL CDlgDimensionAdjustment::OnInitDialog()
{
	CAcUiDialog::OnInitDialog();
	DlgHelper::AdjustPosition(this, DlgHelper::TOP_LEFT);

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	HICON m_IconBtn_Down = AfxGetApp()->LoadIcon(IDI_ICON_DOWN);//����Icon��Դ������m_IconBtn_Down���洢�����
	SetIcon(m_IconBtn_Down, FALSE);
	m_btn_NumDown.SetIcon(m_IconBtn_Down);
	HICON m_IconBtn_Up = AfxGetApp()->LoadIcon(IDI_ICON_UP);//����Icon��Դ������m_IconBtn_Up���洢�����
	SetIcon(m_IconBtn_Up, FALSE);
	m_btn_NumUp.SetIcon(m_IconBtn_Up);
	HICON m_Icon_Zoom = AfxGetApp()->LoadIcon(IDI_ICON_ZOOM);//����Icon��Դ������m_Icon_Zoom���洢�����
	SetIcon(m_Icon_Zoom, FALSE);
	m_edit_StepValue.SetWindowText(ms_sStepValue);

	return TRUE;
}

BOOL CDlgDimensionAdjustment::DestroyWindow()
{
	__super::DestroyWindow();

	COperaDimensionAdjustment::m_dimadjustDlg= NULL;

	return TRUE;
}

void CDlgDimensionAdjustment::OnCancel()
{
	CAcUiDialog::OnClose();
	DestroyWindow();//���ٶԻ���

	delete this;
}

void CDlgDimensionAdjustment::DoDataExchange(CDataExchange* pDX)
{
	CAcUiDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BUTTON_NUM_DOWN, m_btn_NumDown);
	DDX_Control(pDX, IDC_BUTTON_NUM_UP, m_btn_NumUp);
	DDX_Control(pDX, IDC_SHOW_SCALEVALUE, m_show_ScaleValue);
	DDX_Control(pDX, IDC_EDIT_STEPVALUE, m_edit_StepValue);
}


LRESULT CDlgDimensionAdjustment::OnKeepFocus(WPARAM, LPARAM)
{
	return TRUE;
}

BEGIN_MESSAGE_MAP(CDlgDimensionAdjustment, CAcUiDialog)
	ON_MESSAGE(WM_ACAD_KEEPFOCUS, &CDlgDimensionAdjustment::OnKeepFocus)
	ON_BN_CLICKED(IDC_BUTTON_NUM_UP, &CDlgDimensionAdjustment::OnBnClickedButtonNumUp)
	ON_BN_CLICKED(IDC_BUTTON_NUM_DOWN, &CDlgDimensionAdjustment::OnBnClickedButtonNumDown)
END_MESSAGE_MAP()



double CDlgDimensionAdjustment::getCurrentScale()
{
	// ����Ѿ����ڵı�ע��ʽISO-25
     Acad::ErrorStatus es;
	AcDbDimStyleTableRecord *pOldStyle = NULL;
	AcDbDimStyleTable *pDimStyleTbl;
	acdbCurDwg()->getDimStyleTable(pDimStyleTbl, AcDb::kForRead);
	es = pDimStyleTbl->getAt(_T("���������ߴ��ע��ʽ"), pOldStyle, AcDb::kForRead);
	if (es != eOk)
	{	
	    acutPrintf(_T("��ȡ���������ߴ��ע��ʽʧ�ܣ�"));
		if (pDimStyleTbl)
			pDimStyleTbl->close();
		return 0;
	}
	double scalenum = pOldStyle->dimscale();

	if (pOldStyle)
		pOldStyle->close();
	if (pDimStyleTbl)
		pDimStyleTbl->close();

	return scalenum;
}

bool CDlgDimensionAdjustment::setCurrentScale(double scalenum)
{
	Acad::ErrorStatus es;
	AcDbDimStyleTableRecord *pOldStyle = NULL;
	AcDbDimStyleTable *pDimStyleTbl;
	acdbCurDwg()->getDimStyleTable(pDimStyleTbl, AcDb::kForWrite);
	es = pDimStyleTbl->getAt(_T("���������ߴ��ע��ʽ"), pOldStyle, AcDb::kForWrite);
	if (es != eOk)
	{
		acutPrintf(_T("��ȡ���������ߴ��ע��ʽʧ�ܣ�"));
		pDimStyleTbl->close();
		return false;
	}
	if (pOldStyle->setDimscale(scalenum) != eOk)
	{
		pOldStyle->close();
		pDimStyleTbl->close();
		return false;
	}

	if (pOldStyle)
		pOldStyle->close();
	if (pDimStyleTbl)
		pDimStyleTbl->close();

	return true;
}

// CDlgDimensionAdjustment ��Ϣ�������
void __stdcall CDlgDimensionAdjustment::delayDownClick(WPARAM wp, LPARAM lp, void* anyVal)
{
	CDlgDimensionAdjustment* pThis = (CDlgDimensionAdjustment*)anyVal;
	CString sStepValue;
	pThis->m_edit_StepValue.GetWindowText(sStepValue);
	double dStepValue = _tstof(sStepValue.GetString());
	Doc_Locker doc_locker;
	double dScaleValue = pThis->getCurrentScale();
	if (dScaleValue > 0)
	{
		double updataValue = dScaleValue - dStepValue;
		if (!setCurrentScale(updataValue))
		{
			acedAlert(_T("\n�޸ĳ��������ߴ��ע��ʽ����ʧ�ܣ�"));
		}
	}
	double dNewScaleValue = pThis->getCurrentScale();
	CString sNewScaleValue;
	sNewScaleValue.Format(_T("%.1f"), dNewScaleValue);
	pThis->m_show_ScaleValue.SetWindowText(sNewScaleValue);
	pThis->ms_sStepValue = sStepValue;
	acedGetAcadDwgView()->SetFocus();
	DBHelper::CallCADCommand(_T("REGEN "));
}
void __stdcall CDlgDimensionAdjustment::delayUpClick(WPARAM wp, LPARAM lp, void* anyVal)
{
	CDlgDimensionAdjustment* pThis = (CDlgDimensionAdjustment*)anyVal;
	CString sStepValue;
	pThis->m_edit_StepValue.GetWindowText(sStepValue);
	double dStepValue = _tstof(sStepValue.GetString());
	Doc_Locker doc_locker;
	double dScaleValue = pThis->getCurrentScale();
	if (dScaleValue > 0)
	{
		double updataValue = dScaleValue + dStepValue;
		if (!setCurrentScale(updataValue))
		{
			acedAlert(_T("\n�޸ĳ��������ߴ��ע��ʽ����ʧ�ܣ�"));
		}
	}
	double dNewScaleValue = pThis->getCurrentScale();
	CString sNewScaleValue;
	sNewScaleValue.Format(_T("%.1f"), dNewScaleValue);
	pThis->m_show_ScaleValue.SetWindowText(sNewScaleValue);
	pThis->ms_sStepValue = sStepValue;
	acedGetAcadDwgView()->SetFocus();
	DBHelper::CallCADCommand(_T("REGEN "));
}

void CDlgDimensionAdjustment::OnBnClickedButtonNumUp()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	SetDelayExecute(delayUpClick, 0, 0, this, 1000, true);
}


void CDlgDimensionAdjustment::OnBnClickedButtonNumDown()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	SetDelayExecute(delayDownClick, 0, 0, this, 1000, true);
}
