#include "StdAfx.h"
#include <Windows.h>
#include "HardDiskSerial.h"
#include <WinIoCtl.h>


char * HardDiskSerial::flipAndCodeBytes(const char * str, int pos, int flip, char * buf)
{
	int i;
	int j = 0;
	int k = 0;

	buf[0] = '\0';
	if (pos <= 0)
		return buf;

	if (!j)
	{
		char p = 0;

		// First try to gather all characters representing hex digits only.
		j = 1;
		k = 0;
		buf[k] = 0;
		for (i = pos; j && str[i] != '\0'; ++i)
		{
			char c = tolower(str[i]);

			if (isspace(c))
				c = '0';

			++p;
			buf[k] <<= 4;

			if (c >= '0' && c <= '9')
				buf[k] |= (unsigned char)(c - '0');
			else if (c >= 'a' && c <= 'f')
				buf[k] |= (unsigned char)(c - 'a' + 10);
			else
			{
				j = 0;
				break;
			}

			if (p == 2)
			{
				//char b = buf[k];
				if (buf[k] != '\0' && (buf[k] < 0 || !isprint(buf[k])))
				{
					j = 0;
					break;
				}
				++k;
				p = 0;
				buf[k] = 0;
			}

		}
	}

	if (!j)
	{
		// There are non-digit characters, gather them as is.
		j = 1;
		k = 0;
		for (i = pos; j && str[i] != '\0'; ++i)
		{
			char c = str[i];

			if (!isprint(c))
			{
				j = 0;
				break;
			}

			buf[k++] = c;
		}
	}

	if (!j)
	{
		// The characters are not there or are not printable.
		k = 0;
	}

	buf[k] = '\0';

	if (flip)
		// Flip adjacent characters
		for (j = 0; j < k; j += 2)
		{
			char t = buf[j];
			buf[j] = buf[j + 1];
			buf[j + 1] = t;
		}

		// Trim any beginning and end space
		i = j = -1;
		for (k = 0; buf[k] != '\0'; ++k)
		{
			if (!isspace(buf[k]))
			{
				if (i < 0)
					i = k;
				j = k;
			}
		}

		if ((i >= 0) && (j >= 0))
		{
			for (k = i; (k <= j) && (buf[k] != '\0'); ++k)
				buf[k - i] = buf[k];
			buf[k - i] = '\0';
		}

		return buf;
}

ULONG HardDiskSerial::GetSerial(PCHAR pszIDBuff, int nBuffLen, int nDriveID)
{
	HANDLE hPhysicalDrive = INVALID_HANDLE_VALUE;
	ULONG ulSerialLen = 0;
	__try
	{
		//  Try to get a handle to PhysicalDrive IOCTL, report failure
		//  and exit if can't.
		TCHAR szDriveName[32];
		wsprintf(szDriveName, TEXT("\\\\.\\PhysicalDrive%d"), nDriveID);

		//  Windows NT, Windows 2000, Windows XP - admin rights not required
		hPhysicalDrive = CreateFile(szDriveName, 0,
			FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,
			OPEN_EXISTING, 0, NULL);
		if (hPhysicalDrive == INVALID_HANDLE_VALUE)
		{
			__leave;
		}
		STORAGE_PROPERTY_QUERY query;
		DWORD cbBytesReturned = 0;
		static char local_buffer[10000];

		memset((void *)&query, 0, sizeof(query));
		query.PropertyId = StorageDeviceProperty;
		query.QueryType = PropertyStandardQuery;

		memset(local_buffer, 0, sizeof(local_buffer));

		if (DeviceIoControl(hPhysicalDrive, IOCTL_STORAGE_QUERY_PROPERTY,
			&query,
			sizeof(query),
			&local_buffer[0],
			sizeof(local_buffer),
			&cbBytesReturned, NULL))
		{
			STORAGE_DEVICE_DESCRIPTOR * descrip = (STORAGE_DEVICE_DESCRIPTOR *)& local_buffer;
			char serialNumber[1000];

			flipAndCodeBytes(local_buffer,
				descrip->SerialNumberOffset,
				!isLittleEndian(), serialNumber);

			if (isalnum(serialNumber[0]))
			{
				ULONG ulSerialLenTemp = strnlen(serialNumber, nBuffLen - 1);
				memcpy(pszIDBuff, serialNumber, ulSerialLenTemp);
				pszIDBuff[ulSerialLenTemp] = NULL;
				ulSerialLen = ulSerialLenTemp;
				__leave;
			}

		}
	}
	__finally
	{
		if (hPhysicalDrive != INVALID_HANDLE_VALUE)
		{
			CloseHandle(hPhysicalDrive);
		}
	}

	return ulSerialLen;
}

void HardDiskSerial::PrintAllSerial()
{
	static char szBuff[0x100];
	for (int nDriveNum = 0; nDriveNum < 128; nDriveNum++)
	{
		ULONG ulLen = GetSerial(szBuff, sizeof(szBuff), nDriveNum);
		if (ulLen > 0)
		{
			printf("第%d块硬盘的序列号为：%s\n", nDriveNum + 1, szBuff);
		}
	}
}
