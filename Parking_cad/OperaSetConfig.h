#pragma once
#include "Opera.h"
#include "DlgSetConfig.h"
class COperaSetConfig :
	public COpera
{
public:
	COperaSetConfig();
	~COperaSetConfig();
	virtual void Start();
	AcGeTol m_tol;
	static class CDlgSetConfig* ms_SetConfigDlg;
};

