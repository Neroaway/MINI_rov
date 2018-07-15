// Com.h: interface for the CCom class.
//
//////////////////////////////////////////////////////////////////////


#if !defined(AFX_COM_H__33845A38_1B54_4E40_94A9_3544ED62CA26__INCLUDED_)
#define AFX_COM_H__33845A38_1B54_4E40_94A9_3544ED62CA26__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define THREAD_TIME_WAIT 50

class CCom
{

public:
	BOOL ResetBaud(int baudrate);
	CCom();
	virtual ~CCom();

	bool	Init(const CString &com,int baudrate,int readbufsize,HWND hwnd,int iName);
	bool	SetReadSize(int size);
	void	SetOwner(HWND hwnd);
	void	WriteData(char *array,int size);
	void	WriteDataThread(char *array,int size);
	void	BeginRead(void);
	void	StopRead(void);
	char * 	GetReadData(int &iSize);

private:

	HANDLE	m_hCom;
	char	*m_pArray;
	HWND	m_hWnd;
	bool	m_isContinue;
	int		m_iReadSize;
	int		m_iReadBufferSize;
	int		m_iReadSizeResult;
	int		m_iName;

	char    *m_pWriteArray;
	int		m_iWriteSize;

	bool	InitComm(const CString &com,int baudrate);
	UINT	Read(void);
	UINT	Write(void);
	static	UINT RunRead(LPVOID lpparam);
    static	UINT RunWrite(LPVOID lpparam);

};

#endif // !defined(AFX_COM_H__33845A38_1B54_4E40_94A9_3544ED62CA26__INCLUDED_)
