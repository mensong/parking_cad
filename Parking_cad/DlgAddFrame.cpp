// DlgAddFrame.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "DlgAddFrame.h"
#include "afxdialogex.h"
#include "DBHelper.h"
#include "CommonFuntion.h"
#include "EquipmentroomTool.h"

//ͼǩ����֮�Ŵ�ϵ��
#define Magnification 2
// CDlgAddFrame �Ի���

IMPLEMENT_DYNAMIC(CDlgAddFrame, CAcUiDialog)

CDlgAddFrame::CDlgAddFrame(CWnd* pParent /*=NULL*/)
	: CAcUiDialog(CDlgAddFrame::IDD, pParent)
	, m_sFrameWidth(_T(""))
	, m_sFrameLen(_T(""))
{
	
}

CDlgAddFrame::~CDlgAddFrame()
{
}

void CDlgAddFrame::setContextExtents(const AcDbExtents& ext)
{
	m_extents = ext;
}

void CDlgAddFrame::refreshPreview()
{
	CString sLen;
	m_editFrameLen.GetWindowText(sLen);
	int nLen = _ttoi(sLen.GetString());
	CString sWidth;
	m_editFrameWidth.GetWindowText(sWidth);
	int nWidth = _ttoi(sWidth.GetString());

	if (nLen < 10 || nWidth < 10)
	{
		m_staPreview.Clear();
		return;
	}

	double dRatio = (double)nLen / (double)nWidth;

	double dExtLen = m_extents.maxPoint().x - m_extents.minPoint().x;
	double dExtWidth = m_extents.maxPoint().y - m_extents.minPoint().y;
	
	//dExtLen / dExtWidth == dRatio;
	double dTestLen = dRatio * dExtWidth;
	double dTestWidth = dExtWidth;
	int tag = 0;
	if (dTestLen < dExtLen)
	{//��������
		dTestLen = dExtLen;
		dTestWidth = dExtLen / dRatio;
		tag = 1;
	}

	AcGeVector3d vecMid;
	vecMid.x = 0.5 *(m_extents.minPoint().x + m_extents.maxPoint().x);
	vecMid.y = 0.5 *(m_extents.minPoint().y + m_extents.maxPoint().y);
	vecMid.z = 0.0;

	mExtsCentepoint = AcGePoint3d((m_extents.maxPoint().x + m_extents.minPoint().x) / 2, (m_extents.maxPoint().y + m_extents.minPoint().y) / 2, 0);

	AcDbDatabase* pDb = new AcDbDatabase(true, true);

	AcDbPolyline* pContext = new AcDbPolyline;

	pContext->addVertexAt(0, AcGePoint2d(-dExtLen / 2, -dExtWidth / 2));
	pContext->addVertexAt(1, AcGePoint2d(dExtLen / 2, -dExtWidth / 2));
	pContext->addVertexAt(2, AcGePoint2d(dExtLen / 2, dExtWidth / 2));
	pContext->addVertexAt(3, AcGePoint2d(-dExtLen / 2, dExtWidth / 2));
	pContext->setClosed(Adesk::kTrue);
	pContext->setColorIndex(1);
	pContext->transformBy(vecMid);
	DBHelper::AppendToDatabase(pContext, pDb);
	pContext->close();

	//TODO: �����Ĵ�С��Ҫ����
    //zhangzechi
	AcGePoint2d pFramept0;
	AcGePoint2d pFramept1;
	AcGePoint2d pFramept2;
	AcGePoint2d pFramept3;

	AcGePoint2d pPicpt0;
	AcGePoint2d pPicpt1;
	AcGePoint2d pPicpt2;
	AcGePoint2d pPicpt3;

	if (!InpromDRenceFromDWG(dExtLen))
		acedAlert(_T("�����ⲿͼǩʧ��!"));

	if (tag == 0)
	{
		if (IsDistanceAppoint(AcGePoint3d(-dTestLen / 2, -dTestWidth / 2, 0), AcGePoint3d(dTestLen / 2, -dTestWidth / 2, 0), AcGePoint3d(dExtLen / 2, -dExtWidth / 2, 0)))
		{
			pFramept0 = AcGePoint2d(-dTestLen / 2, -dTestWidth / 2);
			pFramept1 = AcGePoint2d(dTestLen / 2, -dTestWidth / 2);
			pFramept2 = AcGePoint2d(dTestLen / 2, dTestWidth / 2);
			pFramept3 = AcGePoint2d(-dTestLen / 2, dTestWidth / 2);

			SetPoints(pPicpt0, pPicpt1, pPicpt2, pPicpt3, pFramept0, pFramept1, pFramept2, pFramept3);
		}
		else
		{
			double dis = CCommonFuntion::PointToSegDist(AcGePoint3d(-dTestLen / 2, -dTestWidth / 2, 0), AcGePoint3d(dTestLen / 2, -dTestWidth / 2, 0), AcGePoint3d(dExtLen / 2, -dExtWidth / 2, 0));
			double dDifferdis = 2*(mBlockextentsWidth - dis) + 10000;
			dTestWidth = dTestWidth + dDifferdis;
			dTestLen = dRatio * dTestWidth;

			pFramept0 = AcGePoint2d(-dTestLen / 2, -dTestWidth / 2);
			pFramept1 = AcGePoint2d(dTestLen / 2, -dTestWidth / 2);
			pFramept2 = AcGePoint2d(dTestLen / 2, dTestWidth / 2);
			pFramept3 = AcGePoint2d(-dTestLen / 2, dTestWidth / 2);

			SetPoints(pPicpt0, pPicpt1, pPicpt2, pPicpt3, pFramept0, pFramept1, pFramept2, pFramept3);

		}
	}
	else if (tag == 1)
	{
		if (IsDistanceAppoint(AcGePoint3d(-dTestLen / 2, -dTestWidth / 2, 0), AcGePoint3d(dTestLen / 2, -dTestWidth / 2, 0), AcGePoint3d(dExtLen / 2, -dExtWidth / 2, 0)))
		{
			dTestLen = dTestLen + 10000;
			dTestWidth = dExtLen / dRatio;

			pFramept0 = AcGePoint2d(-dTestLen / 2, -dTestWidth / 2);
			pFramept1 = AcGePoint2d(dTestLen / 2, -dTestWidth / 2);
			pFramept2 = AcGePoint2d(dTestLen / 2, dTestWidth / 2);
			pFramept3 = AcGePoint2d(-dTestLen / 2, dTestWidth / 2);

			SetPoints(pPicpt0, pPicpt1, pPicpt2, pPicpt3, pFramept0, pFramept1, pFramept2, pFramept3);
		}
		else
		{
			double dis = CCommonFuntion::PointToSegDist(AcGePoint3d(-dTestLen / 2, -dTestWidth / 2, 0), AcGePoint3d(dTestLen / 2, -dTestWidth / 2, 0), AcGePoint3d(dExtLen / 2, -dExtWidth / 2, 0));
			double dDifferdis = 2 * (mBlockextentsWidth - dis) + 10000;

			dTestWidth = dTestWidth + dDifferdis;
			dTestLen = dRatio * dTestWidth;

			pFramept0 = AcGePoint2d(-dTestLen / 2, -dTestWidth / 2);
			pFramept1 = AcGePoint2d(dTestLen / 2, -dTestWidth / 2);
			pFramept2 = AcGePoint2d(dTestLen / 2, dTestWidth / 2);
			pFramept3 = AcGePoint2d(-dTestLen / 2, dTestWidth / 2);

			SetPoints(pPicpt0, pPicpt1, pPicpt2, pPicpt3, pFramept0, pFramept1, pFramept2, pFramept3);

		}
	}

	AcDbPolyline* pFrame = new AcDbPolyline;
	pFrame->addVertexAt(0, pFramept0);
	pFrame->addVertexAt(1, pFramept1);
	pFrame->addVertexAt(2, pFramept2);
	pFrame->addVertexAt(3, pFramept3);
	pFrame->setClosed(Adesk::kTrue);
	pFrame->transformBy(vecMid);
	DBHelper::AppendToDatabase(pFrame, pDb);
	pFrame->close();

	AcDbPolyline* pPicFrame = new AcDbPolyline;
	pPicFrame->addVertexAt(0, pPicpt0);
	pPicFrame->addVertexAt(1, pPicpt1);
	pPicFrame->addVertexAt(2, pPicpt2);
	pPicFrame->addVertexAt(3, pPicpt3);
	pPicFrame->setClosed(Adesk::kTrue);
	pPicFrame->transformBy(vecMid);
	DBHelper::AppendToDatabase(pPicFrame, pDb);
	pPicFrame->close();

	m_staPreview.Init(pDb);

	m_staPreview.SetMouseEventEnable(
		CCadPreviewCtrl::meMBMove | 
		CCadPreviewCtrl::meRBMove |
		CCadPreviewCtrl::meLBDBClick | 
		CCadPreviewCtrl::meMBDBClick |
		CCadPreviewCtrl::meRBDBClick |
		CCadPreviewCtrl::meMBWheel);
}

