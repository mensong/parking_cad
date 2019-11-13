#include "UrlCoding.h"
#include <windows.h>

void UrlCoding::Gb2312ToUnicode(wchar_t* pOut, const char *gbBuffer)
{
	::MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, gbBuffer, 2, pOut, 1);
}
void UrlCoding::UTF8ToUnicode(wchar_t* pOut, const char *pText)
{
	char* uchar = (char *)pOut;

	uchar[1] = ((pText[0] & 0x0F) << 4) + ((pText[1] >> 2) & 0x0F);
	uchar[0] = ((pText[1] & 0x03) << 6) + (pText[2] & 0x3F);

	return;
}

void UrlCoding::UnicodeToUTF8(char* pOut, const wchar_t* pText)
{
	// ע�� wchar_t�ߵ��ֵ�˳��,���ֽ���ǰ�����ֽ��ں�
	char* pchar = (char *)pText;

	pOut[0] = (0xE0 | ((pchar[1] & 0xF0) >> 4));
	pOut[1] = (0x80 | ((pchar[1] & 0x0F) << 2)) + ((pchar[0] & 0xC0) >> 6);
	pOut[2] = (0x80 | (pchar[0] & 0x3F));

	return;
}
void UrlCoding::UnicodeToGB2312(char* pOut, wchar_t uData)
{
	WideCharToMultiByte(CP_ACP, NULL, &uData, 1, pOut, sizeof(wchar_t), NULL, NULL);
	return;
}

//��Ϊ��Urlʹ��
char UrlCoding::CharToInt(char ch) {
	if (ch >= '0' && ch <= '9')return (char)(ch - '0');
	if (ch >= 'a' && ch <= 'f')return (char)(ch - 'a' + 10);
	if (ch >= 'A' && ch <= 'F')return (char)(ch - 'A' + 10);
	return -1;
}

char UrlCoding::StrToBin(const char *str) {
	char tempWord[2];
	char chn;

	tempWord[0] = CharToInt(str[0]);                         //make the B to 11 -- 00001011
	tempWord[1] = CharToInt(str[1]);                         //make the 0 to 0 -- 00000000

	chn = (tempWord[0] << 4) | tempWord[1];                //to change the BO to 10110000

	return chn;
}


//UTF_8 תgb2312
void UrlCoding::UTF8ToGB2312(std::string &pOut, const char *pText, int pLen)
{
	char buf[4];
	char* rst = new char[pLen + (pLen >> 2) + 2];
	memset(buf, 0, 4);
	memset(rst, 0, pLen + (pLen >> 2) + 2);

	int i = 0;
	int j = 0;

	while (i < pLen)
	{
		if (*(pText + i) >= 0)
		{

			rst[j++] = pText[i++];
		}
		else
		{
			wchar_t Wtemp;


			UTF8ToUnicode(&Wtemp, pText + i);

			UnicodeToGB2312(buf, Wtemp);

			unsigned short int tmp = 0;
			tmp = rst[j] = buf[0];
			tmp = rst[j + 1] = buf[1];
			tmp = rst[j + 2] = buf[2];

			//newBuf[j] = Ctemp[0];
			//newBuf[j + 1] = Ctemp[1];

			i += 3;
			j += 2;
		}

	}
	rst[j] = '\0';
	pOut = rst;
	delete[]rst;
}

void UrlCoding::UTF8ToGB2312(std::string &pOut, const std::string& sText)
{
	UTF8ToGB2312(pOut, sText.c_str(), sText.length());
}

//GB2312 תΪ UTF-8
void UrlCoding::GB2312ToUTF8(std::string& pOut, const char *pText, int pLen)
{
	char buf[4];
	memset(buf, 0, 4);

	pOut.clear();

	int i = 0;
	while (i < pLen)
	{
		//�����Ӣ��ֱ�Ӹ��ƾͿ���
		if (pText[i] >= 0)
		{
			char asciistr[2] = { 0 };
			asciistr[0] = (pText[i++]);
			pOut.append(asciistr);
		}
		else
		{
			wchar_t pbuffer;
			Gb2312ToUnicode(&pbuffer, pText + i);

			UnicodeToUTF8(buf, &pbuffer);

			pOut.append(buf);

			i += 2;
		}
	}

	return;
}

void UrlCoding::GB2312ToUTF8(std::string& pOut, const std::string& sText)
{
	GB2312ToUTF8(pOut, sText.c_str(), sText.length());
}

//��str����Ϊ��ҳ�е� GB2312 url encode ,Ӣ�Ĳ��䣬����˫�ֽ� ��%3D%AE%88
std::string UrlCoding::UrlGB2312Encode(const char * str)
{
	std::string dd;
	size_t len = strlen(str);
	for (size_t i = 0; i < len; i++)
	{
		if (isalnum((BYTE)str[i]))
		{
			char tempbuff[2];
			sprintf(tempbuff, "%c", str[i]);
			dd.append(tempbuff);
		}
		else if (isspace((BYTE)str[i]))
		{
			//dd.append("+");
			char tempbuff[4];
			sprintf(tempbuff, "%%%02X", str[i]);
			dd.append(tempbuff);
		}
		else
		{
			char tempbuff[4];
			sprintf(tempbuff, "%%%X%X", ((BYTE*)str)[i] >> 4, ((BYTE*)str)[i] % 16);
			dd.append(tempbuff);
		}

	}
	return dd;
}

//��str����Ϊ��ҳ�е� UTF-8 url encode ,Ӣ�Ĳ��䣬�������ֽ� ��%3D%AE%88

std::string UrlCoding::UrlUTF8Encode(const char * str)
{
	std::string tt;
	std::string dd;
	GB2312ToUTF8(tt, str, (int)strlen(str));

	size_t len = tt.length();
	for (size_t i = 0; i < len; i++)
	{
		if (isalnum((BYTE)tt.at(i)))
		{
			char tempbuff[2] = { 0 };
			sprintf(tempbuff, "%c", (BYTE)tt.at(i));
			dd.append(tempbuff);
		}
		else if (isspace((BYTE)tt.at(i)))
		{
			//dd.append("+");
			char tempbuff[4];
			sprintf(tempbuff, "%%%02X", str[i]);
			dd.append(tempbuff);
		}
		else
		{
			char tempbuff[4];
			sprintf(tempbuff, "%%%X%X", ((BYTE)tt.at(i)) >> 4, ((BYTE)tt.at(i)) % 16);
			dd.append(tempbuff);
		}

	}
	return dd;
}
//��url GB2312����
std::string UrlCoding::UrlGB2312Decode(const std::string& str)
{
	std::string output = "";
	char tmp[2];
	int i = 0, idx = 0, ndx, len = str.length();

	while (i < len) {
		if (str[i] == '%') {
			tmp[0] = str[i + 1];
			tmp[1] = str[i + 2];
			output += StrToBin(tmp);
			i = i + 3;
		}
		else if (str[i] == '+') {
			output += ' ';
			i++;
		}
		else {
			output += str[i];
			i++;
		}
	}

	return output;
}
//��url utf8����
std::string UrlCoding::UrlUTF8Decode(const std::string& str)
{
	std::string output = "";

	std::string temp = UrlGB2312Decode(str);//

	UTF8ToGB2312(output, (char *)temp.data(), strlen(temp.data()));

	return output;

}