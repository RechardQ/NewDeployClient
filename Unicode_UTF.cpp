#include "Header.h"

// 将UTF字符集转换为字符串
#ifdef WIN32
char* UTF8ToUniCode(const char* utf8, int srclen, int &len)
{
	int wlen = MultiByteToWideChar(CP_UTF8, 0, utf8, srclen, NULL, 0);
	wchar_t* wstr = new wchar_t[wlen + 1];

	memset(wstr, 0, wlen + 1);
	MultiByteToWideChar(CP_UTF8, 0, utf8, srclen, wstr, wlen);
	len = WideCharToMultiByte(CP_ACP, 0, wstr, wlen, NULL, 0, NULL, NULL);
	char* str = new char[len + 1];
	memset(str, 0, len + 1);
	WideCharToMultiByte(CP_ACP, 0, wstr, wlen, str, len, NULL, NULL);
	if (wstr) delete[] wstr;
	return str;
}

// 将字符串src转换为UTF字符集，返回，其中srclen为src的长度，len为返回的长度
char* UnicodeToUTF8(const char* src, int srclen, int &len)
{
	// convert an widechar string to utf8
	int wlen = MultiByteToWideChar(CP_ACP, 0, src, srclen, NULL, 0); //计算src转换为宽字符串的长度
	wchar_t* wstr = new wchar_t[wlen + 1];
	memset(wstr, 0, (wlen + 1) * sizeof(wchar_t));
	MultiByteToWideChar(CP_ACP, 0, src, srclen, wstr, wlen); // 将src转换为宽字符串放在wstr中
	len = WideCharToMultiByte(CP_UTF8, 0, wstr, wlen, NULL, 0, NULL, NULL); // 计算宽字符串转换utf的长度
	char* str = new char[len + 1];
	memset(str, 0, len + 1);
	WideCharToMultiByte(CP_UTF8, 0, wstr, wlen, str, len, NULL, NULL); // 将宽字符串转换为utf字符
	if (wstr) delete[] wstr;
	return str;
}
#endif