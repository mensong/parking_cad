#include "stdafx.h"
#include "Authenticate.h"
#include "HardDiskSerial.h"
#include "..\base64\base64.h"
#include "..\DES_CBC_5\DesHelper.h"
#include "..\pystring\pystring.h"
#include "../DES_CBC_5/utils.h"
#include <time.h>


Authenticate::Authenticate(void)
	: m_checkedExpireTime(0)
{
}


Authenticate::~Authenticate(void)
{
}

bool Authenticate::setDesKey(const std::string& key)
{
	if (m_desKey.size() > 8)
		return false;
	m_desKey = key;
	return true;
}

void Authenticate::loadLicenseFile(const std::string& file)
{
	std::string sLicenseText = Authenticate::ReadText(file.c_str());
	std::vector<std::string> licenseTexts;
	pystring::splitlines(sLicenseText, licenseTexts);
	for (int i = 0; i < licenseTexts.size(); ++i)
	{
		std::string license = pystring::replace(licenseTexts[i], " ", "");
		if (!license.empty())
			appendLicense(license);
	}
}

bool Authenticate::appendLicense(const std::string& code)
{
	char sDeBase64[1024];
	int nDeBase64Len = 0;
	base64_decode(code.c_str(), code.size(), (unsigned char*)sDeBase64, nDeBase64Len);
	char sDeDes[1024];
	int nDeDesLen = 0;
	GL::DES_cbc_decrypt(sDeBase64, nDeBase64Len, sDeDes, nDeDesLen,
		m_desKey.c_str(), m_desKey.size(), m_desKey.c_str(), m_desKey.size());
	sDeDes[nDeDesLen] = '\0';

	std::vector<std::string> splits;
	pystring::split(sDeDes, splits, "|");
	if (splits.size() == 0)
		return false;

	if (splits.size() == 1)
	{
		LICENSE_INFO li;
		li.licenceCode = code;
		li.expireTime = 0;
		m_licences.insert(std::make_pair(splits[0].c_str(), li));
		return true;
	}
	else if (splits.size() == 2)
	{
		LICENSE_INFO li;
		li.licenceCode = code;
		sscanf(splits[1].c_str(), "%u", &li.expireTime);
		m_licences.insert(std::make_pair(splits[0].c_str(), li));
		return true;
	}
	else if (splits.size() == 3)
	{
		LICENSE_INFO li;
		li.licenceCode = code;
		sscanf(splits[1].c_str(), "%u", &li.expireTime);
		li.userName = splits[2].c_str();
		m_licences.insert(std::make_pair(splits[0].c_str(), li));
		return true;
	}

	return false;
}

int Authenticate::check(const std::string& userName/*=""*/)
{
	m_checkedSerial.clear();
	m_checkedUser.clear();
	m_checkedLicenceCode.clear();
	m_checkedExpireTime = 0;

	char serialNum[MAX_PATH];
	ULONG len = HardDiskSerial::GetSerial(serialNum, MAX_PATH, 0);
	if (len == 0)
		return 1;

	std::map<std::string, LICENSE_INFO>::iterator itFinder = m_licences.find(serialNum);
	if (itFinder == m_licences.end())
		return 2;

	time_t timep;
	struct tm *p;
	time(&timep);
	p = gmtime(&timep);
	char szBuf[64];
	sprintf(szBuf, "%d%02d%02d", 1900 + p->tm_year, 1 + p->tm_mon, p->tm_mday);
	DWORD nowTime;
	sscanf(szBuf, "%u", &nowTime);
	if (itFinder->second.expireTime != 0 && nowTime > itFinder->second.expireTime)
		return 3;

	if (!userName.empty() && itFinder->second.userName != userName)
		return 4;

	m_checkedSerial = serialNum;
	m_checkedUser = itFinder->second.userName.c_str();
	m_checkedLicenceCode = itFinder->second.licenceCode;
	m_checkedExpireTime = itFinder->second.expireTime;

	return 0;
}

const std::string& Authenticate::getCheckedLicenceCode()
{
	return m_checkedLicenceCode;
}

const std::string& Authenticate::getCheckedSerial()
{
	return m_checkedSerial;
}

const std::string& Authenticate::getCheckedUser()
{
	return m_checkedUser;
}

DWORD Authenticate::getCheckedExpireTime()
{
	return m_checkedExpireTime;
}

std::string Authenticate::localEncode(DWORD expireTime, const std::string& userName)
{
	char szBuff[20];
	sprintf(szBuff, "%u", expireTime);
	return localEncode(szBuff, userName);
}

std::string Authenticate::localEncode(const std::string& expireTime, const std::string& userName)
{
	char szBuff[0x100];
	ULONG ulLen = HardDiskSerial::GetSerial(szBuff, sizeof(szBuff), 0);
	if (ulLen > 0)
	{
		return encode(szBuff, expireTime, userName);
	}

	return "";
}

std::string Authenticate::encode(const std::string& serial, DWORD expireTime, const std::string& userName)
{
	char szBuff[20];
	sprintf(szBuff, "%u", expireTime);
	return encode(serial, szBuff, userName);
}

std::string Authenticate::encode(const std::string& serial, const std::string& expireTime, const std::string& userName)
{
	std::string sData = serial.c_str();

	if (!expireTime.empty() || !userName.empty())
	{
		sData += "|";
		sData += expireTime.empty() ? "0" : expireTime.c_str();
	}

	if (!userName.empty())
	{
		sData += "|";
		sData += userName.c_str();
	}

	char mmCode[1024];
	int nOutLen = 0;
	GL::DES_cbc_encrypt(sData.c_str(), sData.size(), mmCode, nOutLen, m_desKey.c_str(), m_desKey.size(), m_desKey.c_str(), m_desKey.size());
	char szBase64[1024 * 2];
	int nBase64 = 0;
	base64_encode((const unsigned char*)mmCode, nOutLen, szBase64, nBase64);
	szBase64[nBase64] = '\0';

	return szBase64;
}

std::string Authenticate::ReadText(const char * path)
{
	FILE *f = NULL;
	long sz;

	if (!path)
	{
		return "";
	}

	std::string sRet;

	f = fopen(path, "rb");
	if (!f)
	{
		return "";
	}

	do
	{
		if (fseek(f, 0, SEEK_END) < 0)
		{
			break;
		}

		sz = ftell(f);
		if (sz < 0)
		{
			break;
		}

		if (fseek(f, 0, SEEK_SET) < 0)
		{
			break;
		}

		sRet.resize((size_t)sz + 1, '\0');

		if ((size_t)fread(const_cast<char*>(sRet.c_str()), 1, (size_t)sz, f) != (size_t)sz)
		{
			sRet = "";
			break;
		}
	} while (0);

	fclose(f);

	return sRet;
}
