#pragma once
#include "IOperaLog.h"
#include "DBHelper.h"
class COperaFirePoolSet :
	public CIOperaLog
{
public:
	COperaFirePoolSet(const AcString& group, const AcString& cmd, const AcString& alias, Adesk::Int32 cmdFlag);
	~COperaFirePoolSet();
	
	virtual void Start();
	static CWnd* g_dlg;
	HideDialogHolder* m_holder;

	virtual void Ended() override;

};

