#pragma once
#include "Opera.h"
#include "DlgDimensionAdjustment.h"
class COperaDimensionAdjustment :
	public COpera
{
public:
	COperaDimensionAdjustment();
	~COperaDimensionAdjustment();

	virtual void Start();
public:
	static CDlgDimensionAdjustment* m_dimadjustDlg;
};

