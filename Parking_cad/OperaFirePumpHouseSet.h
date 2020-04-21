#pragma once
#include "IOperaLog.h"
#include "DBHelper.h"
class COperaFirePumpHouseSet :
	public CIOperaLog
{
public:
	COperaFirePumpHouseSet(const AcString& group, const AcString& cmd, const AcString& alias, Adesk::Int32 cmdFlag);
	~COperaFirePumpHouseSet();
	virtual void Start();
	static CWnd* g_dlg;
	HideDialogHolder* m_holder;
	virtual void Ended() override;

};

