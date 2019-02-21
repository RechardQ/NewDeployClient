#include "Header.h"

// ��UTF�ַ���ת��Ϊ�ַ���
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

// ���ַ���srcת��ΪUTF�ַ��������أ�����srclenΪsrc�ĳ��ȣ�lenΪ���صĳ���
char* UnicodeToUTF8(const char* src, int srclen, int &len)
{
	// convert an widechar string to utf8
	int wlen = MultiByteToWideChar(CP_ACP, 0, src, srclen, NULL, 0); //����srcת��Ϊ���ַ����ĳ���
	wchar_t* wstr = new wchar_t[wlen + 1];
	memset(wstr, 0, (wlen + 1) * sizeof(wchar_t));
	MultiByteToWideChar(CP_ACP, 0, src, srclen, wstr, wlen); // ��srcת��Ϊ���ַ�������wstr��
	len = WideCharToMultiByte(CP_UTF8, 0, wstr, wlen, NULL, 0, NULL, NULL); // ������ַ���ת��utf�ĳ���
	char* str = new char[len + 1];
	memset(str, 0, len + 1);
	WideCharToMultiByte(CP_UTF8, 0, wstr, wlen, str, len, NULL, NULL); // �����ַ���ת��Ϊutf�ַ�
	if (wstr) delete[] wstr;
	return str;
}
#endif