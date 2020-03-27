#include "stdafx.h"
#include "KVHelp.h"
#include "KV.h"
#include "ModulesManager.h"

#define INIT_KV_DLL INIT_KV(ModulesManager::Instance().loadModule("KV.dll"))

CString KVHelp::getStr(const CString& k, const CString& def /*= _T("")*/)
{
	INIT_KV_DLL;

	if (HasStr(k.GetString()))
		return GetStr(k.GetString());

	return def;
}

std::string KVHelp::getStrA(const std::string& k, const std::string& def /*= ""*/)
{
	INIT_KV_DLL;

	if (HasStrA(k.c_str()))
		return GetStrA(k.c_str());

	return def;
}

std::wstring KVHelp::getStrW(const std::wstring& k, const std::wstring& def /*= L""*/)
{
	INIT_KV_DLL;

	if (HasStrW(k.c_str()))
		return GetStrW(k.c_str());

	return def;
}

bool KVHelp::setStr(const CString& k, const CString& v)
{
	INIT_KV_DLL;
	return SetStr(k.GetString(), v.GetString());
}

bool KVHelp::setStrA(const std::string& k, const std::string& v)
{
	INIT_KV_DLL;
	return SetStrA(k.c_str(), v.c_str());
}

bool KVHelp::setStrW(const std::wstring& k, const std::wstring& v)
{
	INIT_KV_DLL;
	return SetStrW(k.c_str(), v.c_str());
}

void KVHelp::delStr(const CString& k)
{
	INIT_KV_DLL;
	return DelStr(k.GetString());
}

void KVHelp::delStrA(const std::string& k)
{
	INIT_KV_DLL;
	return DelStrA(k.c_str());
}

void KVHelp::delStrW(const std::wstring& k)
{
	INIT_KV_DLL;
	return DelStrW(k.c_str());
}

int KVHelp::getInt(const std::string& k, int def /*= 0*/)
{
	INIT_KV_DLL;

	if (HasInt(k.c_str()))
		return GetInt(k.c_str());

	return def;
}

bool KVHelp::setInt(const std::string& k, int v)
{
	INIT_KV_DLL;
	return SetInt(k.c_str(), v);
}

void KVHelp::delInt(const std::string& k)
{
	INIT_KV_DLL;
	return DelInt(k.c_str());
}

int KVHelp::getDouble(const std::string& k, double def /*= 0*/)
{
	INIT_KV_DLL;

	if (HasDouble(k.c_str()))
		return GetDouble(k.c_str());

	return def;
}

bool KVHelp::setDouble(const std::string& k, double v)
{
	INIT_KV_DLL;
	return SetDouble(k.c_str(), v);
}

void KVHelp::delDouble(const std::string& k)
{
	INIT_KV_DLL;
	return DelDouble(k.c_str());
}
