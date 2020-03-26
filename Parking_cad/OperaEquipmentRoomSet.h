#pragma once
#include "IOperaLog.h"
class COperaEquipmentRoomSet :
	public CIOperaLog
{
public:
	COperaEquipmentRoomSet(const AcString& group, const AcString& cmd, const AcString& alias, Adesk::Int32 cmdFlag);
	~COperaEquipmentRoomSet();
	virtual void Start();
	static class CDlgEquipmentRoomSet* ms_eqdlg;
private:
	AcGeTol m_tol;
};

