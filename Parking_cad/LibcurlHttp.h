#pragma once

#ifdef LIBCURLHTTP_EXPORTS
#define LIBCURLHTTP_API extern "C" __declspec(dllexport)
#else
#define LIBCURLHTTP_API extern "C" __declspec(dllimport)
#endif

class LibcurlHttp
{
public:
	//���ó�ʱʱ��
	virtual void setTimeout(int t) = 0;
	//��������ͷ
	virtual void setRequestHeader(const char* key, const char* value) = 0;
	//����UserAgent
	virtual void setUserAgent(const char* val) = 0;
	//���������ת��
	virtual void setMaxRedirectNum(int num) = 0;

	//http get
	virtual int get(const char* url, bool dealRedirect = true) = 0;
	//http get
	//����������const char* key1, const char* val1, const char* key2, const char* val2, ����, NULL
	//  ��������Ϊ�ɶԵ���ʽ���֣����Һ���һ����һ��NULL��־���������Ľ�����
	virtual int get_a(const char* url, bool dealRedirect, ...) = 0;
	virtual int get_b(const char* url, bool dealRedirect, va_list argv) = 0;

	//http post
	virtual int post(const char* url, const char* content, int contentLen, bool dealRedirect = true, const char* contentType = "application/x-www-form-urlencoded") = 0;
	//http post
	//����������const char* key1, const char* val1, const char* key2, const char* val2, ����, NULL
	//  ��������Ϊ�ɶԵ���ʽ���֣����Һ���һ����һ��NULL��־���������Ľ�����
	virtual int post_a(const char* url, bool dealRedirect, ...) = 0;
	virtual int post_b(const char* url, bool dealRedirect, va_list argv) = 0;

	//���������ļ�������
	virtual int download(const char* url, const char* localFileName=NULL) = 0;

	//�ύ��
	// url, fieldname1, fieldvalue1, fieldtype1(0:��ͨ�ֶΣ�1:file�ֶ�), fieldname2, fieldvalue2, fieldtype2, ����, NULL
	virtual int postForm_a(const char* url, ...) = 0;
	virtual int postForm_b(const char* url, va_list argv) = 0;

	//��õ�ǰurl
	virtual const char* getLocation() = 0;

	//����ύ���body����
	virtual const char* getBody(int& len) = 0;
	//����ύ��ķ���code
	virtual int getCode() = 0;

	//urlת��
	virtual const char* UrlGB2312Encode(const char * strIn) = 0;
	//url����
	virtual const char* UrlGB2312Decode(const char * strIn) = 0;

	//urlת��
	virtual const char* UrlUTF8Encode(const char * strIn) = 0;
	//url����
	virtual const char* UrlUTF8Decode(const char * strIn) = 0;
};

//����cookies�ļ�
LIBCURLHTTP_API void LoadCookies(const char* file);
//����cookies���ļ�
LIBCURLHTTP_API void SaveCookies(const char* file);

//�Զ���ķ�ʽ
LIBCURLHTTP_API LibcurlHttp* CreateHttp(void);
LIBCURLHTTP_API void ReleaseHttp(LibcurlHttp* p);

//�Ե����ķ�ʽ��ע�ͼ�LibcurlHttp
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
LIBCURLHTTP_API int getCode();

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
typedef int (*FN_getCode)();
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
	DEF_PROC(__hDll__, getCode); \
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
			this->getCode				=   getCode;
			this->download				=   download; 
			this->postForm				=   postForm; 
			this->UrlGB2312Encode		=   UrlGB2312Encode;
			this->UrlGB2312Decode		=   UrlGB2312Decode;
			this->UrlUTF8Encode			=   UrlUTF8Encode;
			this->UrlUTF8Decode			=   UrlUTF8Decode;
		}
		else
		{
			::MessageBoxA(NULL, "�Ҳ���LibcurlHttp.dllģ��", "�Ҳ���ģ��", MB_OK | MB_ICONERROR);
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
	FN_getCode				getCode;
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