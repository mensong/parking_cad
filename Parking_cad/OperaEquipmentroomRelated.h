#pragma once
#include "IOperaLog.h"
#include "DBHelper.h"


class COperaEquipmentroomRelated :
	public CIOperaLog
{
public:
	COperaEquipmentroomRelated(const AcString& group, const AcString& cmd, const AcString& alias, Adesk::Int32 cmdFlag);
	~COperaEquipmentroomRelated();
	virtual void Start();
	
	static CWnd* g_dlg;
	HideDialogHolder* m_holder;

	virtual void Ended() override;

};

