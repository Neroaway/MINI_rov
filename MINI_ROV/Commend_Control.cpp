#include "StdAfx.h"
#include "Commend_Control.h"


CCommend_Control::CCommend_Control(void)
{
}


CCommend_Control::~CCommend_Control(void)
{
}


void CCommend_Control::Func_SendPackage(char *pSendBuffer, int Length)
{
	//��pArray��ָ����ַ�������д�봮��
	char *buffer;
	int size=Length; //pArray->GetSize();	
	buffer=new char[size];
	memcpy(buffer, pSendBuffer, sizeof(char)*Length);
	m_Com.WriteData(buffer,size);//length
	delete [] buffer;
}


BOOL CCommend_Control::Func_CommendControl_Com_Init(HWND hWnd)
{
	BOOL  Init_Flag;//ǰ�˷ֻ����ڳ�ʼ����־
	m_hWnd=hWnd;
	//�Դ��ڽ��г�ʼ��
	Init_Flag=m_Com.Init(COM_COMMEND_NAME, COM_COMMEND_BAUD, 33000, hWnd, COM_COMMEND_CODE);
	if(Init_Flag)
	{
		m_Com.BeginRead();
		m_Com.SetReadSize(1);//���͵��ֽ���Ŀ
	}
	return Init_Flag;
}


void CCommend_Control::Func_CommendControl_Send(char *data_buf, int number)
{
	char Send_buf[20];
	Send_buf[0] = 0x7e;
	Send_buf[1] = 0x7e;

	for (int i = 0; i<number; i++)//9λ����
	{
		Send_buf[i + 2] = data_buf[i];
	}
	Send_buf[18] = 0xfd;
	Send_buf[19] = 0xfd;
	Func_SendPackage(Send_buf, number+4);
}