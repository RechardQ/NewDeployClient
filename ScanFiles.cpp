#include "Header.h"
#include "md5.h"
#include "MyLog.h"

extern bool g_progFlag;
extern char serverIP[15];
char* UnicodeToUTF8(const char* src, int srclen, int &len);

MyLog m_log;


// 根据scanPath路径，通过递归扫描类型为scanType的文件，并放于fileList。
void ScanFiles(char *scanPath, char *scanType, vector<Stru_ScanRets> &fileList)
{
#ifdef WIN32
	char dirNew[300];
	memset(dirNew, 0, 300);
	strcpy(dirNew, scanPath);
	strcat(dirNew, "/*.*");    // 获得该路径下所有的文件

	_finddata_t findData;
	intptr_t handle;
	handle = _findfirst(dirNew, &findData);
	if (handle == -1)
	{
		char buffer[40];
		sprintf(buffer, "open dir %s failed\n", dirNew);
		//writeLog(buffer);
		return;
	}


	do
	{
		if (findData.attrib & _A_SUBDIR) // 判断该文件是否为文件夹
		{
			if (strcmp(findData.name, ".") == 0 || strcmp(findData.name, "..") == 0)
				continue;

			// 组成新的文件路径，进行扫描
			strcpy(dirNew, scanPath);
			strcat(dirNew, "/");
			strcat(dirNew, findData.name);

			ScanFiles(dirNew, scanType, fileList);
		}
		else
		{
			// 当前为文件
			if ((strcmp(scanType, "") == 0) || m_log.decFileName(findData.name, scanType) == true)
			{
				Stru_ScanRets scanRets;
				char utfPath[300];
				memset(utfPath, 0, 300);
				strcat(utfPath, scanPath);
				strcat(utfPath, "/");
				strcat(utfPath, findData.name);
				int utflens = 0;
				char *temppath = (char*)UnicodeToUTF8(utfPath, strlen(utfPath), utflens);
				memcpy(scanRets.filePath, temppath, utflens);

				// 计算该文件的MD5值
				MD5_CTX mdvalue;
				mdvalue.GetFileMd5(scanRets.md5Value, utfPath);
				// 将扫描结果放入队列中
				fileList.push_back(scanRets);
			}

		}
	} while (_findnext(handle, &findData) == 0);

	_findclose(handle);
#endif
#ifdef LINUX
	char dirNew[300];
	memset(dirNew, 0, 300);
	strcpy(dirNew, scanPath);
	DIR *pDir;
	pDir = opendir(dirNew);
	if (pDir == NULL)
	{
		char *buffer = new char[40];
		sprintf(buffer, "open dir %s failed\n", dirNew);
		writeLog((const char*)buffer);
		delete buffer;
		return;
	}
	struct dirent *	pDirEnt = NULL;

	while ((pDirEnt = readdir(pDir)) != NULL)
	{
		if ((strcmp(pDirEnt->d_name, ".") == 0) || (strcmp(pDirEnt->d_name, "..") == 0))
			continue;
		struct stat fileStat;
		char filePath[256];					//文件路径
		memset(filePath, 0, 256);
		strcat(filePath, scanPath);
		strcat(filePath, "/");
		strcat(filePath, pDirEnt->d_name);
		// 获得文件属性
		stat(filePath, &fileStat);

		if (S_ISDIR(fileStat.st_mode)) // 判断是否为文件夹
		{
			strcpy(dirNew, scanPath);
			strcat(dirNew, "/");
			strcat(dirNew, pDirEnt->d_name);
			scanFiles(dirNew, scanType, fileList);
		}
		else
		{
			// 当前为文件
			if ((strcmp(scanType, "") == 0) || decFileName(pDirEnt->d_name,
				scanType) == true)
			{
				Stru_ScanRets scanRets;
				strcat(scanRets.filePath, filePath); // 锟斤拷锟铰凤拷锟?
				// 计算该文件的MD5值
				MD5_CTX mdvalue;
				mdvalue.GetFileMd5(scanRets.md5Value, scanRets.filePath);
				// 将扫描结果放入队列中
				fileList.push_back(scanRets);
		}
	}
	closedir(pDir);
#endif
#ifdef DVXWOKR
	char dirNew[300];
	memset(dirNew, 0, 300);
	strcpy(dirNew, scanPath);
	DIR *pDir;
	pDir = opendir(dirNew);
	printf("open dirNew:%s\n", dirNew);
	if (pDir == NULL)
	{
		char *buffer = new char[40];
		sprintf(buffer, "open dir %s failed\n", dirNew);
		writeLog((const char*)buffer);
		delete buffer;
		return;
	}
	struct dirent *	pDirEnt = NULL;

	while ((pDirEnt = readdir(pDir)) != NULL)
	{
		if ((strcmp(pDirEnt->d_name, ".") == 0) || (strcmp(pDirEnt->d_name, "..") == 0))
			continue;
		struct stat fileStat;
		char filePath[256];					//文件路径
		memset(filePath, 0, 256);
		strcat(filePath, scanPath);
		strcat(filePath, "/");
		strcat(filePath, pDirEnt->d_name);
		// 获得文件属性
		stat(filePath, &fileStat);

		if (S_ISDIR(fileStat.st_mode))
		{
			strcpy(dirNew, scanPath);
			strcat(dirNew, "/");
			strcat(dirNew, pDirEnt->d_name);
			scanFiles(dirNew, scanType, fileList);
		}
		else
		{
			if ((strcmp(scanType, "") == 0) || decFileName(pDirEnt->d_name,
				scanType) == true)
			{
				Stru_ScanRets scanRets;
				strcat(scanRets.filePath, filePath);
				MD5_CTX mdvalue;
				printf("get sValue %s\n", scanRets.filePath);
				bool flag = mdvalue.GetFileMd5(scanRets.md5Value, scanRets.filePath);
				if (flag == false)
					continue;
				fileList.push_back(scanRets);
			}
		}
	}
	closedir(pDir);
#endif
}


// 通过tcp返回扫描结果
void SendScanRets(Stru_ScanRetReco retReco)
{
#ifdef WIN32
	WSADATA wsa;
	WSAStartup(MAKEWORD(2, 2), &wsa);
#endif
	int client;
	bool flag = true;
	client = socket(AF_INET, SOCK_STREAM, 0);
	if (client < 0)
	{
		flag = false;
		//writeLog("create scan failed.\n");
	}
	sockaddr_in server;
	if (flag&&g_progFlag)
	{
		if (strcmp(serverIP, "0.0.0.0") != 0)
		{
			server.sin_family = AF_INET; // TCP/IP
			server.sin_addr.s_addr = inet_addr(serverIP);;
			server.sin_port = htons(SCANRETS);
			if (connect(client, (struct sockaddr*)&server, sizeof(struct sockaddr)) < 0)
			{
				//writeLog("scan connect failed\n");
			}
			else
			{
				int len = sizeof(retReco) + retReco.fileNum * (256 + 34);
				char *sendbuf = new char[len];
				memset(sendbuf, 0, len);
				int index = 0;
				memcpy(sendbuf, retReco.sign, sizeof(retReco.sign));
				index += sizeof(retReco.sign);
				memcpy(sendbuf + index, retReco.browserID, sizeof(retReco.browserID));
				index += sizeof(retReco.browserID); // 申请ID号
				memcpy(sendbuf + index, retReco.deviceID, sizeof(retReco.deviceID));
				index += sizeof(retReco.deviceID); // 设备ID号
				memcpy(sendbuf + index, retReco.compID, sizeof(retReco.compID));
				index += sizeof(retReco.compID); // 组件ID号
				// 遍历扫描内容，按照 文件名称、md5放于发送队列
				printf("the scans file Num is:%d\n", retReco.fileNum);
				for (int i = 0; i < retReco.fileNum; i++)
				{
					memcpy(sendbuf + index, retReco.scanRets[i].filePath, sizeof(retReco.scanRets[i].filePath));
					index += sizeof(retReco.scanRets[i].filePath);
					memcpy(sendbuf + index, retReco.scanRets[i].md5Value, sizeof(retReco.scanRets[i].md5Value));
					index += sizeof(retReco.scanRets[i].md5Value);
				}
				int ret = send(client, (const char*)sendbuf, len, 0);
#ifdef WIN32
				cout << "发送扫描结果成功" << endl;
#endif
#ifdef LINUX
				cout << "发送扫描结果成功" << endl;
#endif
#ifdef DVXWORK
				logMsg("send scanResult\n", 0, 0, 0, 0, 0, 0);
#endif

				if (ret < 0)
				{
#ifdef WIN32
					cout << "发送扫描结果失败" << endl;
#endif
#ifdef LINUX
					cout << "发送扫描结果失败" << endl;
#endif
#ifdef DVXWORK
					logMsg("send scanResult fail\n", 0, 0, 0, 0, 0, 0);
#endif
					//writeLog("send scanRet failed.\n");
				}
				delete sendbuf;
			}
		}
	}
#ifdef WIN32
	closesocket(client);
	WSACleanup();
#endif
#ifdef linux
	close(client);
#endif
#ifdef DVXWORK
	close(client);
#endif
}