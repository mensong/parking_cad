#pragma once
#include "IOperaLog.h"
#include "DBHelper.h"

class COperaLivingWaterTankSet :
	public CIOperaLog
{
public:
	COperaLivingWaterTankSet(const AcString& group, const AcString& cmd, const AcString& alias, Adesk::Int32 cmdFlag);
	~COperaLivingWaterTankSet();
	virtual void Start();
	static CWnd* g_dlg;
	HideDialogHolder* m_holder;
	virtual void Ended() override;

};

