#pragma once


// CMyListBox

class CMyListBox 
	: public CListBox
{
	DECLARE_DYNAMIC(CMyListBox)

public:
	CMyListBox();
	virtual ~CMyListBox();

public:
	int AddString(LPCTSTR lpszItem);
	int DeleteString(UINT nIndex);
	int InsertString(int nIndex, LPCTSTR lpszItem);
	void ResetContent();

	void UpdateWidth(LPCTSTR lpszItem);

private:
	int  m_nMaxWidth;

protected:
	DECLARE_MESSAGE_MAP()
};


