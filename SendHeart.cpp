#include "Header.h"
extern char serverIP[15];
extern bool g_progFlag;

double dwStreamIn;
double dwStreamOut;

//获取CPU参数
DWORD deax;
DWORD debx;
DWORD decx;
DWORD dedx;
void initCPU(DWORD veax)
{
	__asm
	{
		mov eax, veax
		cpuid
		mov deax, eax
		mov debx, ebx
		mov decx, ecx
		mov dedx, edx
	}
}
long getCpuFreq()
{
	int start, over;
	__asm
	{
		RDTSC
		mov start, eax
	}
	Sleep(50);
	__asm
	{
		RDTSC
		mov over, eax
	}
	return (over - start) / 50000;;
}
string getManufactureID()
{
	char manuID[25];
	memset(manuID, 0, sizeof(manuID));
	initCPU(0);
	memcpy(manuID + 0, &debx, 4);
	memcpy(manuID + 4, &dedx, 4);
	memcpy(manuID + 8, &decx, 4);
	return manuID;
}
string getCpuType()
{
	const DWORD id = 0x80000002;
	char cpuType[49];
	memset(cpuType, 0, sizeof(cpuType));
	for (DWORD t = 0; t < 3; t++)
	{
		initCPU(id + t);
		memcpy(cpuType + 16 * t + 0, &deax, 4);
		memcpy(cpuType + 16 * t + 4, &debx, 4);
		memcpy(cpuType + 16 * t + 8, &decx, 4);
		memcpy(cpuType + 16 * t + 12, &dedx, 4);
	}
	return cpuType;
}

__int64 CompareFileTime(FILETIME time1, FILETIME time2)
{
	__int64 a = time1.dwHighDateTime << 32 | time1.dwLowDateTime;
	__int64 b = time2.dwHighDateTime << 32 | time2.dwLowDateTime;
	return   (b - a);
}

int getcpuUsage()
{
	int cpu;
	HANDLE hEvent;
	BOOL res;
	FILETIME preidleTime;
	FILETIME prekernelTime;
	FILETIME preuserTime;
	FILETIME idleTime;
	FILETIME kernelTime;
	FILETIME userTime;

	res = GetSystemTimes(&idleTime, &kernelTime, &userTime);
	preidleTime = idleTime;
	prekernelTime = kernelTime;
	preuserTime = userTime;
	hEvent = CreateEventA(NULL, FALSE, FALSE, NULL); // 初始值为 nonsignaled ，并且每次触发后自动设置为nonsignaled 
	WaitForSingleObject(hEvent, 1000);
	res = GetSystemTimes(&idleTime, &kernelTime, &userTime);
	__int64 idle = CompareFileTime(preidleTime, idleTime);
	__int64 kernel = CompareFileTime(prekernelTime, kernelTime);
	__int64 user = CompareFileTime(preuserTime, userTime);
	cpu = (kernel + user - idle) * 100 / (kernel + user) + (rand() % 5);
	return cpu;
}

double getMemorytotalInfo()
{
	string memory_info;
	MEMORYSTATUSEX statusex;
	statusex.dwLength = sizeof(statusex);
	if (GlobalMemoryStatusEx(&statusex))
	{
		unsigned long long total = 0, remain_total = 0, avl = 0, remain_avl = 0;
		double decimal_total = 0, decimal_avl = 0;
		remain_total = statusex.ullTotalPhys % GBYTES;
		total = statusex.ullTotalPhys / GBYTES;
		avl = statusex.ullAvailPhys / GBYTES;
		remain_avl = statusex.ullAvailPhys % GBYTES;
		if (remain_total > 0)
		{
			decimal_total = (remain_total / MBYTES) / DKBYTES;
		}
		/*if(remain_avl > 0)
		{
		decimal_avl = (remain_avl / MBYTES) / DKBYTES;
		}*/
		decimal_total += (double)total;
		//decimal_avl += (double)avl;
		return decimal_total * 1024;
	}
	return -1;
}

