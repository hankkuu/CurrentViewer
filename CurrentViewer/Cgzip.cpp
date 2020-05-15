#include "stdafx.h"
#include "Cgzip.h"


//���� 
DWORD WINAPI CompressThreadCallback(LPVOID parameter)
{
	Cgzip *Owner = (Cgzip*)parameter;
	Owner->CompressThreadCallback();

	return 0;
}

DWORD WINAPI DCompressThreadCallback(LPVOID parameter)
{
	Cgzip *Owner = (Cgzip*)parameter;
	Owner->DCompressThreadCallback();

	return 0;
}

//Data 
DWORD WINAPI CompressDataThreadCallback(LPVOID parameter)
{
	Cgzip *Owner = (Cgzip*)parameter;
	Owner->CompressDataThreadCallback();

	return 0;
}

DWORD WINAPI CompressXmlDataThreadCallback(LPVOID parameter)
{
	Cgzip *Owner = (Cgzip*)parameter;
	Owner->CompressXmlDataThreadCallback();

	return 0;
}

DWORD WINAPI DCompressDataThreadCallback(LPVOID parameter)
{
	Cgzip *Owner = (Cgzip*)parameter;
	Owner->DCompressDataThreadCallback();

	return 0;
}

Cgzip::Cgzip(void)
{
	fp = NULL;
	read = 0;
	memset(destfile, 0, sizeof(FILENAME_MAX));
	memset(buf, 0, sizeof(FILENAME_MAX));

	m_nLen = 0;
	m_XmlData = NULL;
	m_nType = 0;
	m_nOffice = 0;
	m_DlId = 0;

	//destfile = "D:\\Test\\TestSampleZip\\����(4310).zip";
}

Cgzip::~Cgzip(void)
{
}

//���� ����
bool Cgzip::Compress(char *input, char *output, int clevel)
{
// 	End();
// 	
// 	//������ ���� ����
// 	fp = fopen(input, "rb");
// 	if(fp == NULL)
// 		return false;
// 
// 	//������ ���� ����
// 	gFile = gzopen(output, "wb");
// 	if(gFile == NULL)
// 		return false;
// 
// 	//���� ���� ���� (���� 1~9���� ����) 1:�������, 9:�ִ����
// 	if(clevel >= 9) clevel = 9;
// 	if(clevel <= 1) clevel = 1;
// 	if(gzsetparams(gFile, clevel, Z_DEFAULT_STRATEGY) != Z_OK)
// 		return false;
// 
// 	//���� ���� ������ ���� �� ���� ����
// 	HANDLE WorkerThread = CreateThread(NULL, 0, ::CompressThreadCallback, this, 0, NULL);
// 	WaitForSingleObject(WorkerThread, INFINITE);
	return true;
}

//�޸� ����
bool Cgzip::Compress(std::string strSouBuf, int clevel)
{
	m_SorBuf = strSouBuf;

	//���� ���� ������ ���� �� ���� ����
	HANDLE WorkerThread = CreateThread(NULL, 0, ::CompressDataThreadCallback, this, 0, NULL);
	WaitForSingleObject(WorkerThread, INFINITE);
	return true;
}

bool Cgzip::Compress(byte* XmlData, int nLen, int office, int dlid, int nType)
{
	m_XmlData = XmlData;
	m_nLen = nLen;
	m_nType = nType;
	m_nOffice = office;
	m_DlId = dlid;
	//���� ���� ������ ���� �� ���� ����
	HANDLE WorkerThread = CreateThread(NULL, 0, ::CompressXmlDataThreadCallback, this, 0, NULL);
	WaitForSingleObject(WorkerThread, INFINITE);
	return true;
}

//���� ���� ����
bool Cgzip::DCompress(char *input)
{
// 	End();
// 
// 	//������ ���ϸ� 
// 	memset(destfile, 0, sizeof(FILENAME_MAX));
// 	strncpy(destfile, input, strlen(input)-3);
// 	destfile[strlen(input)-3] = 0;
// 
// 	//���� ���� ���� ���� ����
// 	fp = fopen(destfile, "wb");
// 	if(fp == NULL)
// 		return false;
// 
// 	gFile = gzopen(input, "rb");
// 	if(gFile == NULL)
// 		return false;
// 
// 	//���� ���� ������ ���� �� ���� ����
// 	HANDLE WorkerThread = CreateThread(NULL, 0, ::DCompressThreadCallback, this, 0, NULL);
// 	WaitForSingleObject(WorkerThread, INFINITE);
	return true;
}

//�޸� ���� ����
std::wstring Cgzip::DCompress(int nInSize, int nOutSize, std::string strCompData)
{
	m_nInSize = nInSize;
	m_nOutSize = nOutSize;
	m_strCompData = strCompData;

	//���� ���� ������ ���� �� ���� ����
	HANDLE WorkerThread = CreateThread(NULL, 0, ::DCompressDataThreadCallback, this, 0, NULL);
	WaitForSingleObject(WorkerThread, INFINITE);
	return m_strData;
}

