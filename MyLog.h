/******************************************************************* 
 *  2018-2019 Company NanJing RenGu 
 *  All rights reserved. 
 *   
 *  文件名称: MyLog.h
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
class MyLog
{
public:
	MyLog();
	~MyLog();

	//创建日志文件
	void FoundLog(char*,char[]);
	//分析配置文件
	void AnalyzeConfig(char[]);
	//日志记录函数
	void WriteLog(const char*,char*,char[]);
	// 创建文件夹
	int CreatDir(char *pDir);
	//转换文件名称
	int my_mkdir(char*);
};

