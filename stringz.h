/******************************************************************* 
 *  2018-2019 Company NanJing RenGu 
 *  All rights reserved. 
 *   
 *  文件名称: stringz.h
 *  简要描述: 工程的主函数，用于启动各个线程
 *   
 *  创建日期: 2019.2.20
 *  作者: RechardWu
 *  说明: 
 *   
 *  修改日期: 
 *  作者: 
 *  说明: 
 ******************************************************************/

#ifndef _STRINGZ_H__
#define _STRINGZ_H__
#include <iostream>
#include <vector>

/*  C++  去除左右空格  */
int trim_z(std::string& s);

/*  C++  去掉字符串首部空格  */
int ltrim_z(char* s);

/*  C++  去掉字符串尾部空格  */
int rtrim_z(char* s);

/*  C++  去掉字符串收尾空格  */
int trim_z(char* s);

std::vector<char*> split_z(char* pch,char* sp);

/*  去掉右侧（一个或多个）换行符\n  */
int rtrim_n_z(char* s);

/*  去掉左侧（一个或多个）换行符\n  */
int ltrim_n_z(char* s);

/*  去掉左侧、右侧（一个或多个）换行符\n  */
int trim_n_z(char* s);

#endif // _STRINGZ_H__