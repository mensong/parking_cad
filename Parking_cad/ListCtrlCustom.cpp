#include "StdAfx.h"
#include "ListCtrlCustom.h"

IMPLEMENT_DYNAMIC(CListCtrlCustom, CListCtrl)

CListCtrlCustom::CListCtrlCustom(void)
	: m_pfnResizeExCtrl(NULL)
	, m_bAutoReleaseSubCtrls(true)
{
}

CListCtrlCustom::~CListCtrlCustom(void)
{
	if (m_bAutoReleaseSubCtrls)
		ClearExCtrls();
	m_lstImgForRowHeight.DeleteImageList();
}

BEGIN_MESSAGE_MAP(CListCtrlCustom, CListCtrl)
	//{{AFX_MSG_MAP(SDCtrlComboboxListctrl)
	ON_NOTIFY_REFLECT(LVN_ENDSCROLL, &CListCtrlCustom::OnLvnEndScroll)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

LRESULT CListCtrlCustom::DefWindowProc( UINT message, WPARAM wParam, LPARAM lParam )
{
	LRESULT lRet = CListCtrl::DefWindowProc(message, wParam, lParam);
	LVITEM *pItem = (LVITEM *)lParam;

	switch (message)
	{
	case LVM_INSERTITEM:
		{
			int nCurRow = (int)lRet;		//当前行号
			LP_COLUMN_INFO pArrColumnCreatedCtrls = new COLUMN_INFO;
			int nColumnCount = GetHeaderCtrl()->GetItemCount();
			for (int i=0; i<nColumnCount; ++i)
			{
				pArrColumnCreatedCtrls->Add(NULL);
			}
			m_arrExCtrls.InsertAt(nCurRow, pArrColumnCreatedCtrls);//如果此行没有自定义控件的话，则插入一个NULL
			if (NULL!=pArrColumnCreatedCtrls)
			{
				UpdateExCtrlsPos();
			}
		}
		break;

	case LVM_DELETEITEM:
		{
			int nCurRow = (int)wParam;		//当前行号

			LP_COLUMN_INFO pColInfo = m_arrExCtrls[nCurRow];
			if (NULL==pColInfo)
			{
				break;
			}
			int nCtrls = (int)pColInfo->GetCount();
			for (int i=0; i<nCtrls; ++i)
			{
				CWnd *pCtrl = pColInfo->GetAt(i);
				if (NULL==pCtrl)
				{
					continue;
				}

				if (m_setAutoDelCtrls.find(pCtrl) != m_setAutoDelCtrls.end())
					m_setAutoDelCtrls.erase(pCtrl);

				if (m_setTempCtrlWillDel.find(pCtrl) != m_setTempCtrlWillDel.end())
				{
					pCtrl->DestroyWindow();
					delete (pCtrl);
				}				
			}

			delete pColInfo;
			m_setTempCtrlWillDel.clear();
			m_arrExCtrls.RemoveAt(nCurRow);

			UpdateExCtrlsPos();
		}
		break;

	case LVM_DELETEALLITEMS:
		{
			ClearExCtrls();
		}
		break;

	case LVM_INSERTCOLUMN:
		{
			//附加控件
			int nInsertColumn = (int)wParam;
			int nRowCount = (int)m_arrExCtrls.GetCount();
			for (int i=0; i<nRowCount; ++i)
			{
				LP_COLUMN_INFO pColInfo = m_arrExCtrls[i];
				if (NULL == pColInfo)
				{
					continue;
				}

				pColInfo->InsertAt(nInsertColumn, (CWnd *)NULL);
			}
			UpdateExCtrlsPos();
		}
		break;

	case LVM_DELETECOLUMN:
		{
			//附加控件
			int nRemoveColumn = (int)wParam;
			int nRowCount = (int)m_arrExCtrls.GetCount();
			for (int i=0; i<nRowCount; ++i)
			{
				LP_COLUMN_INFO pColInfo = m_arrExCtrls[i];
				if (NULL == pColInfo)
				{
					continue;
				}
				if (nRemoveColumn < pColInfo->GetCount())
				{
					//删除列的时候，如果有自定控件则需要删除自定义控件
					CWnd *pCtrl = pColInfo->GetAt(nRemoveColumn);
					if (NULL!=pCtrl)
					{
						pCtrl->DestroyWindow();
						delete (pCtrl);
					}
					pColInfo->RemoveAt(nRemoveColumn);
				}
			}
			UpdateExCtrlsPos();
		}
		break;
	case WM_SIZE:
		{
			UpdateExCtrlsPos();
		}
		break;
	}

	return lRet;
}

