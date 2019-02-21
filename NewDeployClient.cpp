/******************************************************************* 
 *  2018-2019 Company NanJing RenGu 
 *  All rights reserved. 
 *   
 *  文件名称: NewDeployClient.cpp
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

#include "NewDeployClient.h"

char filelogName[128];					// 存放本次启动软件记录日志的名称
char *LogFloder = (char*)"RGLOGS/";		// 存放日志的路径
char serverIP[15] = "0.0.0.0";			// 存放服务器的IP地址
char hostIP[15] = "0.0.0.0";			// 存放本机IP地址

int main()
{
#ifdef WIN32
	WSADATA wsa;
	WSAStartup(MAKEWORD(2, 2), &wsa);

	//启动程序打印当前日期
	MyTime m_mytime;
	m_mytime.CoutData();
#endif
	//启动在日志文件夹中创建一个日志文件
	MyLog m_mylog;
	m_mylog.FoundLog(LogFloder, filelogName);
	g_progFlag = true;
	g_deployFlag = true;
	//启动时解析配置文件中的本地IP
	m_mylog.AnalyzeConfig(hostIP);
#ifdef WIN32
	MyThread m_thread;
	m_thread.FoundHeartThread();
	m_thread.FoundMulticastThread();
	m_thread.FoundDeployThread();
	m_thread.FoundRecvOrderThread();
	WaitForSingleObject(m_thread.heartThread, INFINITE); // 等待线程结束
#endif
	return 0;
}
