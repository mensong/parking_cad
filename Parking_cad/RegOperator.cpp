/********************************************************************
*�� �ļ�����RegOperator.cpp
*�� �ļ���������ͷ�ļ��ж�����ĳ�Ա����������ʵ��
*�� �޸ļ�¼��
*********************************************************************/
#include "StdAfx.h"
#include "RegOperator.h"

/*============================================================
* �� �� ����IsValid
* �Ρ���������
* �������������캯������ʼ������
* �� �� ֵ����Ч����TRUE,��Ч����FALSE
*============================================================*/
BOOL CRegOperator::IsValid()
{
	return m_hKey != NULL;
}

/*============================================================
* �� �� ����CRegOperator
* �Ρ�������HKEY [IN] : Ĭ����HKEY_LOCAL_MACHINE
* �������������캯������ʼ������
* �� �� ֵ����
*============================================================*/
CRegOperator::CRegOperator(HKEY hKey)
	: m_pRefCount(NULL)
{
	m_hKey = hKey;

	if (m_hKey)
	{
		m_pRefCount = new int;
		*m_pRefCount = 1;
	}
}


void CRegOperator::_deRef()
{
	if (m_pRefCount)
	{
		--(*m_pRefCount);

		if ((*m_pRefCount) <= 0)
		{
			*m_pRefCount = 0;
			delete m_pRefCount;
			m_pRefCount = NULL;
			Close();
		}
	}
}

/*============================================================
* �� �� ����~CRegOperator
* �Ρ�������NULL [IN]
* ���������������������رմ򿪵�ע�����
* �� �� ֵ����
* �׳��쳣��
*============================================================*/
CRegOperator::~CRegOperator()
{
	_deRef();
}

/*============================================================
* �� �� ����operator=
* �Ρ�����������һ������ [IN]
* ������������������
* �� �� ֵ����
* �׳��쳣��
*============================================================*/
CRegOperator& CRegOperator::operator=(CRegOperator& o)
{
	_deRef();

	m_pRefCount = o.m_pRefCount;
	if (m_pRefCount)
		++(*m_pRefCount);

	m_hKey = o.m_hKey;
	return *this;
}
/*============================================================
* �� �� ����CRegOperator
* �Ρ�������NULL [IN]
* �����������������캯��
* �� �� ֵ����
* �׳��쳣��
*============================================================*/
CRegOperator::CRegOperator(CRegOperator& o)
{
	*this = o;
}

/*============================================================
* �� �� ����Close
* �Ρ�������NULL [IN]
* �����������رմ򿪼��ľ��
* �� �� ֵ��void
* �׳��쳣��
*============================================================*/
void CRegOperator::Close()
{
	if (m_hKey)
	{
		/* ����ǿս��йر� */
		RegCloseKey(m_hKey);
		m_hKey = NULL;
	}
}

/*============================================================
* �� �� ����SetHKEY
* �Ρ�������LPCTSTR [IN] : ����ֵ
* �����������ɴ���ĸ����ַ������õ�ǰ�����ĸ�����ֵ
* �� �� ֵ��BOOL
* �׳��쳣��
*============================================================*/

CRegOperator::CRegOperator(const char* strKey)
{
	assert(m_hKey);
	assert(strKey);

	/* ������бȽ� */
	if (0 == strcmp(strKey, ("HKEY_CLASSES_ROOT")))
	{
		m_hKey = HKEY_CLASSES_ROOT;
	}
	if (0 == strcmp(strKey, ("HKEY_CURRENT_USER")))
	{
		m_hKey = HKEY_CURRENT_USER;
	}
	if (0 == strcmp(strKey, ("HKEY_LOCAL_MACHINE")))
	{
		m_hKey = HKEY_LOCAL_MACHINE;
	}
	if (0 == strcmp(strKey, ("HKEY_USERS")))
	{
		m_hKey = HKEY_USERS;
	}
	if (0 == strcmp(strKey, ("HKEY_PERFORMANCE_DATA")))
	{
		m_hKey = HKEY_PERFORMANCE_DATA;
	}
	if (0 == strcmp(strKey, ("HKEY_PERFORMANCE_TEXT")))
	{
		m_hKey = HKEY_PERFORMANCE_TEXT;
	}
	if (0 == strcmp(strKey, ("HKEY_PERFORMANCE_NLSTEXT")))
	{
		m_hKey = HKEY_PERFORMANCE_NLSTEXT;
	}
	/* �Բ���ϵͳ�汾���в��� */
#if(WINVER >= 0x0400)
	if (0 == strcmp(strKey, ("HKEY_CURRENT_CONFIG")))
	{
		m_hKey = HKEY_CURRENT_CONFIG;
	}
	if (0 == strcmp(strKey, ("HKEY_DYN_DATA")))
	{
		m_hKey = HKEY_DYN_DATA;
	}
#endif

}

