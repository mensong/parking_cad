#pragma once
#include "Opera.h"
#include "DlgResetEntityLayer.h"
class COperaResetEntityLayer :
	public COpera
{
public:
	COperaResetEntityLayer();
	~COperaResetEntityLayer();

	virtual void Start();

public:
	static CDlgResetEntityLayer* ms_pResetLayerDlg;
};

