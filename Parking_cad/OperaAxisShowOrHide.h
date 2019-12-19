#pragma once
#include "Opera.h"
class COperaAxisShowOrHide :
	public COpera
{
public:
	COperaAxisShowOrHide();
	~COperaAxisShowOrHide();
	virtual void Start();
	void setLayerClose(const CString& layerName);
	bool isLayerClosed(const CString& strLayerName);
	void setLayerOpen(const CString& strLayerName);
};

