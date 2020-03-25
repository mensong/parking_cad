#pragma once
#include "IOperaLog.h"
class COperaVillageSet :
	public CIOperaLog
{
public:
	COperaVillageSet(const AcString& group, const AcString& cmd, const AcString& alias, Adesk::Int32 cmdFlag);
	~COperaVillageSet();
	virtual void Start();
};

