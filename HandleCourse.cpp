#include "Header.h"

extern bool g_progFlag;
extern char serverIP[15];

void GetModuleInfo(char *processID, char *processName, char *priority, char *processMemory, vector<Stru_ModuleRets> &moduleList)
{
#ifdef WIN32
	Stru_ModuleRets moduleRets;
	FILE* fp;
	char szTest[1024];
	string temp;
	int subscript = 0;
	int count = 1;
	int temp_count = 0;
	memset(szTest, 0, 1024);
	fp = _popen("tasklist /FO CSV /NH", "r");
	while ((fgets(szTest, sizeof(szTest) - 1, fp)) != NULL)
	{
		if (strlen(szTest) != 0)
		{
			szTest[1023] = '\0';    //数组补齐，要不会报越界错误
			temp = szTest;
			while (1)
			{
				++subscript;
				if (szTest[subscript] == '"')
				{
					count++;
					if (count == 2)
					{
						memset(moduleRets.processName, 0, 128);
						strcpy(moduleRets.processName, temp.substr(1, (subscript - 1)).c_str());
						temp_count = subscript;
						//cout << moduleRets.processName << "          ";
					}
					else if (count == 4)
					{
						memset(moduleRets.processID, 0, 5);
						strcpy(moduleRets.processID, temp.substr((temp_count + 3), (subscript - temp_count - 3)).c_str());
						//cout << moduleRets.processID << "           ";
						memset(moduleRets.priority, 0, 8);
					}
					else if (count == 8)
					{
						temp_count = subscript;
					}
					else if (count == 10)
					{
						memset(moduleRets.processMemory, 0, 8);
						strcpy(moduleRets.processMemory, temp.substr((temp_count + 3), (subscript - temp_count - 5)).c_str());
						for (int i = 0; i <= 7; i++)
						{
							if (moduleRets.processMemory[i] == ',')
							{
								moduleRets.processMemory[i] = moduleRets.processMemory[i + 1];
								moduleRets.processMemory[i + 1] = moduleRets.processMemory[i + 2];
								moduleRets.processMemory[i + 2] = moduleRets.processMemory[i + 3];
								moduleRets.processMemory[i + 3] = moduleRets.processMemory[i + 4];
								moduleRets.processMemory[i + 4] = moduleRets.processMemory[i + 5];
								moduleRets.processMemory[i + 5] = moduleRets.processMemory[i + 6];
							}
						}
						//cout << moduleRets.processMemory << endl;
						moduleList.push_back(moduleRets);
						subscript = 0;
						count = 1;
						temp_count = 0;
						break;
					}
				}
				else if (subscript > 1023)
				{
					subscript = 0;
					count = 1;
					temp_count = 0;
					break;
				}
			}
		}
	}
	_pclose(fp);

#endif
#ifdef linux
	Stru_ModuleRets moduleRets;
	int i = 0;
	FILE* fp;
	char szTest[1024];
	string temp;
	fp = popen("ps -aux", "r");
	while ((fgets(szTest, sizeof(szTest) - 1, fp)) != NULL)
	{
		if (strlen(szTest) != 1)
		{
			memset(szTest, 0, sizeof(szTest));
			fgets(szTest, sizeof(szTest) - 1, fp);
			szTest[1023] = '\0';    //数组补齐，要不会报越界错误
			temp = szTest;
			memset(moduleRets.processName, 0, 128);
			strcpy(moduleRets.processName, temp.substr(0, 8).c_str());
			memset(moduleRets.processID, 0, 5);
			strcpy(moduleRets.processID, temp.substr(11, 4).c_str());
			//            qDebug() << moduleRets.processName;
			//            qDebug() << moduleRets.processID;
			memset(moduleRets.priority, 0, 8);
			moduleList.push_back(moduleRets);
		}
	}
	pclose(fp);
#endif
#ifdef VXWORKS
	Stru_ModuleRets moduleRets;
	DWORD aProcesses[1024], cbNeeded, cProcesses;
	unsigned int i;
	if (!EnumProcesses(aProcesses, sizeof(aProcesses), &cbNeeded))
		return;
	cProcesses = cbNeeded / sizeof(DWORD);
	for (i = 0; i < cProcesses; i++)
	{
		char *imagename = new char[128];
		memset(imagename, 0, 128);
		char tmp[256];
		sprintf(tmp, "%d", aProcesses[i]);
		string str = tmp;
		char *p = (char*)str.data();
		imagenamebypid_z(p, imagename);
		memcpy(moduleRets.processID, p, sizeof(p));
		//memcpy(processName,imagename,sizeof(imagename));
		strtok(imagename, "\n");
		memset(moduleRets.processName, 0, 128);
		int namelens = 0;
		char *name = (char*)UnicodeToUTF8(imagename, strlen(imagename), namelens);
		strcpy(moduleRets.processName, name);
		memset(moduleRets.priority, 0, 8);
		moduleList.push_back(moduleRets);
	}
#endif
}

