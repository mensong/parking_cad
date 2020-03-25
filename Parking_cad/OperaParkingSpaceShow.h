#pragma once
#include "IOperaLog.h"

class COperaParkingSpaceShow :
	public CIOperaLog
{
public:
	COperaParkingSpaceShow(const AcString& group, const AcString& cmd, const AcString& alias, Adesk::Int32 cmdFlag);
	~COperaParkingSpaceShow();
	virtual void Start();

	static class CArxDialog* ms_dlg;

	bool setDesKey(const std::string& key);
	std::string decrypt(const std::string& code);
	std::string encrypt(const std::string& sData);
private:
	AcGeTol m_tol;
	std::string	 m_desKey;	
};

