// DlgResetEntityLayer.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "DlgResetEntityLayer.h"
#include "afxdialogex.h"
#include "OperaResetEntityLayer.h"
#include "DBHelper.h"
#include "FileHelper.h"
#include "json\reader.h"

// CDlgResetEntityLayer �Ի���

IMPLEMENT_DYNAMIC(CDlgResetEntityLayer, CAcUiDialog)

CDlgResetEntityLayer::CDlgResetEntityLayer(CWnd* pParent /*=NULL*/)
	: CAcUiDialog(IDD_DIALOG_RESETENTITYLAYER, pParent)
{

}

CDlgResetEntityLayer::~CDlgResetEntityLayer()
{
}

void CDlgResetEntityLayer::DoDataExchange(CDataExchange* pDX)
{
	CAcUiDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_list);
}

BOOL CDlgResetEntityLayer::OnInitDialog()
{
	CAcUiDialog::OnInitDialog();

	CenterWindow();

	CRect rect;
	// ��ȡ��������б���ͼ�ؼ���λ�úʹ�С   
	m_list.GetClientRect(&rect);
	// Ϊ�б���ͼ�ؼ����ȫ��ѡ�к�դ����   
	m_list.SetExtendedStyle(m_list.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_LIST /*| LVS_EX_CHECKBOXES*/);
	m_list.InsertColumn(0, _T(""), LVCFMT_RIGHT, rect.Width() * 0, 0);
	m_list.InsertColumn(1, _T("ԭͼ��"), LVCFMT_CENTER, rect.Width() * 1 / 2, 1);
	m_list.InsertColumn(2, _T("Ŀ��ͼ��"), LVCFMT_CENTER, rect.Width() * 1 / 2, 2);

	std::map<std::string, std::string> keywordmap;
	if (readConfig(keywordmap))
	{
		std::vector<CString> vecMismatchlayer;
		std::map<std::string, std::string> outkeywordmap;
		getAllLayers(keywordmap, outkeywordmap, vecMismatchlayer);

		std::map<std::string, std::string>::iterator iter;
		int i = 0;
		for (iter = outkeywordmap.begin(); iter != outkeywordmap.end(); iter++)
		{
			m_list.InsertItem(i, NULL);
			CString orignlayer = iter->first.c_str();
			m_list.SetItemText(i, 1, orignlayer);
			CString tagetlayer = iter->second.c_str();
			m_list.SetItemText(i, 2, tagetlayer);
			i++;
		}

		for (int k = i; k < i + vecMismatchlayer.size(); k++)
		{
			m_list.InsertItem(k, NULL);
			m_list.SetItemText(k, 1, vecMismatchlayer[k - i]);
			m_list.SetItemText(k, 2, _T("������"));
		}

	}

	haveccomboboxcreate = false;//��ʼ����־λ����ʾ��û�д��������б��

	return TRUE;
}

BOOL CDlgResetEntityLayer::DestroyWindow()
{
	__super::DestroyWindow();

	COperaResetEntityLayer::ms_pResetLayerDlg = NULL;

	return TRUE;
}

void CDlgResetEntityLayer::OnCancel()
{
	CAcUiDialog::OnClose();
	DestroyWindow();//���ٶԻ���

	delete this;
}