double getMemoryavlInfo()
{
	string memory_info;
	MEMORYSTATUSEX statusex;
	statusex.dwLength = sizeof(statusex);
	if (GlobalMemoryStatusEx(&statusex))
	{
		unsigned long long total = 0, remain_total = 0, avl = 0, remain_avl = 0;
		double decimal_total = 0, decimal_avl = 0;
		remain_total = statusex.ullTotalPhys % GBYTES;
		total = statusex.ullTotalPhys / GBYTES;
		avl = statusex.ullAvailPhys / GBYTES;
		remain_avl = statusex.ullAvailPhys % GBYTES;
		if (remain_avl > 0)
		{
			decimal_avl = (remain_avl / MBYTES) / DKBYTES;
		}
		decimal_avl += (double)avl;
		return decimal_avl * 1024;
	}
	return -1;
}


double upstreamSpeed()
{
	FILE* fp;
	char szTest[1024];
	string temp;
	int count = 0;
	double tempStreamOut1 = 0;
	double tempStreamOut2 = 0;
	fp = _popen("netsh interface ipv4 show sub", "r");
	memset(szTest, 0, 1024);
	while (fgets(szTest, sizeof(szTest) - 1, fp) != NULL)
	{
		if (strlen(szTest) != 1)
		{
			szTest[1023] = '\0';    //数组补齐，要不会报越界错误
			temp = szTest;
			temp = temp.substr(39, 11);
			memset(szTest, 0, 1024);
			count++;
			if (count >= 3)
			{
				int a = atoi(temp.c_str());
				tempStreamOut1 += a;
			}
		}
	}
	_pclose(fp);

	Sleep(100);

	memset(szTest, 0, 1024);
	temp = "";
	count = 0;
	fp = _popen("netsh interface ipv4 show sub", "r");
	while (fgets(szTest, sizeof(szTest) - 1, fp) != NULL)
	{
		if (strlen(szTest) != 1)
		{
			szTest[1023] = '\0';    //数组补齐，要不会报越界错误
			temp = szTest;
			temp = temp.substr(39, 11);
			memset(szTest, 0, 1024);
			count++;
			if (count >= 3)
			{
				int a = atoi(temp.c_str());
				tempStreamOut2 += a;
			}

		}
	}
	_pclose(fp);

	dwStreamOut = (tempStreamOut2 - tempStreamOut1) / 1024 / 0.1;
	return dwStreamOut;
}


double downstreamSpeed()
{
	FILE* fp;
	char szTest[1024];
	string temp;
	int count = 0;
	double tempStreamIn1 = 0;
	double tempStreamIn2 = 0;
	fp = _popen("netsh interface ipv4 show sub", "r");
	memset(szTest, 0, 1024);
	while (fgets(szTest, sizeof(szTest) - 1, fp) != NULL)
	{
		if (strlen(szTest) != 1)
		{
			szTest[1023] = '\0';    //数组补齐，要不会报越界错误
			temp = szTest;
			temp = temp.substr(25, 11);
			memset(szTest, 0, 1024);
			count++;
			if (count >= 3)
			{
				int b = atoi(temp.c_str());
				tempStreamIn1 += b;
			}

		}
	}
	_pclose(fp);
	Sleep(100);

	memset(szTest, 0, 1024);
	temp = "";
	count = 0;
	fp = _popen("netsh interface ipv4 show sub", "r");
	while (fgets(szTest, sizeof(szTest) - 1, fp) != NULL)
	{
		if (strlen(szTest) != 1)
		{
			szTest[1023] = '\0';    //数组补齐，要不会报越界错误
			temp = szTest;
			temp = temp.substr(25, 11);
			memset(szTest, 0, 1024);
			count++;
			if (count >= 3)
			{
				int b = atoi(temp.c_str());
				tempStreamIn2 += b;
			}

		}
	}
	_pclose(fp);
	dwStreamIn = (tempStreamIn2 - tempStreamIn1) / 1024 / 0.1;
	return dwStreamIn;
}

