// Monitor.h : Monitor DLL 的主头文件
//

#pragma once

#ifndef __AFXWIN_H__
	#error "在包含此文件之前包含“stdafx.h”以生成 PCH 文件"
#endif

#include "resource.h"		// 主符号


// CMonitorApp
// 有关此类实现的信息，请参阅 Monitor.cpp
//

class CMonitorApp : public CWinApp
{
public:
	CMonitorApp();

// 重写
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};
DWORD WINAPI WorkThread(PVOID param);
typedef struct WindowsNameStruct
{
	char WindowsName[256];//窗口类名

}WantWindowsNameStruct, *PWindowsNameStruct;
typedef struct TerminateProStruct
{
	HWND Phwnd;//窗口句柄
	HANDLE hProc;
	CString Winname;

}WantTerminateNameStruct, *PTerminateNameStruct;

BOOL CALLBACK EnumWindowsProcess(HWND hwnd, LPARAM lParam);
//获取系统时间
CString Getsystime();
//清除进程线程
DWORD WINAPI TerminateThread(PVOID param);