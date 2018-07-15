// Com.cpp: implementation of the CCom class.
//
//////////////////////////////////////////////////////////////////////

/*
文件名：Com.cpp
用途：用于串口的读写。
日期：2004.09.01
版本：1.0
编制：严 华
*/
/*
版本 2.0
日期：2018.7.14
编制：卢敏


*/

#include "stdafx.h"
#include "Com.h"

//#define ME_TIME

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCom::CCom()
{
	m_pArray=NULL;
	m_isContinue=true;
	m_iReadSizeResult=0;
}

CCom::~CCom()
{
	if (m_pArray!=NULL) 
	{
		delete [] m_pArray;
	}
	CloseHandle(m_hCom);
}

bool CCom::InitComm(const CString &com,int baudrate)
{
	//初始化串口
	COMMTIMEOUTS commtimeouts;

	m_hCom=CreateFile("\\\\.\\"+com,GENERIC_READ | GENERIC_WRITE,0,
						 NULL,OPEN_EXISTING,FILE_FLAG_OVERLAPPED,0);

	if (m_hCom==INVALID_HANDLE_VALUE) 
	return false;
	DCB dcb={0};
	if (!GetCommState(m_hCom, &dcb)) 
	return false;

	dcb.BaudRate=baudrate;
	dcb.ByteSize=8;
	dcb.StopBits=ONESTOPBIT;
//	if((com == "COM2")|| (com =="COM3"))
//	{
//		dcb.Parity=ODDPARITY; //光电前端分机、接收分机
//	}
//	else
//	{
		dcb.Parity=NOPARITY;  //信号处理分机
//	}
	dcb.fBinary=TRUE;
	dcb.fDsrSensitivity=false;
	dcb.fParity=0;
	dcb.fInX=false;
	dcb.fOutX=false;
	dcb.fNull=false;
	dcb.fAbortOnError=false;
	dcb.fOutxCtsFlow=false;
	dcb.fOutxDsrFlow=false;
	dcb.fDtrControl=DTR_CONTROL_DISABLE;
	dcb.fRtsControl=RTS_CONTROL_DISABLE;

	BOOL res;
	res=SetCommState(m_hCom, &dcb);
	if (!res) 
	return false;

	commtimeouts.ReadIntervalTimeout =0;//MAXDWORD;
	commtimeouts.ReadTotalTimeoutMultiplier = 0;
	commtimeouts.ReadTotalTimeoutConstant = 0;
	commtimeouts.WriteTotalTimeoutMultiplier = 0;
	commtimeouts.WriteTotalTimeoutConstant = 0;

	res=SetCommTimeouts(m_hCom, &commtimeouts);
	if (!res) return false;

	PurgeComm(m_hCom, PURGE_RXCLEAR | PURGE_TXCLEAR | PURGE_RXABORT | PURGE_TXABORT); 

	return true;
}

bool CCom::Init(const CString &com,int baudrate,int readbufsize,HWND hwnd,int iName)
{
	bool res;

	m_hWnd=hwnd;
	m_iName=iName;

	if (readbufsize<=0) return false;

	m_iReadSize=readbufsize;//指定串口读取大小
	m_iReadBufferSize=readbufsize;//指定串口读取缓存大小

	m_pArray=new char[readbufsize];//开辟指定内存区域
	if (m_pArray==NULL) 
		return false; 

	res=InitComm(com,baudrate);//根据指定的名字以及波特率初始化串口

	return res;
}
	
void CCom::SetOwner(HWND hwnd)
{
	m_hWnd=hwnd;//指定句柄
}
	
bool CCom::SetReadSize(int size)
{
	//设置读取大小，如果新值小于原缓存大小，仅将大小赋给m_iReadSize，否则不但按新值指定串口读取大小及缓存大小
	//而且重新开辟指定的内存区域
	if (size<=0) return false;

	if (size<=m_iReadBufferSize)
	{
		m_iReadSize=size;
		return true;
	}

	m_iReadBufferSize=size;
	m_iReadSize=size;

	if (m_pArray!=NULL) 
	{
		delete [] m_pArray;
	}
	m_pArray=new char[size];
	if (m_pArray==NULL) 
	{
		return false;
	}
	return true;
}

/*******************************读取下位机返回的数据******************************/
extern BOOL b_exit_System;
extern int  nFibre_rxnum;//接收通过光纤从下位机传来的数据
int  characterSigTraEof(unsigned char c);        /*字符处理程序，用于判断伺服返回数据的有效性*/

