#pragma once

#ifdef LIBCURLHTTP_EXPORTS
#define LIBCURLHTTP_API extern "C" __declspec(dllexport)
#else
#define LIBCURLHTTP_API extern "C" __declspec(dllimport)
#endif

class LibcurlHttp
{
public:
	//设置超时时长
	virtual void setTimeout(int t) = 0;
	//设置请求头
	virtual void setRequestHeader(const char* key, const char* value) = 0;
	//设置UserAgent
	virtual void setUserAgent(const char* val) = 0;
	//设置最大跳转数
	virtual void setMaxRedirectNum(int num) = 0;

	//http get
	virtual int get(const char* url, bool dealRedirect = true) = 0;
	//http get
	//不定参数：const char* key1, const char* val1, const char* key2, const char* val2, ……, NULL
	//  不定参数为成对的形式出现，并且后面一定有一个NULL标志不定参数的结束。
	virtual int get_a(const char* url, bool dealRedirect, ...) = 0;
	virtual int get_b(const char* url, bool dealRedirect, va_list argv) = 0;

	//http post
	virtual int post(const char* url, const char* content, int contentLen, bool dealRedirect = true, const char* contentType = "application/x-www-form-urlencoded") = 0;
	//http post
	//不定参数：const char* key1, const char* val1, const char* key2, const char* val2, ……, NULL
	//  不定参数为成对的形式出现，并且后面一定有一个NULL标志不定参数的结束。
	virtual int post_a(const char* url, bool dealRedirect, ...) = 0;
	virtual int post_b(const char* url, bool dealRedirect, va_list argv) = 0;

	//下载网络文件到本地
	virtual int download(const char* url, const char* localFileName=NULL) = 0;

	//提交表单
	// url, fieldname1, fieldvalue1, fieldtype1(0:普通字段；1:file字段), fieldname2, fieldvalue2, fieldtype2, ……, NULL
	virtual int postForm_a(const char* url, ...) = 0;
	virtual int postForm_b(const char* url, va_list argv) = 0;

	//获得当前url
	virtual const char* getLocation() = 0;

	//获得提交后的body内容
	virtual const char* getBody(int& len) = 0;
	//获得提交后的返回code
	virtual int getCode() = 0;

	//url转码
	virtual const char* UrlGB2312Encode(const char * strIn) = 0;
	//url解码
	virtual const char* UrlGB2312Decode(const char * strIn) = 0;

	//url转码
	virtual const char* UrlUTF8Encode(const char * strIn) = 0;
	//url解码
	virtual const char* UrlUTF8Decode(const char * strIn) = 0;
};

//加载cookies文件
LIBCURLHTTP_API void LoadCookies(const char* file);
//保存cookies到文件
LIBCURLHTTP_API void SaveCookies(const char* file);

//以对象的方式
LIBCURLHTTP_API LibcurlHttp* CreateHttp(void);
LIBCURLHTTP_API void ReleaseHttp(LibcurlHttp* p);

//以单例的方式，注释见LibcurlHttp
LIBCURLHTTP_API void setTimeout(int timeout);
LIBCURLHTTP_API void setRequestHeader(const char* key, const char* value);
LIBCURLHTTP_API void setUserAgent(const char* val);

LIBCURLHTTP_API void setMaxRedirectNum(int num);
LIBCURLHTTP_API int get(const char* url, bool dealRedirect = true);
LIBCURLHTTP_API int get_a(const char* url, bool dealRedirect, ...);
LIBCURLHTTP_API int post(const char* url, const char* content, int contentLen, bool dealRedirect = true, const char* contentType = "application/x-www-form-urlencoded");
LIBCURLHTTP_API int post_a(const char* url, bool dealRedirect, ...);
LIBCURLHTTP_API const char* getLocation();
LIBCURLHTTP_API const char* getBody(int& len);

LIBCURLHTTP_API int download(const char* url, const char* localFileName=NULL);

LIBCURLHTTP_API int postForm(const char* url, ...);

LIBCURLHTTP_API const char* UrlGB2312Encode(const char * strIn);
LIBCURLHTTP_API const char* UrlGB2312Decode(const char * strIn);
LIBCURLHTTP_API const char* UrlUTF8Encode(const char * strIn);
LIBCURLHTTP_API const char* UrlUTF8Decode(const char * strIn);


