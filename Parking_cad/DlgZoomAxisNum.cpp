// DlgZoomAxisNum.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "DlgZoomAxisNum.h"
#include "afxdialogex.h"
#include "DBHelper.h"
#include "EquipmentroomTool.h"
#include "OperaZoomAxisNumber.h"


// CDlgZoomAxisNum �Ի���
CString CDlgZoomAxisNum::ms_sStepValue = "1";
IMPLEMENT_DYNAMIC(CDlgZoomAxisNum, CAcUiDialog)

void __stdcall CDlgZoomAxisNum::delayDownClick(WPARAM wp, LPARAM lp, void* anyVal)
{
	CDlgZoomAxisNum* pThis = (CDlgZoomAxisNum*)anyVal;
	CString sStepValue;
	pThis->m_edit_StepValue.GetWindowText(sStepValue);
	double dStepValue = _tstof(sStepValue.GetString());
	Doc_Locker doc_locker;
	double dScaleValue = pThis->getCurrentScale();
	double updataValue = dScaleValue - dStepValue;
	if (!COperaZoomAxisNumber::axisNumZoomOpera(updataValue))
	{
		acedAlert(_T("\n�����С����ʧ�ܣ�"));
	}
	double dNewScaleValue = pThis->getCurrentScale();
	CString sNewScaleValue;
	sNewScaleValue.Format(_T("%.1f"), dNewScaleValue);
	pThis->m_show_ScaleValue.SetWindowText(sNewScaleValue);
	pThis->ms_sStepValue = sStepValue;
	acedGetAcadDwgView()->SetFocus();
	DBHelper::CallCADCommand(_T("REGEN "));
}
void __stdcall CDlgZoomAxisNum::delayUpClick(WPARAM wp, LPARAM lp, void* anyVal)
{
	CDlgZoomAxisNum* pThis = (CDlgZoomAxisNum*)anyVal;
	CString sStepValue;
	pThis->m_edit_StepValue.GetWindowText(sStepValue);
	double dStepValue = _tstof(sStepValue.GetString());
	Doc_Locker doc_locker;
	double dScaleValue = pThis->getCurrentScale();
	double updataValue = dScaleValue + dStepValue;
	if (!COperaZoomAxisNumber::axisNumZoomOpera(updataValue))
	{
		acedAlert(_T("\n��ŷŴ����ʧ�ܣ�"));
	}
	double dNewScaleValue = pThis->getCurrentScale();
	CString sNewScaleValue;
	sNewScaleValue.Format(_T("%.1f"), dNewScaleValue);
	pThis->m_show_ScaleValue.SetWindowText(sNewScaleValue);
	pThis->ms_sStepValue = sStepValue;
	acedGetAcadDwgView()->SetFocus();
	DBHelper::CallCADCommand(_T("REGEN "));
}


double CDlgZoomAxisNum::getCurrentScale()
{
	Acad::ErrorStatus es;
	double returnValue=0;
	CString sAadAxleNumLayerName(CEquipmentroomTool::getLayerName("axis_dimensions").c_str());
	AcDbObjectIdArray ids = DBHelper::GetEntitiesByLayerName(sAadAxleNumLayerName);
	AcDbEntity *pEnt = NULL;
	for (int i = 0; i < ids.length(); i++)
	{
		if (acdbOpenObject(pEnt, ids[i], AcDb::kForRead) != eOk)
			continue;
		if (pEnt->isKindOf(AcDbBlockReference::desc()))
		{
			AcDbBlockReference *pBlkRef = AcDbBlockReference::cast(pEnt);
			AcDbObjectId pBlkTblRecId;
			pBlkTblRecId = pBlkRef->blockTableRecord();
			AcDbBlockTableRecord *pBlkTblRec = NULL;
			es = acdbOpenObject(pBlkTblRec, pBlkTblRecId, AcDb::kForRead);
			if (es!=eOk)
			{
				pBlkRef->close();
				continue;
			}
			AcString blockName;
			pBlkTblRec->getName(blockName);
			pBlkTblRec->close();
			if (blockName == _T("_AxleNumber"))
			{
				AcGeScale3d scale3d;
				scale3d = pBlkRef->scaleFactors();
				returnValue = scale3d.sx;
				pEnt->close();
				break;
			}
		}
		if (pEnt)
			pEnt->close();
	}
	return returnValue;
}

CDlgZoomAxisNum::CDlgZoomAxisNum(CWnd* pParent /*=NULL*/)
	: CAcUiDialog(CDlgZoomAxisNum::IDD, pParent)
{

}

