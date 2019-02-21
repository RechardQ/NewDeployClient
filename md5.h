/******************************************************************* 
 *  2018-2019 Company NanJing RenGu 
 *  All rights reserved. 
 *   
 *  文件名称: md5.h
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

#ifndef MD5_H
#define MD5_H

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <io.h>
#endif


#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <string.h>
#include <vector>
#include <iostream>
#include <fstream>

using namespace std;

/* MD5 Class. */
class MD5_CTX {
public:
	MD5_CTX();
	virtual ~MD5_CTX();
	bool GetFileMd5(char *pMd5,  const char *pFileName);
private:

	unsigned long int state[4];					/* state (ABCD) */
	unsigned long int count[2];					/* number of bits, modulo 2^64 (lsb first) */
	unsigned char buffer[64];       /* input buffer */
	unsigned char PADDING[64];		/* What? */

private:
	void MD5Init ();
	void MD5Update( unsigned char *input, unsigned int inputLen);
	void MD5Final (unsigned char digest[16]);
	void MD5Transform (unsigned long int state[4], unsigned char block[64]);
	void MD5_memcpy (unsigned char* output, unsigned char* input,unsigned int len);
	void Encode (unsigned char *output, unsigned long int *input,unsigned int len);
	void Decode (unsigned long int *output, unsigned char *input, unsigned int len);
	void MD5_memset (unsigned char* output,int value,unsigned int len);
};

#endif //MD5_H
