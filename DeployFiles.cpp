#include "Header.h"
#include "MyTime.h"
#include "md5.h"
#include "MyLog.h"


long long int processLen;			//����ƫ����
char tailBuf[TAILBUFSIZE];			//���ճ�����ֵ�buffer

extern char* UTF8ToUniCode(const char* utf8, int srclen, int &len);
extern bool g_progFlag;
extern char serverIP[15];

void DeployFiles()
{
	sockaddr_in addrSvr;
#ifdef WIN32
	int len = sizeof(sockaddr_in);
#endif
#ifdef linux
	socklen_t len = sizeof(sockaddr_in);
#endif
#ifdef DVXWORK
	int len = sizeof(sockaddr_in);
#endif
	int server;
	bool flag = true;
	MyTime m_time;
	MyLog m_log;

	server = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (server < 0)
	{
		//writeLog("deploy create socket failed.\n");
		cout << "deploy create socket failed." << endl;
		flag = false;
	}

	sockaddr_in service;
	service.sin_family = AF_INET;
	service.sin_addr.s_addr = htonl(INADDR_ANY);
	service.sin_port = htons(DEPLOYPORT);

	/* ���ùر�ʱ���崦��ʽ
	struct linger opt;
	opt.l_onoff=1;
	opt.l_linger = 0;
	if(flag==true && setsockopt(server, SOL_SOCKET, SO_LINGER, (char *)&opt, sizeof(linger)) < 0)
	{
	printf( "deploy setLINGER failed.\n" );
	flag = false;
	}
	*/

	int tcp_nodelay = 1;

	if (setsockopt(server, SOL_SOCKET, SO_KEEPALIVE, (char*)&tcp_nodelay, sizeof(int)) < 0)
	{
		//writeLog("deploy set alive failed.\n");
	}
	if (setsockopt(server, IPPROTO_TCP, TCP_NODELAY, (char*)&tcp_nodelay, sizeof(int)) < 0)
	{
		//writeLog("deploy set delay failed.\n");
	}


	if (flag == true && bind(server, (sockaddr *)&service, sizeof(sockaddr_in)) < 0)
	{
		//writeLog("deploy bind failed.\n");
		flag = false;
#ifdef WIN32
		Sleep(100); // ��ʱ 
#endif
#ifdef linux
		usleep(100000);
#endif
#ifdef DVXWORK
		slepp(1 / 10);
#endif
	}

	// ���� socket
	int ls = listen(server, 5);
	if (ls == -1)
	{
		//writeLog("deploy listen failed.\n");
		flag = false;
	}

	int AcceptSocket;
	sockaddr_in client;
	bool recvFlag;
	// ��ʼ�����ļ�
	while (flag && g_progFlag)
	{
#ifdef WIN32
		int addrsize = sizeof(client);
#endif
#ifdef linux
		socklen_t addrsize = sizeof(client);
#endif
#ifdef DVXWORK
		int addrsize = sizeof(client);
#endif
		AcceptSocket = accept(server, (sockaddr*)&client, &addrsize);
		if (AcceptSocket > 10000)
		{
			continue;
		}

		// �����ļ���������
		int RecvLen;
		char recvbuf[REVBUFSIZE] = ""; // ���ڽ����յ�ת��UTF8��ʽ


		while (1)
		{
			// �ж��Ƿ�ʼ�����ļ� �յ�DeployStart��ʾ��ʼ���գ�DeployEnd
			memset(recvbuf, 0, sizeof(recvbuf));
			//(��TCP����ʹ��recvfrom�᲻�᲻̫�ã�Ŀ��ip�ĵ�ַ��Ϣ�Ѿ���accept�б������)
			RecvLen = recvfrom(AcceptSocket, recvbuf, REVBUFSIZE, 0, (sockaddr*)&addrSvr, &len);
			if (RecvLen <= 0)
			{
				int errorCode = WSAGetLastError();
				if (errorCode == EWOULDBLOCK || errorCode == ETIMEDOUT)
					continue;
				else {
					cout << "ERROR>>>>>>>>>>>>>>>>>Client recv error��Has been exit recv_process." << endl;
					break;
				}
			}
			m_time.CoutTime();
			//�ж��Ƿ������
			int serialflag = 0;
			memcpy((char*)&serialflag, recvbuf, 4);
			if (serialflag == -1) {
				cout << "deploy end!" << endl;
				break;
			}
			long long size = 0;//�����ܳ���
			memcpy((char*)&size, (recvbuf + SERINUMLENGTH + PATHLENGTH + MD5LENGTH), 8);


			char path[PATHLENGTH];
			char md5[MD5LENGTH];
			memset(path, 0, PATHLENGTH);
			memset(md5, 0, MD5LENGTH);
			memcpy(md5, (recvbuf + PATHLENGTH + SERINUMLENGTH), MD5LENGTH);

			// ת��ΪUTF-8��ʽ
#ifdef WIN32
			char *convertBuf = UTF8ToUniCode(recvbuf, RecvLen, RecvLen);
			memcpy(recvbuf, convertBuf, RecvLen);
			delete convertBuf;
#endif
#ifdef linux
			memcpy(recvbuf, recvbuf1, recvLenTemp);
			RecvLen = recvLenTemp;
#endif
#ifdef DVXWORK
			memcpy(recvbuf, recvbuf1, recvLenTemp);
			RecvLen = recvLenTemp;
#endif
			memcpy(path, (recvbuf + SERINUMLENGTH), PATHLENGTH);
			cout << "Start Deploy>>>>>> " << path << endl;


			DeployPackageEntity package(0, path, md5);
			FILE* pFile = fopen(package.targetPath, "r");
			//�����ǰ�ļ������ڣ���֪ͨ��������ʼ���͵�ǰ�ļ��ı���
			if (NULL == pFile)
			{
				char requestFlag[16];
				memset(requestFlag, 0, 16);
				strcpy(requestFlag, "t");
				send(AcceptSocket, (const char*)requestFlag, strlen(requestFlag), 0);
				recvFlag = true;
				//fclose(pFile);
			}
			else
			{
				//������ڣ�MD5Ҳһ�£���֪ͨ������������һ���ļ�
				MD5_CTX mdvalue;
				char md5temp[34];
				memset(md5temp, 0, 34);
				mdvalue.GetFileMd5(md5temp, package.targetPath);
				if (0 == strcmp(md5temp, package.MD5Value))
				{
					char requestFlag[16];
					memset(requestFlag, 0, 16);
					strcpy(requestFlag, "f");
					send(AcceptSocket, (const char*)requestFlag, strlen(requestFlag), 0);
					recvFlag = false;
					fclose(pFile);
					continue;
				}
				else
				{
					//����ļ����ڣ�MD5ֵ��һ�£������²�����ļ�
					fclose(pFile);
					//pFile = fopen(package.targetPath,"w");
					char requestFlag[16];
					memset(requestFlag, 0, 16);
					strcpy(requestFlag, "t");
					send(AcceptSocket, (const char*)requestFlag, strlen(requestFlag), 0);
					recvFlag = true;
				}
			}
			if (size == 0)
			{
				char requestFlag[16];
				memset(requestFlag, 0, 16);
				strcpy(requestFlag, "c");
				send(AcceptSocket, (const char*)requestFlag, strlen(requestFlag), 0);
				continue;
			}

			char *filepath = new char[256];
			int filepathlen = 0;
			memset(filepath, 0, 256);
			//cout << "Start Deploy>>>>>> " << path << endl;
			if (m_log.find_last_of(path, filepath, filepathlen) == true)
			{
				// �ж��ļ����Ƿ���ڣ�����������򴴽�
				int iRet = access(filepath, 0);
				if (iRet != 0)
				{
					iRet = m_log.CreatDir(filepath);
					if (iRet != 0)
					{
						// �������β���
						continue;
					}
				}
			}
			else
			{
				// �������β���
				continue;
			}
			std::ofstream ofs;
			// ���ļ���д
			ofs.open(path, ios::binary);

			static int recveddatanum = 0;			//�ѽ��յ����ݰ�����
			static int tailDataLength = 0;			//ճ���ĳ���			
			char* array_data;						//���ݶ�
			bool deployEnd = false;					//��ǰ�ļ����������־λ
			memset(tailBuf, 0, TAILBUFSIZE);

			if (recvFlag == true)
			{
				char recvdatabuf[REVBUFSIZE] = { 0 };
				while (1)
				{
					static int RecvDataLen;

					memset(recvdatabuf, 0, sizeof(recvdatabuf));
					RecvDataLen = recvfrom(AcceptSocket, recvdatabuf, REVBUFSIZE, 0, (sockaddr*)&addrSvr, &len);
					//����recvfrom���쳣����������ź��жϺͽ��ճ�ʱ����������½��գ������˳���ǰ����
					if (RecvDataLen <= 0)
					{
						int errorCode = WSAGetLastError();
						if (errorCode == EWOULDBLOCK || errorCode == ETIMEDOUT)
							continue;
						else {
							cout << "ERROR>>>>>>>>>>>>>>>>>Client recv error��Has been exit recv_process." << endl;
							break;
						}
					}
					bool hasTailBuff = false;
					char * newBuff = NULL;
					int newBuffLen;
					//�ж�ճ��buf���Ƿ���ڶ��������������еĶ������յ����µ�recvdatabuf����ƴ��
					if (tailDataLength != 0) {
						newBuffLen = RecvDataLen + tailDataLength;
						newBuff = (char *)malloc(newBuffLen);
						memcpy(newBuff, tailBuf, tailDataLength);
						memcpy(newBuff + tailDataLength, recvdatabuf, RecvDataLen);
						hasTailBuff = true;
					}
					else {
						newBuff = recvdatabuf;
						newBuffLen = RecvDataLen;
					}
					processLen = 0;
					do {
						//����������bufʣ�೤���Ƿ����Խ���һ������ͷ���㹻�������Ƚ���һ��ͷ��
						if (newBuffLen - processLen >= sizeof(HeadPacket)) {
							HeadPacket head;
							memcpy((char*)&head, newBuff + processLen, sizeof(head));
							//������ͷ��ʣ���buf�����Ƿ��㹻����һ���������ȣ��㹻��������������䱣����������ȥ�����µı���
							if (newBuffLen - processLen - sizeof(head) >= head.dataSize) {
								processLen += sizeof(head);

								//�ɽ���һ��
								char* array_data;
								array_data = (char*)malloc(head.dataSize);
								memcpy(array_data, newBuff + processLen, head.dataSize);
								processLen += head.dataSize;
								//��¼�ѽ��յ������ܳ���
								recveddatanum += head.dataSize;
								//������յı����ֽڳ��ȵ��ڱ��ĵ��ܳ��ȣ�����ļ�������ϣ�֪ͨ������������һ��·��
								if (recveddatanum == head.totalSize) {
									ofs.write(array_data, head.dataSize);
									ofs.close();
									ofs.clear();
									//֪ͨ���������ļ��Ѿ������������ʼ������һ��·��
									char requestFlag[16];
									memset(requestFlag, 0, 16);
									strcpy(requestFlag, "c");
									int realSendLen = send(AcceptSocket, (const char*)requestFlag, strlen(requestFlag), 0);
									if (realSendLen <= 0) {
										cout << "Send request error" << endl;
										break;
									}

									tailDataLength = 0;
									recveddatanum = 0;
									deployEnd = true;
									break;
								}
								else {
									ofs.write(array_data, head.dataSize);
								}
								//���ƫ�����ͽ������ĵ��ܳ�����ȣ���ճ��buf�е������Ѵ������
								if (processLen == newBuffLen) {
									tailDataLength = 0;
									break;
								}
							}
							else {
								memset(tailBuf, 0, TAILBUFSIZE);
								int leftLen = newBuffLen - processLen;
								memcpy(tailBuf, newBuff + processLen, leftLen);
								tailDataLength = leftLen;
								break;
							}
						}
						else {
							memset(tailBuf, 0, TAILBUFSIZE);
							int leftLen = newBuffLen - processLen;
							memcpy(tailBuf, newBuff + processLen, leftLen);
							tailDataLength = leftLen;
							break;
						}
					} while (1);

					//�����ʱ������buf�����ڿղ��ұ�־λΪtrue����Ҫ�ͷſ��ٵ�buf�ڴ�
					if (newBuff != NULL && hasTailBuff)
						free(newBuff);
					//�����ǰ�ļ��Ĳ��������־Ϊtrue����������ǰ��whileѭ����ȥ�����µĲ���·��
					if (deployEnd == true) {
						free(filepath);
						filepathlen = 0;
						break;
					}
				}
			}
		}
	}
#ifdef WIN32
	closesocket(server);
	WSACleanup();
#endif
#ifdef linux
	close(server);
#endif
#ifdef DVXWORK
	close(server);
#endif
}