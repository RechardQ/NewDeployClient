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
	//获得当前文件的文件路径 返回值  返回参数中文件的路径  其中filepath不含最后的\\信息
	bool find_last_of(char* absfile, char *filepath, int &len);
	//source内容以,分隔，查找innerbuf是否在buffer内容中，如果是返回对应的位置，否则返回-1值
	int findIndex(char *source, char *innerbuf);
	//判断文件filename是否为type类型 多个后缀类型以,分隔
	bool decFileName(char *fileName, char *fileType);

};

