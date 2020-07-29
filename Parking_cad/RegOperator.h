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
	HKEY m_hKey;													/* ���浱ǰ���������ľ��						*/

public:
	BOOL IsValid();													/* �Ƿ���Ч										*/

	CRegOperator(HKEY hKey = NULL);									/* ���캯����Ĭ�ϲ���Ϊ��HKEY_LOCAL_MACHINE		*/
	CRegOperator(const char* strKey);								/* ���õ�ǰ�����ĸ���							*/

	CRegOperator OpenKey(const char* lpSubKey, bool bX64 = false);	/* ��д����ʽ��ע���							*/
	void Close();													/* �رռ����									*/

	CRegOperator CreateKey(const char* lpSubKey, bool bX64 = false);/* ��������ʽ��ע���							*/
	BOOL DeleteKey(const char* lpSubKey, bool bX64 = false);		/* ɾ����Ӧ���Ӽ����ӽ��±���Ϊ�գ�				*/
	BOOL DeleteTree(const char* lpSubKey);							/* ɾ����Ӧ���Ӽ�								*/
	BOOL DeleteValue(const char* lpValueName);						/* ɾ���Ӽ�������Ӧ�ļ�ֵ						*/
	
	BOOL SaveKey(const char* lpFileName);							/* �ѵ�ǰ��ֵ���浽ָ���ļ�						*/
	BOOL RestoreKey(const char* lpFileName);						/* ��ָ��ע����ļ��лָ�						*/

	BOOL ReadValue(const char* lpValueName, std::string& outVal);	/* ����REG_SZ���͵�ֵ							*/
	BOOL ReadValue(const char* lpValueName, DWORD& dwVal);			/* ����DWORD���͵�ֵ							*/
	
	BOOL WriteValue(const char* lpSubKey, const char* lpVal);		/* д��REG_SZ����ֵ								*/
	BOOL WriteValue(const char* lpSubKey, DWORD dwVal);				/* д��DWORD����ֵ								*/
	BOOL WriteValueEX(const char* lpSubKey, const char* lpVal);	    /* д��REG_EXPAND_SZ����ֵ						*/

	BOOL EnumSub(
		OUT std::vector<std::string>* subTreeNames = NULL,
		OUT std::vector<std::string>* subValueNames = NULL);		/* ö��											*/

	~CRegOperator();

	CRegOperator& operator=(CRegOperator& o);
	CRegOperator(CRegOperator& o);

protected:
	void _deRef();

private:
	int* m_pRefCount;
};

#endif