void CDlgAddFrame::DoDataExchange(CDataExchange* pDX)
{
	CAcUiDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_FRAME_LENGTH, m_editFrameLen);
	DDX_Control(pDX, IDC_EDIT_FRAME_WIDTH, m_editFrameWidth);
	DDX_Control(pDX, IDC_STA_PREVIEW, m_staPreview);
	DDX_Text(pDX, IDC_EDIT_FRAME_WIDTH, m_sFrameWidth);
	DDX_Text(pDX, IDC_EDIT_FRAME_LENGTH, m_sFrameLen);
	DDX_Control(pDX, IDC_BUTTON_EXCHANGE, m_btnExchange);
}


BOOL CDlgAddFrame::OnInitDialog()
{
	if (!CAcUiDialog::OnInitDialog())
		return FALSE;

	CenterWindow(GetDesktopWindow());//����������Ļ�м�

	((CButton*)GetDlgItem(IDC_RAD_A0))->SetCheck(TRUE);
	OnBnClickedRadA0();

	setExchangeButtonImg();

	return TRUE;
}

void CDlgAddFrame::setExchangeButtonImg()
{
	AcString ImgPath = DBHelper::GetArxDir() + _T("exchange.ico");

	HICON hBitmap = (HICON)::LoadImage(
		NULL,
		ImgPath,                       // ͼƬȫ·�� 
		IMAGE_ICON,                   // ͼƬ��ʽ 
		0, 0,
		LR_LOADFROMFILE | LR_CREATEDIBSECTION);  // ע��LR_LOADFROMFILE

	m_btnExchange.SetIcon(hBitmap);

}

