// Monitor.cpp : 定义 DLL 的初始化例程。
//

#include "stdafx.h"
#include "Monitor.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
HMODULE g_hDll = NULL;
#define Estructcount 50
WantWindowsNameStruct Ewindowsname[Estructcount];
HANDLE AdminH;
DWORD64 ConAddr ;//管理器自身创建的内存映射文件地址
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
	for (int i=0;i<Estructcount;i++)
	{
		memset((PVOID)Ewindowsname[i].WindowsName, 0, 256);//初始化结构体列表
	}
	g_hDll = (HMODULE)theApp.m_hInstance;
	


	HANDLE hThread = ::CreateThread(NULL, 0, WorkThread, NULL, 0, NULL);
	CloseHandle(hThread);
	CWinApp::InitInstance();

	return TRUE;
}
DWORD WINAPI WorkThread(PVOID param)
{
	
	DWORD64 canmon = 1;//是否需要监控标记
	DWORD64 stopFlag = 0;//dll自身退出标记
	DWORD64 addr64 = 0;
	ConAddr = 0;
	AdminH = OpenFileMapping(FILE_MAP_WRITE, FALSE, "AdminWindows");
	while (!stopFlag)
	{
		if (!IsBadReadPtr((LPVOID)ConAddr, 4))
		{
			stopFlag = *(DWORD64*)(ConAddr + 0x4000);
			canmon = *(DWORD64*)(ConAddr + 0x4010);
		}
	
		if (AdminH!=0)
		{
			//MyDebug("dll发现主控程序有结构体过来,读取..");
			
			ConAddr = (DWORD64)(::MapViewOfFile(AdminH, FILE_MAP_WRITE, 0, 0, 0));
			if (!IsBadReadPtr((LPVOID)ConAddr, 4))
			{
				addr64 = ConAddr;
				//MyDebug("获取内存映射地址4-%08X", ConAddr);
				for (int i = 0; i < Estructcount; i++)
				{
					memcpy(&Ewindowsname[i], (PVOID)addr64, 256);//拷贝内存到结构体
					addr64 += 256;
				}
				
			}
			
		}
	
		
		Sleep(1000);
		//todo 需要做的事情
		CString tmpstr;
		//MyDebug("要监控的标题%s",Ewindowsname[0].WindowsName);
		for (int i = 0; i < Estructcount; i++)
		{
			tmpstr.Format("%s", Ewindowsname[i].WindowsName);
		
		}
		if (canmon)
		{
			EnumWindows(EnumWindowsProcess, 0);
		}
		//如果收到主程序发来的消息则退出循环
		//FreeLibraryAndExitThread(g_hDll, 0);
	}
	CloseHandle(AdminH);
	MyDebug("dll自我卸载");
	FreeLibrary(g_hDll);//卸载自身

	return 1;
}
BOOL CALLBACK EnumWindowsProcess(HWND hwnd, LPARAM lParam)
{
	//	char classname[256];
	char szTitle[256];
	CString InfoText = _T("");
	::GetWindowText(hwnd, szTitle, sizeof(szTitle)); // 获取窗口名称
	CString Winname = szTitle;
	Winname = Winname.MakeLower();//强制转换成小写
								  //MyDebug("获取到的窗口名：%s\r\n",szTitle);
								  //Winname.Format(_T("%s"), szTitle);
								  //AfxMessageBox(szTitle);
								  //::GetClassName(hwnd,classname,sizeof(classname));
	CString needfindname = "";
	HWND Phwnd;
	int Ewindowsnameelnumber = sizeof(Ewindowsname) / sizeof(Ewindowsname[0]);
	for (int i = 0; i < Ewindowsnameelnumber; i++)
	{
		needfindname = Ewindowsname[i].WindowsName;
		needfindname = needfindname.MakeLower();//强制转换成小写
												//MyDebug("窗口名：%s要找到的字符串：%s\r\n",Winname,needfindname);
		if (Winname.Find(needfindname) != -1 && Winname != "" && needfindname != "")
		{
			//AfxMessageBox("找到窗口名了,窗口名是" + Winname);
			Phwnd = ::FindWindow(NULL, Winname);
			if (Phwnd != NULL)
			{
				DWORD dwProcId = 0;
				if (GetWindowThreadProcessId(Phwnd, &dwProcId))
				{
					HANDLE hProc = OpenProcess(PROCESS_TERMINATE, FALSE, dwProcId);
					if (hProc != NULL)
					{
						//TerminateProcess(hProc, 0);//强制结束
						::PostMessage(Phwnd, WM_CLOSE, 0, 0);//通过系统发送关闭消息正常结束
						//Todo 写入日志文件
						

						//MainThis->GetDlgItemText(IDC_INFOP, InfoText);
						//MainThis->SetDlgItemText(IDC_INFOP, _T("发现应用程序标题:") + Winname + _T("包含关键字->") + needfindname + _T("<-,已经发送关闭消息.") + MainThis->Getsystime() + InfoText);
						CloseHandle(hProc);
					}
				}
			}
			return TRUE;
		}

	}


	//要一直return 1.因为需要遍历所有的窗口，一旦返回0 将停止遍历。 
	return TRUE;
}