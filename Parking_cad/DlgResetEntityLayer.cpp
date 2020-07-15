// DlgResetEntityLayer.cpp : 实现文件
//

#include "stdafx.h"
#include "DlgResetEntityLayer.h"
#include "afxdialogex.h"
#include "OperaResetEntityLayer.h"
#include "DBHelper.h"
#include "FileHelper.h"
#include "json\reader.h"

// CDlgResetEntityLayer 对话框

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
	DDX_Control(pDX, IDC_COMBOX_CREATEID, m_comBox);
}

BOOL CDlgResetEntityLayer::OnInitDialog()
{
	CAcUiDialog::OnInitDialog();

	DlgHelper::AdjustPosition(this, DlgHelper::TOP_LEFT);

	CRect rect;
	// 获取编程语言列表视图控件的位置和大小   
	m_list.GetClientRect(&rect);
	// 为列表视图控件添加全行选中和栅格风格   
	m_list.SetExtendedStyle(m_list.GetExtendedStyle() | LVS_EX_FULLROWSELECT |LVS_LIST );
	m_list.InsertColumn(0, _T(""), LVCFMT_RIGHT, rect.Width() * 0, 0);
	m_list.InsertColumn(1, _T("原图层"), LVCFMT_CENTER, rect.Width() * 0.48, 1);
	m_list.InsertColumn(2, _T("目标图层"), LVCFMT_CENTER, rect.Width() * 0.48, 2);

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
			m_list.SetItemText(k, 2, _T("不处理"));
		}

	} 
	ComneedSave = false;
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
	DestroyWindow();//销毁对话框

	delete this;
}

BOOL CDlgResetEntityLayer::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_MOUSEWHEEL)
	{
		if (ComneedSave)
		{
			hideCombox();
			ComneedSave = false;
		}
		POINT pos;
		GetCursorPos(&pos);
		pMsg->hwnd = WindowFromPoint(pos)->GetSafeHwnd();
	}
	return CDialog::PreTranslateMessage(pMsg);
}

bool CDlgResetEntityLayer::readParkingConfig()
{
	std::string sConfigFile = GetUserDirA() + "ParkingConfig.json";
	std::string sConfigStr = FileHelper::ReadText(sConfigFile.c_str());
	Json::Reader reader;
	Json::Value root;

	
	if (reader.parse(sConfigStr, root))
	{
		vecTargetlayer.push_back("不处理");
		Json::Value &jsLayerNames = root["layer_config"];
		if (!jsLayerNames.isNull())
		{
			std::vector<std::string> vecLayernames = jsLayerNames.getMemberNames();
			for (int i = 0; i < vecLayernames.size(); i++)
			{
				std::string  strLayerName = jsLayerNames[vecLayernames[i]]["layer_name"].asString();
				vecTargetlayer.push_back(strLayerName);
			}
		}
		else
		{
			acedAlert(_T("ParkingConfig配置文件字段格式不匹配！"));
			return false;
		}
	}
	else
	{
		acedAlert(_T("读取ParkingConfig配置文件失败！"));
		return false;
	}

	return true;
}

