// Monitor.h : Monitor DLL ����ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CMonitorApp
// �йش���ʵ�ֵ���Ϣ������� Monitor.cpp
//

class CMonitorApp : public CWinApp
{
public:
	CMonitorApp();

// ��д
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};
DWORD WINAPI WorkThread(PVOID param);
typedef struct WindowsNameStruct
{
	char WindowsName[256];//��������

}WantWindowsNameStruct, *PWindowsNameStruct;
BOOL CALLBACK EnumWindowsProcess(HWND hwnd, LPARAM lParam);