BOOL CListCtrlCustom::OnNotify( WPARAM wParam, LPARAM lParam, LRESULT* pResult )
{
	NMHDR *pNMHDR = (NMHDR *)lParam;

	switch (pNMHDR->code)
	{
		//拖动和双击都会引起表头宽度的变化
	case HDN_BEGINTRACK:	//开始拖动列头
		{
			//检测列是否被禁止拖动
			NMHEADER *pNMHD = (NMHEADER*)pNMHDR;
			if (m_setNotTrackColumn.find(pNMHD->iItem) != m_setNotTrackColumn.end())
			{
				*pResult = TRUE;
				return TRUE;
			}

			//隐藏掉全部控件
			int nRows = (int)m_arrExCtrls.GetCount();
			for (int i=0; i<nRows; ++i)
			{
				LP_COLUMN_INFO pColInfo = m_arrExCtrls[i];
				if (NULL==pColInfo)
				{
					continue;
				}
				int nCtrls = (int)pColInfo->GetCount();
				for (int i=0; i<nCtrls; ++i)
				{
					CWnd *pCtrl = pColInfo->GetAt(i);
					if (NULL==pCtrl)
					{
						continue;
					}

					pCtrl->ShowWindow(SW_HIDE);
				}
			}
		}
		break;
	case HDN_ENDTRACK:			//拖动列头
		{
			//检测列是否被禁止拖动
			NMHEADER *pNMHD = (NMHEADER*)pNMHDR;
			if (m_setNotTrackColumn.find(pNMHD->iItem) != m_setNotTrackColumn.end())
			{
				*pResult = TRUE;
				return TRUE;
			}

			UpdateExCtrlsPos();
			RedrawWindow();//防止拖动列（列由大拖到小）的时候，控件不及时刷新
		}
		break;
	case HDN_DIVIDERDBLCLICK:	//双击列头
		{
			//禁止双击表头分割线调整列的宽度
			*pResult = TRUE;
			return TRUE;
		}
		break;
	}

	return CListCtrl::OnNotify(wParam, lParam, pResult);
}

void CListCtrlCustom::OnLvnEndScroll( NMHDR *pNMHDR, LRESULT *pResult )
{
	LPNMLVSCROLL pStateChanged = reinterpret_cast<LPNMLVSCROLL>(pNMHDR);
	UpdateExCtrlsPos();
	*pResult = 0;
}