BOOL CDlgAddFrame::PreTranslateMessage(MSG* pMsg)
{
	if ( pMsg->message == WM_KEYDOWN &&	pMsg->wParam == VK_RETURN)
	{
		if (pMsg->hwnd == m_editFrameLen.m_hWnd || 
			pMsg->hwnd == m_editFrameWidth.m_hWnd)
		{
			refreshPreview();
			return TRUE;
		}
	}

	return CAcUiDialog::PreTranslateMessage(pMsg);
}

BEGIN_MESSAGE_MAP(CDlgAddFrame, CAcUiDialog)
	ON_BN_CLICKED(IDC_RAD_A0, &CDlgAddFrame::OnBnClickedRadA0)
	ON_BN_CLICKED(IDC_RAD_A1, &CDlgAddFrame::OnBnClickedRadA1)
	ON_BN_CLICKED(IDC_RAD_A2, &CDlgAddFrame::OnBnClickedRadA2)
	ON_BN_CLICKED(IDC_RAD_A3, &CDlgAddFrame::OnBnClickedRadA3)
	ON_BN_CLICKED(IDC_RAD_A4, &CDlgAddFrame::OnBnClickedRadA4)
	ON_BN_CLICKED(IDOK, &CDlgAddFrame::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON_EXCHANGE, &CDlgAddFrame::OnBnClickedButtonExchange)
END_MESSAGE_MAP()


// CDlgAddFrame ��Ϣ�������


void CDlgAddFrame::OnBnClickedRadA0()
{
	m_editFrameLen.SetWindowText(_T("1189"));
	m_editFrameWidth.SetWindowText(_T("841"));
	refreshPreview();
}


void CDlgAddFrame::OnBnClickedRadA1()
{
	m_editFrameLen.SetWindowText(_T("841"));
	m_editFrameWidth.SetWindowText(_T("594"));
	refreshPreview();
}


void CDlgAddFrame::OnBnClickedRadA2()
{
	m_editFrameLen.SetWindowText(_T("594"));
	m_editFrameWidth.SetWindowText(_T("420"));
	refreshPreview();
}


void CDlgAddFrame::OnBnClickedRadA3()
{
	m_editFrameLen.SetWindowText(_T("420"));
	m_editFrameWidth.SetWindowText(_T("297"));
	refreshPreview();
}


