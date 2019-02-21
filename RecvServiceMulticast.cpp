#include "Header.h"
extern char serverIP[15];
extern char hostIP[15];
extern bool g_progFlag;

bool AnalysisFile()
{
	bool rState;//返回状态
	FILE *file;
	char ln[80];
	fopen_s(&file, "returnpingdata.txt", "r");

	fgets(ln, 80, file);//读入空行，舍弃
	fgets(ln, 80, file);//读入ping信息，舍弃
	fgets(ln, 80, file);//读入ping对象返回值，用来分析

	string data = ln;
	int iPos = data.find("=");
	data = data.substr(iPos + 1, 3);//截取字符串返回字节数
	int  n = atoi(data.c_str());
	rState = n > 0;
	fclose(file);
	return rState;
}

void RecvServiceMulticast()
{
	int r;
	char recvline[20];
	struct sockaddr_in presaddr;

#ifdef WIN32
	WSADATA wsa;
	WSAStartup(MAKEWORD(2, 2), &wsa); //initial Ws2_32.dll by a process,
#endif
	bool flag = true;
	SOCKET sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (INVALID_SOCKET == sockfd)
	{
		cout << "create serviceBroadcast failed!";
		flag = false;
		return;
	}
	// 接收广播信息
	/*bool bOpt = true;
	if(setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, (char*)&bOpt, sizeof(bOpt)) < 0)
	{
	printf( "set serviceBroadcast failed %d.\n" );
	return;
	}*/

	struct sockaddr_in saddr;
	memset(&saddr, 0, sizeof(saddr));
	saddr.sin_family = AF_INET;
	saddr.sin_port = htons(BOARDCASTPORT);
	saddr.sin_addr.s_addr = inet_addr(hostIP);


	// 将套接字绑定到本地地址结构
	bind(sockfd, (struct sockaddr *)&saddr, sizeof(saddr));
	if (bind < 0)
	{
		cout << "bind failed" << endl;
		flag = false;
	}

	// 加入组播地址
	ip_mreq mreq;
	mreq.imr_interface.S_un.S_addr = inet_addr(hostIP);     //本地某一网络设备接口的IP地址。
	mreq.imr_multiaddr.S_un.S_addr = inet_addr("224.10.10.15"); //组播组的IP地址。
	int ret = setsockopt(sockfd, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char*)&mreq, sizeof(mreq));
	//cout << ret << endl;
	if (ret < 0)
	{
		cout << "setsockopt error" << "----------请重新配置ip" << endl;
		flag = false;
	}


	// 	if(flag==true && bind(sockfd, (struct sockaddr*)&saddr, sizeof(saddr)) < 0)
	// 	{
	// 		writeLog("bind serviceBroadcast failed.\n");
	// 		flag = false;
	// 	}
#ifdef WIN32
	int len = sizeof(sockaddr_in);
#endif
#ifdef linux
	socklen_t len = sizeof(sockaddr_in);
#endif
#ifdef DVXWORK
	int len = sizeof(sockaddr_in);
#endif
	char sign[5];
	while (flag&&g_progFlag)
	{
		int nNetTimeout = 10000; //10秒
		//接收时限
		int back = setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (char *)&nNetTimeout, sizeof(int));
		if (back)
		{
			cout << "setsockopt error" << "----------设置延时错误" << endl;
			closesocket(sockfd);
			WSACleanup();
			RecvServiceMulticast();
			return;
		}
		memset(recvline, 0, 20);
		r = recvfrom(sockfd, recvline, sizeof(recvline), 0, (struct sockaddr*)&presaddr, &len);
		if (r <= 0)
		{
			cout << "网络连接错误，请检查配置文件中的ip" << endl;
			closesocket(sockfd);
			WSACleanup();
			RecvServiceMulticast();
			return;
		}

		memset(sign, 0, 5);
		// 获得报文标识
		memcpy(sign, recvline, 4);
		if (strcmp(sign, "S101") == 0)
		{
			// 获得服务器的IP地址
			memcpy(serverIP, recvline + 4, r - 4);

			//开放100,防止数组越界
			char frontstr[100] = "cmd /c ping ";
			strncat(frontstr, serverIP, sizeof(serverIP));
			char afterstr[100] = " -n 1 -w 1000 >returnpingdata.txt";
			strncat(frontstr, afterstr, sizeof(afterstr));

			WinExec((char*)frontstr, SW_HIDE);
			Sleep(1000);//等待1000ms
			bool returndata = AnalysisFile();
			if (returndata == false)
			{
				memset(serverIP, 0, 15);
			}
			else if (returndata == true)
			{
				cout << "server_IP: " << serverIP << "   connect success!" << endl;
				memset(frontstr, 0, sizeof(frontstr));
				break;
			}		
		}

	}
#ifdef WIN32
	closesocket(sockfd);
	WSACleanup();
#endif
#ifdef linux
	close(sockfd);
#endif
#ifdef DVXWORK
	close(sockfd);
#endif
}