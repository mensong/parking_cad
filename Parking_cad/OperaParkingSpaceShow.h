#pragma once
#include "Opera.h"
class COperaParkingSpaceShow :
	public COpera
{
public:
	COperaParkingSpaceShow();
	~COperaParkingSpaceShow();
	virtual void Start();

	static class CArxDialog* ms_dlg;

private:
	AcGeTol m_tol;
	/*CString m_outlinelayerName;
	CString m_shearwalllayerName;
	int directionComboshow;*/
	
};

