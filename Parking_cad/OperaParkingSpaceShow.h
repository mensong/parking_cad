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

	bool setDesKey(const std::string& key);
	std::string decrypt(const std::string& code);
	std::string encrypt(const std::string& sData);
private:
	AcGeTol m_tol;
	std::string	 m_desKey;	
};