bool CDlgResetEntityLayer::readConfig(std::map<std::string, std::string>& keywordmap)
{
	std::string sConfigFile = GetUserDirA() + "ParkingConfig.json";
	std::string sConfigStr = FileHelper::ReadText(sConfigFile.c_str());
	Json::Reader reader;
	Json::Value root;

	try
	{
		if (reader.parse(sConfigStr, root))
		{
			vecTargetlayer.push_back("������");
			Json::Value &jsLayerNames = root["layer_config"];
			if (!jsLayerNames.isNull())
			{
				std::vector<std::string> vecLayernames = jsLayerNames.getMemberNames();
				for (int i = 0; i < vecLayernames.size(); i++)
				{
					std::string  strLayerName = root["layer_config"][vecLayernames[i]]["layer_name"].asString();
					vecTargetlayer.push_back(strLayerName);
				}
			}
			else
				throw 1;

			Json::Value &jsSideSlopeType = root["resetentitylayer"];
			if (!jsSideSlopeType.isNull())
			{
				if (jsSideSlopeType.isArray())
				{
					for (unsigned int i = 0; i < jsSideSlopeType.size(); i++)
					{
						if (!jsSideSlopeType[i]["Ŀ��ͼ��"].isNull())
						{
							std::string  tagetLayerName = jsSideSlopeType[i]["Ŀ��ͼ��"].asString();
							if (!jsSideSlopeType[i]["�ؼ���"].isNull())
							{
								if (jsSideSlopeType[i]["�ؼ���"].isArray())
								{
									Json::Value& keyvalue = jsSideSlopeType[i]["�ؼ���"];
									for (unsigned int i = 0; i < keyvalue.size(); i++)
									{
										std::string str = keyvalue[i].asString();
										keywordmap[str] = tagetLayerName;
									}
								}
								else
									throw 1;
							}
							else
								throw 1;
						}
						else
							throw 1;
					}
				}
				else
					throw 1;
			}
			else
				throw 1;
		}
		else
			throw 2;
	}
	catch (int e)
	{
		if (e == 1)
		{
			acedAlert(_T("�����ļ��ֶθ�ʽ��ƥ�䣡"));
			return false;
		}
		else if (e == 2)
		{
			acedAlert(_T("��ȡ�����ļ�ʧ�ܣ�"));
			return false;
		}
	}

	return true;

}

int CharInString(const wchar_t findedChar, const wchar_t* theChars)
{
	int i = 0;
	while (*theChars != '\0')
	{
		if (findedChar == *theChars)
		{
			return i;
			break;
		}
		++theChars;
		++i;
	}
	return -1;

}

bool StringA_In_StringB(const wchar_t* chars1, const wchar_t* chars2)
{
	int indexOfFirstLetter = CharInString(*chars1, chars2);
	if (indexOfFirstLetter != -1)
	{
		while (*chars1 != '\0'&& *++chars1 == chars2[++indexOfFirstLetter])
		{
		}
		if (*chars1 == '\0')
			return TRUE;
		else
		{
			return FALSE;
		}
	}
	else
		return FALSE;

}
ACHAR* ChartoACHAR(const char* src)
{
	size_t srcsize = strlen(src) + 1;
	size_t newsize = srcsize;
	size_t convertedChars = 0;
	wchar_t *wcstring;
	wcstring = new wchar_t[newsize];
	mbstowcs_s(&convertedChars, wcstring, srcsize, src, _TRUNCATE);

	return wcstring;
}

void CDlgResetEntityLayer::getAllLayers(std::map<std::string, std::string>& keywordmap, std::map<std::string, std::string>& outkeywordmap, std::vector<CString>& vecMismatchlayer)
{
	Doc_Locker _lock;
	vecMismatchlayer.clear();

	Acad::ErrorStatus es;
	AcDbLayerTable *pLayerTable = NULL;
	es = acdbHostApplicationServices()->workingDatabase()->getSymbolTable(pLayerTable, AcDb::kForRead);
	if (es != Acad::eOk)
	{
		pLayerTable->close();
		return;
	}
	//����һ����������
	AcDbLayerTableIterator *pLayerTableIterator;
	pLayerTable->newIterator(pLayerTableIterator);
	pLayerTable->close();

	for (int i = 0; !pLayerTableIterator->done(); pLayerTableIterator->step(), i++)
	{
		AcDbLayerTableRecord *pLayerTableRecord = NULL;
		pLayerTableIterator->getRecord(pLayerTableRecord, AcDb::kForRead);
		AcString pLayerName;
		pLayerTableRecord->getName(pLayerName);
		CString csLayername = CT2A(pLayerName);

		bool flag = true;
		std::string slayername = CT2A(csLayername);
		if (std::find(vecTargetlayer.begin(), vecTargetlayer.end(), slayername) == vecTargetlayer.end())
		{
			std::map<std::string, std::string>::iterator iter;
			for (iter = keywordmap.begin(); iter != keywordmap.end(); iter++)
			{
				std::string str = iter->first;
				if (StringA_In_StringB(ChartoACHAR(str.c_str()), pLayerName))
				{
					outkeywordmap[slayername] = iter->second;
					flag = false;
					break;
				}
			}
		}
		else
			flag = false;

		if (flag)
		{
			vecMismatchlayer.push_back(csLayername);
		}

		pLayerTableRecord->close();

	}

	delete pLayerTableIterator;
	pLayerTableIterator = NULL;
	return;
}

