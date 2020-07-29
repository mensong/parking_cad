/********************************************************************
*　 文件名：RegOperator.cpp
*　 文件描述：对头文件中定义类的成员函数进行了实现
*　 修改记录：
*********************************************************************/
#include "StdAfx.h"
#include "RegOperator.h"

/*============================================================
* 函 数 名：IsValid
* 参　　数：无
* 功能描述：构造函数，初始化根键
* 返 回 值：有效返回TRUE,无效返回FALSE
*============================================================*/
BOOL CRegOperator::IsValid()
{
	return m_hKey != NULL;
}

/*============================================================
* 函 数 名：CRegOperator
* 参　　数：HKEY [IN] : 默认是HKEY_LOCAL_MACHINE
* 功能描述：构造函数，初始化根键
* 返 回 值：无
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
* 函 数 名：~CRegOperator
* 参　　数：NULL [IN]
* 功能描述：析构函数，关闭打开的注册表句柄
* 返 回 值：无
* 抛出异常：
*============================================================*/
CRegOperator::~CRegOperator()
{
	_deRef();
}

/*============================================================
* 函 数 名：operator=
* 参　　数：另外一个对象 [IN]
* 功能描述：拷贝函数
* 返 回 值：无
* 抛出异常：
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
* 函 数 名：CRegOperator
* 参　　数：NULL [IN]
* 功能描述：拷贝构造函数
* 返 回 值：无
* 抛出异常：
*============================================================*/
CRegOperator::CRegOperator(CRegOperator& o)
{
	*this = o;
}

/*============================================================
* 函 数 名：Close
* 参　　数：NULL [IN]
* 功能描述：关闭打开键的句柄
* 返 回 值：void
* 抛出异常：
*============================================================*/
void CRegOperator::Close()
{
	if (m_hKey)
	{
		/* 句柄非空进行关闭 */
		RegCloseKey(m_hKey);
		m_hKey = NULL;
	}
}

/*============================================================
* 函 数 名：SetHKEY
* 参　　数：LPCTSTR [IN] : 根键值
* 功能描述：由传入的根键字符串设置当前操作的根键的值
* 返 回 值：BOOL
* 抛出异常：
*============================================================*/

CRegOperator::CRegOperator(const char* strKey)
{
	assert(m_hKey);
	assert(strKey);

	/* 逐个进行比较 */
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
	/* 对操作系统版本进行测试 */
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
* 函 数 名：OpenKey
* 参　　数：LPCTSTR [IN] : 子键字符串
* 功能描述：通过传入子键的字符串打开注册表相应的位置
* 返 回 值：BOOL
* 抛出异常：
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
	/* 打开失败 */
	return CRegOperator();
}

/*============================================================
* 函 数 名：CreateKey
* 参　　数：LPCTSTR [IN] : 子键字符串
* 功能描述：通过传入子键的字符串打开（存在子键）或者创建（不存在子键）相应的子键
* 返 回 值：BOOL
* 抛出异常：
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
	/* 打开或者创建失败 */
	return CRegOperator();
}

/*============================================================
* 函 数 名：DeleteKey
* 参　　数：LPCTSTR LPCTSTR [IN] : 根键值 子键值
* 功能描述：通过传入的根键和子键，将子键删除删除，不能包含子键
* 返 回 值：BOOL
* 抛出异常：
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
		/* 删除成功 */
		return TRUE;
	}
	/* 删除失败 */
	return FALSE;
}

/*============================================================
* 函 数 名：DeleteTree
* 参　　数：LPCTSTR [IN] : 要删除键值的名称
* 功能描述：通过传入键值名称，删除对应的键值
* 返 回 值：BOOL
* 抛出异常：
*============================================================*/
BOOL CRegOperator::DeleteTree(const char* lpSubKey)
{
	assert(lpSubKey);
	assert(m_hKey);
	
	long lReturn = RegDeleteTreeA(m_hKey, lpSubKey);

	if (ERROR_SUCCESS == lReturn)
	{
		/* 删除成功 */
		return TRUE;
	}
	/* 删除失败 */
	return FALSE;
}

/*============================================================
* 函 数 名：DeleteValue
* 参　　数：LPCTSTR [IN] : 要删除键值的名称
* 功能描述：通过传入键值名称，删除对应的键值
* 返 回 值：BOOL
* 抛出异常：
*============================================================*/
BOOL CRegOperator::DeleteValue(const char* lpValueName)
{
	assert(m_hKey);
	assert(lpValueName);

	long lReturn = RegDeleteValueA(m_hKey, lpValueName);

	if (ERROR_SUCCESS == lReturn)
	{
		/* 删除成功 */
		return TRUE;
	}
	/* 删除失败 */
	return FALSE;
}

