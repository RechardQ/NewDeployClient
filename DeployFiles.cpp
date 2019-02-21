#include "Header.h"
#include "MyTime.h"
#include "md5.h"
#include "MyLog.h"


long long int processLen;			//进度偏移量
char tailBuf[TAILBUFSIZE];			//存放粘连部分的buffer

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

	/* 设置关闭时缓冲处理方式
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
		Sleep(100); // 定时 
#endif
#ifdef linux
		usleep(100000);
#endif
#ifdef DVXWORK
		slepp(1 / 10);
#endif
	}

	// 监听 socket
	int ls = listen(server, 5);
	if (ls == -1)
	{
		//writeLog("deploy listen failed.\n");
		flag = false;
	}

	int AcceptSocket;
	sockaddr_in client;
	bool recvFlag;
	// 开始接收文件
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

		// 接受文件传输数据
		int RecvLen;
		char recvbuf[REVBUFSIZE] = ""; // 用于将接收的转换UTF8格式


		while (1)
		{
			// 判断是否开始接收文件 收到DeployStart表示开始接收，DeployEnd
			memset(recvbuf, 0, sizeof(recvbuf));
			//(在TCP里面使用recvfrom会不会不太好，目的ip的地址信息已经在accept中保存过了)
			RecvLen = recvfrom(AcceptSocket, recvbuf, REVBUFSIZE, 0, (sockaddr*)&addrSvr, &len);
			if (RecvLen <= 0)
			{
				int errorCode = WSAGetLastError();
				if (errorCode == EWOULDBLOCK || errorCode == ETIMEDOUT)
					continue;
				else {
					cout << "ERROR>>>>>>>>>>>>>>>>>Client recv error！Has been exit recv_process." << endl;
					break;
				}
			}
			m_time.CoutTime();
			//判断是否部署结束
			int serialflag = 0;
			memcpy((char*)&serialflag, recvbuf, 4);
			if (serialflag == -1) {
				cout << "deploy end!" << endl;
				break;
			}
			long long size = 0;//包的总长度
			memcpy((char*)&size, (recvbuf + SERINUMLENGTH + PATHLENGTH + MD5LENGTH), 8);


			char path[PATHLENGTH];
			char md5[MD5LENGTH];
			memset(path, 0, PATHLENGTH);
			memset(md5, 0, MD5LENGTH);
			memcpy(md5, (recvbuf + PATHLENGTH + SERINUMLENGTH), MD5LENGTH);

			// 转换为UTF-8格式
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
			//如果当前文件不存在，则通知服务器开始发送当前文件的报文
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
				//如果存在，MD5也一致，则通知服务器部署下一个文件
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
					//如果文件存在，MD5值不一致，则重新部署该文件
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
				// 判断文件夹是否存在，如果不存在则创建
				int iRet = access(filepath, 0);
				if (iRet != 0)
				{
					iRet = m_log.CreatDir(filepath);
					if (iRet != 0)
					{
						// 结束本次部署
						continue;
					}
				}
			}
			else
			{
				// 结束本次部署
				continue;
			}
			std::ofstream ofs;
			// 打开文件读写
			ofs.open(path, ios::binary);

			static int recveddatanum = 0;			//已接收的数据包长度
			static int tailDataLength = 0;			//粘包的长度			
			char* array_data;						//数据段
			bool deployEnd = false;					//当前文件部署结束标志位
			memset(tailBuf, 0, TAILBUFSIZE);

			if (recvFlag == true)
			{
				char recvdatabuf[REVBUFSIZE] = { 0 };
				while (1)
				{
					static int RecvDataLen;

					memset(recvdatabuf, 0, sizeof(recvdatabuf));
					RecvDataLen = recvfrom(AcceptSocket, recvdatabuf, REVBUFSIZE, 0, (sockaddr*)&addrSvr, &len);
					//捕获recvfrom的异常情况，捕获到信号中断和接收超时，则继续重新接收，否则退出当前接收
					if (RecvDataLen <= 0)
					{
						int errorCode = WSAGetLastError();
						if (errorCode == EWOULDBLOCK || errorCode == ETIMEDOUT)
							continue;
						else {
							cout << "ERROR>>>>>>>>>>>>>>>>>Client recv error！Has been exit recv_process." << endl;
							break;
						}
					}
					bool hasTailBuff = false;
					char * newBuff = NULL;
					int newBuffLen;
					//判断粘连buf中是否存在东西，存在则将其中的东西与收到的新的recvdatabuf进行拼接
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
						//即将解析的buf剩余长度是否足以解析一个报文头，足够解析则先解析一个头部
						if (newBuffLen - processLen >= sizeof(HeadPacket)) {
							HeadPacket head;
							memcpy((char*)&head, newBuff + processLen, sizeof(head));
							//解析完头部剩余的buf长度是否足够解析一个单步长度，足够则解析，不够则将其保留下来继续去接收新的报文
							if (newBuffLen - processLen - sizeof(head) >= head.dataSize) {
								processLen += sizeof(head);

								//可解析一次
								char* array_data;
								array_data = (char*)malloc(head.dataSize);
								memcpy(array_data, newBuff + processLen, head.dataSize);
								processLen += head.dataSize;
								//记录已接收的数据总长度
								recveddatanum += head.dataSize;
								//如果已收的报文字节长度等于报文的总长度，则该文件接收完毕，通知服务器部署下一个路径
								if (recveddatanum == head.totalSize) {
									ofs.write(array_data, head.dataSize);
									ofs.close();
									ofs.clear();
									//通知服务器该文件已经部署结束，开始传输下一个路径
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
								//如果偏移量和解析报文的总长度相等，则粘连buf中的内容已处理完毕
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

					//如果此时解析的buf不等于空并且标志位为true则需要释放开辟的buf内存
					if (newBuff != NULL && hasTailBuff)
						free(newBuff);
					//如果当前文件的部署结束标志为true，则跳出当前的while循环，去接收新的部署路径
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