#pragma once

#ifdef KV_EXPORTS
#define KV_API extern "C" __declspec(dllexport)
#else
#define KV_API extern "C" __declspec(dllimport)
#endif


KV_API bool SetStrA(const char* k, const char* v);
KV_API bool SetStrW(const wchar_t* k, const wchar_t* v);
KV_API const char* GetStrA(const char* k);
KV_API const wchar_t* GetStrW(const wchar_t* k);
KV_API bool HasStrA(const char* k);
KV_API bool HasStrW(const wchar_t* k);
KV_API void DelStrA(const char* k);
KV_API void DelStrW(const wchar_t* k);

KV_API bool SetInt(const char* k, int v);
KV_API int GetInt(const char* k);
KV_API bool HasInt(const char* k);
KV_API void DelInt(const char* k);

KV_API bool SetDouble(const char* k, double v);
KV_API double GetDouble(const char* k);
KV_API bool HasDouble(const char* k);
KV_API void DelDouble(const char* k);

KV_API bool SetBuff(const char* k, const char* buff, int buffLen);
KV_API const char* GetBuff(const char* k, int& outLen);
KV_API bool HasBuff(const char* k);
KV_API void DelBuff(const char* k);


#define INIT_KV(hDll) \
	typedef bool(*FN_SetStrA)(const char* k, const char* v); \
	typedef bool(*FN_SetStrW)(const wchar_t* k, const wchar_t* v); \
	typedef const char* (*FN_GetStrA)(const char* k); \
	typedef const wchar_t* (*FN_GetStrW)(const wchar_t* k); \
	typedef bool(*FN_HasStrA)(const char* k); \
	typedef bool(*FN_HasStrW)(const wchar_t* k); \
	typedef void(*FN_DelStrA)(const char* k); \
	typedef void(*FN_DelStrW)(const wchar_t* k); \
	 \
	typedef bool(*FN_SetInt)(const char* k, int v); \
	typedef int(*FN_GetInt)(const char* k); \
	typedef bool(*FN_HasInt)(const char* k); \
	typedef void(*FN_DelInt)(const char* k); \
	 \
	typedef bool(*FN_SetDouble)(const char* k, double v); \
	typedef double(*FN_GetDouble)(const char* k); \
	typedef bool(*FN_HasDouble)(const char* k); \
	typedef void(*FN_DelDouble)(const char* k); \
	 \
	typedef bool(*FN_SetBuff)(const char* k, const char* buff, int buffLen); \
	typedef const char* (*FN_GetBuff)(const char* k, int& outLen); \
	typedef bool(*FN_HasBuff)(const char* k); \
	typedef void(*FN_DelBuff)(const char* k); \
	 \
	FN_SetStrA SetStrA = NULL; \
	FN_SetStrW SetStrW = NULL; \
	FN_GetStrA GetStrA = NULL; \
	FN_GetStrW GetStrW = NULL; \
	FN_HasStrA HasStrA = NULL; \
	FN_HasStrW HasStrW = NULL; \
	FN_DelStrA DelStrA = NULL; \
	FN_DelStrW DelStrW = NULL; \
	 \
	FN_SetInt SetInt = NULL; \
	FN_GetInt GetInt = NULL; \
	FN_HasInt HasInt = NULL; \
	FN_DelInt DelInt = NULL; \
	 \
	FN_SetDouble SetDouble = NULL; \
	FN_GetDouble GetDouble = NULL; \
	FN_HasDouble HasDouble = NULL; \
	FN_DelDouble DelDouble = NULL; \
	 \
	FN_SetBuff SetBuff = NULL; \
	FN_GetBuff GetBuff = NULL; \
	FN_HasBuff HasBuff = NULL; \
	FN_DelBuff DelBuff = NULL; \
	 \
	SetStrA = (FN_SetStrA)GetProcAddress(hDll, "SetStrA"); \
	SetStrW = (FN_SetStrW)GetProcAddress(hDll, "SetStrW"); \
	GetStrA = (FN_GetStrA)GetProcAddress(hDll, "GetStrA"); \
	GetStrW = (FN_GetStrW)GetProcAddress(hDll, "GetStrW"); \
	HasStrA = (FN_HasStrA)GetProcAddress(hDll, "HasStrA"); \
	HasStrW = (FN_HasStrW)GetProcAddress(hDll, "HasStrW"); \
	DelStrA = (FN_DelStrA)GetProcAddress(hDll, "DelStrA"); \
	DelStrW = (FN_DelStrW)GetProcAddress(hDll, "DelStrW"); \
	\
	SetInt = (FN_SetInt)GetProcAddress(hDll, "SetInt");	\
	GetInt = (FN_GetInt)GetProcAddress(hDll, "GetInt");	\
	HasInt = (FN_HasInt)GetProcAddress(hDll, "HasInt");	\
	DelInt = (FN_DelInt)GetProcAddress(hDll, "DelInt");	\
	\
	SetDouble = (FN_SetDouble)GetProcAddress(hDll, "SetDouble"); \
	GetDouble = (FN_GetDouble)GetProcAddress(hDll, "GetDouble"); \
	HasDouble = (FN_HasDouble)GetProcAddress(hDll, "HasDouble"); \
	DelDouble = (FN_DelDouble)GetProcAddress(hDll, "DelDouble"); \
	\
	SetBuff = (FN_SetBuff)GetProcAddress(hDll, "SetBuff"); \
	GetBuff = (FN_GetBuff)GetProcAddress(hDll, "GetBuff"); \
	HasBuff = (FN_HasBuff)GetProcAddress(hDll, "HasBuff"); \
	DelBuff = (FN_DelBuff)GetProcAddress(hDll, "DelBuff"); \
	if (!SetStrA || !SetStrW || !GetStrA || !GetStrW || \
	!HasStrA || !HasStrW || !DelStrA || !DelStrW || \
	!SetInt || !GetInt || !HasInt || !DelInt || \
	!SetDouble || !GetDouble || !HasDouble || !DelDouble || \
	!SetBuff || !GetBuff || !HasBuff || !DelBuff) { \
		throw std::runtime_error("KV.dll��ʧ"); \
	}

#ifdef UNICODE
#define SetStr SetStrW
#define GetStr GetStrW
#define HasStr HasStrW
#define DelStr DelStrW
#else
#define SetStr SetStrA
#define GetStr GetStrA
#define HasStr HasStrA
#define DelStr DelStrA
#endif