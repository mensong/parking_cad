#pragma once

class CParkingLog
{
public:
	static void Init();

public:
	static bool AddLog(const CString& type, int error, const CString& descr,
		int trigger_count = 1, CString& user_udid=CString());

	static bool AddLogA(const std::string& type, int error, const std::string& descr,
		int trigger_count = 1, const std::string& user_udid = "");

protected:
	static void _addLogHandle(const char* name, void* data);
};