/*============================================================
* �� �� ����OpenKey
* �Ρ�������LPCTSTR [IN] : �Ӽ��ַ���
* ����������ͨ�������Ӽ����ַ�����ע�����Ӧ��λ��
* �� �� ֵ��BOOL
* �׳��쳣��
*============================================================*/
CRegOperator CRegOperator::OpenKey(const char* lpSubKey, bool bX64/*=false*/)
{
	assert(m_hKey);
	assert(lpSubKey);

	DWORD opt = KEY_ALL_ACCESS;
	if (bX64)
		opt |= KEY_WOW64_64KEY;

	HKEY hKey;
	long lReturn = RegOpenKeyExA(m_hKey, lpSubKey, 0L, opt, &hKey);

	if (ERROR_SUCCESS == lReturn)
	{
		return CRegOperator(hKey);
	}
	/* ��ʧ�� */
	return CRegOperator();
}

/*============================================================
* �� �� ����CreateKey
* �Ρ�������LPCTSTR [IN] : �Ӽ��ַ���
* ����������ͨ�������Ӽ����ַ����򿪣������Ӽ������ߴ������������Ӽ�����Ӧ���Ӽ�
* �� �� ֵ��BOOL
* �׳��쳣��
*============================================================*/
CRegOperator CRegOperator::CreateKey(const char* lpSubKey, bool bX64/*=false*/)
{
	assert(m_hKey);
	assert(lpSubKey);

	DWORD opt = KEY_ALL_ACCESS;
	if (bX64)
		opt |= KEY_WOW64_64KEY;
	
	HKEY hKey;
	DWORD dw;
	long lReturn = RegCreateKeyExA(m_hKey, lpSubKey, 0L, NULL, REG_OPTION_NON_VOLATILE, opt, NULL, &hKey, &dw);

	if (ERROR_SUCCESS == lReturn)
	{
		return CRegOperator(hKey);
	}
	/* �򿪻��ߴ���ʧ�� */
	return CRegOperator();
}

/*============================================================
* �� �� ����DeleteKey
* �Ρ�������LPCTSTR LPCTSTR [IN] : ����ֵ �Ӽ�ֵ
* ����������ͨ������ĸ������Ӽ������Ӽ�ɾ��ɾ�������ܰ����Ӽ�
* �� �� ֵ��BOOL
* �׳��쳣��
*============================================================*/
BOOL CRegOperator::DeleteKey(const char* lpSubKey, bool bX64/*=false*/)
{
	assert(lpSubKey);
	assert(m_hKey);

	DWORD opt = KEY_ALL_ACCESS;
	if (bX64)
		opt |= KEY_WOW64_64KEY;

	long lReturn = RegDeleteKeyExA(m_hKey, lpSubKey, opt, 0);

	if (ERROR_SUCCESS == lReturn)
	{
		/* ɾ���ɹ� */
		return TRUE;
	}
	/* ɾ��ʧ�� */
	return FALSE;
}

/*============================================================
* �� �� ����DeleteTree
* �Ρ�������LPCTSTR [IN] : Ҫɾ����ֵ������
* ����������ͨ�������ֵ���ƣ�ɾ����Ӧ�ļ�ֵ
* �� �� ֵ��BOOL
* �׳��쳣��
*============================================================*/
BOOL CRegOperator::DeleteTree(const char* lpSubKey)
{
	assert(lpSubKey);
	assert(m_hKey);
	
	long lReturn = RegDeleteTreeA(m_hKey, lpSubKey);

	if (ERROR_SUCCESS == lReturn)
	{
		/* ɾ���ɹ� */
		return TRUE;
	}
	/* ɾ��ʧ�� */
	return FALSE;
}

/*============================================================
* �� �� ����DeleteValue
* �Ρ�������LPCTSTR [IN] : Ҫɾ����ֵ������
* ����������ͨ�������ֵ���ƣ�ɾ����Ӧ�ļ�ֵ
* �� �� ֵ��BOOL
* �׳��쳣��
*============================================================*/
BOOL CRegOperator::DeleteValue(const char* lpValueName)
{
	assert(m_hKey);
	assert(lpValueName);

	long lReturn = RegDeleteValueA(m_hKey, lpValueName);

	if (ERROR_SUCCESS == lReturn)
	{
		/* ɾ���ɹ� */
		return TRUE;
	}
	/* ɾ��ʧ�� */
	return FALSE;
}

