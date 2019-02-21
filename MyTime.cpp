/******************************************************************* 
 *  2018-2019 Company NanJing RenGu 
 *  All rights reserved. 
 *   
 *  文件名称: MyTime.cpp
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

#include "MyTime.h"

MyTime::MyTime()
{
}


MyTime::~MyTime()
{
}

void MyTime::CoutTime()
{
	SYSTEMTIME systime;
	GetLocalTime(&systime);
	cout << "-------------------------------" << setw(2) << setfill('0') << systime.wHour << ":" << setw(2) << setfill('0') << systime.wMinute
		<< ":" << setw(2) << setfill('0') << systime.wSecond << "-------------------------------" << endl;
}

void MyTime::CoutData()
{
	SYSTEMTIME systime;
	GetLocalTime(&systime);
	cout << "Data:" << " " << systime.wYear << "-" << setw(2) << setfill('0') << systime.wMonth << "-"  << setw(2) << setfill('0') << systime.wDay << endl;
}

//localtime函数被检测出64位移植性的编译错误，在此忽略此编译错误
#pragma warning(disable : 4996)
// 返回系统时间的名称
string MyTime::getTime()
{
	time_t timep;
	time(&timep);
	char tmp[64];
	strftime(tmp, sizeof(tmp), "%Y-%m-%d %H-%M-%S", localtime(&timep));
	return tmp;
}
