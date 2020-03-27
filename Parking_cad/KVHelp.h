#pragma once

class KVHelp
{
public:
	static CString getStr(const CString& k, const CString& def = _T(""));
	static std::string getStrA(const std::string& k, const std::string& def = "");
	static std::wstring getStrW(const std::wstring& k, const std::wstring& def = L"");
	static bool setStr(const CString& k, const CString& v);
	static bool setStrA(const std::string& k, const std::string& v);
	static bool setStrW(const std::wstring& k, const std::wstring& v);
	static void delStr(const CString& k);
	static void delStrA(const std::string& k);
	static void delStrW(const std::wstring& k);
	
	static int getInt(const std::string& k, int def = 0);
	static bool setInt(const std::string& k, int v);
	static void delInt(const std::string& k);
	
	static int getDouble(const std::string& k, double def = 0);
	static bool setDouble(const std::string& k, double v);
	static void delDouble(const std::string& k);

};