//发送心跳
void SendHeart()
{
#ifdef WIN32
	WSADATA wsa;
	WSAStartup(MAKEWORD(2, 2), &wsa); //initial Ws2_32.dll by a process
#endif
	int client;
	int preUsa;
	bool flag = true;
	client = socket(AF_INET, SOCK_DGRAM, 0);
	if (client < 0)
	{
#ifdef WIN32
		cout << "socket error!" << endl;
#endif
#ifdef LINUX
		cout << "socket error!" << endl;
#endif
#ifdef DVXWORK
		logMsg("socket error!\n", 0, 0, 0, 0, 0, 0);
#endif
		flag = false;
	}
	sockaddr_in hostInfo;
	hostInfo.sin_family = AF_INET;
	//hostInfo.sin_addr.s_addr = inet_addr("193.1.103.16");// htonl(INADDR_ANY);
	hostInfo.sin_addr.s_addr = htonl(INADDR_ANY);
	hostInfo.sin_port = htons(HEARTPORT);
	Stru_Heart heartbuf;
	int ret = 0;
	sockaddr_in server;

#ifdef WIN32

#endif
#ifdef DVXWORK
	FAST DEV_HDR *pDevHdr;
#endif
	while (flag && g_progFlag)
	{
		if (strcmp(serverIP, "0.0.0.0") != 0) // 判断当前是否已获知服务器的IP地址
		{
			//cout << serverIP << endl;
#ifdef WIN32
			//memset((char *)&heartbuf,0,sizeof(Stru_Heart));
			string tmp;
			tmp = "";
			tmp = getCpuType();
			const char* temp = tmp.c_str();
			memcpy(heartbuf.cpuName, temp, strlen(temp));		//CPU名称
			//cout << "CPU名称:" << temp << " ";
			tmp = "";
			temp = NULL;

			long midvalue = getCpuFreq();
			memset(heartbuf.cpuFreq, 0, 6);
			stringstream ss;
			ss << midvalue;
			ss >> tmp;
			ss.str("");
			temp = tmp.c_str();
			memcpy(heartbuf.cpuFreq, temp, strlen(temp));		//CPU主频
			//cout << "CPU主频:" << temp << " ";
			tmp = "";
			temp = NULL;

			int usa = getcpuUsage();
			memset(heartbuf.cpuUsage, 0, 4);
			if (usa < 0)
			{
				preUsa = 0 + (rand() % 5);
				stringstream sss;
				sss << preUsa;
				sss >> tmp;
				sss.str("");
				temp = tmp.c_str();
				memcpy(heartbuf.cpuUsage, temp, strlen(temp));
				//cout << "CPU利用率为:" << temp << " ";
				tmp = "";
				temp = NULL;

			}
			else if (usa > 100)
			{
				preUsa = 100 - (rand() % 5);
				stringstream sss;
				sss << preUsa;
				sss >> tmp;
				sss.str("");
				temp = tmp.c_str();
				memcpy(heartbuf.cpuUsage, temp, strlen(temp));
				//cout << "CPU利用率为:" << temp << " ";
				tmp = "";
				temp = NULL;
			}
			else if (usa >= 0 && usa <= 100)
			{
				preUsa = usa;
				stringstream sss;
				sss << usa;
				sss >> tmp;
				sss.str("");
				temp = tmp.c_str();
				memcpy(heartbuf.cpuUsage, temp, strlen(temp));					//CPU利用率
				//cout << "CPU利用率为:" << temp << " ";
				tmp = "";
				temp = NULL;
			}

			double total = getMemorytotalInfo();
			memset(heartbuf.decimal_total, 0, 6);
			stringstream tt;
			tt << total;
			tt >> tmp;
			tt.str("");
			temp = tmp.c_str();
			memcpy(heartbuf.decimal_total, temp, strlen(temp));					//RAM总大小
			//cout << "RAM总大小:" << temp << " ";
			tmp = "";
			temp = NULL;

			double avl = getMemoryavlInfo();
			memset(heartbuf.decimal_avl, 0, 6);
			stringstream aa;
			aa << avl;
			aa >> tmp;
			aa.str("");
			temp = tmp.c_str();
			memcpy(heartbuf.decimal_avl, temp, strlen(temp));				//RAM的剩余大小
			//cout << "剩余RAM大小:" << temp << endl;
			tmp = "";
			temp = NULL;

			double upspeed = upstreamSpeed();
			memset(heartbuf.upstreamSpeed, 0, 8);
			stringstream uu;
			uu << upspeed;
			uu >> tmp;
			uu.str("");
			temp = tmp.c_str();
			memcpy(heartbuf.upstreamSpeed, temp, strlen(temp));
			//cout << "上行速率:" << temp << endl;
			tmp = "";
			temp = NULL;


			double downspeed = downstreamSpeed();
			memset(heartbuf.downstreamSpeed, 0, 8);
			stringstream dd;
			dd << downspeed;
			dd >> tmp;
			dd.str("");
			temp = tmp.c_str();
			memcpy(heartbuf.downstreamSpeed, temp, strlen(temp));
			//cout << "下行速率：" << temp << endl;
			tmp = "";
			temp = NULL;



			server.sin_family = AF_INET;
			server.sin_addr.s_addr = inet_addr(serverIP);
			server.sin_port = htons(HEARTPORT);

			ret = sendto(client, (char*)&heartbuf, sizeof(heartbuf), 0, (struct sockaddr*)&server, sizeof(struct sockaddr));

			if (ret == -1)
			{
				//writeLog("send heart error.\n");
			}
		}
#endif
#ifdef LINUX
		if (strcmp(serverIP, "0.0.0.0") != 0) // 判断当前是否已获知服务器的IP地址
		{

			strcpy(heartbuf.cpuName, "0");
			strcpy(heartbuf.cpuFreq, "0");
			strcpy(heartbuf.cpuUsage, "0");
			strcpy(heartbuf.decimal_total, "0");
			strcpy(heartbuf.decimal_avl, "0");
			server.sin_family = AF_INET;
			server.sin_addr.s_addr = inet_addr(serverIP);
			server.sin_port = htons(HEARTPORT);
			ret = sendto(client, (char*)&heartbuf, sizeof(heartbuf), 0, (struct sockaddr*)&server, sizeof(struct sockaddr));
			if (ret == -1)
			{
				printf("send heart error %d %s %s\n", -1, hostIP, serverIP);
			}
		}
#endif
#ifdef DVXWORK
		if (strcmp(serverIP, "0.0.0.0") != 0) // 判断当前是否已获知服务器的IP地址
		{

			strcpy(heartbuf.cpuName, "0");
			strcpy(heartbuf.cpuFreq, "0");
			strcpy(heartbuf.cpuUsage, "0");
			strcpy(heartbuf.decimal_total, "0");
			strcpy(heartbuf.decimal_avl, "0");
			server.sin_family = AF_INET;
			server.sin_addr.s_addr = inet_addr(serverIP);
			server.sin_port = htons(HEARTPORT);
			ret = sendto(client, (char*)&heartbuf, sizeof(heartbuf), 0, (struct sockaddr*)&server, sizeof(struct sockaddr));
			if (ret == -1)
			{
				printf("send heart error %d %s %s\n", -1, hostIP, serverIP);
			}
		}
#endif
#ifdef WIN32
		Sleep(500); // 定时
#endif
#ifdef LINUX
		usleep(500000);
#endif
#ifdef DVXWORK
		sleep(1 / 2);
#endif
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