/*============================================================
* �� �� ����SaveKey
* �Ρ�������LPCTSTR [IN] : ��������ļ���
* ����������ͨ��������ļ����ƣ������Ӧ���Ӽ�
* �� �� ֵ��BOOL
* �׳��쳣��
*============================================================*/
BOOL CRegOperator::SaveKey(const char* lpFileName)
{
	assert(m_hKey);
	assert(lpFileName);

	long lReturn = RegSaveKeyA(m_hKey, lpFileName, NULL);

	if (ERROR_SUCCESS == lReturn)
	{
		/* ����ɹ� */
		return TRUE;
	}
	/* ����ʧ�� */
	return FALSE;
}

/*============================================================
* �� �� ����RestoreKey
* �Ρ�������LPCTSTR [IN] : ���ָ����ļ���
* ����������ͨ���ļ����ƣ������е���ע�����
* �� �� ֵ��BOOL
* �׳��쳣��
*============================================================*/
BOOL CRegOperator::RestoreKey(const char* lpFileName)
{
	assert(m_hKey);
	assert(lpFileName);

	long lReturn = RegRestoreKeyA(m_hKey, lpFileName, 0);

	if (ERROR_SUCCESS == lReturn)
	{
		/* ����ɹ� */
		return TRUE;
	}
	/* ����ʧ�� */
	return FALSE;
}

/*============================================================
* �� �� ����Read
* �Ρ�������LPCTSTR CString [IN] : ��ֵ ��ȡֵ��key����Ϊ���ַ���ʱ��ȡĬ��ֵ
* ������������ָ��λ�õ�CString���͵�ֵ����
* �� �� ֵ��BOOL
* �׳��쳣��
*============================================================*/
BOOL CRegOperator::ReadValue(const char* lpValueName, std::string& outVal)
{
	assert(m_hKey);
	assert(lpValueName);

	DWORD dwType = 0;
	DWORD dwSize = 2047;
	char szString[2048];
	memset(szString, 0, 2048);

	long lReturn = RegQueryValueExA(m_hKey, lpValueName, NULL, &dwType, (BYTE *)szString, &dwSize);

	if (ERROR_SUCCESS == lReturn)
	{
		/* ��ѯ�ɹ� */
		outVal = szString;

		return TRUE;
	}
	/* ��ѯʧ�� */
	return FALSE;
}

/*============================================================
* �� �� ����Read
* �Ρ�������LPCTSTR DWORD [IN] : ��ֵ ��ȡֵ��key����Ϊ���ַ���ʱ��ȡĬ��ֵ
* ������������ָ��λ��DWORD���͵�ֵ����
* �� �� ֵ��BOOL
* �׳��쳣��
*============================================================*/
BOOL CRegOperator::ReadValue(const char* lpValueName, DWORD& dwVal)
{
	assert(m_hKey);
	assert(lpValueName);

	DWORD dwType;
	DWORD dwSize = sizeof(DWORD);
	DWORD dwDest;
	long lReturn = RegQueryValueExA(m_hKey, lpValueName, NULL, &dwType, (BYTE *)&dwDest, &dwSize);

	if (ERROR_SUCCESS == lReturn)
	{
		/* ��ѯ�ɹ� */
		dwVal = dwDest;

		return TRUE;
	}
	DWORD err = GetLastError();
	/* ��ѯʧ�� */
	return FALSE;
}

/*============================================================
* �� �� ����Write
* �Ρ�������LPCTSTR LPCTSTR [IN] : ��ֵ ��ȡֵ��key����Ϊ���ַ���ʱ��ȡĬ��ֵ
* ������������LPCTSTR���͵�ֵд��ָ��λ��
* �� �� ֵ��BOOL
* �׳��쳣��
*============================================================*/
BOOL CRegOperator::WriteValue(const char* lpValueName, const char* lpValue)
{
	assert(m_hKey);
	assert(lpValueName);
	assert(lpValue);

	long lReturn = RegSetValueExA(m_hKey, lpValueName, 0L, REG_SZ, (const BYTE *)lpValue, (DWORD)(strlen(lpValue) * sizeof(lpValue[0])) + 1);

	if (ERROR_SUCCESS == lReturn)
	{
		/* �ɹ�д�� */
		return TRUE;
	}
	/* д��ʧ�� */
	return FALSE;
}

