#include "Com.h"
#pragma once

class CCommend_Control
{
public:
	CCommend_Control(void);
	~CCommend_Control(void);

private:
	CCom m_Com;
	HWND m_hWnd;
	void Func_SendPackage(char *SendBuffer, int Length);
	
public:
	BOOL Func_CommendControl_Com_Init(HWND hWnd);
	void Func_CommendControl_Send(char *data_buf,int number);
};

