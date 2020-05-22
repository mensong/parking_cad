// MyListBox.cpp : 实现文件
//

#include "stdafx.h"
#include "WaitingDialog.h"
#include "MyListBox.h"


// CMyListBox

IMPLEMENT_DYNAMIC(CMyListBox, CListBox)

CMyListBox::CMyListBox()
{

}

CMyListBox::~CMyListBox()
{
}


int CMyListBox::AddString(LPCTSTR lpszItem)
{
	int nResult = CListBox::AddString(lpszItem);
	if (nResult<0)
	{
		return nResult;
	}
	UpdateWidth(lpszItem);

	return nResult;
}

int CMyListBox::DeleteString(UINT nIndex)
{
	int nResult = CListBox::DeleteString(nIndex);
	if (nResult<0)
	{
		return nResult;
	}

	CFont *pFont = CListBox::GetFont();//考虑不同字体时的情况
	CClientDC dc(this);
	dc.SelectObject(pFont);
	m_nMaxWidth = 0;
	for (int i=0;i<CListBox::GetCount();i++)
	{
		CString str;
		CListBox::GetText(i,str);
		CSize sz = dc.GetTextExtent(str);
		sz.cx += (3 * ::GetSystemMetrics(SM_CXBORDER));//考虑有垂直滚动条时加上滚动条宽度
		if (sz.cx>m_nMaxWidth)
		{
			m_nMaxWidth = sz.cx+2;
		}
	}
	CListBox::SetHorizontalExtent(m_nMaxWidth);

	return nResult;
}

int CMyListBox::InsertString(int nIndex, LPCTSTR lpszItem)
{
	int nResult = CListBox::InsertString(nIndex,lpszItem);
	if (nResult<0)
	{
		return nResult;
	}

	UpdateWidth(lpszItem);

	return nResult;
}

void CMyListBox::ResetContent()
{
	CListBox::ResetContent();
	m_nMaxWidth = 0;
}

void CMyListBox::UpdateWidth(LPCTSTR lpszItem)
{
	CFont *pFont = CListBox::GetFont();
	CClientDC dc(this);
	dc.SelectObject(pFont);
	CSize sz = dc.GetTextExtent(lpszItem,_tcslen(lpszItem));
	sz.cx += (3*::GetSystemMetrics(SM_CXBORDER));
	if (sz.cx > m_nMaxWidth)
	{
		m_nMaxWidth = sz.cx+2;
		CListBox::SetHorizontalExtent(m_nMaxWidth);
	}
}

BEGIN_MESSAGE_MAP(CMyListBox, CListBox)
END_MESSAGE_MAP()



// CMyListBox 消息处理程序


