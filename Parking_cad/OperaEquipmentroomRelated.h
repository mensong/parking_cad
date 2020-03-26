#pragma once
#include "IOperaLog.h"


class COperaEquipmentroomRelated :
	public CIOperaLog
{
public:
	COperaEquipmentroomRelated(const AcString& group, const AcString& cmd, const AcString& alias, Adesk::Int32 cmdFlag);
	~COperaEquipmentroomRelated();
	virtual void Start();
	
};

