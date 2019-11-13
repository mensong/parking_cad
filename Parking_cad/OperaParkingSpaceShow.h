#pragma once
#include "Opera.h"
class COperaParkingSpaceShow :
	public COpera
{
public:
	COperaParkingSpaceShow();
	~COperaParkingSpaceShow();
	virtual void Start();

private:
	AcGeTol m_tol;
	/*CString m_outlinelayerName;
	CString m_shearwalllayerName;
	int directionComboshow;*/
	static class CArxDialog* ms_dlg;
};

