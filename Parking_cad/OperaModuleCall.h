#pragma once
#include "Opera.h"

class COperaModuleCall :
	public COpera
{
public:
	COperaModuleCall();
	~COperaModuleCall();
	virtual void Start();
	void mirrorJigshow(const CString& blockName);
};

