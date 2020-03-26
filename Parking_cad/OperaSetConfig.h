#pragma once
#include "IOperaLog.h"
#include "DlgSetConfig.h"
class COperaSetConfig :
	public CIOperaLog
{
public:
	COperaSetConfig(const AcString& group, const AcString& cmd, const AcString& alias, Adesk::Int32 cmdFlag);
	~COperaSetConfig();
	virtual void Start();
	AcGeTol m_tol;
	static class CDlgSetConfig* ms_SetConfigDlg;
};

