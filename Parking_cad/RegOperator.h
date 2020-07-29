#ifndef REGOP_H
#define REGOP_H

#include <windows.h>
#include <assert.h>
#include <vector>

#define MAX_KEY_LENGTH 255
#define MAX_VALUE_NAME 16383

class CRegOperator
{
private:
	HKEY m_hKey;													/* 保存当前操作根键的句柄						*/

public:
	BOOL IsValid();													/* 是否有效										*/

	CRegOperator(HKEY hKey = NULL);									/* 构造函数，默认参数为：HKEY_LOCAL_MACHINE		*/
	CRegOperator(const char* strKey);								/* 设置当前操作的根键							*/

	CRegOperator OpenKey(const char* lpSubKey, bool bX64 = false);	/* 读写的形式打开注册表							*/
	void Close();													/* 关闭键句柄									*/

	CRegOperator CreateKey(const char* lpSubKey, bool bX64 = false);/* 创建的形式打开注册表							*/
	BOOL DeleteKey(const char* lpSubKey, bool bX64 = false);		/* 删除相应的子键（子健下必须为空）				*/
	BOOL DeleteTree(const char* lpSubKey);							/* 删除相应的子键								*/
	BOOL DeleteValue(const char* lpValueName);						/* 删除子键处的相应的键值						*/
	
	BOOL SaveKey(const char* lpFileName);							/* 把当前键值保存到指定文件						*/
	BOOL RestoreKey(const char* lpFileName);						/* 从指定注册表文件中恢复						*/

	BOOL ReadValue(const char* lpValueName, std::string& outVal);	/* 读出REG_SZ类型的值							*/
	BOOL ReadValue(const char* lpValueName, DWORD& dwVal);			/* 读出DWORD类型的值							*/
	
	BOOL WriteValue(const char* lpSubKey, const char* lpVal);		/* 写入REG_SZ类型值								*/
	BOOL WriteValue(const char* lpSubKey, DWORD dwVal);				/* 写入DWORD类型值								*/
	BOOL WriteValueEX(const char* lpSubKey, const char* lpVal);	    /* 写入REG_EXPAND_SZ类型值						*/

	BOOL EnumSub(
		OUT std::vector<std::string>* subTreeNames = NULL,
		OUT std::vector<std::string>* subValueNames = NULL);		/* 枚举											*/

	~CRegOperator();

	CRegOperator& operator=(CRegOperator& o);
	CRegOperator(CRegOperator& o);

protected:
	void _deRef();

private:
	int* m_pRefCount;
};

#endif