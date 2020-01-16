#pragma once

#ifdef LIBBIPLOGIN_EXPORTS
#define LIBBIPLOGIN_API extern "C" __declspec(dllexport)
#else
#define LIBBIPLOGIN_API extern "C" __declspec(dllimport)
#endif

struct UserInfo
{
	const char* uid;
	const char* empNumber;
	const char* displayName;	
};

class LibBipSignIn
{
public:
	virtual int init(const char* configFile, const char* iniEncryptKey = NULL, const char* iniEncryptChain = NULL) = 0;
	virtual int login(const char* user, const char* password, const char** errMsg = NULL, const char* grantType = "password") = 0;
	virtual int getUserInfo(UserInfo& ui) = 0;
	virtual void uninit() = 0;
};

//以对象的方式
LIBBIPLOGIN_API LibBipSignIn* CreateSingnIn(void);
LIBBIPLOGIN_API void ReleaseSingnIn(LibBipSignIn* p);

//以单例的方式
LIBBIPLOGIN_API int init(const char* configFile);
LIBBIPLOGIN_API int login(const char* user, const char* password, const char** errMsg = NULL, const char* grantType = "password");
LIBBIPLOGIN_API int getUserInfo(UserInfo& ui);
LIBBIPLOGIN_API void uninit();

//保存配置文件
LIBBIPLOGIN_API void WriteConfigFile(
	const char* loginUrl, const char* getUserInfoUrl, 
	const char* clientId, const char* clientSecret,
	const char* aesKey, const char* aesChain,
	const char* configFile, const char* iniEncryptKey = NULL, const char* iniEncryptChain = NULL);