#pragma once

#include <locale.h>
#include <tchar.h>
#include <windows.h>
#include <stdio.h>

class HardDiskSerial
{
public:
	/************************************************************************
	GetSerial�����ڻ�ȡָ����ŵ�Ӳ�����кţ������κ�Ȩ������
	������
	PCHAR pszIDBuff��������ַ��������������ڽ���Ӳ�����к�
	int nBuffLen��������ַ�����������С����Ӳ�����кŴ��ڸ�ֵʱ��ֻ����nBuffLen����
	int nDriveID��Ҫ��ȡ����������ţ���0��ʼ����15Ϊֹ
	����ֵ��
	�ɹ���ȡ����Ӳ�����кų��ȣ�Ϊ0��ʾ��ȡʧ��
	���ߣ�
	famous214��blog.csdn.net/LPWSTR��
	Դ��ο���diskid32��https://www.winsim.com/diskid32/diskid32.html��
	�汾��ʷ��
	20171226 ��һ�棬��diskid32Դ������ȡ
	20171226 �ڶ��棬����Unicode���뷽ʽ
	20171230 �ع��󷢲�������
	20191107 ���İ棬����õ��Ľ����ת������
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
	flipAndCodeBytes��dump�ڴ浽�ַ���
	������
	const char * str���ڴ�
	int pos����ʼλ��
	int flip����С�˷�ת
	char * buf��������ڴ�
	����ֵ��
	������ڴ�
	************************************************************************/
	static char * flipAndCodeBytes(const char * str, int pos, int flip, char * buf);
};

