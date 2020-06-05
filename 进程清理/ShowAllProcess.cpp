// ShowAllProcess.cpp : 实现文件
//

#include "stdafx.h"
#include "进程清理.h"
#include "ShowAllProcess.h"
#include "afxdialogex.h"
#include "进程清理Dlg.h"
#include "Tlhelp32.h"
#include <windows.h>
#include <Psapi.h>
#pragma comment (lib,"Psapi.lib")

CMyDlg* Mydlgptr;
// ShowAllProcess 对话框
//根据pid获取命令行
/*
TCHAR szBuffer[256] = {0};
GetProcessCommandLine(628, szBuffer, sizeof(szBuffer)); //628改成你要获取的进程PID,*/
int WINAPI GetProcessCommandLine(DWORD dwPID, LPTSTR lpszCommandLine, DWORD dwByteOfSize)
{
	
	HANDLE hProcess = ::OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ | PROCESS_CREATE_THREAD, FALSE, dwPID);

	if (!hProcess)
	{
		return 0;
	}

	DWORD dwThreadId = 0;
	DWORD dwExitCode = 0;
	SIZE_T dwReaded = 0;
	HANDLE hThread  = ::CreateRemoteThread(hProcess, NULL, NULL, (LPTHREAD_START_ROUTINE)GetCommandLine, NULL, 0, &dwThreadId);

	if (hThread)
	{
		::WaitForSingleObject(hThread, 100);
		::GetExitCodeThread(hThread, &dwExitCode);
		::ReadProcessMemory(hProcess, (LPCVOID)dwExitCode, lpszCommandLine, dwByteOfSize, &dwReaded);
	}
	CloseHandle(hThread);
	CloseHandle(hProcess);
	return 1;
}
IMPLEMENT_DYNAMIC(ShowAllProcess, CDialog)

	ShowAllProcess::ShowAllProcess(CWnd* pParent /*=NULL*/)
	: CDialog(ShowAllProcess::IDD, pParent)
{

}

ShowAllProcess::~ShowAllProcess()
{
}

void ShowAllProcess::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_SHOW, m_showlist);
}


BEGIN_MESSAGE_MAP(ShowAllProcess, CDialog)
	ON_BN_CLICKED(IDOK, &ShowAllProcess::OnBnClickedOk)
	//	ON_WM_ACTIVATE()
	ON_COMMAND(ID_32778, &ShowAllProcess::On32778)
	ON_NOTIFY(NM_RCLICK, IDC_LIST_SHOW, &ShowAllProcess::OnNMRClickListShow)
	ON_BN_CLICKED(IDC_BUTTON1, &ShowAllProcess::OnBnClickedButton1)
	ON_COMMAND(ID_32779, &ShowAllProcess::OpenExeDir)
	ON_BN_CLICKED(IDCANCEL, &ShowAllProcess::OnBnClickedCancel)
	ON_WM_CLOSE()
	ON_COMMAND(ID_32781, &ShowAllProcess::HideSelectedWindow)
	ON_COMMAND(ID_32782, &ShowAllProcess::ShowSelectedWindow)
	ON_COMMAND(ID_32785, &ShowAllProcess::SuspendProcess)
	ON_COMMAND(ID_32786, &ShowAllProcess::ResumeProcess)
END_MESSAGE_MAP()


// ShowAllProcess 消息处理程序


void ShowAllProcess::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	GetAllProcess();
}




