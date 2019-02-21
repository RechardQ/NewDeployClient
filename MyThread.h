/******************************************************************* 
 *  2018-2019 Company NanJing RenGu 
 *  All rights reserved. 
 *   
 *  文件名称: MyThread.h
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

#include "Header.h"

#pragma once
class MyThread
{
public:
	MyThread();
	~MyThread();

	HANDLE heartThread = NULL, multicastThread = NULL, deployThread = NULL, recvComdThread;
	DWORD heartdwThreadID = 0, multicastThreadID = 0, deployThreadID = 0, comdID = 0;
	//创建心跳线程
	void FoundHeartThread();
	//创建广播线程
	void FoundMulticastThread();
	//创建文件部署线程
	void FoundDeployThread();
	//创建命令接收线程
	void FoundRecvOrderThread();
	//创建扫描磁盘线程
	void FoundDiskThread(Stru_Disk);
	//创建扫描进程线程
	void FoundModuleProxy(Stru_Module);
	//创建文件扫描线程
	void FonudScanFiles(Stru_Scans);
};

