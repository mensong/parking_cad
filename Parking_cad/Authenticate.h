#pragma once
#include <map>
#include <string>

class Authenticate
{
public:
	typedef struct LICENSE_INFO
	{
		DWORD expireTime;
		std::string userName;
	} LICENSE_INFO;

public:
	Authenticate(void);
	~Authenticate(void);

	bool setDesKey(const std::string& key);
	void loadLicenseFile(const std::string& file);
	bool appendLicense(const std::string& code);
	int check(const std::string& userName="");
	
	std::string localEncode(DWORD expireTime, const std::string& userName);
	std::string localEncode(const std::string& expireTime, const std::string& userName);
	std::string encode(const std::string& serial, DWORD expireTime, const std::string& userName);
	std::string encode(const std::string& serial, const std::string& expireTime, const std::string& userName);

protected:
	static std::string ReadText(const char * path);

private:
	std::string m_desKey;
	std::map<std::string, LICENSE_INFO> m_licences;
};