BOOL ShowAllProcess::OnInitDialog()
{
	CDialog::OnInitDialog();
	MyDebug("进程管理初始化");
	// TODO:  在此添加额外的初始化
	Mydlgptr=(CMyDlg*)MainWinThis;
	DWORD exstyle = m_showlist.GetExtendedStyle();
	m_showlist.SetExtendedStyle(exstyle | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES |
		WS_EX_STATICEDGE );
	CRect rect;
	m_showlist.GetClientRect(&rect);
	int nColInterval = rect.Width()/6;//设置每格的宽度为list列表宽度的4分之一
	m_showlist.InsertColumn(0,_T("进程名"), LVCFMT_LEFT, nColInterval);
	m_showlist.InsertColumn(1,_T("线程数"), LVCFMT_LEFT, nColInterval/3);
	m_showlist.InsertColumn(2,_T("PID"), LVCFMT_LEFT, nColInterval/4);
	m_showlist.InsertColumn(3,_T("优先级"), LVCFMT_LEFT, nColInterval/3);
	m_showlist.InsertColumn(4,_T("路径"), LVCFMT_LEFT, nColInterval*2);
	m_showlist.InsertColumn(5,_T("参数"), LVCFMT_LEFT, nColInterval);
	m_showlist.InsertColumn(6,_T("内存"), LVCFMT_LEFT, nColInterval/2);
	m_showlist.InsertColumn(7,_T("窗口标题"), LVCFMT_LEFT, nColInterval/2);
	GetAllProcess();
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}
//获取所有进程
void ShowAllProcess::GetAllProcess()
{
	m_showlist.DeleteAllItems();
	HANDLE Snapshot=::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);
	//获得某一时刻系统的进程、堆（heap）、模块（module）或线程的快照信息
	PROCESSENTRY32 processListStr;
	processListStr.dwSize=sizeof(PROCESSENTRY32);
	BOOL return_value;
	return_value=::Process32First(Snapshot,&processListStr);
	//获得系统进程链表中第一个进程的信息
	if (!return_value)
	{
		CloseHandle(Snapshot);
		return;
	}
	int i=0;//item index
	while(return_value)
	{
		m_showlist.InsertItem(i,processListStr.szExeFile,0);
		CString s;
		s.Format("%d",processListStr.cntThreads);
		m_showlist.SetItemText(i,1,s);
		s.Format("%d",processListStr.th32ProcessID);
		m_showlist.SetItemText(i,2,s);
		s.Format("%d",processListStr.pcPriClassBase);
		m_showlist.SetItemText(i,3,s);
		s.Format("%s",PidToPath(processListStr.th32ProcessID));
		m_showlist.SetItemText(i,4,s);
		TCHAR szBuffer[256] = {0};
		GetProcessCommandLine(processListStr.th32ProcessID, szBuffer, sizeof(szBuffer));
		m_showlist.SetItemText(i,5,szBuffer);
		s.Format("%d",GetProcessMem(processListStr.th32ProcessID));
		m_showlist.SetItemText(i,6,s+_T("K"));
		TCHAR buf[1024]={0};
		::SendMessage(GetAppWindow(processListStr.th32ProcessID),WM_GETTEXT, sizeof(buf)/sizeof(TCHAR), (LPARAM)(void*)buf);
		m_showlist.SetItemText(i,7,buf);
		return_value=::Process32Next(Snapshot,&processListStr);
		//获得系统进程链表中下一个进程的信息
		i++;
	}
	
	CloseHandle(Snapshot);
}

//点击结束进程按钮
void ShowAllProcess::On32778()
{
	int iItemSel = m_showlist.GetNextItem(-1, LVIS_SELECTED);
	CString exepid,exename,InfoText=_T("");
	while(iItemSel != -1)//结束所有选择的进程
	{
		//m_cListCtrl.DeleteItem(iItemSel);//delete selected item in the list control
		exepid=m_showlist.GetItemText(iItemSel,2);//利用该方法可以获取数据记录编号列,此处索引2是进程pid
		exename=m_showlist.GetItemText(iItemSel,0);//利用该方法可以获取数据记录编号列,此处索引0是进程名
		//关闭指定的PID进程
		HANDLE hprocess=::OpenProcess(PROCESS_ALL_ACCESS,FALSE,atoi(exepid));
		if(hprocess!=NULL && atoi(exepid)!=0)
		{
			::TerminateProcess(hprocess,0);
			::CloseHandle(hprocess);
			Mydlgptr->GetDlgItemText(IDC_INFOP,InfoText);
			Mydlgptr->SetDlgItemText(IDC_INFOP,_T("进程")+exename+_T("已干掉")+ Mydlgptr->Getsystime() + InfoText );
		}
		else
		{
			Mydlgptr->GetDlgItemText(IDC_INFOP,InfoText);
			Mydlgptr->SetDlgItemText(IDC_INFOP,_T("结束进程")+exename+_T("失败")+ Mydlgptr->Getsystime() + InfoText );
			
		}
		iItemSel = m_showlist.GetNextItem(iItemSel, LVIS_SELECTED);
	}
	GetAllProcess();
}

