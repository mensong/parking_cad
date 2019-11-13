#pragma once

class CMyEdit :
	public CEdit
{
	DECLARE_DYNAMIC(CMyEdit);
public:
	CMyEdit();
	virtual ~CMyEdit();
//й╣ож
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
};

