// 进程清理.cpp : 定义应用程序的类行为。
//

#include "stdafx.h"
#include "进程清理.h"
#include "进程清理Dlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMyApp

BEGIN_MESSAGE_MAP(CMyApp, CWinApp)
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()


// CMyApp 构造

CMyApp::CMyApp()
{
	// TODO: 在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中
	// 

}


// 唯一的一个 CMyApp 对象

CMyApp theApp;


// CMyApp 初始化

BOOL CMyApp::InitInstance()
{
	// 如果一个运行在 Windows XP 上的应用程序清单指定要
	// 使用 ComCtl32.dll 版本 6 或更高版本来启用可视化方式，
	//则需要 InitCommonControls()。否则，将无法创建窗口。
	HANDLE Testhandle=::CreateMutex(NULL,FALSE,TEXT("CleanProcess_chtcrack"));
	if(GetLastError()==ERROR_ALREADY_EXISTS)
	{
		//说明该互斥体内核对象已经存在,即应用程序已经运行了
		CloseHandle(Testhandle);
		return FALSE;
	}


	
	InitCommonControls();

	CWinApp::InitInstance();

	AfxEnableControlContainer();

	// 标准初始化
	// 如果未使用这些功能并希望减小
	// 最终可执行文件的大小，则应移除下列
	// 不需要的特定初始化例程
	// 更改用于存储设置的注册表项
	// TODO: 应适当修改该字符串，
	// 例如修改为公司或组织名
	SetRegistryKey(_T("应用程序向导生成的本地应用程序"));

	CMyDlg dlg;
	m_pMainWnd = &dlg;
	//dlg.SetWindowText("进程清理V1.1");
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: 在此放置处理何时用“确定”来关闭
		//对话框的代码
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: 在此放置处理何时用“取消”来关闭
		//对话框的代码
	}

	// 由于对话框已关闭，所以将返回 FALSE 以便退出应用程序，
	// 而不是启动应用程序的消息泵。
	return FALSE;
}