//为点击鼠标右键添加菜单
void ShowAllProcess::OnNMRClickListShow(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	DWORD dwPos = GetMessagePos();
	CPoint point( LOWORD(dwPos), HIWORD(dwPos) );   
	CMenu menu;
	VERIFY( menu.LoadMenu( IDR_ProcessMENU ) );       //IDR_ProcessMENU是进程菜单ID
	CMenu* popup = menu.GetSubMenu(0);
	popup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y,this );//TPM_RIGHTBUTTON使右键点击菜单也起作用
	popup=0;
	*pResult = 0;
}
CString ShowAllProcess::PidToPath(DWORD id)//根据进程pid获取进程路径
{  
	CString s;
	HANDLE  hToken;
	TCHAR path[MAX_PATH+1]={0};
	OpenProcessToken(GetCurrentProcess(),TOKEN_ADJUST_PRIVILEGES,&hToken);  //获取令牌
	EnablePrivileges(hToken,SE_DEBUG_NAME);  //提升权限
	HANDLE h_Process=OpenProcess(PROCESS_ALL_ACCESS,FALSE,id);  
	//GetModuleFileNameEx(h_Process,NULL,path,MAX_PATH+1);
	if(Mydlgptr->IsLaterNT())
	{
		DWORD dwSize=MAX_PATH+1;
	QueryFullProcessImageName(h_Process,0,path,&dwSize);
	}
	else
	{
		GetModuleFileNameEx(h_Process,NULL,path,MAX_PATH+1);
	}

	s.Format("%s",path);
	CloseHandle(h_Process);
	return s; 
}
BOOL ShowAllProcess::EnablePrivileges(HANDLE hToken,LPCSTR szPrivName)
{

	TOKEN_PRIVILEGES tkp;

	LookupPrivilegeValue( NULL,szPrivName,&tkp.Privileges[0].Luid );//修改进程权限
	tkp.PrivilegeCount=1;
	tkp.Privileges[0].Attributes=SE_PRIVILEGE_ENABLED;
	AdjustTokenPrivileges( hToken,FALSE,&tkp,sizeof tkp,NULL,NULL );//通知系统修改进程权限

	return( (GetLastError()==ERROR_SUCCESS) );

}
//点击结束选择进程按钮
void ShowAllProcess::OnBnClickedButton1()
{
	// TODO: 在此添加控件通知处理程序代码
	On32778();
}


void ShowAllProcess::OpenExeDir()
{
	int iItemSel = m_showlist.GetNextItem(-1, LVIS_SELECTED);
	CString exepath="";
	while(iItemSel != -1)
	{
		//m_cListCtrl.DeleteItem(iItemSel);//delete selected item in the list control
		exepath=m_showlist.GetItemText(iItemSel,4);//利用该方法可以获取数据记录编号列,此处索引4是进程路径

		//关闭指定的PID进程
		//AfxMessageBox(exepath);
		CString szDir;
		CString InfoText=_T("");
		GetWindowsDirectory(szDir.GetBuffer(MAX_PATH), MAX_PATH);
		szDir.ReleaseBuffer(-1);
	RunApp(szDir,_T("Explorer.exe /select,")+exepath,true);
		iItemSel = m_showlist.GetNextItem(iItemSel, LVIS_SELECTED);
	}
	
}


void ShowAllProcess::OnBnClickedCancel()
{
	// TODO: 在此添加控件通知处理程序代码

	CDialog::OnCancel();
}


void ShowAllProcess::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	Mydlgptr=0;
	delete Mydlgptr;
	CDialog::OnClose();
}
//根据进程pid获得进程占用内存,返回单位大小KB
int ShowAllProcess::GetProcessMem(DWORD th32ProcessID)
{
	int ret = 0;
	BOOL bInheritHandle = FALSE;
	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS,bInheritHandle,th32ProcessID);
	if(hProcess)
	{
		PROCESS_MEMORY_COUNTERS pmc;
		pmc.cb = sizeof(PROCESS_MEMORY_COUNTERS);
		if(GetProcessMemoryInfo(hProcess,&pmc,sizeof(pmc)))
		{
			ret = pmc.PagefileUsage/1024; //返回内存值，单位KByte
			
		}
		else 
			int rc = GetLastError();

		CloseHandle(hProcess);
	}
	return ret;
}
BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam)
{
	DWORD dwCurProcessId = *((DWORD*)lParam);
	DWORD dwProcessId = 0;



	GetWindowThreadProcessId(hwnd, &dwProcessId);
	TCHAR classname[256];
	TCHAR szTitle[256];
	GetWindowText(hwnd, szTitle, sizeof(szTitle) / sizeof(TCHAR)); // 获取窗口名称
	GetClassName(hwnd,classname,sizeof(classname));
	CString TempClassName=classname;
	if(dwProcessId == dwCurProcessId && GetParent(hwnd) == NULL)
	{
		*((HWND *)lParam) = hwnd;
		return FALSE;
	}
	return TRUE;
}
//根据pid获取窗口句柄
HWND GetAppWindow(DWORD Pid)
{
	if(!EnumWindows(EnumWindowsProc, (LPARAM)&Pid))
	{    
		return (HWND)Pid;
	}
	return NULL;
}

