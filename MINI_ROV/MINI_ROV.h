
// MINI_ROV.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CMINI_ROVApp:
// �йش����ʵ�֣������ MINI_ROV.cpp
//

class CMINI_ROVApp : public CWinApp
{
public:
	CMINI_ROVApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CMINI_ROVApp theApp;