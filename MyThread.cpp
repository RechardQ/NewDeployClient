/******************************************************************* 
 *  2018-2019 Company NanJing RenGu 
 *  All rights reserved. 
 *   
 *  文件名称: MyThread.cpp
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

#include "MyThread.h"
extern void SendHeart();
extern void RecvServiceMulticast();
extern void RecvCommand();
extern void DeployFiles();
extern void GetDiskInfo(char *drive, char *totalDiskInfo, char *freeDiskInfo, vector<Stru_DiskRets> &diskList);
extern void SendDiskRets(Stru_DiskRetReco diskRet);
extern void GetModuleInfo(char *processID, char *processName, char *priority, char *processMemory, vector<Stru_ModuleRets> &moduleList);
extern void SendModuleRets(Stru_ModuleRetReco moduleRet);
extern void ScanFiles(char *scanPath, char *scanType, vector<Stru_ScanRets> &fileList);
extern void SendScanRets(Stru_ScanRetReco retReco);


MyThread::MyThread()
{
}


MyThread::~MyThread()
{
}

// 以下为windows线程函数
#ifdef WIN32
DWORD WINAPI winSendHeartProc(LPVOID para)
{
	SendHeart();
	return 0;
}

DWORD WINAPI winMulticastProc(LPVOID para)
{
	RecvServiceMulticast();
	return 0;
}

DWORD WINAPI winRecvComdProc(LPVOID para)
{
	RecvCommand();
	return 0;
}

DWORD WINAPI winDeployProc(LPVOID para)
{
	DeployFiles();
	return 0;
}
#endif

// 以下为LINUX线程函数
#ifdef LINUX
void *LinuxSendHeartProc(void *argc)
{
	SendHeart();
	pthread_exit(NULL);
	return 0;
}

void *LinuxBroadCastProc(void *argc)
{
	RecvServiceMulticast();
	pthread_exit(NULL);
	return 0;
}

void *LinuxDeployProc(void *argc)
{
	DeployFiles();
	pthread_exit(NULL);
	return 0;
}

void *LinuxrecvComdProc(void *argc)
{
	RecvCommand();
	pthread_exit(NULL);
	return 0;
}
#endif

//以下为Vxworks线程函数
#ifdef DVXWORK
void vxSendHeartProc(void *argc)
{
	SendHeart();
	return;
}

void vxBroadCastProc(void *argc)
{
	RecvServiceMulticast();
	return;
}

void vxDeployProc(void *argc)
{
	DeployFiles();
	return;
}

void vxrecvComdProc(void *argc)
{
	RecvCommand();
	return;
}
#endif

// window线程，负责获取本地磁盘信息并发送结果
#ifdef WIN32
DWORD WINAPI winDiskInfo(LPVOID para)
#endif
#ifdef LINUX
void* LINUXDiskInfo(void* info)
#endif
#ifdef DVXWORK
void vxDiskInfo(void* info)
#endif
{
	Stru_Disk* disk = (Stru_Disk*)para;
	if (disk == NULL)
	{
		return 0;
	}
	Stru_DiskRetReco diskRetReco;
	GetDiskInfo(disk->drive, disk->totalDiskInfo, disk->freeDiskInfo, diskRetReco.DiskRets);
	memcpy(diskRetReco.browserID, disk->browserID, sizeof(disk->browserID));
	delete disk;
	diskRetReco.diskNum = diskRetReco.DiskRets.size();
	SendDiskRets(diskRetReco);
	diskRetReco.DiskRets.clear();
	return 0;
}

// window线程，负责获取本地进程信息并发送结果
#ifdef WIN32
DWORD WINAPI winModuleInfo(LPVOID para)
#endif
#ifdef LINUX
void* LINUXModuleInfo(void* info)
#endif
#ifdef DVXWORK
void vxModuleInfo(void* info)
#endif
{
	Stru_Module* module = (Stru_Module*)para;
	if (module == NULL)
	{
		return 0;
	}
	Stru_ModuleRetReco moduleRetReco;
	GetModuleInfo(module->processID, module->processName, module->priority, module->processMemory, moduleRetReco.ModuleRets);
	memcpy(moduleRetReco.browserID, module->browserID, 37);
	delete module;
	moduleRetReco.taskNum = moduleRetReco.ModuleRets.size();
	SendModuleRets(moduleRetReco);
	moduleRetReco.ModuleRets.clear();
	return 0;
}

// window线程，负责扫描本地文件并发送扫描结果
#ifdef WIN32
DWORD WINAPI winScanProc(LPVOID para)
#endif
#ifdef LINUX
void *LINUXScanProc(void *argc)
#endif
#ifdef DVXWORK
void vxScanProc(void *argc)
#endif
{
	Stru_Scans* scans = (Stru_Scans*)para;
	if (scans == NULL)
	{
		return 0;
	}
	Stru_ScanRetReco scanRetReco;
	// 开始本地文件扫描
	ScanFiles(scans->scanPath, scans->scanType, scanRetReco.scanRets);
	memcpy(scanRetReco.browserID, scans->browserID, sizeof(scans->browserID));
	memcpy(scanRetReco.deviceID, scans->deviceID, sizeof(scans->deviceID));
	memcpy(scanRetReco.compID, scans->compID, sizeof(scans->compID));
	delete scans;
	// 发送扫描的结果
	scanRetReco.fileNum = scanRetReco.scanRets.size();
	SendScanRets(scanRetReco);
	scanRetReco.scanRets.clear();
	return 0;
}


//启动心跳线程
void MyThread::FoundHeartThread()
{
#ifdef WIN32
	heartThread = CreateThread(0, 0, winSendHeartProc, NULL, 0, &heartdwThreadID);
#endif
#ifdef LINUX
	int ret;
	pthread_t heartdwThreadID;
	ret = pthread_create(&broadcastThreadID, NULL, LinuxBroadCastProc, NULL); // 启动发送心跳线程
#endif
#ifdef DVXWORK

#endif
}

//启动组播线程
void MyThread::FoundMulticastThread()
{
#ifdef WIN32
	multicastThread = CreateThread(0, 0, winMulticastProc, NULL, 0, &multicastThreadID);
#endif
#ifdef LINUX
	int ret;
	pthread_t broadcastThreadID;
	ret = pthread_create(&heartdwThreadID, NULL, LinuxSendHeartProc, NULL); // 启动接收广播线程
#endif
#ifdef DVXWORK

#endif
}

//启动部署线程
void MyThread::FoundDeployThread()
{
#ifdef WIN32
	deployThread = CreateThread(0, 0, winDeployProc, NULL, 0, &deployThreadID);
#endif
#ifdef LINUX
	int ret;
	pthread_t deployThreadID;
	ret = pthread_create(&deployThreadID, NULL, LinuxDeployProc, NULL); // 启动接收文件部署线程
#endif
#ifdef DVXWORK

#endif
}

//启动命令线程
void MyThread::FoundRecvOrderThread()
{
#ifdef WIN32
	recvComdThread = CreateThread(0, 0, winRecvComdProc, NULL, 0, &comdID);
#endif
#ifdef LINUX
	int ret;
	pthread_t comdID;
	ret = pthread_create(&comdID, NULL, LinuxrecvComdProc, NULL); // 启动接收命令线程
#endif
#ifdef DVXWORK

#endif
}


//启动磁盘扫描线程
void MyThread::FoundDiskThread(Stru_Disk disk)
{
	Stru_Disk *diskbuf = new Stru_Disk();
	memcpy(diskbuf, &disk, sizeof(Stru_Disk));
#ifdef WIN32
	HANDLE diskThread = NULL;
	DWORD diskThreadID = 0;
	diskThread = CreateThread(0, 0, winDiskInfo, (LPVOID)diskbuf, 0, &diskThreadID);
#endif
#ifdef LINUX
	pthread_t diskThreadID;
	pthread_create(&diskThreadID, NULL, LINUXDiskInfo, (void*)diskbuf);
#endif
#ifdef DVXWORK
	taskSpawn("diskThread", 190, 0, 6000, (FUNCPTR)vxDiskInfo, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
#endif
}

//启动进程扫描线程
void MyThread::FoundModuleProxy(Stru_Module module)
{
	Stru_Module *modulebuf = new Stru_Module();
	memcpy(modulebuf, &module, sizeof(Stru_Module));
#ifdef WIN32
	HANDLE moduleThread = NULL;
	DWORD moduleThreadID = 0;
	moduleThread = CreateThread(0, 0, winModuleInfo, (LPVOID)modulebuf, 0, &moduleThreadID);
#endif
#ifdef LINUX
	pthread_t moduleThreadID;
	pthread_create(&moduleThreadID, NULL, LINUXModuleInfo, (void*)modulebuf);
#endif
#ifdef DVXWORK
	taskSpawn("moduleThread", 180, 0, 6000, (FUNCPTR)vxModuleInfo, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
#endif
}

//启动文件扫描线程
void MyThread::FonudScanFiles(Stru_Scans scans)
{
	Stru_Scans *scanbuf = new Stru_Scans();
	memcpy(scanbuf, &scans, sizeof(Stru_Scans));
#ifdef WIN32
	HANDLE scanThread = NULL;
	DWORD scanThreadID = 0;
	scanThread = CreateThread(0, 0, winScanProc, (LPVOID)scanbuf, 0, &scanThreadID);//锟斤拷锟斤拷锟竭筹拷
#endif
#ifdef LINUX
	pthread_t scanThreadID;
	pthread_create(&scanThreadID, NULL, LINUXScanProc, (void*)scanbuf);
#endif
#ifdef DVXWORK
	taskSpawn("scanThread", 140, 0, 0x20000, (FUNCPTR)vxScanProc, (int)scanbuf, 0, 0, 0, 0, 0, 0, 0, 0, 0);
#endif
}