//隐藏指定窗口
void ShowAllProcess::HideSelectedWindow()
{
	int iItemSel = m_showlist.GetNextItem(-1, LVIS_SELECTED);
	CString exepid="";
	while(iItemSel != -1)
	{
		//m_cListCtrl.DeleteItem(iItemSel);//delete selected item in the list control
		exepid=m_showlist.GetItemText(iItemSel,2);//利用该方法可以获取数据记录编号列,此处索引2是进程pid
		HWND AppHwnd=GetAppWindow(atoi(exepid));
		
		::ShowWindow(AppHwnd, SW_HIDE);
		iItemSel = m_showlist.GetNextItem(iItemSel, LVIS_SELECTED);
	}
}

//显示指定窗口
void ShowAllProcess::ShowSelectedWindow()
{
	int iItemSel = m_showlist.GetNextItem(-1, LVIS_SELECTED);
	CString exepid="";
	while(iItemSel != -1)
	{
		//m_cListCtrl.DeleteItem(iItemSel);//delete selected item in the list control
		exepid=m_showlist.GetItemText(iItemSel,2);//利用该方法可以获取数据记录编号列,此处索引2是进程pid
		HWND AppHwnd=GetAppWindow(atoi(exepid));
		::ShowWindow(AppHwnd, SW_SHOW);
		::SetForegroundWindow(AppHwnd);
		iItemSel = m_showlist.GetNextItem(iItemSel, LVIS_SELECTED);
	}
}

//冻结进程
void ShowAllProcess::SuspendProcess()
{
	int iItemSel = m_showlist.GetNextItem(-1, LVIS_SELECTED);
	
	while(iItemSel != -1)
	{
		//m_cListCtrl.DeleteItem(iItemSel);//delete selected item in the list control
		DWORD processID= _ttol(m_showlist.GetItemText(iItemSel,2).GetBuffer(0));//利用该方法可以获取数据记录编号列,此处索引2是进程pid
		HANDLE hProcess = ::OpenProcess(PROCESS_ALL_ACCESS, FALSE, processID);

		if (hProcess)
		{
			HMODULE h_module=LoadLibrary("ntdll.dll");
			m_NtSuspendProcess=(NtResumeProcess)GetProcAddress(h_module,"NtSuspendProcess");
			m_NtSuspendProcess(hProcess);
		}
		CloseHandle(hProcess);
	
		iItemSel = m_showlist.GetNextItem(iItemSel, LVIS_SELECTED);
	}

}

//恢复进程
void ShowAllProcess::ResumeProcess()
{
	int iItemSel = m_showlist.GetNextItem(-1, LVIS_SELECTED);

	while(iItemSel != -1)
	{
		//m_cListCtrl.DeleteItem(iItemSel);//delete selected item in the list control
		DWORD processID= _ttol(m_showlist.GetItemText(iItemSel,2).GetBuffer(0));//利用该方法可以获取数据记录编号列,此处索引2是进程pid
		HANDLE hProcess = ::OpenProcess(PROCESS_ALL_ACCESS, FALSE, processID);

		if (hProcess)
		{
			HMODULE h_module=LoadLibrary("ntdll.dll");
			m_NtResumeProcess=(NtResumeProcess)GetProcAddress(h_module,"NtResumeProcess");
			m_NtResumeProcess(hProcess);
		}
		CloseHandle(hProcess);

		iItemSel = m_showlist.GetNextItem(iItemSel, LVIS_SELECTED);
	}

}
