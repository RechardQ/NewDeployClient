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

DWORD WINAPI winrecvComdProc(LPVOID para)
{
	RecvCommand();
	return 0;
}

DWORD WINAPI winDeployProc(LPVOID para)
{
	DeployFiles();
	return 0;
}


DWORD WINAPI winDiskInfo(LPVOID para)
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


DWORD WINAPI winModuleInfo(LPVOID para)
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

DWORD WINAPI winScanProc(LPVOID para)
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
	heartThread = CreateThread(0, 0, winSendHeartProc, NULL, 0, &heartdwThreadID);
}

//启动组播线程
void MyThread::FoundMulticastThread()
{
	multicastThread = CreateThread(0, 0, winMulticastProc, NULL, 0, &multicastThreadID);
}

//启动部署线程
void MyThread::FoundDeployThread()
{
	deployThread = CreateThread(0, 0, winDeployProc, NULL, 0, &deployThreadID);
}

//启动命令线程
void MyThread::FoundRecvOrderThread()
{
	recvComdThread = CreateThread(0, 0, winrecvComdProc, NULL, 0, &comdID);
}


void MyThread::FoundDiskThread(Stru_Disk disk)
{
	Stru_Disk *diskbuf = new Stru_Disk();
	memcpy(diskbuf, &disk, sizeof(Stru_Disk));
#ifdef WIN32
	HANDLE diskThread = NULL;
	DWORD diskThreadID = 0;
	diskThread = CreateThread(0, 0, winDiskInfo, (LPVOID)diskbuf, 0, &diskThreadID);
#endif
#ifdef linux
	pthread_t diskThreadID;
	pthread_create(&diskThreadID, NULL, linuxDiskInfo, (void*)diskbuf);
#endif
#ifdef DVXWORK
	taskSpawn("diskThread", 190, 0, 6000, (FUNCPTR)vxDiskInfo, diskbuf, 0, 0, 0, 0, 0, 0, 0, 0, 0);
#endif
}


void MyThread::FoundModuleProxy(Stru_Module module)
{
	Stru_Module *modulebuf = new Stru_Module();
	memcpy(modulebuf, &module, sizeof(Stru_Module));
#ifdef WIN32
	HANDLE moduleThread = NULL;
	DWORD moduleThreadID = 0;
	moduleThread = CreateThread(0, 0, winModuleInfo, (LPVOID)modulebuf, 0, &moduleThreadID);
#endif
#ifdef linux
	pthread_t moduleThreadID;
	pthread_create(&moduleThreadID, NULL, linuxModuleInfo, (void*)modulebuf);
#endif
#ifdef DVXWORK
	taskSpawn("moduleThread", 180, 0, 6000, (FUNCPTR)vxModuleInfo, modulebuf, 0, 0, 0, 0, 0, 0, 0, 0, 0);
#endif
}

void MyThread::FonudScanFiles(Stru_Scans scans)
{
	Stru_Scans *scanbuf = new Stru_Scans();
	memcpy(scanbuf, &scans, sizeof(Stru_Scans));
#ifdef WIN32
	HANDLE scanThread = NULL;
	DWORD scanThreadID = 0;
	scanThread = CreateThread(0, 0, winScanProc, (LPVOID)scanbuf, 0, &scanThreadID);//锟斤拷锟斤拷锟竭筹拷
#endif
#ifdef linux
	pthread_t scanThreadID;
	pthread_create(&scanThreadID, NULL, linuxScanProc, (void*)scanbuf);
#endif
#ifdef DVXWORK
	taskSpawn("scanThread", 160, 0, 6000, (FUNCPTR)vxScanProc, scanbuf, 0, 0, 0, 0, 0, 0, 0, 0, 0);
#endif
}