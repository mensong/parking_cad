#pragma once
#include "IOperaLog.h"
class COperaLifePumpHouseSet :
	public CIOperaLog
{
public:
	COperaLifePumpHouseSet(const AcString& group, const AcString& cmd, const AcString& alias, Adesk::Int32 cmdFlag);
	~COperaLifePumpHouseSet();
	virtual void Start();
};

