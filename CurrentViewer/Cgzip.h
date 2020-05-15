#ifndef __CGZIP_TYPES_H
#define __CGZIP_TYPES_H

//#pragma once

#include <Windows.h>
#include "zlib.h"

class Cgzip
{
	FILE *fp;
	//gzFile gFile;
	char destfile[FILENAME_MAX];
	char buf[FILENAME_MAX];
	int read;

	std::string m_SorBuf;
	std::string m_DesBuf;

	int m_nInSize;
	int m_nOutSize;
	std::string m_strCompData;
	std::wstring m_strData;

	byte* m_XmlData;
	int m_nLen;
	int m_nType;
	int m_nOffice;
	int m_DlId;
public:
	Cgzip(void);
	~Cgzip(void);

	//파일
	bool CompressThreadCallback();	
	bool DCompressThreadCallback();
	//데이터
	bool CompressDataThreadCallback();
	bool DCompressDataThreadCallback();

	bool CompressXmlDataThreadCallback();
	void End();

	//파일
	bool Compress(char *input, char *output, int clevel);	
	bool DCompress(char *input);
	//데이터
	bool Compress(std::string strSouBuf, int clevel);
	bool Compress(byte* XmlData, int nLen, int office, int dlid, int nType);
	std::wstring DCompress(int nInSize, int nOutSize, std::string strCompData);

	std::wstring	OldMulitByteToWideChar(const char* pChar, int nLen);
};
#else
#endif