bool CDlgResetEntityLayer::readConfig(std::map<std::string, std::string>& keywordmap)
{
	std::string sConfigFile = GetUserDirA() + "resetentitylayerconfig.json";
	std::string sConfigStr = FileHelper::ReadText(sConfigFile.c_str());
	Json::Reader reader;
	Json::Value root;

	try
	{
		if (reader.parse(sConfigStr, root))
		{
			if (!readParkingConfig())
				return false;

			Json::Value &jsSideSlopeType = root["resetentitylayer"];
			if (!jsSideSlopeType.isNull())
			{
				if (jsSideSlopeType.isArray())
				{
					for (unsigned int i = 0; i < jsSideSlopeType.size(); i++)
					{
						if (!jsSideSlopeType[i]["目标图层"].isNull())
						{
							std::string  tagetLayerName = jsSideSlopeType[i]["目标图层"].asString();
							if (!jsSideSlopeType[i]["关键字"].isNull())
							{
								if (jsSideSlopeType[i]["关键字"].isArray())
								{
									Json::Value& keyvalue = jsSideSlopeType[i]["关键字"];
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
			acedAlert(_T("配置文件字段格式不匹配！"));
			return false;
		}
		else if (e == 2)
		{
			acedAlert(_T("读取配置文件失败！"));
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
	//创建一个层表迭代器
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

	//是否已经包含指定的层表记录
	if (!pLayerTbl->has(layername))
	{
		//创建层表记录
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
	//获取指定图层对象ID
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
	//获得块表
	AcDbBlockTable *pBlkTbl = NULL;
	acdbHostApplicationServices()->workingDatabase()->getSymbolTable(pBlkTbl, AcDb::kForRead);
	//块表记录
	AcDbBlockTableRecord *pBlkTblRcd = NULL;
	pBlkTbl->getAt(ACDB_MODEL_SPACE, pBlkTblRcd, AcDb::kForRead);
	pBlkTbl->close();
	//创建遍历器，依次访问模型空间中的每一个实体
	AcDbBlockTableRecordIterator *it = NULL;
	pBlkTblRcd->newIterator(it);
	for (it->start(); !it->done(); it->step())
	{
		AcDbEntity *pEnt = NULL;
		Acad::ErrorStatus es = it->getEntity(pEnt, AcDb::kForRead);
		if (es == Acad::eOk)
		{
			if (bFilterlayer)//过滤图层
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

	//获得指定层表记录的指针
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

int CDlgResetEntityLayer::setCombox(CString& text)
{
	int num = 0;
	m_comBox.ResetContent();
	for (int i = 0; i < vecTargetlayer.size(); i++)
	{
		CString str = vecTargetlayer[i].c_str();
		if (str.Compare(text) == 0)
			num = i;
		m_comBox.AddString(str);
	}

	return num;
}

void CDlgResetEntityLayer::hideCombox()
{
	CString strCom;
	m_comBox.GetWindowText(strCom);
	m_list.SetItemText(m_ComItem, m_ComSubItem, strCom);
	m_comBox.ShowWindow(SW_HIDE);
}

LRESULT CDlgResetEntityLayer::OnAcadKeepFocus(WPARAM, LPARAM)
{
	return (TRUE);
}

BEGIN_MESSAGE_MAP(CDlgResetEntityLayer, CAcUiDialog)
	ON_MESSAGE(WM_ACAD_KEEPFOCUS, OnAcadKeepFocus)
	ON_NOTIFY(NM_CLICK, IDC_LIST1, &CDlgResetEntityLayer::OnNMClickList1)
	ON_BN_CLICKED(IDC_BUTTON_TRUE, &CDlgResetEntityLayer::OnBnClickedButtonTrue)
	ON_CBN_SELCHANGE(IDC_COMBOX_CREATEID, &CDlgResetEntityLayer::OnCbnSelchangeComboxCreateid)
END_MESSAGE_MAP()

void CDlgResetEntityLayer::OnNMClickList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码  
  
	NM_LISTVIEW  *pEditCtrl = (NM_LISTVIEW *)pNMHDR;
	CRect  ComRect;

	if (pEditCtrl->iItem == -1)
	{
		//当点击其他区域时,显示内容  
		if (ComneedSave)
		{
			hideCombox();
		}
		else
		{
			m_list.SetItemText(m_ComItem, m_ComSubItem, _T("不处理"));
			m_comBox.ShowWindow(SW_HIDE);
		}
		ComneedSave = false;

		return;
	}  
	else if(pEditCtrl->iSubItem != 2)
	{
		if (ComneedSave)
		{
			hideCombox();
			ComneedSave = false;
			
		}
		else
		{
			m_ComItem = pEditCtrl->iItem;//将点中的单元格的行赋值给“刚编辑过的行”以便后期处理
			m_ComSubItem = pEditCtrl->iSubItem;
			m_comBox.ShowWindow(SW_HIDE);
		}
	}
	else
	{
		if (ComneedSave)
		{
			if (!(m_ComItem == pEditCtrl->iItem && m_ComSubItem == pEditCtrl->iSubItem))//如果点中的单元格不是之前创建好的
			{
				hideCombox();
				ComneedSave = false;		
			}		
		}
		else
		{
			m_ComItem = pEditCtrl->iItem; //行数  
			m_ComSubItem = pEditCtrl->iSubItem; //列数  
			m_list.GetSubItemRect(m_ComItem, m_ComSubItem, LVIR_LABEL, ComRect);

			CString  ComstrItem;
			m_comBox.SetParent(&m_list);
			ComstrItem = m_list.GetItemText(m_ComItem, m_ComSubItem);
			ComRect.SetRect(ComRect.left, ComRect.top, ComRect.left + m_list.GetColumnWidth(m_ComSubItem), ComRect.bottom);
			m_comBox.MoveWindow(&ComRect);
			m_comBox.ShowWindow(SW_SHOW);
			m_comBox.SetCurSel(setCombox(ComstrItem));	
			m_comBox.ShowDropDown();
			ComneedSave = true;
		}	
	}

	*pResult = 0;
}

void CDlgResetEntityLayer::OnBnClickedButtonTrue()
{
	// TODO: 在此添加控件通知处理程序代码

	Doc_Locker _lock;
	bool flag = true;
	for (int i = 0; i < m_list.GetItemCount(); i++)
	{
		CString oldlayername = m_list.GetItemText(i, 1);
		CString newlayername = m_list.GetItemText(i, 2);

		if (newlayername.Compare(_T("不处理")) == 0)
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
				str.Format(_T("设置实体到%s图层失败"), newlayername);
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
		AfxMessageBox(_T("完成！"));
		OnCancel();
	}

}

void CDlgResetEntityLayer::OnCbnSelchangeComboxCreateid()
{
	// TODO: 在此添加控件通知处理程序代码
	CString strCom;
	m_comBox.GetWindowText(strCom);
	m_list.SetItemText(m_ComItem, m_ComSubItem, strCom);
	m_comBox.ShowWindow(SW_HIDE);
	ComneedSave = false;
}