#include "stdafx.h"
#include "IOperaLog.h"


CIOperaLog::CIOperaLog(const AcString& group, const AcString& cmd, const AcString& alias, Adesk::Int32 cmdFlag)
	: COpera(group, cmd, alias, cmdFlag)
{
	m_curcmd = cmd.constPtr();
}


CIOperaLog::~CIOperaLog()
{
}

void CIOperaLog::Ended()
{
	COpera::Ended();

	CString sType = _T("opera_") + m_curcmd;
	CParkingLog::AddLog(sType, 0, _T("Ö´ÐÐÃüÁî:") + m_curcmd);
}
