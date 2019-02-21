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
extern void GetDiskInfo(char *drive, char *totalDiskInfo, char *freeDiskInfo, vector<Stru_DiskRets> &diskList);
extern void SendDiskRets(Stru_DiskRetReco diskRet);

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
	getModuleInfo(module->processID, module->processName, module->priority, module->processMemory, moduleRetReco.ModuleRets);
	memcpy(moduleRetReco.browserID, module->browserID, 37);
	delete module;
	moduleRetReco.taskNum = moduleRetReco.ModuleRets.size();
	sendModuleRets(moduleRetReco);
	moduleRetReco.ModuleRets.clear();
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