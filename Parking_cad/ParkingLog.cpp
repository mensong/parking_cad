#include "stdafx.h"
#include "ParkingLog.h"
#include "ModulesManager.h"
#include <json/json.h>
#include <Convertor.h>
#include "LibcurlHttp.h"
#include "KV.h"

bool CParkingLog::AddLog(const CString& type, int error, const CString& descr,
	int trigger_count /*= 0*/, CString& user_udid/*=_T("")*/)
{
	std::string add_log_url = KV::Ins().GetStrA("add_log_url", "");
	if (add_log_url.empty())
		return false;

	if (user_udid.IsEmpty() || user_udid == _T(""))
	{
		user_udid = KV::Ins().GetStrW(_T("bip_id"), _T("unknow user"));
	}

	Json::Value js;
	js["user_udid"] = GL::WideByte2Utf8(user_udid.GetString());
	js["type"] = GL::WideByte2Utf8(type.GetString());
	js["error"] = error;
	js["descr"] = GL::WideByte2Utf8(descr.GetString());
	js["trigger_count"] = trigger_count;
	js["mac"] = KV::Ins().GetStrA("mac", "");

	Json::FastWriter jsWriter;
	std::string sJson = jsWriter.write(js);

	int code = HTTP_CLIENT::Ins().post(add_log_url.c_str(), sJson.c_str(), sJson.size(), true, "application/json");
	if (code != 200)
		return false;

	int len = 0;
	std::string sBody = HTTP_CLIENT::Ins().getBody(len);
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
