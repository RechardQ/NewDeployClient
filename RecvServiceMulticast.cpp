#include "Header.h"

extern char serverIP[15];
extern char hostIP[15];
extern bool g_progFlag;



bool AnalysisFile()
{
	bool rState;//����״̬
	FILE *file;
	char ln[80];
#ifdef WIN32
	fopen_s(&file, "returnpingdata.txt", "r");
#endif
#ifdef LINUX
	//fopen_s(&file,"w");
	return false;
#endif
#ifdef DVXWORK
	/*char filePath[128];
	memset(filePath,0,128);
	string path = "/ata0a/deployClient/NewDeployClient/returnpingdata.txt";
	strcpy(filePath,path.c_str());
	FILE *fp = fopen(filePath,"w+");
	if(fp == NULL)
		return false;*/
#endif

	fgets(ln, 80, file);//������У�����
	fgets(ln, 80, file);//����ping��Ϣ������
	fgets(ln, 80, file);//����ping���󷵻�ֵ����������

	string data = ln;
	int iPos = data.find("=");
	data = data.substr(iPos + 1, 3);//��ȡ�ַ��������ֽ���
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
#ifdef WIN32
		cout << "create serviceBroadcast failed!";
#endif
#ifdef LINUX
		printf("create serviceBroadcast failed!\n");
#endif
#ifdef DVXWORK
		logMsg("create serviceBroadcast failed!\n", 0, 0, 0, 0, 0, 0);
#endif
		flag = false;
		return;
	}

	struct sockaddr_in saddr;
	memset(&saddr, 0, sizeof(saddr));
	saddr.sin_family = AF_INET;
	saddr.sin_port = htons(BOARDCASTPORT);

#ifdef WIN32
	saddr.sin_addr.s_addr = inet_addr(hostIP);
#endif
#ifdef LINUX
	saddr.sin_addr.s_addr = htonl(INADDR_ANY);
#endif
#ifdef DVXWORK
	saddr.sin_addr.s_addr = htonl(INADDR_ANY);
#endif

	// ���׽��ְ󶨵����ص�ַ�ṹ
	bind(sockfd, (struct sockaddr *)&saddr, sizeof(saddr));
	if (bind < 0)
	{
		cout << "bind failed" << endl;
		flag = false;
	}

	// �����鲥��ַ
	ip_mreq mreq;
	mreq.imr_interface.S_un.S_addr = inet_addr(hostIP);     //����ĳһ�����豸�ӿڵ�IP��ַ��
	mreq.imr_multiaddr.S_un.S_addr = inet_addr("224.10.10.15"); //�鲥���IP��ַ��
	int ret = setsockopt(sockfd, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char*)&mreq, sizeof(mreq));
	if (ret < 0)
	{
#ifdef WIN32
		cout << "setsockopt error----------����������ip" << endl;
#endif
#ifdef LINUX
		cout << "setsockopt error----------����������ip" << endl;
#endif
#ifdef DVXWORK
		logMsg("setsockopt error----------����������ip\n", 0, 0, 0, 0, 0, 0);
#endif
		flag = false;
	}

#ifdef WIN32
	int len = sizeof(sockaddr_in);
#endif
#ifdef LINUX
	socklen_t len = sizeof(sockaddr_in);
#endif
#ifdef DVXWORK
	int len = sizeof(sockaddr_in);
#endif
	char sign[5];
	while (flag&&g_progFlag)
	{
#ifdef WIN32
		int nNetTimeout = 10000; //10��
		//����ʱ��
		int back = setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (char *)&nNetTimeout, sizeof(int));
#endif
#ifdef LINUX
		struct timeval timeout = { 10,0 };// 10��
		//����ʱ��
		int back = setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(timeval));
#endif
#ifdef DVXWORK
		struct timeval timeout = { 10,0 };// 10��
		//����ʱ��
		int back = setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(timeval));
#endif

#ifdef WIN32
		if (back)
		{
			printf("win setsockopt error----------������ʱ����\n");
			closesocket(sockfd);
			WSACleanup();
			RecvServiceMulticast();
			return;
		}
#endif
#ifdef LINUX
		if (back < 0)
		{
			cout << "linux setsockopt error----------������ʱ����" << back << endl;
			close(sockfd);
			RecvServiceMulticast();
			return;
		}
#endif
#ifdef DVXWORK
		if (back)
		{
			logMsg("vxworks setsockopt error----------set timeo error\n", 0, 0, 0, 0, 0, 0);
			close(sockfd);
			RecvServiceMulticast();
			return;
		}
#endif
		memset(recvline, 0, 20);
		r = recvfrom(sockfd, recvline, sizeof(recvline), 0, (struct sockaddr*)&presaddr, &len);
		if (r <= 0)
		{
#ifdef WIN32
			cout << "�������Ӵ������������ļ��е�ip" << endl;
			closesocket(sockfd);
			WSACleanup();
#endif
#ifdef LINUX
			cout << "δ���յ��������鲥����Ϣ" << endl;
			close(sockfd);
#endif
#ifdef DVXWORK
			logMsg("no recv server info\n", 0, 0, 0, 0, 0, 0);
			close(sockfd);
#endif
			RecvServiceMulticast();
			return;
		}

		memset(sign, 0, 5);
		// ��ñ��ı�ʶ
		memcpy(sign, recvline, 4);
		if (strcmp(sign, "S101") == 0)
		{
			// ��÷�������IP��ַ
			memcpy(serverIP, recvline + 4, r - 4);
#ifdef WIN32
			//����100,��ֹ����Խ��
			char frontstr[100] = "cmd /c ping ";
			strncat(frontstr, serverIP, sizeof(serverIP));
			char afterstr[100] = " -n 1 -w 1000 >returnpingdata.txt";
			strncat(frontstr, afterstr, sizeof(afterstr));

			WinExec((char*)frontstr, SW_HIDE);
			Sleep(1000);//�ȴ�1000ms
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
#endif
#ifdef LINUX
			/*char frontstr[50] = "cmd /c ping ";
			strncat(frontstr,serverIP,sizeof(serverIP));
			char afterstr[50] = " -n 1 -w 1000 >returnpingdata.txt";
			strncat(frontstr,afterstr,sizeof(afterstr));
			WinExec((char*)frontstr, SW_HIDE);
			Sleep(1000);//�ȴ�1000ms
			*/

#endif
#ifdef DVXWORK

#endif
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