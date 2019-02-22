/******************************************************************* 
 *  2018-2019 Company NanJing RenGu 
 *  All rights reserved. 
 *   
 *  文件名称: Header.h
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

#ifndef _HEADER_H
#define _HEADER_H

//Windows平台所需头文件
#ifdef WIN32
#include <stdio.h>
#include <string>
#include <WinSock2.h>
#include <Windows.h>
#include <io.h>
#include <vector>
#include <iostream>
#include <fstream>
#include <direct.h>
#include <time.h>
#include <stdlib.h>
#include <conio.h>
#include <psapi.h>
#include <atlstr.h>
#include <stdarg.h>
#include <sstream>
#include <string.h>
#include <tchar.h>
#include <regex>
#include <IPHlpApi.h>
#include <Ws2tcpip.h>
#include <iomanip>
#include <Pdh.h>
#pragma comment(lib,"pdh.lib")
#pragma comment(lib, "IPHlpApi.lib")
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib,"Psapi.Lib")
#endif

//LINUX平台所需头文件
#ifdef linux
#include <unistd.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <dirent.h>
#include <string.h>
#include <netinet/tcp.h>
#include <netdb.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <unistd.h>  
#include <sys/types.h> 
#include <stdio.h>  
#include <stdlib.h>
#include <sys/types.h>
#include <time.h>
#include <QDebug>
#include <sstream>
#endif

//VXWOKRS平台所需头文件
#ifdef DVXWORK
#include <vxworks.h>
#include <stdio.h>
#include <string>
#include <sockLib.h>
#include <inetLib.h>
#include <unistd.h>
#include <stat.h>
#include <string.h>
#include <iosLib.h>
#include <string.h>
#include <netDrv.h>
#include <hrFsLib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <utime.h>
#include <sys/stat.h>
#include <hostLib.h>
#include <vector>
#include <fstream>
#endif

using namespace std;

//宏定义
#define		HEARTPORT		6004		// 发送心跳端口
#define		BOARDCASTPORT	3086		// 接收广播端口
#define		GROUPPROT		30900		//接受组播端口
#define		COMMAND			3087		// 接收命令端口
#define		SCANRETS		6005		// 发送扫描端口
#define		DEPLOYPORT		3088
#define		GBYTES			1073741824
#define		MBYTES			1048576
#define		KBYTES			1024
#define		DKBYTES			1024.0
#define		VMRSS_LINE		17
#define		VMSIZE_LINE		13
#define		PROCESS_ITEM	14
#define		GB(x) (x.HighPart << 2) + (x.LowPart >> 20) / 1024.0
#define		REVBUFSIZE		10400
#define		TAILBUFSIZE		20800
#define		SERINUMLENGTH	4
#define		PATHLENGTH		256
#define		MD5LENGTH		34
#define		WRITELOG

//变量



//结构体
#pragma warning(disable : 4996)
// 心跳信息结构体
struct Stru_Heart
{
	char sign[4];
	char osType;   //   1:windows 2:linux/中标 3:vxworks/道系统
	char osVersion[16]; // 系统版本的信息
	char cpuName[64];					//CPU名称
	char cpuFreq[6];					//CPU主频
	char cpuUsage[4];					//CPU利用率
	char decimal_total[6];				//RAM总大小
	char decimal_avl[6];				//RAM的剩余大小
	char upstreamSpeed[8];				//上行速率
	char downstreamSpeed[8];			//下行速率
	Stru_Heart()
	{
		memset(this, 0, sizeof(Stru_Heart));
		memcpy(sign, "C101", 4);
		memset(&osVersion, 0, 16);
		memset(&cpuName, 0, 64);
		memset(&cpuFreq, 0, 6);
		memset(&cpuUsage, 0, 4);
		memset(&decimal_total, 0, 6);
		memset(&decimal_avl, 0, 6);
		strcpy(upstreamSpeed, "0.0");
		strcpy(downstreamSpeed, "0.0");
#ifdef WIN32
		osType = 1;
#endif
#ifdef LINUX
		osType = 2;
#endif
#ifdef DVXWORK
		osType = 3;
#endif
	}
};

// 服务器下发的命令信息结构体
struct Stru_Command
{
	char sign[4];							//报文表示
	char command[1];						//类型查询
	Stru_Command()
	{
		memcpy(sign, "S105", 4);
	}
};


// 返回的扫描信息结构体
struct Stru_Scans
{
	char browserID[37];					//申请ID
	char deviceID[37];					//设备ID
	char compID[37];					//组件ID
	char scanPath[256];					//文件路径
	char scanType[128];					//文件标识
	Stru_Scans()
	{
		memset(scanPath, 0, 256);
		memset(scanType, 0, 10);
		memset(browserID, 0, 37);
		memset(deviceID, 0, 37);
		memset(compID, 0, 37);
	}
};

// 返回进程信息结构体
struct Stru_Module
{
	char priority[8];					//优先级
	char processName[128];				//进程名称
	char processMemory[8];
	char processID[5];					//进程ID
	char browserID[37];					//申请ID
	Stru_Module()
	{
		memset(browserID, 0, 37);
		memset(processID, 0, 5);
		memset(processName, 0, 128);
		memset(priority, 0, 8);
		memset(processMemory, 0, 8);
	}
};

//返回磁盘信息结构体
struct Stru_Disk
{
	char browserID[37];					//申请ID
	char drive[32];						//盘符
	char totalDiskInfo[12];				//总大小
	char freeDiskInfo[12];				//剩余大小
		
	Stru_Disk()
	{
		memset(browserID, 0, 37);
		memset(drive, 0, 32);
		memset(totalDiskInfo, 0, 12);
		memset(freeDiskInfo, 0, 12);
	}
};


// 扫描内容结构体
struct Stru_ScanRets
{
	char filePath[256];					//文件路径
	char md5Value[34];					//文件md5
	Stru_ScanRets()
	{
		memset(filePath, 0, 256);
		memset(md5Value, 0, 34);
	}
};


// 内部维护的扫描信息结构体
struct Stru_ScanRetReco
{
	char sign[4];						//报文标识		
	char browserID[36];					//申请ID
	char deviceID[36];					//设备ID
	char compID[36];					//组件ID
	int fileNum;						//文件路径
	vector<Stru_ScanRets> scanRets;
	Stru_ScanRetReco()
	{
		memcpy(&sign, "C102", 4);
		memset(browserID, 0, 36);
		memset(deviceID, 0, 36);
		memset(compID, 0, 36);
		fileNum = 0;
		scanRets.clear();
	}
};

// 进程内容结构体
struct Stru_ModuleRets
{
	char processID[5];					//进程ID
	char processName[128];				//进程名称
	char priority[8];					//优先级
	char processMemory[8];				//进程内存占用情况
	Stru_ModuleRets()
	{
		memset(processID, 0, 5);
		memset(processName, 0, 128);
		memset(priority, 0, 8);
		memset(processMemory, 0, 8);
	}
};


// 内部维护的进程信息结构体
struct Stru_ModuleRetReco
{
	char sign[4];						//报文标识		
	char browserID[37];					//申请ID
	int taskNum;						//任务数量
	vector<Stru_ModuleRets> ModuleRets;
	Stru_ModuleRetReco()
	{
		memcpy(&sign, "C105", 4);
		memset(browserID, 0, 37);
		taskNum = 0;
		ModuleRets.clear();
	}
};


// 磁盘内容结构体
struct Stru_DiskRets
{
	char drive[32];						//盘符
	char totalDiskInfo[12];				//总大小
	char freeDiskInfo[12];				//剩余大小
	Stru_DiskRets()
	{
		memset(drive, 0, 32);
		memset(totalDiskInfo, 0, 12);
		memset(freeDiskInfo, 0, 12);
	}
};

// 内部维护的磁盘信息结构体
struct Stru_DiskRetReco
{
	char sign[4];						//报文标识
	char browserID[37];					//申请ID
	int diskNum;						//磁盘数量
	vector<Stru_DiskRets> DiskRets;
	Stru_DiskRetReco()
	{
		memcpy(&sign, "C106", 4);
		memset(browserID, 0, 37);
		diskNum = 0;
		DiskRets.clear();
	}
};

//部署结构体
struct DeployPackageEntity
{
	//序列号，0表示需要校验MD5，查看文件是否已传，正常传输从1开始
	int serialNum;
	//文件路径
	char targetPath[256];
	//MD5值
	char MD5Value[34];
	//单个数据包长度
	int dataSize;
	//包总大小
	long long totalSize;
	//正文
	char* data;
	DeployPackageEntity(int serialNum, char *targetPath, char *MD5Value)
	{
		this->serialNum = serialNum;
		memcpy(this->targetPath, targetPath, 128);
		memcpy(this->MD5Value, MD5Value, 34);
	}
	DeployPackageEntity(int serialNum, int dataSize, long long totalSize, char* data)
	{
		this->serialNum = serialNum;
		this->dataSize = dataSize;
		this->totalSize = totalSize;
		this->data = (char*)malloc(this->dataSize);
		memcpy(this->data, data, this->dataSize);
	}
};

struct HeadPacket
{
	int serialNum;				//序列号
	int dataSize;				//单步长度
	long long int totalSize;	//总长度
};


#endif //_HEADER_H
