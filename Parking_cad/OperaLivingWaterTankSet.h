#pragma once
#include "IOperaLog.h"

class COperaLivingWaterTankSet :
	public CIOperaLog
{
public:
	COperaLivingWaterTankSet(const AcString& group, const AcString& cmd, const AcString& alias, Adesk::Int32 cmdFlag);
	~COperaLivingWaterTankSet();
	virtual void Start();
};