void CListCtrlCustom::UpdateExCtrlsPos()
{
	int nRows = (int)m_arrExCtrls.GetCount();
	int nTopRowIndex = GetTopIndex();
	//int nRowCount = GetItemCount();

	//隐藏掉全部控件
	bool bHasChanged = false;
	for (int i=0; i<nRows; ++i)
	{
		LP_COLUMN_INFO pColInfo = m_arrExCtrls[i];
		if (NULL==pColInfo)
		{
			continue;
		}
		int nCtrls = (int)pColInfo->GetCount();
		for (int i=0; i<nCtrls; ++i)
		{
			CWnd *pCtrl = pColInfo->GetAt(i);
			if (NULL==pCtrl)
			{
				continue;
			}

			if (pCtrl->IsWindowVisible())
				pCtrl->ShowWindow(SW_HIDE);
			bHasChanged = true;
		}
	}
	if (!bHasChanged)
		return;
	
	//获得listctrl的范围
	CRect rectWin;
	GetWindowRect(&rectWin);
	ScreenToClient(&rectWin);

	//如果控件的位置在listctrl的外面，则不需要显视和调整位置
	//重新调整位置
	//判断高度是否退出
	bool bExit = false;
	std::map<CString, bool>::iterator itVisible;
	int nRowMove = nTopRowIndex;
	for (; nRowMove<m_arrExCtrls.GetSize(); ++nRowMove)
	{
		//重新匹配
		LP_COLUMN_INFO pColInfo = m_arrExCtrls[nRowMove];
		if (NULL==pColInfo)
		{
			continue;
		}
		int nCtrls = (int)pColInfo->GetCount();
		for (int nColumn=0; nColumn<nCtrls; ++nColumn)
		{
			CWnd *pCtrl = pColInfo->GetAt(nColumn);
			if (NULL==pCtrl)
			{
				continue;
			}

			CRect rectColumn;
			//GetSubItemRect(nRowMove, nColumn, LVIR_BOUNDS, rectColumn);
			GetItemRect(nRowMove, nColumn, rectColumn);
			if (rectColumn.top>rectWin.bottom)
			{
				bExit = true;
				break;
			}

			itVisible = m_mpVisible.find(makeRowColKey(nRowMove, nColumn));
			if (itVisible == m_mpVisible.end() || itVisible->second)//是否可见
			{
				pCtrl->ShowWindow(SW_SHOW);
				if (NULL == m_pfnResizeExCtrl
					|| m_pfnResizeExCtrl(this, pCtrl, rectColumn) == FALSE)
				{
					pCtrl->MoveWindow(&rectColumn);
				}
			}
		}

		if (bExit)
		{
			break;
		}
	}

	//RedrawWindow();
}

BOOL CListCtrlCustom::Create( _In_ DWORD dwStyle, _In_ const RECT& rect, _In_ CWnd* pParentWnd, _In_ UINT nID )
{
	BOOL bRet = CListCtrl::Create(dwStyle, rect, pParentWnd, nID);

	//强制使用report方式
	LONG lStyle = GetWindowLong(GetSafeHwnd(), GWL_STYLE);
	lStyle |= LVS_REPORT;
	SetWindowLong(GetSafeHwnd(), GWL_STYLE, lStyle);

	return bRet;
}

BOOL CListCtrlCustom::CreateEx( _In_ DWORD dwExStyle, _In_ DWORD dwStyle, _In_ const RECT& rect, _In_ CWnd* pParentWnd, _In_ UINT nID )
{
	BOOL bRet = CListCtrl::CreateEx(dwExStyle, dwStyle, rect, pParentWnd, nID);

	//强制使用report方式
	LONG lStyle = GetWindowLong(GetSafeHwnd(), GWL_STYLE);
	lStyle |= LVS_REPORT;
	SetWindowLong(GetSafeHwnd(), GWL_STYLE, lStyle);

	return bRet;
}

void CListCtrlCustom::ClearExCtrls()
{
	int nRows = (int)m_arrExCtrls.GetCount();
	for (int i=0; i<nRows; ++i)
	{
		LP_COLUMN_INFO pColInfo = m_arrExCtrls[i];
		if (NULL==pColInfo)
		{
			continue;
		}
		int nCtrls = (int)pColInfo->GetCount();
		for (int i=0; i<nCtrls; ++i)
		{
			CWnd *pCtrl = pColInfo->GetAt(i);
			if (NULL==pCtrl)
				continue;

			if (m_setAutoDelCtrls.find(pCtrl) != m_setAutoDelCtrls.end())
			{
				pCtrl->DestroyWindow();
				delete (pCtrl);
			}			
		}
		delete pColInfo;
	}

	m_setTempCtrlWillDel.clear();
	m_setAutoDelCtrls.clear();
	
	m_arrExCtrls.RemoveAll();
}