CDlgZoomAxisNum::~CDlgZoomAxisNum()
{
}

static void __smartLog(bool*& data, bool isDataValid)
{
	if (isDataValid)
	{
		bool end = (*data);
		if (!end)
			CParkingLog::AddLogA("DEBUG_��֧�ֳ���ִ�еĲ���", 0, "CDlgZoomAxisNum::DoDataExchange");
		delete data;
	}
}

void CDlgZoomAxisNum::DoDataExchange(CDataExchange* pDX)
{
	Smart<bool*> end(new bool(false), __smartLog);
	CAcUiDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BUTTON_AXISNUM_DOWN, m_btn_AxisNumDown);
	DDX_Control(pDX, IDC_BUTTON_AXISNUM_UP, m_btn_AxisNumUp);
	DDX_Control(pDX, IDC_SHOW_SCALEVALUE, m_show_ScaleValue);
	DDX_Control(pDX, IDC_EDIT_STEPVALUE, m_edit_StepValue);
	*(end()) = true;
}

//��ģ̬���ڱ��ֽ���(2/3)
LRESULT CDlgZoomAxisNum::OnKeepFocus(WPARAM, LPARAM)
{
	return TRUE;
}

BEGIN_MESSAGE_MAP(CDlgZoomAxisNum, CAcUiDialog)
	ON_MESSAGE(WM_ACAD_KEEPFOCUS, &CDlgZoomAxisNum::OnKeepFocus)//��ģ̬���ڱ��ֽ���(3/3)
	ON_WM_SIZE()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON_AXISNUM_UP, &CDlgZoomAxisNum::OnBnClickedButtonAxisnumUp)
	ON_BN_CLICKED(IDC_BUTTON_AXISNUM_DOWN, &CDlgZoomAxisNum::OnBnClickedButtonAxisnumDown)
	ON_EN_KILLFOCUS(IDC_SHOW_SCALEVALUE, &CDlgZoomAxisNum::OnEnKillfocusShowScalevalue)
END_MESSAGE_MAP()


// CDlgZoomAxisNum ��Ϣ�������


void CDlgZoomAxisNum::OnBnClickedButtonAxisnumUp()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	SetDelayExecute(delayUpClick, 0, 0, this, 1000, true);
}


void CDlgZoomAxisNum::OnBnClickedButtonAxisnumDown()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	SetDelayExecute(delayDownClick, 0, 0, this, 1000, true);
}


BOOL CDlgZoomAxisNum::OnInitDialog()
{
	CAcUiDialog::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	HICON m_IconBtn_Down = AfxGetApp()->LoadIcon(IDI_ICON_DOWN);//����Icon��Դ������m_IconBtn_Down���洢�����
	SetIcon(m_IconBtn_Down,FALSE);
	m_btn_AxisNumDown.SetIcon(m_IconBtn_Down);
	HICON m_IconBtn_Up = AfxGetApp()->LoadIcon(IDI_ICON_UP);//����Icon��Դ������m_IconBtn_Up���洢�����
	SetIcon(m_IconBtn_Up, FALSE);
	m_btn_AxisNumUp.SetIcon(m_IconBtn_Up);
	HICON m_Icon_Zoom = AfxGetApp()->LoadIcon(IDI_ICON_ZOOM);//����Icon��Դ������m_Icon_Zoom���洢�����
	SetIcon(m_Icon_Zoom, FALSE);
	m_edit_StepValue.SetWindowText(ms_sStepValue);
	Doc_Locker doc_locker;
	//acedGetAcadDwgView()->SetFocus();
	double dScaleValue = getCurrentScale();
	CString sScaleValue;
	sScaleValue.Format(_T("%.1f"), dScaleValue);
	m_show_ScaleValue.SetWindowText(sScaleValue);
	return TRUE;  // return TRUE unless you set the focus to a control
				  // �쳣: OCX ����ҳӦ���� FALSE
}


void CDlgZoomAxisNum::OnEnKillfocusShowScalevalue()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CString s_ScaleValue;
	m_show_ScaleValue.GetWindowText(s_ScaleValue);
	double dStepValue = _tstof(s_ScaleValue.GetString());
	double dOldScaleValue = getCurrentScale();
	Doc_Locker doc_locker;
	if (dStepValue==dOldScaleValue)
	{
		return;
	}
	if (!COperaZoomAxisNumber::axisNumZoomOpera(dStepValue))
	{
		acedAlert(_T("\n�����С����ʧ�ܣ�"));
	}
	acedGetAcadDwgView()->SetFocus();
	DBHelper::CallCADCommand(_T("REGEN "));
}