bool CDlgResetEntityLayer::setLayerofEntity(const AcString& setlayername, AcDbObjectId& entityId)
{
	AcDbEntity *pEnt = NULL;
	Acad::ErrorStatus es = acdbOpenObject(pEnt, entityId, AcDb::kForWrite);
	if (es != eOk)
		return false;
	pEnt->setLayer(setlayername);
	pEnt->close();

	return true;
}

void CDlgResetEntityLayer::createLayer(CString& layername, AcDbDatabase *pDb /*= acdbCurDwg()*/)
{
	AcDbLayerTable *pLayerTbl = NULL;
	pDb->getLayerTable(pLayerTbl, AcDb::kForWrite);

	//�Ƿ��Ѿ�����ָ���Ĳ���¼
	if (!pLayerTbl->has(layername))
	{
		//��������¼
		AcDbLayerTableRecord *pLayerTblRcd = new AcDbLayerTableRecord();
		pLayerTblRcd->setName(layername);
		AcDbObjectId layerTblRcdId;
		pLayerTbl->add(layerTblRcdId, pLayerTblRcd);
		pLayerTblRcd->close();
	}
	if (pLayerTbl)
		pLayerTbl->close();
}

AcDbObjectIdArray CDlgResetEntityLayer::GetAllEntityId(const TCHAR* layername, AcDbDatabase *pDb /*= acdbCurDwg()*/)
{
	AcDbObjectIdArray entIds;
	bool bFilterlayer = false;
	AcDbObjectId layerId;
	//��ȡָ��ͼ�����ID
	if (layername != NULL)
	{
		AcDbLayerTable *pLayerTbl = NULL;
		acdbHostApplicationServices()->workingDatabase()->getSymbolTable(pLayerTbl, AcDb::kForRead);
		if (!pLayerTbl->has(layername))
		{
			pLayerTbl->close();
			return entIds;
		}
		pLayerTbl->getAt(layername, layerId);
		pLayerTbl->close();
		bFilterlayer = true;
	}
	//��ÿ��
	AcDbBlockTable *pBlkTbl = NULL;
	acdbHostApplicationServices()->workingDatabase()->getSymbolTable(pBlkTbl, AcDb::kForRead);
	//����¼
	AcDbBlockTableRecord *pBlkTblRcd = NULL;
	pBlkTbl->getAt(ACDB_MODEL_SPACE, pBlkTblRcd, AcDb::kForRead);
	pBlkTbl->close();
	//���������������η���ģ�Ϳռ��е�ÿһ��ʵ��
	AcDbBlockTableRecordIterator *it = NULL;
	pBlkTblRcd->newIterator(it);
	for (it->start(); !it->done(); it->step())
	{
		AcDbEntity *pEnt = NULL;
		Acad::ErrorStatus es = it->getEntity(pEnt, AcDb::kForRead);
		if (es == Acad::eOk)
		{
			if (bFilterlayer)//����ͼ��
			{
				if (pEnt->layerId() == layerId)
				{
					entIds.append(pEnt->objectId());
				}
			}
			else
			{
				pEnt->close();
			}
		}
		if (pEnt)
			pEnt->close();
	}
	delete it;
	pBlkTblRcd->close();
	return entIds;
}

