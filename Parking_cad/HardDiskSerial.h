#pragma once

#include <locale.h>
#include <tchar.h>
#include <windows.h>
#include <stdio.h>

class HardDiskSerial
{
public:
	/************************************************************************
	GetSerial：用于获取指定编号的硬盘序列号，无需任何权限提升
	参数：
	PCHAR pszIDBuff：传入的字符串缓冲区，用于接收硬盘序列号
	int nBuffLen：传入的字符串缓冲区大小，当硬盘序列号大于该值时，只复制nBuffLen长度
	int nDriveID：要获取的驱动器编号，从0开始，到15为止
	返回值：
	成功获取到的硬盘序列号长度，为0表示获取失败
	作者：
	famous214（blog.csdn.net/LPWSTR）
	源码参考了diskid32（https://www.winsim.com/diskid32/diskid32.html）
	版本历史：
	20171226 第一版，从diskid32源码中提取
	20171226 第二版，兼容Unicode编译方式
	20171230 重构后发布第三版
	20191107 第四版，解决拿到的结果反转的问题
	************************************************************************/
	static ULONG GetSerial(PCHAR pszIDBuff, int nBuffLen, int nDriveID);

	static void PrintAllSerial();

protected:
	union p{
		int a;
		char b;
	};
	static inline bool isLittleEndian()
	{
		p p1;
		p1.a = 1;
		return p1.a==p1.b;
	}

	/************************************************************************
	flipAndCodeBytes：dump内存到字符串
	参数：
	const char * str：内存
	int pos：起始位置
	int flip：大小端反转
	char * buf：输出的内存
	返回值：
	输出的内存
	************************************************************************/
	static char * flipAndCodeBytes(const char * str, int pos, int flip, char * buf);
};

