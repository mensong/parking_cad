#pragma once
#include "Opera.h"

class CIOperaLog :
	public COpera
{
public:
	CIOperaLog(const AcString& group, const AcString& cmd, const AcString& alias, Adesk::Int32 cmdFlag);
	~CIOperaLog();

	virtual void Ended() override;

protected:
	CString m_curcmd;
};