void CDlgAddFrame::OnBnClickedRadA4()
{
	m_editFrameLen.SetWindowText(_T("297"));
	m_editFrameWidth.SetWindowText(_T("210"));
	refreshPreview();
}


void CDlgAddFrame::OnBnClickedOk()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	this->OnOK();
}

void CDlgAddFrame::setBigFramePoints()
{
	AcGeVector2d vec = AcGeVector2d(AcGePoint2d(mExtsCentepoint.x, mExtsCentepoint.y) - AcGePoint2d(0, 0));
	mBigFramept0.transformBy(vec);
	mBigFramept1.transformBy(vec);
	mBigFramept2.transformBy(vec);
	mBigFramept3.transformBy(vec);

	mInserPicPoint = AcGePoint3d(mBigFramept1.x, mBigFramept1.y, 0);

}

bool CDlgAddFrame::setBlockInserPoint(std::string& Textstr)
{
	TEXT_STYLE ts(_T("arial.ttf"));
	ts.XScale = 1;
	DBHelper::SetTextStyle(_T("ͼǩ������ʽ"), ts);

	//std::string Textstr = "SP=4865|SPT=3740|SPF=1125|SPF1=210|SPF2=450|SPF3=463|CP=132|JSPC=25|SPC=33|H=3.55|HT=1";//����
	std::vector<std::string> outstr;
	CCommonFuntion::Split(Textstr, "|", outstr);

	std::map<AcString, AcString> mAttrMap;
	for (int i = 0; i < outstr.size(); ++i)
	{
		std::vector<std::string> tempvt;
		CCommonFuntion::Split(outstr[i], "=", tempvt);
		if (tempvt.size() != 2)
			continue;

		AcString bloacktag = CCommonFuntion::ChartoACHAR(tempvt[0].c_str());		
		AcString tagvalue = CCommonFuntion::ChartoACHAR(tempvt[1].c_str());

		mAttrMap[bloacktag] = tagvalue;

	}

	AcGeMatrix3d mat;
	mat.setToScaling(mMultiple, mInserPicPoint);
	AcDbObjectId mInserblockId;
	if (DBHelper::InsertBlkRefWithAttribute(mInserblockId, _T("����ָ����"), mInserPicPoint, mAttrMap, &mat))
	{
		CEquipmentroomTool::layerSet(_T("0"), 7);
		CString sMapSignLayer(CEquipmentroomTool::getLayerName("mapsign").c_str());
		CEquipmentroomTool::creatLayerByjson("mapsign");
		CEquipmentroomTool::setEntToLayer(mInserblockId, sMapSignLayer);
		CEquipmentroomTool::deletLayerByName(_T("ͼǩ����"));
		CEquipmentroomTool::deletLayerByName(_T("ͼǩ��"));
		CEquipmentroomTool::layerSet(_T("0"), 7);
		return true;
	}
	else
	{
		acutPrintf(_T("����ָ�������ʧ��"));
		return false;
	}

}

void CDlgAddFrame::SetPoints(AcGePoint2d& pt1, AcGePoint2d& pt2, AcGePoint2d& pt3, AcGePoint2d& pt4, AcGePoint2d& pt5, AcGePoint2d& pt6, AcGePoint2d& pt7, AcGePoint2d& pt8)
{
	mBigFramept0 = pt5;
	mBigFramept1 = pt6;
	mBigFramept2 = pt7;
	mBigFramept3 = pt8;

	pt1 = AcGePoint2d(pt6.x - mBlockextentsLen, pt6.y);
	pt2 = pt6;
	pt3 = AcGePoint2d(pt6.x, pt6.y + mBlockextentsWidth);
	pt4 = AcGePoint2d(pt6.x - mBlockextentsLen, pt6.y + mBlockextentsWidth);
}