/** 
* @brief     	获取某行谋列的附加控件
* @param        nRow - 行号
* @param     	nCol - 列号
* @retval    	NULL:此行列没有附加控件
* @note      	
*/
CWnd * CListCtrlCustom::GetCtrl( int nRow, int nCol )
{
	LP_COLUMN_INFO pColumnInfo = m_arrExCtrls[nRow];
	if (NULL == pColumnInfo)
	{
		return NULL;
	}

	if (nCol < pColumnInfo->GetCount())
	{
		return pColumnInfo->GetAt(nCol);
	}

	return NULL;
}

BOOL CListCtrlCustom::GetItemRect( int nRow, int nCol, CRect &rect )
{
	BOOL bRet = GetSubItemRect(nRow, nCol, LVIR_BOUNDS, rect);
	//对于第一列，需要特殊处理
	if (TRUE==bRet &&  0==nCol)
	{
		int nWidth0 = GetColumnWidth(0);
		rect.right = rect.left + nWidth0;
	}
	return bRet;
}

CString CListCtrlCustom::GetText( int nRow, int nCol )
{
	//如果存在附加控件则返回附加控件的文本，如果没有附加控件则直接返回格子的值
	CWnd *pCtrl = GetCtrl(nRow, nCol);
	if (NULL == pCtrl)
	{
		return GetItemText(nRow, nCol);
	}

	CString sText;
	pCtrl->GetWindowText(sText);
	return sText;
}

BOOL CListCtrlCustom::SetItemExCtrl( int nItem, int nSubItem, CWnd *pExCtrl, bool bDelIfExist/*=true*/, bool bDelOnListDestory/*=true*/, bool bRefresh/* = true*/)
{
	//获得列的数量
	int nColumnCount = GetHeaderCtrl()->GetItemCount();
	if (nItem >= GetItemCount()			//如果行不够
		|| nSubItem >= nColumnCount		//如果列不够
		)
	{
		return FALSE;
	}

	//为每个格子申请仓库
	LP_COLUMN_INFO &pColumnInfo = m_arrExCtrls[nItem];
	if (NULL == pColumnInfo)
	{
		pColumnInfo = new COLUMN_INFO;
		for (int i=0; i<nColumnCount; ++i)
		{
			pColumnInfo->Add(NULL);	
		}
	}
	else if (bDelIfExist)
	{//如果原来的格子里已经有控件，则需要把原来的控件删除了
		if (pColumnInfo->GetCount() > nSubItem)
		{
			CWnd *pCtrlOld = pColumnInfo->GetAt(nSubItem);
			if (NULL != pCtrlOld)
			{
				if (m_setAutoDelCtrls.find(pCtrlOld) != m_setAutoDelCtrls.end())
					m_setAutoDelCtrls.erase(pCtrlOld);
				delete pCtrlOld;
				pColumnInfo->SetAt(nSubItem, NULL);
			}
		}
	}

	//设置符加控件到仓库
	pColumnInfo->SetAt(nSubItem, pExCtrl);

	if (pExCtrl)
	{
		//设置父窗口为list control
		pExCtrl->ModifyStyleEx(WS_OVERLAPPED | WS_POPUP, WS_CHILD);
		//pExCtrl->ModifyStyle(WS_OVERLAPPED | WS_POPUP, WS_CHILD);
		pExCtrl->SetParent(this);

		if (bDelOnListDestory)
			m_setAutoDelCtrls.insert(pExCtrl);
	}

	if (bRefresh)
		UpdateExCtrlsPos();

	return TRUE;
}

BOOL CListCtrlCustom::SetItemExCtrlVisible(int nItem, int nSubItem, bool bVisible, bool bRefresh/* = true*/)
{
	if (nItem < 0 || nSubItem < 0)
		return FALSE;

	m_mpVisible[makeRowColKey(nItem, nSubItem)] = bVisible;

	if (bRefresh)
		UpdateExCtrlsPos();

	return TRUE;
}