/*============================================================
* 函 数 名：SaveKey
* 参　　数：LPCTSTR [IN] : 待保存的文件名
* 功能描述：通过保存的文件名称，保存对应的子键
* 返 回 值：BOOL
* 抛出异常：
*============================================================*/
BOOL CRegOperator::SaveKey(const char* lpFileName)
{
	assert(m_hKey);
	assert(lpFileName);

	long lReturn = RegSaveKeyA(m_hKey, lpFileName, NULL);

	if (ERROR_SUCCESS == lReturn)
	{
		/* 保存成功 */
		return TRUE;
	}
	/* 保存失败 */
	return FALSE;
}

/*============================================================
* 函 数 名：RestoreKey
* 参　　数：LPCTSTR [IN] : 待恢复的文件名
* 功能描述：通过文件名称，从其中导入注册表中
* 返 回 值：BOOL
* 抛出异常：
*============================================================*/
BOOL CRegOperator::RestoreKey(const char* lpFileName)
{
	assert(m_hKey);
	assert(lpFileName);

	long lReturn = RegRestoreKeyA(m_hKey, lpFileName, 0);

	if (ERROR_SUCCESS == lReturn)
	{
		/* 导入成功 */
		return TRUE;
	}
	/* 导入失败 */
	return FALSE;
}

/*============================================================
* 函 数 名：Read
* 参　　数：LPCTSTR CString [IN] : 键值 读取值的key名，为空字符串时读取默认值
* 功能描述：将指定位置的CString类型的值读出
* 返 回 值：BOOL
* 抛出异常：
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
		/* 查询成功 */
		outVal = szString;

		return TRUE;
	}
	/* 查询失败 */
	return FALSE;
}

/*============================================================
* 函 数 名：Read
* 参　　数：LPCTSTR DWORD [IN] : 键值 读取值的key名，为空字符串时读取默认值
* 功能描述：将指定位置DWORD类型的值读出
* 返 回 值：BOOL
* 抛出异常：
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
		/* 查询成功 */
		dwVal = dwDest;

		return TRUE;
	}
	DWORD err = GetLastError();
	/* 查询失败 */
	return FALSE;
}

/*============================================================
* 函 数 名：Write
* 参　　数：LPCTSTR LPCTSTR [IN] : 键值 读取值的key名，为空字符串时读取默认值
* 功能描述：将LPCTSTR类型的值写入指定位置
* 返 回 值：BOOL
* 抛出异常：
*============================================================*/
BOOL CRegOperator::WriteValue(const char* lpValueName, const char* lpValue)
{
	assert(m_hKey);
	assert(lpValueName);
	assert(lpValue);

	long lReturn = RegSetValueExA(m_hKey, lpValueName, 0L, REG_SZ, (const BYTE *)lpValue, (DWORD)(strlen(lpValue) * sizeof(lpValue[0])) + 1);

	if (ERROR_SUCCESS == lReturn)
	{
		/* 成功写入 */
		return TRUE;
	}
	/* 写入失败 */
	return FALSE;
}

/*============================================================
* 函 数 名：Write
* 参　　数：LPCTSTR DWORD [IN] : 键值 写入值的key名，为空字符串时读取默认值
* 功能描述：将DWORD类型的值写入指定位置
* 返 回 值：BOOL
* 抛出异常：
*============================================================*/
BOOL CRegOperator::WriteValue(const char* lpValueName, DWORD dwVal)
{
	assert(m_hKey);
	assert(lpValueName);

	long lReturn = RegSetValueExA(m_hKey, lpValueName, 0L, REG_DWORD, (const BYTE *)&dwVal, sizeof(DWORD));

	if (ERROR_SUCCESS == lReturn)
	{
		/* 成功写入 */
		return TRUE;
	}
	/* 写入失败 */
	return FALSE;
}

BOOL CRegOperator::WriteValueEX(const char* lpValueName, const char* lpVal)
{
	assert(m_hKey);
	assert(lpValueName);

	long lReturn = RegSetValueExA(m_hKey, lpValueName, 0L, REG_EXPAND_SZ, (const BYTE *)lpVal, (DWORD)(strlen(lpVal) * sizeof(lpVal[0])) + 1);

	if (ERROR_SUCCESS == lReturn)
	{
		/* 成功写入 */
		return TRUE;
	}
	/* 写入失败 */
	return FALSE;
}

/*============================================================
* 函 数 名：Write
* 参　　数：subTreeNames [OUT] 子树列表
* 参　　数：subValueNames [OUT] 键列表
* 功能描述：将整型数写入指定位置
* 返 回 值：BOOL
* 抛出异常：
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