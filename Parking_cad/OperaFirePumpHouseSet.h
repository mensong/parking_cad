#pragma once
#include "IOperaLog.h"
class COperaFirePumpHouseSet :
	public CIOperaLog
{
public:
	COperaFirePumpHouseSet(const AcString& group, const AcString& cmd, const AcString& alias, Adesk::Int32 cmdFlag);
	~COperaFirePumpHouseSet();
	virtual void Start();
};

