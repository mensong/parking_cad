#pragma once
#include "IOperaLog.h"
class COperaFirePoolSet :
	public CIOperaLog
{
public:
	COperaFirePoolSet(const AcString& group, const AcString& cmd, const AcString& alias, Adesk::Int32 cmdFlag);
	~COperaFirePoolSet();
	virtual void Start();
};

