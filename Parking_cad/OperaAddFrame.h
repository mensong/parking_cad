#pragma once
#include "Opera.h"

class COperaAddFrame :
	public COpera
{
public:
	COperaAddFrame();
	~COperaAddFrame();
	virtual void Start();

public:
	//zhangzechi
	static AcGePoint2d GetChangePoint(AcGePoint2d& centerpt, AcGePoint2d& changpt);
};

