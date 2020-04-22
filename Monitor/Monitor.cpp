// Monitor.cpp : 定义 DLL 的初始化例程。
//

#include "stdafx.h"
#include "Monitor.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
HMODULE g_hDll = NULL;
//
//TODO:  如果此 DLL 相对于 MFC DLL 是动态链接的，
//		则从此 DLL 导出的任何调入
//		MFC 的函数必须将 AFX_MANAGE_STATE 宏添加到
//		该函数的最前面。
//
//		例如: 
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// 此处为普通函数体
//		}
//
//		此宏先于任何 MFC 调用
//		出现在每个函数中十分重要。  这意味着
//		它必须作为函数中的第一个语句
//		出现，甚至先于所有对象变量声明，
//		这是因为它们的构造函数可能生成 MFC
//		DLL 调用。
//
//		有关其他详细信息，
//		请参阅 MFC 技术说明 33 和 58。
//

// CMonitorApp

BEGIN_MESSAGE_MAP(CMonitorApp, CWinApp)
END_MESSAGE_MAP()


// CMonitorApp 构造

CMonitorApp::CMonitorApp()
{
	// TODO:  在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中
}


// 唯一的一个 CMonitorApp 对象

CMonitorApp theApp;


// CMonitorApp 初始化

BOOL CMonitorApp::InitInstance()
{
	MyDebug("Dll注入成功");
	g_hDll = (HMODULE)theApp.m_hInstance;
	HANDLE hThread = ::CreateThread(NULL, 0, WorkThread, NULL, 0, NULL);
	CloseHandle(hThread);
	CWinApp::InitInstance();

	return TRUE;
}
DWORD WINAPI WorkThread(PVOID param)
{
	while (1)
	{
		Sleep(100);
		//todo 需要做的事情

		//如果收到主程序发来的消息则退出循环
		//FreeLibraryAndExitThread(g_hDll, 0);
	}
	FreeLibrary(g_hDll);//卸载自身

	return 1;
}