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



#ifndef LIBBIPLOGIN_EXPORTS
//以对象的方式
typedef LibBipSignIn* (*FN_CreateSingnIn)(void);
typedef void(*FN_ReleaseSingnIn)(LibBipSignIn* p);
typedef int(*FN_init)(const char* configFile);
typedef int(*FN_login)(const char* user, const char* password, const char** errMsg, const char* grantType);
typedef int(*FN_getUserInfo)(UserInfo& ui);
typedef void(*FN_uninit)();
typedef void(*FN_WriteConfigFile)(
	const char* loginUrl, const char* getUserInfoUrl,
	const char* clientId, const char* clientSecret,
	const char* aesKey, const char* aesChain,
	const char* configFile, const char* iniEncryptKey, const char* iniEncryptChain);


#define DEF_PROC(hDll, name) \
	FN_##name name = (FN_##name)::GetProcAddress(hDll, #name)

#define INIT_LIBBIPLOGIN(hDll) \
	HMODULE __hDll__ = (hDll); \
	DEF_PROC(__hDll__, CreateSingnIn); \
	DEF_PROC(__hDll__, ReleaseSingnIn); \
	DEF_PROC(__hDll__, init); \
	DEF_PROC(__hDll__, login); \
	DEF_PROC(__hDll__, getUserInfo); \
	DEF_PROC(__hDll__, uninit); \
	DEF_PROC(__hDll__, WriteConfigFile);

class BIP_SIGNIN
{
private:
	static BIP_SIGNIN* s_ins;

public:
	static BIP_SIGNIN& Ins()
	{
		if (!s_ins)
			s_ins = new BIP_SIGNIN;
		return *s_ins;
	}

	static void Release()
	{
		if (s_ins)
		{
			delete s_ins;
			s_ins = NULL;
		}
	}

	BIP_SIGNIN()
	{
		hDll = LoadLibraryA("lib_bipsignin.dll");
		if (hDll)
		{
			INIT_LIBBIPLOGIN(hDll);
			this->CreateSingnIn		= CreateSingnIn;
			this->ReleaseSingnIn	= ReleaseSingnIn;
			this->init				= init;
			this->login				= login;
			this->getUserInfo		= getUserInfo;
			this->uninit			= uninit;
			this->WriteConfigFile	= WriteConfigFile;
		}
		else
		{
			::MessageBoxA(NULL, "找不到lib_bipsignin.dll模块", "找不到模块", MB_OK | MB_ICONERROR);
		}
	}
	~BIP_SIGNIN()
	{
		if (hDll)
		{
			FreeLibrary(hDll);
			hDll = NULL;
		}
	}

	FN_CreateSingnIn	 CreateSingnIn;
	FN_ReleaseSingnIn	 ReleaseSingnIn;
	FN_init				 init;
	FN_login			 login;
	FN_getUserInfo		 getUserInfo;
	FN_uninit			 uninit;
	FN_WriteConfigFile	 WriteConfigFile;

	HMODULE hDll;
};
__declspec(selectany) BIP_SIGNIN* BIP_SIGNIN::s_ins = NULL;
#endif