#ifndef LIBCURLHTTP_EXPORTS
typedef void(*FN_LoadCookies)(const char* file);
typedef void(*FN_SaveCookies)(const char* file);
typedef LibcurlHttp* (*FN_CreateHttp)(void);
typedef void(*FN_ReleaseHttp)(LibcurlHttp* p);
typedef void(*FN_setTimeout)(int timeout);
typedef void(*FN_setRequestHeader)(const char* key, const char* value);
typedef void(*FN_setUserAgent)(const char* val);
typedef void(*FN_setMaxRedirectNum)(int num);
typedef int(*FN_get)(const char* url, bool dealRedirect);
typedef int(*FN_get_a)(const char* url, bool dealRedirect, ...);
typedef int(*FN_post)(const char* url, const char* content, int contentLen, bool dealRedirect, const char* contentType);
typedef int(*FN_post_a)(const char* url, bool dealRedirect, ...);
typedef const char* (*FN_getLocation)();
typedef const char* (*FN_getBody)(int& len);
typedef int(*FN_download)(const char* url, const char* localFileName);
typedef int(*FN_postForm)(const char* url, ...);
typedef const char* (*FN_UrlGB2312Encode)(const char * strIn);
typedef const char* (*FN_UrlGB2312Decode)(const char * strIn);
typedef const char* (*FN_UrlUTF8Encode)(const char * strIn);
typedef const char* (*FN_UrlUTF8Decode)(const char * strIn);


#define DEF_PROC(hDll, name) \
	FN_##name name = (FN_##name)::GetProcAddress(hDll, #name)

#define INIT_LIBCURLHTTP(hDll) \
	HMODULE __hDll__ = (hDll); \
	DEF_PROC(__hDll__, LoadCookies); \
	DEF_PROC(__hDll__, SaveCookies); \
	DEF_PROC(__hDll__, CreateHttp); \
	DEF_PROC(__hDll__, ReleaseHttp); \
	DEF_PROC(__hDll__, setTimeout); \
	DEF_PROC(__hDll__, setRequestHeader); \
	DEF_PROC(__hDll__, setUserAgent); \
	DEF_PROC(__hDll__, setMaxRedirectNum); \
	DEF_PROC(__hDll__, get); \
	DEF_PROC(__hDll__, get_a); \
	DEF_PROC(__hDll__, post); \
	DEF_PROC(__hDll__, post_a); \
	DEF_PROC(__hDll__, getLocation); \
	DEF_PROC(__hDll__, getBody); \
	DEF_PROC(__hDll__, download); \
	DEF_PROC(__hDll__, postForm); \
	DEF_PROC(__hDll__, UrlGB2312Encode); \
	DEF_PROC(__hDll__, UrlGB2312Decode); \
	DEF_PROC(__hDll__, UrlUTF8Encode); \
	DEF_PROC(__hDll__, UrlUTF8Decode);

class HTTP_CLIENT
{
public:
private:
	static HTTP_CLIENT* s_ins;

public:
	static HTTP_CLIENT& Ins()
	{
		if (!s_ins)
			s_ins = new HTTP_CLIENT;
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

	HTTP_CLIENT()
	{
		hDll = LoadLibraryA("LibcurlHttp.dll");
		if (hDll)
		{
			INIT_LIBCURLHTTP(hDll);
			this->LoadCookies			=   LoadCookies;
			this->SaveCookies			=   SaveCookies;
			this->CreateHttp			=   CreateHttp;
			this->ReleaseHttp			=   ReleaseHttp;
			this->setTimeout			=   setTimeout;
			this->setRequestHeader		=   setRequestHeader;
			this->setUserAgent			=   setUserAgent;
			this->setMaxRedirectNum		=   setMaxRedirectNum;
			this->get					=   get;
			this->get_a					=   get_a;
			this->post					=   post;
			this->post_a				=   post_a;
			this->getLocation			=   getLocation; 
			this->getBody				=   getBody; 
			this->download				=   download; 
			this->postForm				=   postForm; 
			this->UrlGB2312Encode		=   UrlGB2312Encode;
			this->UrlGB2312Decode		=   UrlGB2312Decode;
			this->UrlUTF8Encode			=   UrlUTF8Encode;
			this->UrlUTF8Decode			=   UrlUTF8Decode;
		}
		else
		{
			::MessageBoxA(NULL, "找不到LibcurlHttp.dll模块", "找不到模块", MB_OK | MB_ICONERROR);
		}
	}
	~HTTP_CLIENT()
	{
		if (hDll)
		{
			FreeLibrary(hDll);
			hDll = NULL;
		}
	}

	FN_LoadCookies			LoadCookies;
	FN_SaveCookies			SaveCookies;
	FN_CreateHttp			CreateHttp;
	FN_ReleaseHttp			ReleaseHttp;
	FN_setTimeout			setTimeout;
	FN_setRequestHeader		setRequestHeader;
	FN_setUserAgent			setUserAgent;
	FN_setMaxRedirectNum	setMaxRedirectNum;
	FN_get					get;
	FN_get_a				get_a;
	FN_post					post;
	FN_post_a				post_a;
	FN_getLocation			getLocation;
	FN_getBody				getBody;
	FN_download				download;
	FN_postForm				postForm;
	FN_UrlGB2312Encode		UrlGB2312Encode;
	FN_UrlGB2312Decode		UrlGB2312Decode;
	FN_UrlUTF8Encode		UrlUTF8Encode;
	FN_UrlUTF8Decode		UrlUTF8Decode;

	HMODULE hDll;
};
__declspec(selectany) HTTP_CLIENT* HTTP_CLIENT::s_ins = NULL;

#endif