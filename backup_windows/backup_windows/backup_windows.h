
// backup_windows.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// Cbackup_windowsApp:
// �йش����ʵ�֣������ backup_windows.cpp
//

class Cbackup_windowsApp : public CWinApp
{
public:
	Cbackup_windowsApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern Cbackup_windowsApp theApp;