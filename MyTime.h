/******************************************************************* 
 *  2018-2019 Company NanJing RenGu 
 *  All rights reserved. 
 *   
 *  文件名称: MyTime.h
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

#pragma once

#include "Header.h"

class MyTime
{
public:
	MyTime();
	~MyTime();

	//输出时分秒
	void CoutTime();
	//输出年月日
	void CoutData();
	//返回系统时间的名称
	string getTime();
};

