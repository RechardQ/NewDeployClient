/******************************************************************* 
 *  2018-2019 Company NanJing RenGu 
 *  All rights reserved. 
 *   
 *  文件名称: stringz.cpp
 *  简要描述: 
 *   
 *  创建日期: 2019.2.20
 *  作者: RechardWu
 *  说明: 
 *   
 *  修改日期: 
 *  作者: 
 *  说明: 
 ******************************************************************/

#include "stringz.h"
using namespace std;
#pragma warning(disable : 4996)


/*  C++  去除左右空格  */
int trim_z(string& s)
{
	if(s.empty())
	{
		return 0;
	}
	s.erase(0,s.find_first_not_of(" "));
	s.erase(s.find_last_not_of(" ") + 1);
	return 1;
}

/*  C++  去掉字符串首部空格  */
int ltrim_z(char* s)
{
	_strrev(s);
	rtrim_z(s);
	_strrev(s);
	return 1;
}

/*  C++  去掉字符串尾部空格  */
int rtrim_z(char* s)
{
	int s_len = strlen(s);
	int x = 0;
	for(int i = s_len - 1;i >= 0;i--)
	{
		if(i == s_len - 1 && s[i] == ' ')
		{
			s[i] = '\0';
			break;
		}
		x = 1;
		break;
	}
	if(x == 0)
	{
		rtrim_z(s);
	}
	return 1;
}

/*  C++  去掉字符串收尾空格  */
int trim_z(char* s)
{
	rtrim_z(s);
	ltrim_z(s);
	return 1;
}

vector<char*> split_z(char* pch,char* sp)
{
	pch = strtok(pch,sp);
	vector<char*> vec;
	int i = 0;
	while(pch != NULL)
	{
		ltrim_z(pch);
		vec.push_back(pch);
		pch = strtok(NULL,sp);
		i++;
	}
	return vec;
}
	

/*  去掉右侧（一个或多个）换行符\n  */
int rtrim_n_z(char* s)
{
	int s_len = strlen(s);
	int x = 0;
	for(int i = s_len - 1;i >= 0;i--)
	{
		if(i == s_len - 1 && s[i] == '\n')
		{
			s[i] = '\0';
			break;
		}
		x = 1;
		break;
	}
	if(x == 0)
	{
		rtrim_n_z(s);
	}
	return 1;
}

/*  去掉左侧（一个或多个）换行符\n  */
int ltrim_n_z(char* s)
{
	_strrev(s);
	int s_len = strlen(s);
	int x = 0 ;
	for(int i = s_len - 1 ;i >= 0;i--)
	{
		if(i == s_len - 1 && s[i] == '\n')
		{
			s[i] = '\0';
			break;
		}
		x= 1;
		break;
	}
	_strrev(s);
	if(x == 0)
	{
		ltrim_n_z(s);
	}
	return 1;
}

/*  去掉左侧、右侧（一个或多个）换行符\n  */
int trim_n_z(char* s)
{
	rtrim_n_z(s);
	ltrim_n_z(s);
	return 1;
}