BOOL CListCtrlCustom::SetRowHeight(int nHeight)
{
	if (nHeight < 1)
		nHeight = 1;

	CFont * pFont = GetFont();
	LOGFONT LogFont;
	pFont->GetLogFont(&LogFont);
	if (nHeight < abs(LogFont.lfHeight))
	{
		LogFont.lfHeight = nHeight;//设置字体大小
		m_Font.CreateFontIndirect(&LogFont);
		SetFont(&m_Font);
	}

	m_lstImgForRowHeight.DeleteImageList();
	m_lstImgForRowHeight.Create(1, nHeight, ILC_COLOR32, 1, 0);
	if (SetImageList(&m_lstImgForRowHeight, LVSIL_SMALL) == NULL)
	{
		return FALSE;
	}

	UpdateExCtrlsPos();

	//目的是让滚动条出来（粗糙的方法）
	DeleteItem(InsertItem(0, _T("")));

	return TRUE;
}

int CListCtrlCustom::CurPosHitTest(LVHITTESTINFO &ht)
{
	::GetCursorPos(&(ht.pt));
	ScreenToClient(&(ht.pt));
	return SubItemHitTest(&ht);
}

CString CListCtrlCustom::makeRowColKey(int nItem, int nSubItem)
{
	TCHAR szKey[50];
	_stprintf(szKey, _T("%d|%d"), nItem, nSubItem);

	return szKey;
}

void CListCtrlCustom::PreSubclassWindow()
{
	//强制使用report方式
	LONG lStyle = GetWindowLong(GetSafeHwnd(), GWL_STYLE);
	lStyle |= LVS_REPORT;
	SetWindowLong(GetSafeHwnd(), GWL_STYLE, lStyle);

	CListCtrl::PreSubclassWindow();
}

void CListCtrlCustom::SetAutoReleaseSubCtrls(bool bAutoRelease)
{
	m_bAutoReleaseSubCtrls = bAutoRelease;
}

BOOL CListCtrlCustom::DeleteItem(_In_ int nItem, bool bDelChildCtrl/*=true*/)
{
	if (bDelChildCtrl)
	{
		for (int col=0; col<GetHeaderCtrl()->GetItemCount(); ++col)
		{
			CWnd* pCtrl = GetCtrl(nItem, col);
			if (NULL == pCtrl)
				continue;
			if (m_setAutoDelCtrls.find(pCtrl) != m_setAutoDelCtrls.end())
				m_setTempCtrlWillDel.insert(pCtrl);
		}
	}
	
	return CListCtrl::DeleteItem(nItem);
}

BOOL CListCtrlCustom::DeleteAllItems(bool bDelChildCtrl/*=true*/)
{
	if (bDelChildCtrl)
	{
		for (int row=0; row<GetItemCount(); ++row)
		{
			for (int col=0; col<GetHeaderCtrl()->GetItemCount(); ++col)
			{
				CWnd* pCtrl = GetCtrl(row, col);
				if (NULL == pCtrl)
					continue;
				if (m_setAutoDelCtrls.find(pCtrl) != m_setAutoDelCtrls.end())
					m_setTempCtrlWillDel.insert(pCtrl);
			}
		}
		
	}

	return CListCtrl::DeleteAllItems();
}

void CListCtrlCustom::SetColumnEnableTrack(int nCol, bool bCanBeTrack)
{
	if (!bCanBeTrack)
	{
		m_setNotTrackColumn.insert(nCol);
	}
	else
	{
		m_setNotTrackColumn.erase(nCol);
	}
}

void CListCtrlCustom::RegResizeEvent(LPFUNC_SETTING_RECT pfnResizeExCtrl)
{
	m_pfnResizeExCtrl = pfnResizeExCtrl;
}
