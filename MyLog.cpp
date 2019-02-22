/******************************************************************* 
 *  2018-2019 Company NanJing RenGu 
 *  All rights reserved. 
 *   
 *  文件名称: MyLog.cpp
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

#include "MyLog.h"
#include "Header.h"
#include "MyTime.h"

MyLog::MyLog()
{
}


MyLog::~MyLog()
{
}

//strcpy函数被检测出64位移植性的编译错误，在此忽略此编译错误
#pragma warning(disable : 4996)
void MyLog::FoundLog(char* sz_floder,char sz_path[])
{
	MyTime m_mytime;;
	string logName = string(sz_floder) + "log-" + m_mytime.getTime() + ".txt";
	strcpy(sz_path, logName.c_str());
}


void MyLog::AnalyzeConfig(char sz_config[])
{
#ifdef WIN32
	FILE* fp = fopen("config.txt", "r");
	if (fp == NULL)
	{
		char hostName[255];
		gethostname(hostName, sizeof(hostName));
		struct hostent *p = gethostbyname(hostName);
		int i = 0;
		while (p->h_addr_list[i] != NULL)
		{
			printf("hostname: %s\n", p->h_name);
			printf("ip: %s\n", inet_ntoa(*(struct in_addr*)p->h_addr_list[i]));
			i++;
		}
	}
	else
	{
		char ln[80];
		fgets(ln, 80, fp);
		string data = ln;
		const char* ip;
		int iPos = data.find("=");
		data = data.substr(iPos + 1, 14);//截取字符串返回字节数
		int  n = atoi(data.c_str());
		if (n != 192)
		{
			char hostName[255];
			gethostname(hostName, sizeof(hostName));
			struct hostent *p = gethostbyname(hostName);
			int i = 0;
			while (p->h_addr_list[i] != NULL)
			{
				printf("hostname: %s\n", p->h_name);
				printf("ip: %s\n", inet_ntoa(*(struct in_addr*)p->h_addr_list[i]));
				i++;
			}
		}
		else
		{
			ip = data.c_str();
			cout << "local_IP: " << ip << endl;
			memcpy(sz_config, ip, 15);
		}
		fclose(fp);
	}
#endif
#ifdef LINUX
	FILE* fp = fopen("config.txt", "r");
	if (fp == NULL)
	{
		printf("cannot find config.txt,the IP use 127.0.0.1\n");
		writeLog("create scan failed.\n");
		strcpy(hostIP, "127.0.0.1");
	}
	else
	{
		char ln[80];
		fgets(ln, 80, fp);
		string data = ln;
		const char* ip;
		int iPos = data.find("=");
		data = data.substr(iPos + 1, 14);//截取字符串返回字节数
		ip = data.c_str();
		memcpy(hostIP, ip, 15);
		// printf("host %s\n",hostIP);
		fclose(fp);
	}
#endif
#ifdef DVXWORK
	FILE* fp = fopen("config.txt", "r");
	if (fp == NULL)
	{
		printf("cannot find config.txt,the IP use 127.0.0.1\n");
		writeLog("create scan failed.\n");
		strcpy(hostIP, "127.0.0.1");
	}
	else
	{
		char ln[80];
		fgets(ln, 80, fp);
		string data = ln;
		const char* ip;
		int iPos = data.find("=");
		data = data.substr(iPos + 1, 14);//截取字符串返回字节数
		ip = data.c_str();
		memcpy(hostIP, ip, 15);
		printf("host %s\n", hostIP);
		fclose(fp);
	}
#endif
}

void MyLog::WriteLog(const char *buf, char* sz_floder, char sz_config[])
{
	MyTime m_time;
#ifdef WRITELOG
	if (buf != NULL)
	{
		CreatDir(sz_floder);
		FILE *fp = fopen(sz_config, "a+");
		if (fp == NULL)
			return;
		string time = m_time.getTime() + "\t";
		fwrite(time.c_str(), 1, time.size(), fp);
		fwrite(buf, 1, strlen(buf), fp);
		fclose(fp);
	}
#endif
}

// 创建文件夹
int MyLog::CreatDir(char *pDir)
{
	int i = 0;
	int iRet;
	int iLen;
	char* pszDir;

	if (NULL == pDir)
		return 0;

	pszDir = new char[strlen(pDir) + 1];
	memset(pszDir, 0, strlen(pDir) + 1);
	memcpy(pszDir, pDir, strlen(pDir));
	iLen = strlen(pszDir);

	// 遍历依次查找每一级的目录文件
	for (i = 0; i < iLen; i++)
	{
		if ((pszDir[i] == '\\' || pszDir[i] == '/') && i != 0)
		{
			pszDir[i] = '\0';

			// 判断该目录是存在
			iRet = access(pszDir, 0);
			if (iRet != 0)
			{
				//printf("the %s is not exist\n",pszDir);
				iRet = my_mkdir(pszDir);
				if (iRet != 0)
				{
					//printf("Create %s file is failed\n",pszDir);
					return -1;
				}
			}
			else
			{
				//printf("the %s is exist\n",pszDir);
			}
			pszDir[i] = '/';
		}
	}

	iRet = my_mkdir(pszDir);
	free(pszDir);
	return iRet;
}

int MyLog::my_mkdir(char *pszDir)
{
#ifdef WIN32
	return mkdir(pszDir);
#endif
#ifdef LINUX
	return mkdir(pszDir, S_IRUSR | S_IWUSR | S_IXUSR | S_IRWXG | S_IRWXO);
#endif
#ifdef DVXWORK
	return mkdir(pszDir);
#endif
}

// 获得当前文件的文件路径 返回值  返回参数中文件的路径  其中filepath不含最后的\\信息
bool MyLog::find_last_of(char* absfile, char *filepath, int &len)
{
	int findindex = -1;
	for (int i = 0; i < (int)strlen(absfile); i++)
	{
		if (absfile[i] == '\\' || absfile[i] == '/') // 找到\\字符串了
		{
			findindex = i;
			continue;
		}
	}
	if (findindex == -1)
		return false;
	else
	{
		if (filepath == NULL)
		{
			filepath = new char[strlen(absfile)];
			memset(filepath, 0, strlen(absfile));
		}
		memcpy(filepath, absfile, findindex);
		return true;
	}
	return false;
}



// source内容以,分隔，查找innerbuf是否在buffer内容中，如果是返回对应的位置，否则返回-1值
int MyLog::findIndex(char *source, char *innerbuf)
{
	string buffer(source);
	// 首先按照,对buffer进行分隔，放到链表当中
	vector<string> sourcelist;
	size_t pos = 0, found = 0;
	while (found != string::npos)
	{
		found = buffer.find(",", pos);
		sourcelist.push_back(string(buffer, pos, found - pos));
		pos = found + 1;
	}
	// 分析innerbuf是否在source中
	for (int i = 0; i < (int)sourcelist.size(); i++)
	{
		if (strcmp(sourcelist[i].c_str(), innerbuf) == 0)
		{
			// 找到对应的内容
			return i + 1;
		}
	}
	return -1;
}


// 判断文件filename是否为type类型 多个后缀类型以,分隔
bool MyLog::decFileName(char *fileName, char *fileType)
{
	if (fileName == 0 || fileType == 0)
		return false;
	int len = strlen(fileName);
	for (int i = 0; i < len; i++)
	{
		if (fileName[i] == '.'&&i != (len - 1))
		{
			char type[40];
			memcpy(&type, &fileName[i + 1], len - i + 1);
			if (findIndex(fileType, type) > 0)
				return true;
		}
	}
	return false;
}