void CDlgAddFrame::setBlokcLayer(const AcString& setlayername, AcDbObjectId& entityId)
{
	// ��õ�ǰͼ�εĲ��
	AcDbLayerTable *pLayerTbl = NULL;
	//�жϲ���
	acdbHostApplicationServices()->workingDatabase()->getLayerTable(pLayerTbl, AcDb::kForWrite);

	// �Ƿ��Ѿ�����ָ���Ĳ���¼
	if (!pLayerTbl->has(setlayername))
	{
		// �����µĲ���¼
		AcDbLayerTableRecord *pLayerTblRcd = new AcDbLayerTableRecord();
		pLayerTblRcd->setName(setlayername);

		// ������ɫ,����������ԣ����͵ȣ�����ȱʡֵ
		AcCmColor color;
		color.setColorIndex(255);
		pLayerTblRcd->setColor(color);

		// ���½��Ĳ���¼��ӵ������
		AcDbObjectId layerTblRcdId;
		pLayerTbl->add(layerTblRcdId, pLayerTblRcd);
		acdbHostApplicationServices()->workingDatabase()->setClayer(layerTblRcdId);
		pLayerTblRcd->close();
		pLayerTbl->close();
	}

	if (pLayerTbl)
		pLayerTbl->close();

	AcDbEntity *pEnt = NULL;
	Acad::ErrorStatus es = acdbOpenObject(pEnt, entityId, AcDb::kForWrite);
	if (es != eOk)
		return;
	pEnt->setLayer(setlayername);
	pEnt->close();
}

bool CDlgAddFrame::InpromDRenceFromDWG(const double& inputLen)
{
	mBlockextentsLen = 0;
	mBlockextentsWidth = 0;

	std::set<AcString> BlockNames;
	AcString name = _T("����ָ����");
	BlockNames.insert(name);

	ObjectCollector oc;
	oc.start(acdbCurDwg());
	AcString filepath = DBHelper::GetArxDir() + _T("Mapsign.dwg");
	DBHelper::ImportBlkDef(filepath, BlockNames);
	if (oc.m_objsAppended.length() > 0)
	{
		CString sMapSignLayer(CEquipmentroomTool::getLayerName("mapsign").c_str());
		//CEquipmentroomTool::layerSet(sMapSignLayer, 7);
		CEquipmentroomTool::creatLayerByjson("mapsign");
		for (int i=0; i<oc.m_objsAppended.length();i++)
		{
			CEquipmentroomTool::setEntToLayer(oc.m_objsAppended[i], sMapSignLayer);
		}
	}

	AcDbBlockTable *pBlockTable = NULL;
	acdbHostApplicationServices()->workingDatabase()->getBlockTable(pBlockTable, AcDb::kForRead);
	AcDbObjectId EntId;
	pBlockTable->getAt(name, EntId);
	pBlockTable->close();

	AcDbBlockTableRecord *pDBlkRe = NULL;
	Acad::ErrorStatus es;
	es = acdbOpenObject(pDBlkRe, EntId, AcDb::kForRead);
	if (es != eOk)
		return false;

	AcDbBlockTableRecordIterator *pIter;
	es = pDBlkRe->newIterator(pIter);
	if (es != eOk)
	{
		if (pDBlkRe)
			pDBlkRe->close();
		delete pIter;
		return false;
	}
		

	AcDbExtents extents;
	for (; !pIter->done(); pIter->step())
	{
		AcDbEntity *pEnt;
		AcDbBlockReference *pBlkRef;
		if (pIter->getEntity(pEnt, kForRead) == eOk)
		{
			AcDbExtents tempextens;
			es = DBHelper::GetEntityExtents(tempextens, pEnt);
			extents.addExt(tempextens);
			if (pEnt)
				pEnt->close();
		}
	}
	delete pIter;
	if (pDBlkRe)
		pDBlkRe->close();

	double improblockLen = abs(extents.maxPoint().x - extents.minPoint().x);

	//ͼǩ���ű���
	mMultiple = Magnification*(inputLen * 48) / (improblockLen * 315);

	mBlockextentsLen = abs(extents.maxPoint().x - extents.minPoint().x)* mMultiple;
	mBlockextentsWidth = abs(extents.maxPoint().y - extents.minPoint().y) * mMultiple;

	return true;
}

bool CDlgAddFrame::IsDistanceAppoint(AcGePoint3d& pt1, AcGePoint3d& pt2, AcGePoint3d& pt3)
{

	double dis = CCommonFuntion::PointToSegDist(pt1, pt2, pt3);
	if (dis <= mBlockextentsWidth)
		return false;
	else
		return true;

}



void CDlgAddFrame::OnBnClickedButtonExchange()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	UpdateData(TRUE);

	CString csExchang;

	csExchang = m_sFrameLen;
	m_sFrameLen = m_sFrameWidth;
	m_sFrameWidth = csExchang;

	UpdateData(FALSE);

	refreshPreview();
}
