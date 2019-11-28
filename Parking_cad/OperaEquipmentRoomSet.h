#pragma once
#include "Opera.h"
class COperaEquipmentRoomSet :
	public COpera
{
public:
	COperaEquipmentRoomSet();
	~COperaEquipmentRoomSet();
	virtual void Start();
	static class CDlgEquipmentRoomSet* ms_eqdlg;
private:
	AcGeTol m_tol;
};

