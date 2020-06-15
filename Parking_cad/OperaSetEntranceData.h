#pragma once
#include "IOperaLog.h"
#include "DlgEntrance.h"

class COperaSetEntranceData :
	public CIOperaLog
{
public:
	COperaSetEntranceData(const AcString& group, const AcString& cmd, const AcString& alias, Adesk::Int32 cmdFlag);
	~COperaSetEntranceData();
	virtual void Start();
	AcGeTol m_tol;
	static class CDlgEntrance* ms_EntranceDlg;
};

