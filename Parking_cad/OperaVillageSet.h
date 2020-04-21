#pragma once
#include "IOperaLog.h"
#include "DBHelper.h"
class COperaVillageSet :
	public CIOperaLog
{
public:
	COperaVillageSet(const AcString& group, const AcString& cmd, const AcString& alias, Adesk::Int32 cmdFlag);
	~COperaVillageSet();
	virtual void Start();
	static CWnd* g_dlg;
	HideDialogHolder* m_holder;
	virtual void Ended() override;

};

