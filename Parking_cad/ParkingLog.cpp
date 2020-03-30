#include "stdafx.h"
#include "ParkingLog.h"
#include "ModulesManager.h"
#include <json/json.h>
#include <Convertor.h>
#include "KVHelp.h"
#include "DBHelper.h"
#include <windows.h>
#include <shlwapi.h>

bool CParkingLog::AddLog(const CString& type, int error, const CString& descr,
	int trigger_count /*= 0*/, CString& user_udid/*=_T("")*/)
{
	if (user_udid.IsEmpty() || user_udid == _T(""))
	{
		user_udid = KVHelp::getStr(_T("bip_id"), _T("unknow user"));
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

	std::string add_logUrl;
	std::string sIni4u7h = DBHelper::GetArxDirA() + "4u7h.ini";
	if (::PathFileExistsA(sIni4u7h.c_str()))
	{
		char mm[1024];
		DWORD len = ::GetPrivateProfileStringA("URL", "add_log", "", mm, 1024, sIni4u7h.c_str());
		if (len < 1024 && len >0)
			add_logUrl = mm;
	}
	if (add_logUrl.empty())
	{
		acutPrintf(_T("\n找不到4u7h.ini配置文件!"));
		return false;
	}

	Json::Value js;
	js["user_udid"] = GL::WideByte2Utf8(user_udid.GetString());
	js["type"] = GL::WideByte2Utf8(type.GetString());
	js["error"] = error;
	js["descr"] = GL::WideByte2Utf8(descr.GetString());
	js["trigger_count"] = trigger_count;
	js["mac"] = KVHelp::getStrA("mac");

	Json::FastWriter jsWriter;
	std::string sJson = jsWriter.write(js);

	int code = post(add_logUrl.c_str(), sJson.c_str(), sJson.size(), true, "application/json");
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