// 通过tcp返回进程结果
void SendModuleRets(Stru_ModuleRetReco moduleRet)
{
#ifdef WIN32
	WSADATA wsa;
	WSAStartup(MAKEWORD(2, 2), &wsa); //initial Ws2_32.dll by a process

#endif
	int client;
	bool flag = true;
	client = socket(AF_INET, SOCK_STREAM, 0);
	if (client < 0)
	{
		flag = false;
		//writeLog("create module failed.\n");
	}sockaddr_in server;
	if (flag&&g_progFlag)
	{
		if (strcmp(serverIP, "0.0.0.0") != 0)
		{
			server.sin_family = AF_INET; // TCP/IP
			server.sin_addr.s_addr = inet_addr(serverIP);;
			server.sin_port = htons(SCANRETS);
			if (connect(client, (struct sockaddr*)&server, sizeof(struct sockaddr)) < 0)
			{
				//writeLog("module connect failed\n");
			}
			else
			{
				int len = sizeof(moduleRet) + moduleRet.taskNum * (5 + 128 + 8 + 8);
				char *modulebuf = new char[len];
				memset(modulebuf, 0, len);
				int index = 0;
				memcpy(modulebuf, moduleRet.sign, sizeof(moduleRet.sign));						//报文标识
				index += sizeof(moduleRet.sign);
				memcpy(modulebuf + index, moduleRet.browserID, sizeof(moduleRet.browserID));		// 申请ID号
				index += sizeof(moduleRet.browserID);
				for (int i = 0; i < moduleRet.taskNum; i++)
				{
					memcpy(modulebuf + index, moduleRet.ModuleRets[i].processID, sizeof(moduleRet.ModuleRets[i].processID));
					cout << "processID " << moduleRet.ModuleRets[i].processID << endl;
					index += sizeof(moduleRet.ModuleRets[i].processID);
					memcpy(modulebuf + index, moduleRet.ModuleRets[i].processName, sizeof(moduleRet.ModuleRets[i].processName));
					index += sizeof(moduleRet.ModuleRets[i].processName);
					memcpy(modulebuf + index, moduleRet.ModuleRets[i].priority, sizeof(moduleRet.ModuleRets[i].priority));
					index += sizeof(moduleRet.ModuleRets[i].priority);
					memcpy(modulebuf + index, moduleRet.ModuleRets[i].processMemory, sizeof(moduleRet.ModuleRets[i].processMemory));
					index += sizeof(moduleRet.ModuleRets[i].processMemory);
				}
				int ret = send(client, (const char*)modulebuf, len, 0);
				cout << "发送进程信息成功" << endl;

				if (ret < 0)
				{
					printf("发送进程信息失败\n");
					//writeLog("send ModuleRets failed.\n");
				}
				delete modulebuf;
			}

		}
	}

#ifdef WIN32
	closesocket(client);
	WSACleanup();
	//Sleep(29000);
#endif
#ifdef linux
	close(client);
#endif
#ifdef DVXWORK
	close(client);
#endif
}



