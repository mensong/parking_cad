#pragma once
#include "Opera.h"
#include "DlgEntrance.h"
class COperaSetEntranceData :
	public COpera
{
public:
	COperaSetEntranceData();
	~COperaSetEntranceData();
	virtual void Start();
	AcGeTol m_tol;
	static class CDlgEntrance* ms_EntranceDlg;
};