UINT CCom::Read(void)
{
//	int         result;
//	int         i,size;//edit by johnson 080310-2 JZ
	HANDLE			hcom=m_hCom;
	
	DWORD		dwRes;
	DWORD		dwOvRes;
	DWORD		dwCommEvent;
	DWORD		dwStoredFlags;
	BOOL		isread;
	BOOL		iswait;
	OVERLAPPED	osStatus = {0};
	OVERLAPPED	osRead = {0};
    COMSTAT		ComStat;
	int			count=0;
	int			length;
	BOOL		is_timeout;
		
	dwStoredFlags =EV_RXCHAR;

	PurgeComm(hcom, PURGE_RXCLEAR | PURGE_TXCLEAR | PURGE_RXABORT | PURGE_TXABORT);

	if (!SetCommMask(hcom, dwStoredFlags)) 
	{
		AfxMessageBox("error in setcommmask !");
		return 0;
	}
	osStatus.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (osStatus.hEvent == NULL)
	{	
		CloseHandle(osStatus.hEvent);
		return 0;
	}

	osRead.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (osRead.hEvent == NULL)
	{	
		CloseHandle(osRead.hEvent);
		return 0;
	}
	
	isread=false;
	iswait=false;
	is_timeout=false;
	m_isContinue=true;

	while (m_isContinue&&(!b_exit_System))
	{
		if (is_timeout) 
		{
			if (!SetCommMask(hcom, dwStoredFlags)) 
			{
				AfxMessageBox("error in setcommmask !");
				return 0;
			}

			is_timeout=false;
		}
		if (WaitCommEvent(hcom, &dwCommEvent, &osStatus)) isread=true;
		else 
		{
			if (GetLastError()==ERROR_IO_PENDING) iswait=true;
			else 
			{
				CloseHandle(osStatus.hEvent);
				CloseHandle(osRead.hEvent);
				return 0;
			}
		}
		
		if (iswait)
		{
			dwRes = WaitForSingleObject(osStatus.hEvent,THREAD_TIME_WAIT);
			switch(dwRes)
			{
				case WAIT_OBJECT_0: 
					if (!GetOverlappedResult(hcom, &osStatus, &dwOvRes, FALSE))
					{
						CloseHandle(osStatus.hEvent);
						CloseHandle(osRead.hEvent);
						return 0;
					}
					else isread=true;
					break;
				
				case WAIT_TIMEOUT:
					is_timeout=true;
					break;
				
				default:
					CloseHandle(osStatus.hEvent);
					CloseHandle(osRead.hEvent);
					return 0;
			}
			
			iswait=false;
		}

		if (isread)
		{
			ClearCommError(hcom,&dwRes,&ComStat);
			
			length=ComStat.cbInQue;
			if ( (count+length) > m_iReadBufferSize) length=m_iReadBufferSize-count;

			ReadFile(hcom,m_pArray+count,length,&dwRes,&osRead);
			count+=length;

			if (count>=m_iReadSize && length!=0)
			{
				m_iReadSizeResult=count;					// 保留实际读到的字符数，可能大于需要读的字符数			
			
	
				/*if (m_iName == COM_SIGNALTRANSACT_NAME_CODE)  //串口，用于接收信号处理分机上报的数据
				{
					size=count;
					count=0;
					for (i=0;i<size;i++)
					{
						result=characterSigTraEof(m_pArray[i]);
						if (result>0)
						{
//							::SendMessage(m_hWnd,WM_COM_READ_FINISH,result,m_iName);
						}
					}				
				}*/
			}			
			isread=false;
		}
	}
	
	CloseHandle(osStatus.hEvent);
	CloseHandle(osRead.hEvent);
	return 0;
}

UINT CCom::Write(void)
{
	char			*array=m_pWriteArray;
	int				size=m_iWriteSize;
	HWND			hwnd=m_hWnd;
	HANDLE			hcom=m_hCom;

	OVERLAPPED		osWrite = {0};
	DWORD			dwWritten;
	DWORD			dwRes;
	
	osWrite.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (osWrite.hEvent == NULL) 
	{
		return 0;
	}
	
	if (WriteFile(hcom,array,size,&dwWritten,&osWrite))
	{
		CloseHandle(osWrite.hEvent);
		return 0;
	}

	if (GetLastError()!=ERROR_IO_PENDING) 
	{ 
		CloseHandle(osWrite.hEvent);
		return 0;
	}

	dwRes = WaitForSingleObject(osWrite.hEvent, INFINITE);
	switch(dwRes)
	{
		case WAIT_OBJECT_0:
			if (!GetOverlappedResult(hcom, &osWrite, &dwWritten, FALSE))
			{
				CloseHandle(osWrite.hEvent);
				return 0;
			}

			break;
		
		case WAIT_TIMEOUT:
			break;          
			
		default:
			break;
	}

	CloseHandle(osWrite.hEvent);

	return 0;
}

UINT CCom::RunWrite(LPVOID lpparam)
{
	CCom* This = (CCom *)(lpparam);

	This->Write();

    return 0;
}

UINT CCom::RunRead(LPVOID lpparam)
{
	CCom* This = (CCom *)(lpparam);

	This->Read();

    return 0;
}

void CCom::BeginRead(void)
{
	AfxBeginThread(RunRead,this);
}

void CCom::StopRead(void)
{
	m_isContinue=false;
}

void CCom::WriteData(char *array,int size)
{
	m_pWriteArray=array;
	m_iWriteSize=size;

	Write();
}
	
void CCom::WriteDataThread(char *array,int size)
{
	m_pWriteArray=array;
	m_iWriteSize=size;

	AfxBeginThread(RunWrite,this);
}
	
char * CCom::GetReadData(int &iSize)
{
	iSize=m_iReadSizeResult;

	return m_pArray;
}

BOOL CCom::ResetBaud(int baudrate)
{
	DCB dcb={0};
	if (!GetCommState(m_hCom, &dcb)) return false;
	
	dcb.BaudRate=baudrate;
	dcb.ByteSize=8;
	dcb.StopBits=ONESTOPBIT;
	dcb.Parity=ODDPARITY;//NOPARITY;
	dcb.fBinary=TRUE;
	dcb.fDsrSensitivity=false;
	dcb.fParity=0;
	dcb.fInX=false;
	dcb.fOutX=false;
	dcb.fNull=false;
	dcb.fAbortOnError=false;
	dcb.fOutxCtsFlow=false;
	dcb.fOutxDsrFlow=false;
	dcb.fDtrControl=DTR_CONTROL_DISABLE;
	dcb.fRtsControl=RTS_CONTROL_DISABLE;
	
	BOOL res;
	
	res=SetCommState(m_hCom, &dcb);
	if (!res) return false;
	return true;
}





