/*============================================================
* �� �� ����Write
* �Ρ�������LPCTSTR DWORD [IN] : ��ֵ д��ֵ��key����Ϊ���ַ���ʱ��ȡĬ��ֵ
* ������������DWORD���͵�ֵд��ָ��λ��
* �� �� ֵ��BOOL
* �׳��쳣��
*============================================================*/
BOOL CRegOperator::WriteValue(const char* lpValueName, DWORD dwVal)
{
	assert(m_hKey);
	assert(lpValueName);

	long lReturn = RegSetValueExA(m_hKey, lpValueName, 0L, REG_DWORD, (const BYTE *)&dwVal, sizeof(DWORD));

	if (ERROR_SUCCESS == lReturn)
	{
		/* �ɹ�д�� */
		return TRUE;
	}
	/* д��ʧ�� */
	return FALSE;
}

BOOL CRegOperator::WriteValueEX(const char* lpValueName, const char* lpVal)
{
	assert(m_hKey);
	assert(lpValueName);

	long lReturn = RegSetValueExA(m_hKey, lpValueName, 0L, REG_EXPAND_SZ, (const BYTE *)lpVal, (DWORD)(strlen(lpVal) * sizeof(lpVal[0])) + 1);

	if (ERROR_SUCCESS == lReturn)
	{
		/* �ɹ�д�� */
		return TRUE;
	}
	/* д��ʧ�� */
	return FALSE;
}

/*============================================================
* �� �� ����Write
* �Ρ�������subTreeNames [OUT] �����б�
* �Ρ�������subValueNames [OUT] ���б�
* ������������������д��ָ��λ��
* �� �� ֵ��BOOL
* �׳��쳣��
*============================================================*/
BOOL CRegOperator::EnumSub(OUT std::vector<std::string>* subTreeNames/*=NULL*/, OUT std::vector<std::string>* subValueNames/*=NULL*/)
{
	char     achKey[MAX_KEY_LENGTH];   // buffer for subkey name  
	char	 achValue[MAX_VALUE_NAME];
	DWORD	 cchValue = MAX_VALUE_NAME;
	DWORD    cbName;                   // size of name string   
	char     achClass[MAX_PATH] = ("");  // buffer for class name   
	DWORD    cchClassName = MAX_PATH;  // size of class string   
	DWORD    cSubKeys = 0;               // number of subkeys
	DWORD    cValues;              // number of values for key   
	DWORD    cbMaxSubKey;              // longest subkey size   
	DWORD    cchMaxClass;              // longest class string
	DWORD    cchMaxValue;          // longest value name   
	DWORD    cbMaxValueData;       // longest value data   
	DWORD    cbSecurityDescriptor; // size of security descriptor   
	FILETIME ftLastWriteTime;      // last write time   

	DWORD i, retCode;
	
	// Get the class name and the value count.   
	retCode = RegQueryInfoKeyA(
		m_hKey,                    // key handle   
		achClass,                // buffer for class name   
		&cchClassName,           // size of class string   
		NULL,                    // reserved   
		&cSubKeys,               // number of subkeys   
		&cbMaxSubKey,            // longest subkey size   
		&cchMaxClass,            // longest class string   
		&cValues,                // number of values for this key   
		&cchMaxValue,            // longest value name   
		&cbMaxValueData,         // longest value data   
		&cbSecurityDescriptor,   // security descriptor   
		&ftLastWriteTime);       // last write time   

	if (retCode != ERROR_SUCCESS)
		return FALSE;

	// Enumerate the subkeys, until RegEnumKeyEx fails.  
	if (cSubKeys && subTreeNames)
	{
		//printf( "\nNumber of subkeys: %d\n", cSubKeys);  

		for (i = 0; i < cSubKeys; i++)
		{
			cbName = MAX_KEY_LENGTH;
			achKey[0] = '\0';
			retCode = RegEnumKeyExA(m_hKey, i,
				achKey,
				&cbName,
				NULL,
				NULL,
				NULL,
				&ftLastWriteTime);
			if (retCode == ERROR_SUCCESS)
			{
				//_tprintf(TEXT("(%d) %s\n"), i+1, achKey);  
				subTreeNames->push_back(achKey);
			}
		}
	}

	// Enumerate the key values.   
	if (cValues && subValueNames)
	{
		printf("\nNumber of values: %d\n", cValues);

		for (i = 0, retCode = ERROR_SUCCESS; i < cValues; i++)
		{
			cchValue = MAX_VALUE_NAME;
			achValue[0] = '\0';
			retCode = RegEnumValueA(m_hKey, i,
				achValue,
				&cchValue,
				NULL,
				NULL,
				NULL,
				NULL);

			if (retCode == ERROR_SUCCESS)
			{
				//_tprintf(TEXT("(%d) %s\n"), i+1, achValue);   
				subValueNames->push_back(achValue);
			}
		}
	}

	return TRUE;
}