#pragma once
#include "IOperaLog.h"

class COperaGergul :
	public CIOperaLog
{
public:
	COperaGergul(const AcString& group, const AcString& cmd, const AcString& alias, Adesk::Int32 cmdFlag);
	~COperaGergul(void);

	virtual void Start();

};

