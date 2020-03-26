#include "stdafx.h"
#include "ParkingLog.h"
#include "ModulesManager.h"
#include <json/json.h>
#include <Convertor.h>
#include "KV.h"

bool CParkingLog::AddLog(const CString& type, int error, const CString& descr,
	int trigger_count /*= 0*/, CString& user_udid/*=_T("")*/)
{
	HMODULE hDll = ModulesManager::Instance().loadModule("KV.dll");
	if (!hDll)
	{
		acutPrintf(_T("\n找不到KV.dll模块"));
		return false;
	}
	INIT_KV(hDll);
	
	if (user_udid.IsEmpty() || user_udid == _T(""))
	{
		if (HasStr(_T("bip_id")))
		{
			user_udid = GetStr(_T("bip_id"));
		}
	}

	typedef int(*FN_post)(const char* url, const char*, int, bool, const char*);
	FN_post post = ModulesManager::Instance().func<FN_post>("LibcurlHttp.dll", "post");
	typedef const char* (*FN_getBody)(int&);
	FN_getBody getBody = ModulesManager::Instance().func<FN_getBody>("LibcurlHttp.dll", "getBody");
	if (!post || !getBody)
	{
		acutPrintf(_T("\n找不到LibcurlHttp.dll模块"));
		return false;
	}

	Json::Value js;
	js["user_udid"] = GL::WideByte2Utf8(user_udid.GetString());
	js["type"] = GL::WideByte2Utf8(type.GetString());
	js["error"] = error;
	js["descr"] = GL::WideByte2Utf8(descr.GetString());
	js["trigger_count"] = trigger_count;
	js["mac"] = HasStrA("mac") ? GetStrA("mac") : "";

	Json::FastWriter jsWriter;
	std::string sJson = jsWriter.write(js);

	int code = post("http://parking.asdfqwer.net:9463/add_log", sJson.c_str(), sJson.size(), true, "application/json");
	if (code != 200)
		return false;

	int len = 0;
	std::string sBody = getBody(len);
	if (sBody == "ok")
		return true;

	acutPrintf(_T("\nLog faild!"));
	return false;
}

bool CParkingLog::AddLogA(const std::string& type, int error, const std::string& descr, 
	int trigger_count /*= 0*/, const std::string& user_udid /*= ""*/)
{
	return AddLog(GL::Ansi2WideByte(type.c_str()).c_str(), error, GL::Ansi2WideByte(descr.c_str()).c_str(),
		trigger_count, CString(const_cast<TCHAR*>(GL::Ansi2WideByte(user_udid.c_str()).c_str())));
}
