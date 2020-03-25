#pragma once
#include "IOperaLog.h"

class COperaModuleCall :
	public CIOperaLog
{
public:
	COperaModuleCall(const AcString& group, const AcString& cmd, const AcString& alias, Adesk::Int32 cmdFlag);
	~COperaModuleCall();
	virtual void Start();
	void mirrorJigshow(const CString& blockName);
};