void CDlgResetEntityLayer::deleteLayer(CString& layername, AcDbDatabase *pDb/* = acdbCurDwg()*/)
{
	if (layername.Compare(_T("0")) == 0)
		return;

	AcDbLayerTable *pLayerTbl;
	pDb->getLayerTable(pLayerTbl, AcDb::kForRead);

	//���ָ������¼��ָ��
	AcDbLayerTableRecord *pLayerTblRcd = NULL;
	Acad::ErrorStatus es = pLayerTbl->getAt(layername, pLayerTblRcd, AcDb::kForWrite);
	if (es == eOk)
	{
		pLayerTblRcd->erase();
		if (pLayerTblRcd)
			pLayerTblRcd->close();
	}

	if (pLayerTbl)
		pLayerTbl->close();
}

LRESULT CDlgResetEntityLayer::OnAcadKeepFocus(WPARAM, LPARAM)
{
	return (TRUE);
}

BEGIN_MESSAGE_MAP(CDlgResetEntityLayer, CAcUiDialog)
	ON_MESSAGE(WM_ACAD_KEEPFOCUS, OnAcadKeepFocus)
	ON_NOTIFY(NM_CLICK, IDC_LIST1, &CDlgResetEntityLayer::OnNMClickList1)
	ON_BN_CLICKED(IDC_BUTTON_TRUE, &CDlgResetEntityLayer::OnBnClickedButtonTrue)
	ON_CBN_KILLFOCUS(IDC_COMBOX_CREATEID, &CDlgResetEntityLayer::OnKillfocusCcomboBox)
END_MESSAGE_MAP()

void CDlgResetEntityLayer::OnNMClickList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	NM_LISTVIEW  *pEditCtrl = (NM_LISTVIEW *)pNMHDR;
	if (pEditCtrl->iItem == -1)//������ǹ�����
	{
		if (haveccomboboxcreate == true)//���֮ǰ�����������б������ٵ�
		{
			distroyCcombobox(&m_list, &m_comBox, e_Item, e_SubItem);
			haveccomboboxcreate = false;
		}
	}
	else if (pEditCtrl->iSubItem != 2)
	{
		if (haveccomboboxcreate == true)//���֮ǰ�����˱༭������ٵ�
		{
			distroyCcombobox(&m_list, &m_comBox, e_Item, e_SubItem);
			haveccomboboxcreate = false;
		}
		else
		{
			e_Item = pEditCtrl->iItem;//�����еĵ�Ԫ����и�ֵ�����ձ༭�����С��Ա���ڴ���
			e_SubItem = pEditCtrl->iSubItem;//�����еĵ�Ԫ����и�ֵ�����ձ༭�����С��Ա���ڴ���
		}
	}
	else
	{
		if (haveccomboboxcreate == true)
		{
			if (!(e_Item == pEditCtrl->iItem && e_SubItem == pEditCtrl->iSubItem))//������еĵ�Ԫ����֮ǰ�����õ�
			{
				distroyCcombobox(&m_list, &m_comBox, e_Item, e_SubItem);
				haveccomboboxcreate = false;
				createCcombobox(pEditCtrl, &m_comBox, e_Item, e_SubItem, haveccomboboxcreate);//�����༭��
				for (int i = 0; i < vecTargetlayer.size(); i++)
				{
					CString str = vecTargetlayer[i].c_str();
					m_comBox.AddString(str);
				}
				m_comBox.ShowDropDown();//�Զ�����
			}
			else//���еĵ�Ԫ����֮ǰ�����õ�
			{
				m_comBox.SetFocus();//����Ϊ���� 
			}
		}
		else
		{
			e_Item = pEditCtrl->iItem;//�����еĵ�Ԫ����и�ֵ�����ձ༭�����С��Ա���ڴ���
			e_SubItem = pEditCtrl->iSubItem;//�����еĵ�Ԫ����и�ֵ�����ձ༭�����С��Ա���ڴ���
			createCcombobox(pEditCtrl, &m_comBox, e_Item, e_SubItem, haveccomboboxcreate);//�����༭��
			for (int i = 0; i < vecTargetlayer.size(); i++)
			{
				CString str = vecTargetlayer[i].c_str();
				m_comBox.AddString(str);
			}
			m_comBox.ShowDropDown();//�Զ�����
		}
	}


	*pResult = 0;
}

