#pragma once
#include "IOperaLog.h"
class COperaAxisShowOrHide :
	public CIOperaLog
{
public:
	COperaAxisShowOrHide(const AcString& group, const AcString& cmd, const AcString& alias, Adesk::Int32 cmdFlag);
	~COperaAxisShowOrHide();
	virtual void Start();
};