void Cgzip::End()
{
	if(fp) fclose(fp);
	fp = NULL;
	read = 0;
	memset(destfile, 0, sizeof(FILENAME_MAX));
	memset(buf, 0, sizeof(FILENAME_MAX));
}

bool Cgzip::CompressThreadCallback()
{
// 	while(read = fread(buf, sizeof(char), FILENAME_MAX, fp))
// 	{
// 		if(gzwrite(gFile, buf, read) < 0)
// 			return false;
// 	}
// 	gzclose(gFile);
	return true;
}

bool Cgzip::DCompressThreadCallback()
{
// 	while(read = gzread(gFile, buf, FILENAME_MAX))
// 	{
// 		if(fwrite(buf, sizeof(char), read, fp) < 0)
// 		{
// 			gzclose(gFile);
// 			fclose(fp);
// 			return false;
// 		}
// 	}
// 	gzclose(gFile);
// 	fclose(fp);

	return true;
}

bool Cgzip::CompressDataThreadCallback()
{
	int nLen = m_SorBuf.length()+1;
	byte* stream = new byte[nLen];
	memcpy(stream, m_SorBuf.c_str(), nLen);
	stream[m_SorBuf.length()] = '\0';

	int OUTSIZE =(unsigned long)1.001*(nLen+12) + 1;
	byte* CompressData = new byte[OUTSIZE];
	memset( CompressData, 0, OUTSIZE );

	compress2( CompressData,(uLongf*)&OUTSIZE, stream, nLen, 9 );

	//�ʿ��Ұ�� �Ŵ��� Ŭ������ �����ᵵ�ǰ�..
// 	CZedManager::GetInstance()->SetZedData(CompressData);	
// 	CZedManager::GetInstance()->SetZedInSize(nLen);
// 	CZedManager::GetInstance()->SetZedOutSize(OUTSIZE);

	return true;
}

bool Cgzip::CompressXmlDataThreadCallback()
{
// 	int nLen = m_SorBuf.length()+1;
// 	byte* stream = new byte[nLen];
// 	memcpy(stream, m_SorBuf.c_str(), nLen);
// 	stream[m_SorBuf.length()] = '\0';

	int OUTSIZE =(unsigned long)1.001*(m_nLen+12) + 1;
	byte* CompressData = new byte[OUTSIZE];
	memset( CompressData, 0, OUTSIZE );

	compress2( CompressData,(uLongf*)&OUTSIZE, m_XmlData, m_nLen, 9 );

	

// 	if(m_nType == 0)
// 		DirectedNetworkInstance->SetAutoXml(CompressData,m_nOffice, m_DlId, OUTSIZE, m_nLen);
// 	else if(m_nType == 1)
// 		DirectedNetworkInstance->SetAutoManualXml(CompressData,m_nOffice, m_DlId, OUTSIZE, m_nLen);
// 	else if(m_nType == 3)
// 		DirectedNetworkInstance->SetAllXml(CompressData,m_nOffice, m_DlId, OUTSIZE, m_nLen);
	//�ʿ��Ұ�� �Ŵ��� Ŭ������ �����ᵵ�ǰ�..
	// 	CZedManager::GetInstance()->SetZedData(CompressData);	
	// 	CZedManager::GetInstance()->SetZedInSize(nLen);
	// 	CZedManager::GetInstance()->SetZedOutSize(OUTSIZE);

	return true;
}

bool Cgzip::DCompressDataThreadCallback()
{
	byte* DCompressData = new byte[m_nOutSize];
	memcpy(DCompressData, m_strCompData.c_str(), m_nOutSize);

	byte* stream = new byte[m_nInSize+1];
	uncompress( stream, (uLongf*)&m_nInSize, DCompressData, m_nOutSize );

	std::string strZedData(stream, stream + m_nInSize);

	// ���⼭ ����ȯ
	m_strData =OldMulitByteToWideChar(strZedData.c_str(), strZedData.length());
	
	//m_strZedData = strZedData;
	
	if(DCompressData)
	{
		delete DCompressData;
		DCompressData = NULL;
	}
	if(stream)
	{
		delete stream ;
		stream = NULL;
	}
	
	strZedData.clear();

	return true;
}

std::wstring	Cgzip::OldMulitByteToWideChar(const char* pChar, int nLen)
{
	if (!pChar)
		return 0;

	// copy
	//char sChar[256];
	//strcpy(sChar, pChar);

	// length
	int nLength		= MultiByteToWideChar(CP_ACP, 0, pChar, nLen, NULL, NULL);

	// alloc
	BSTR bstr		= SysAllocStringLen(NULL, nLength);

	// convert
	MultiByteToWideChar(CP_ACP, 0, pChar, nLen, bstr, nLength);

	// save
	std::wstring wstrConverted = (LPWSTR)bstr;

	// free
	SysFreeString(bstr);


	return wstrConverted;
}