//������Ԫ�������б����
//pEditCtrlΪ�б����ָ�룬createccomboboxΪ�����б��ָ�����
//ItemΪ������Ԫ�����б��е��У�SubItem��Ϊ�У�havecreatΪ���󴴽���׼
void CDlgResetEntityLayer::createCcombobox(NM_LISTVIEW  *pEditCtrl, CComboBox *createccomboboxobj, int &Item, int &SubItem, bool &havecreat)
{
	Item = pEditCtrl->iItem;//�����еĵ�Ԫ����и�ֵ�����ձ༭�����С��Ա���ڴ���
	SubItem = pEditCtrl->iSubItem;//�����еĵ�Ԫ����и�ֵ�����ձ༭�����С��Ա���ڴ���
	havecreat = true;
	createccomboboxobj->Create(WS_CHILD | WS_VISIBLE | CBS_DROPDOWN | CBS_OEMCONVERT, CRect(0, 0, 0, 0), this, IDC_COMBOX_CREATEID);
	createccomboboxobj->SetFont(this->GetFont(), FALSE);//��������,����������Ļ�������ֻ��ͻأ�ĸо�
	createccomboboxobj->SetParent(&m_list);//��list control����Ϊ������,���ɵ�Ccombobox������ȷ��λ,���Ҳ����Ҫ
	CRect  EditRect;
	m_list.GetSubItemRect(e_Item, e_SubItem, LVIR_LABEL, EditRect);//��ȡ��Ԫ��Ŀռ�λ����Ϣ
	EditRect.SetRect(EditRect.left + 1, EditRect.top + 1, EditRect.left + m_list.GetColumnWidth(e_SubItem) - 1, EditRect.bottom - 1);//+1��-1�����ñ༭�����ڵ�ס�б���е�������
	CString strItem = m_list.GetItemText(e_Item, e_SubItem);//�����Ӧ��Ԫ���ַ�
	createccomboboxobj->SetWindowText(strItem);//����Ԫ���ַ���ʾ�ڱ༭����
	createccomboboxobj->MoveWindow(&EditRect);//���༭��λ�÷�����Ӧ��Ԫ����
	createccomboboxobj->ShowWindow(SW_SHOW);//��ʾ�༭���ڵ�Ԫ������
}

void CDlgResetEntityLayer::distroyCcombobox(CListCtrl *list, CComboBox* distroyccomboboxobj, int &Item, int &SubItem)
{
	CString meditdata;
	distroyccomboboxobj->GetWindowTextW(meditdata);
	list->SetItemText(Item, SubItem, meditdata);//������Ӧ��Ԫ���ַ�
	distroyccomboboxobj->DestroyWindow();//���ٶ����д�����Ҫ�����٣���Ȼ�ᱨ��
}

void CDlgResetEntityLayer::OnKillfocusCcomboBox()
{
	if (haveccomboboxcreate == true)//���֮ǰ�����������б������ٵ�
	{
		distroyCcombobox(&m_list, &m_comBox, e_Item, e_SubItem);
		haveccomboboxcreate = false;
	}
}

void CDlgResetEntityLayer::OnBnClickedButtonTrue()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������

	Doc_Locker _lock;
	bool flag = true;
	for (int i = 0; i < m_list.GetItemCount(); i++)
	{
		CString oldlayername = m_list.GetItemText(i, 1);
		CString newlayername = m_list.GetItemText(i, 2);

		if (newlayername.Compare(_T("������")) == 0)
			continue;

		createLayer(newlayername);
		AcDbObjectIdArray ids = GetAllEntityId(oldlayername);
		if (ids.length() <= 0)
		{
			deleteLayer(oldlayername);
			continue;
		}
		for (int k = 0; k < ids.length(); k++)
		{
			if (!setLayerofEntity((AcString)newlayername, ids[k]))
			{
				CString str;
				str.Format(_T("����ʵ�嵽%sͼ��ʧ��"), newlayername);
				AfxMessageBox(str);
				flag = false;
				continue;
			}
			else
				deleteLayer(oldlayername);
		}
	}

	if (flag)
	{
		AfxMessageBox(_T("��ɣ�"));
		OnClose();
	}

}


