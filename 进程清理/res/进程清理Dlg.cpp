// 进程清理Dlg.cpp : 实现文件
//

#include "stdafx.h"
#include "进程清理.h"
#include "进程清理Dlg.h"
#include "Tlhelp32.h"

#include ".\进程清理dlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define WM_NC (WM_USER+1002)
// CMyDlg 对话框

CIni ini("Setting.ini");
WantMprocess EProcessStruct[50];
WantWindowsNameStruct Ewindowsname[50];
NOTIFYICONDATA NotifyIcon;
bool CanMon=true;
bool CanMonwindows=true;
bool Adminmode=false;
TCHAR outputbuffer[1024] = {0};
CString Dosinfo,Adminps;
HANDLE Monitorprocess;
CMyDlg::CMyDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMyDlg::IDD, pParent)
{

	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMyDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, Keywordlist);
}

BEGIN_MESSAGE_MAP(CMyDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_MESSAGE(WM_NC,OnNotifyIcon)
	//}}AFX_MSG_MAP
	ON_WM_SIZE()
	//	ON_WM_DESTROY()
	ON_WM_DESTROY()
	ON_WM_NCPAINT()
	ON_WM_CLOSE()

	ON_COMMAND(ID_32771, NPause)
	ON_COMMAND(ID__32772, ResumeMon)
	ON_COMMAND(ID__32773, DestorySelf)
	ON_COMMAND(ID_32777, Nsynctime)
	ON_BN_CLICKED(IDC_BUTTON2, &CMyDlg::OnBnClickedButton2)
	ON_COMMAND(ID_32780, &CMyDlg::CallProcessManger)
	ON_BN_CLICKED(IDC_BUTTON1, &CMyDlg::OnBnClickedButton1)
	ON_COMMAND(ID_32783, &CMyDlg::CallTmpFileDlg)
	ON_COMMAND(ID_32784, &CMyDlg::ShowPingTool)
	ON_BN_CLICKED(IDC_BUTTON3, &CMyDlg::OnBnClickedButton3)
	ON_LBN_DBLCLK(IDC_LIST1, &CMyDlg::OnLbnDblclkList1)
	ON_BN_CLICKED(IDC_BUTTON4, &CMyDlg::OnBnClickedButton4)
	ON_BN_CLICKED(IDC_CHECK1, &CMyDlg::OnBnClickedCheck1)
END_MESSAGE_MAP()


// CMyDlg 消息处理程序

BOOL CMyDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标
	//EnablePrivilege();
	LoadiniToListCtrl();
	((CButton*)GetDlgItem(IDC_CHECK1))->SetCheck(BST_CHECKED);
	Adminps=ini.GetValue("PS","Pass");
	synctimeing=false;
	this->SetWindowText(_T("进程清理V1.1"));
	INITPorcessStruct();//初始化进程结构体
	InitNotify();//初始化图标
	Shell_NotifyIcon(NIM_ADD, &NotifyIcon);
	if (AutoStart())
	{

	}
	else
	{
		AfxMessageBox(_T("注册表启动项写入失败,请使用管理员身份运行本程序"));
	}
	Monitorprocess=::CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)CheckProcess,(LPVOID)this,NULL,NULL);//启动一个线程
	::CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)CheckWindows,(LPVOID)this,NULL,NULL);//启动一个线程
	Nsynctime();
	return TRUE;  // 除非设置了控件的焦点，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。
//同步时间函数用于多线程
void SyncTime(CMyDlg* MydlgThis)
{
	MydlgThis->synctimeing=true;
	synctimereg();
	int SyncError=0;
	CString InfoText=_T("");
	CString TimeServerAddress=_T("210.72.145.44");
	MydlgThis->GetDlgItemText(IDC_INFOP,InfoText);
	while(SyncError<=3)
	{
		if(MydlgThis->StartSynctime(TimeServerAddress))
		{
			break;	
		}
		else
		{
			TimeServerAddress=_T("time-a.nist.gov");
			SyncError++;
			if(SyncError==3)
			{
				MydlgThis->SetDlgItemText(IDC_INFOP,_T("本机时间同步失败,原因:")+Dosinfo+ MydlgThis->Getsystime() + InfoText );
				MydlgThis->ShowTrayMessage(_T("本机时间同步失败,点击查看详情"));
			}
		}
		Sleep(1000);
	}	
	MydlgThis->synctimeing =false;
}
//同步时间所需注册表
bool synctimereg()
{


	HKEY hKey;
	//找到系统的启动项  

	LPCTSTR lpRun = TEXT("SYSTEM\\CurrentControlSet\\services\\W32Time\\Config");
	//打开启动项Key
	long lRet = RegOpenKeyEx(HKEY_LOCAL_MACHINE, lpRun, 0, KEY_WRITE, &hKey);
	if(lRet == ERROR_SUCCESS) 
	{       
		DWORD dwValue=0x0000D2F0;
		lRet = RegSetValueEx(hKey, "MaxNegPhaseCorrection", 0, REG_DWORD, (CONST BYTE*)&dwValue, sizeof(DWORD));
		lRet = RegSetValueEx(hKey, "MaxPosPhaseCorrection", 0, REG_DWORD, (CONST BYTE*)&dwValue, sizeof(DWORD));
		//关闭注册表
		RegCloseKey(hKey);
		if(lRet==ERROR_SUCCESS)
		{
			//注册表写入成功
			return true;
		}
		else
		{
			//注册表写入失败 
			return false;
		}
	}
	else
	{
		//注册表打开失败
		return false;
	}
}

//开始同步时间函数
bool CMyDlg::StartSynctime(CString TimeserverAddr)
{
	CString szDir;
	CString InfoText=_T("");
	GetSystemDirectory(szDir.GetBuffer(MAX_PATH), MAX_PATH);
	szDir.ReleaseBuffer(-1);
	PROCESS_INFORMATION ppi;
	ppi=RunApp(szDir,_T("net.exe start w32time"));
	WaitForSingleObject( ppi.hProcess,INFINITE );
	CloseHandle( ppi.hProcess );
	CloseHandle( ppi.hThread );
	ppi=RunApp(szDir,_T("w32tm.exe /config /update /manualpeerlist:")+TimeserverAddr);
	WaitForSingleObject( ppi.hProcess,INFINITE );
	CloseHandle( ppi.hProcess );
	CloseHandle( ppi.hThread );
	ppi=RunApp(szDir,_T("w32tm.exe /resync"));
	WaitForSingleObject( ppi.hProcess, INFINITE );
	CloseHandle( ppi.hProcess );
	CloseHandle( ppi.hThread );
	int n = Dosinfo.Find( _T("执行了") );
	GetDlgItemText(IDC_INFOP,InfoText);

	if (n!=-1)
	{
		SetDlgItemText(IDC_INFOP,_T("本机时间同步成功!")+ Getsystime() + InfoText );
		ShowTrayMessage(_T("本机时间同步成功!"));
		return true;
	}
	else
	{

		return false;

	}
}
//运行指定程序,参数1路径,参数
PROCESS_INFORMATION RunApp(CString runpath,CString exename,bool Showwindows)
{
	CString InfoText=_T("");
	CString str;
	SECURITY_ATTRIBUTES sa;
	HANDLE hRead,hWrite;

	sa.nLength = sizeof(SECURITY_ATTRIBUTES);
	sa.lpSecurityDescriptor = NULL;
	sa.bInheritHandle = TRUE;
	CreatePipe(&hRead,&hWrite,&sa,0); 

	STARTUPINFO si; //一些必备参数设置

	memset(&si,0,sizeof(STARTUPINFO)); 
	GetStartupInfo(&si); 
	si.hStdError = hWrite;            //把创建进程的标准错误输出重定向到管道输入
	si.hStdOutput = hWrite;  //把创建进程的标准输出重定向到管道输入
	si.cb = sizeof(STARTUPINFO); 

	si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
	if(Showwindows)
	{
		si.wShowWindow = SW_SHOW; 
	}
	else
	{
		si.wShowWindow = SW_HIDE; 
	}
	PROCESS_INFORMATION pi; //必备参数设置结束

	str.Format(_T("%s\\%s"),runpath,exename);

	//CreateProcess(NULL,str.AllocSysString(),NULL,NULL,TRUE,0,NULL,runpath,&si,&pi);
	CreateProcess(NULL,(LPSTR)(LPCTSTR)str,NULL,NULL,TRUE,0,NULL,runpath,&si,&pi);
	CloseHandle(hWrite);


	DWORD bytesRead=0;
	memset(outputbuffer,0,1024);
	Dosinfo="";
	while (true) 
	{
		if (ReadFile(hRead,outputbuffer,1024,&bytesRead,NULL) == NULL)
			break;
		//buffer中就是执行的结果，可以保存到文本，也可以直接输出
		Dosinfo=outputbuffer;
		//MyDebug(_T("信息:%s"),Dosinfo);

	}
	CloseHandle(hRead); 



	return pi;

}


//初始化进程结构体
void INITPorcessStruct()
{
	for(int i=0;i<50;i++)
	{
		EProcessStruct[i].NeedEndProcessName=_T("");
		EProcessStruct[i].PClassName=_T("");
		EProcessStruct[i].PWindowsName=_T("");

	}

	//当想一直强制干掉某个进程名时,可将PClassName或者PWindowsName设置为随便不存在的字符串
	EProcessStruct[0].NeedEndProcessName=_T("PPSKernel.exe");
	EProcessStruct[0].PClassName=_T("MainForm");
	EProcessStruct[0].PWindowsName=_T("爱奇艺PPS影音");
	EProcessStruct[1].NeedEndProcessName=_T("QvodTerminal.exe");
	EProcessStruct[1].PClassName=_T("QVODPLAYERWND");
	EProcessStruct[1].PWindowsName=_T("");
	EProcessStruct[2].NeedEndProcessName=_T("PPAP.exe");
	EProcessStruct[2].PClassName=_T("PPLiveGUI");
	EProcessStruct[2].PWindowsName=_T("");
	EProcessStruct[3].NeedEndProcessName=_T("DuoMi.exe");
	EProcessStruct[3].PClassName=_T("duomi_maindlg");
	EProcessStruct[3].PWindowsName=_T("");
	EProcessStruct[4].NeedEndProcessName=_T("XLGameBoxToolTip.exe");
	EProcessStruct[4].PClassName=_T("Randsdsds");
	EProcessStruct[5].NeedEndProcessName=_T("BaiduP2PService.exe");
	EProcessStruct[5].PClassName=_T("WndBase");
	EProcessStruct[5].PWindowsName=_T("百度影音");
	EProcessStruct[6].NeedEndProcessName=_T("FunshionService.exe");
	EProcessStruct[6].PClassName=_T("funshion_player_tzdenjohn");
	EProcessStruct[6].PWindowsName=_T("风行 2.8");
	EProcessStruct[7].NeedEndProcessName=_T("PPSProtect.exe");
	EProcessStruct[7].PClassName=_T("MainForm");
	EProcessStruct[7].PWindowsName=_T("爱奇艺PPS影音");
	EProcessStruct[8].NeedEndProcessName=_T("PPSDesktopTip.exe");
	EProcessStruct[8].PClassName=_T("MainForm");
	EProcessStruct[8].PWindowsName=_T("爱奇艺PPS影音");
	EProcessStruct[9].NeedEndProcessName=_T("QiyiService.exe");
	EProcessStruct[9].PClassName=_T("#32770");
	EProcessStruct[9].PWindowsName=_T("爱奇艺视频");
	EProcessStruct[10].NeedEndProcessName=_T("QiyiKernel.exe");
	EProcessStruct[10].PClassName=_T("#32770");
	EProcessStruct[10].PWindowsName=_T("爱奇艺视频");
	EProcessStruct[0].Count=11;
	//有时间可做成从配置文件中读到此结构体
}


//检测进程
void CheckProcess(CMyDlg* MydlgThis)
{
	HWND Processhwnd=0;
	CString InfoText=_T("");
	while(CanMon)
	{
		for(int i=0;i<EProcessStruct[0].Count;i++)
		{

			if(EProcessStruct[i].NeedEndProcessName!=_T(""))
			{

				if(EProcessStruct[i].PWindowsName==_T("") && EProcessStruct[i].PClassName!=_T(""))
				{
					Processhwnd=::FindWindow(EProcessStruct[i].PClassName,NULL);
				}
				if(EProcessStruct[i].PClassName==_T("") && EProcessStruct[i].PWindowsName!=_T(""))
				{
					Processhwnd=::FindWindow(NULL,EProcessStruct[i].PWindowsName);
				}
				if(EProcessStruct[i].PClassName!=_T("") && EProcessStruct[i].PWindowsName!=_T(""))
				{
					Processhwnd=::FindWindow(EProcessStruct[i].PClassName,EProcessStruct[i].PWindowsName);
				}
				if(Processhwnd==NULL)//没找到对应的播放程序,那么查是否有后台传输程序在运行
				{
					if(MydlgThis->CheckAndKillProcess(EProcessStruct[i].NeedEndProcessName))
					{
						MydlgThis->GetDlgItemText(IDC_INFOP,InfoText);
						MydlgThis->SetDlgItemText(IDC_INFOP,_T("进程")+EProcessStruct[i].NeedEndProcessName+_T("已干掉")+ MydlgThis->Getsystime() + InfoText );
						MydlgThis->ShowTrayMessage(_T("进程")+EProcessStruct[i].NeedEndProcessName+_T("已干掉"));
					}
				}
			}
		}

		Sleep(3000);
	}

}
//判断进程是否存在,存在就干掉返回真,不存在返回假
bool CMyDlg::CheckAndKillProcess(LPCTSTR Processname)
{
	PROCESSENTRY32 pe;
	DWORD id=0;
	HANDLE hSnapshot=CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);
	pe.dwSize=sizeof(PROCESSENTRY32);
	if(!Process32First(hSnapshot,&pe))
		return false;
	while(1)
	{
		pe.dwSize=sizeof(PROCESSENTRY32);
		if(Process32Next(hSnapshot,&pe)==FALSE)
			break;

		if(_tcscmp(pe.szExeFile,Processname)==0)
		{
			id=pe.th32ProcessID;
			HANDLE hprocess=::OpenProcess(PROCESS_ALL_ACCESS,FALSE,id);
			/*	DWORD errorCode=GetLastError();
			CString Infotext,errcodestr;
			GetDlgItemText(IDC_INFOP,Infotext);
			errcodestr.Format(_T("%d"),errorCode);
			SetDlgItemText(IDC_INFOP,_T("错误代码") +errcodestr+ Infotext);*/
			if(hprocess!=NULL)
			{
				if(::TerminateProcess(hprocess,0))
				{
					::CloseHandle(hprocess);

				}
				else
				{
					::CloseHandle(hprocess);
					return false;
				}
			}
			break;
		}
		Sleep(5);
	}
	CloseHandle(hSnapshot);
	if (id>0)
	{
		return true;
	}
	else
	{
		return false;
	}

	return 0;
}



void CMyDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标显示。
HCURSOR CMyDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}
//初始化托盘图标
void CMyDlg::InitNotify()
{
	NotifyIcon.cbSize = (DWORD)sizeof(NOTIFYICONDATA);
	NotifyIcon.hWnd = this->m_hWnd;

	NotifyIcon.uID = IDR_MAINFRAME;//图标ID 用于消息相应函数OnNotifyIcon里的 wParam参数
	NotifyIcon.uFlags=NIF_ICON|NIF_MESSAGE|NIF_TIP|NIF_INFO;
	NotifyIcon.dwInfoFlags=NIIF_INFO;
	NotifyIcon.uTimeout=2000;//气泡消息自动消失时间5秒
	NotifyIcon.uCallbackMessage = WM_NC;// 自定义的消息名称
	NotifyIcon.hIcon = LoadIcon(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME));
	_tcscpy_s(NotifyIcon.szInfoTitle, _T("提示")); // 信息提示条为"进程清理"
	_tcscpy_s(NotifyIcon.szTip, _T("进程清理")); // 信息提示条为"进程清理"
	_tcscpy_s(NotifyIcon.szInfo, _T("进程清理托盘已启动,您可以右键点击托盘图标来操作哦!")); // 信息提示条为"进程清理"



}
//显示托盘信息
void CMyDlg::ShowTrayMessage(CString Msg)
{

	_tcscpy_s(NotifyIcon.szInfoTitle, _T("信息:"));  
	_tcscpy_s(NotifyIcon.szInfo, Msg);
	NotifyIcon.uTimeout=2000;//气泡消息自动消失时间3秒
	Shell_NotifyIcon(NIM_MODIFY, &NotifyIcon); 
}

//托盘响应鼠标事件
LRESULT CMyDlg::OnNotifyIcon(WPARAM wParam,LPARAM IParam)
{
	if(wParam != IDR_MAINFRAME)
		return 0;
	switch(IParam)
	{
	case WM_RBUTTONUP: // 右键起来时弹出菜单
		{
			LPPOINT lpoint = new tagPOINT;
			::GetCursorPos(lpoint); // 得到鼠标位置
			CMenu menu;
			//menu.CreatePopupMenu(); // 声明一个弹出式菜单
			SetForegroundWindow();//将程序置于桌面顶部，否则菜单将不会消失
			//menu.AppendMenu(MF_STRING, WM_DESTROY, _T("关闭"));
			VERIFY( menu.LoadMenu( IDR_MENU1 ) );
			CMenu* popup = menu.GetSubMenu(0);
			if (CanMon)
			{
				menu.EnableMenuItem(ID__32772,MF_DISABLED);

			}
			else
			{
				menu.EnableMenuItem(ID_32771,MF_DISABLED);
			}
			if(synctimeing)
			{
				menu.EnableMenuItem(ID_32777,MF_DISABLED);
			}
			popup->TrackPopupMenu(TPM_LEFTALIGN, lpoint->x ,lpoint->y, this);
			HMENU hmenu = menu.Detach();
			menu.DestroyMenu();
			delete lpoint;
			popup=0;
			//delete popup;
			break;
		}

	case WM_LBUTTONDOWN: // 单击左键的处理
		{

			Showmainwindow();
			
			break;
		}

	case NIN_BALLOONUSERCLICK: // 当用户在气球提示上或托盘图标上单击鼠标（此时气球处于显示状态）时发送此消息
		{
			Showmainwindow();
			break;
		}

	case NIN_BALLOONSHOW: // 显示气球提示时发送
		{
			break;
		}
	case NIN_BALLOONTIMEOUT: // 当由于超时或者用户单击气球上的关闭按钮(X)，使气球消失时发送此消息
		{
			break;
		}
	case NIN_BALLOONHIDE: // 气球提示消失时发送；例如，当图标被删除，如果因为超时或是用户单击鼠标气球消失，此消息不会被发送
		{
			break;
		}
	}
	return 1 ;
}

void CMyDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);
	if(nType == SIZE_MINIMIZED) 
	{ 
		Adminmode=false;
		ShowWindow(SW_HIDE); // 当最小化时，隐藏主窗口 
	} 
	// TODO: 在此处添加消息处理程序代码
}



void CMyDlg::OnDestroy()
{
	CDialog::OnDestroy();
	// 在托盘区删除图标
	Shell_NotifyIcon(NIM_DELETE,  &NotifyIcon); 
	// TODO: 在此处添加消息处理程序代码
}

void CMyDlg::OnNcPaint()
{
	static int i = 2;
	if(i > 0)
	{
		i --;
		ShowWindow(SW_HIDE);
	}
	else
		CDialog::OnNcPaint();
}
//写入系统注册表启动项
bool AutoStart()
{

	TCHAR StartupName[]=TEXT("CleanProcess");
	HKEY hKey;
	//找到系统的启动项  

	LPCTSTR lpRun = TEXT("Software\\Microsoft\\Windows\\CurrentVersion\\Run");
	//打开启动项Key
	long lRet = RegOpenKeyEx(HKEY_LOCAL_MACHINE, lpRun, 0, KEY_WRITE, &hKey);
	if(lRet == ERROR_SUCCESS) 
	{       
		TCHAR pFileName[MAX_PATH] = {0};
		//得到程序自身的全路径
		GetModuleFileName(NULL, pFileName, MAX_PATH);
		//MyDebug(_T("%s"),pFileName);
		//添加一个子Key,并设置值 StartupName=pFileName
		lRet = RegSetValueEx(hKey, StartupName, 0, REG_SZ, (BYTE*)pFileName, (lstrlen(pFileName) + 1) * sizeof(TCHAR));
		//关闭注册表
		RegCloseKey(hKey);
		if(lRet==ERROR_SUCCESS)
		{
			//注册表写入成功
			return true;
		}
		else
		{
			//注册表写入失败 
			return false;
		}
	}
	else
	{
		//注册表打开失败
		return false;
	}
}
void CMyDlg::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	Adminmode=false;
	ShowWindow(SW_MINIMIZE);
	//CDialog::OnClose();
}

//点击暂停清理
void CMyDlg::NPause()
{
	CanMon=FALSE;
	DWORD Checkthreadcode=0;

	while (1) 
	{
		Checkthreadcode=WaitForSingleObject(Monitorprocess,0);
		if (Checkthreadcode==WAIT_TIMEOUT)
		{
		}
		else
		{
			break;
		}
		Sleep(200);
	}
	NotifyIcon.hIcon = LoadIcon(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDI_ICON1));
	_tcscpy_s(NotifyIcon.szInfoTitle, _T("啊啊啊!"));  
	_tcscpy_s(NotifyIcon.szInfo,  _T("进程清理已停止工作了!"));
	NotifyIcon.dwInfoFlags=NIIF_ERROR;
	NotifyIcon.uTimeout=100;//气泡消息自动消失时间500毫秒
	Shell_NotifyIcon(NIM_MODIFY, &NotifyIcon);
}
//点击恢复清理
void CMyDlg::ResumeMon()
{
	CanMon=TRUE;

	Monitorprocess=::CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)CheckProcess,(LPVOID)this,NULL,NULL);//启动一个线程
	NotifyIcon.hIcon = LoadIcon(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME));
	_tcscpy_s(NotifyIcon.szInfoTitle, _T("嘿嘿!"));  
	_tcscpy_s(NotifyIcon.szInfo,  _T("进程清理又开始工作了!"));
	NotifyIcon.dwInfoFlags=NIIF_INFO;
	NotifyIcon.uTimeout=100;//气泡消息自动消失时间500毫秒
	Shell_NotifyIcon(NIM_MODIFY, &NotifyIcon);
}

void CMyDlg::DestorySelf()//点击退出程序
{
	if(Adminmode)
	{
		HANDLE hself = GetCurrentProcess();

		TerminateProcess(hself, 0);
		this->DestroyWindow();
	}
	
}
BOOL CMyDlg::IsLaterNT()//判断是否为NT6以后的版本
{
	OSVERSIONINFO osvi;//版本信息结构体
	ZeroMemory(&osvi,sizeof(OSVERSIONINFO));
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);

	GetVersionEx(&osvi);
	if (osvi.dwMajorVersion >= 6)
	{
		return TRUE;
	}
	return FALSE;
}
BOOL CMyDlg::EnablePrivilege()//提升当前进程权限
{
	if(IsLaterNT())
	{
		HANDLE hToken = NULL;
		BOOL bFlag = FALSE;

		if(OpenProcessToken(GetCurrentProcess(),TOKEN_ADJUST_PRIVILEGES,&hToken))//获取进程的令牌信息
		{
			TOKEN_PRIVILEGES tp;
			tp.PrivilegeCount = 1;
			if (!LookupPrivilegeValue(NULL,SE_DEBUG_NAME,&tp.Privileges[0].Luid))//
			{
				CloseHandle(hToken);
				return FALSE;
			}
			tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
			if (!AdjustTokenPrivileges(hToken ,FALSE ,&tp, sizeof(tp), NULL, NULL))
			{
				AfxMessageBox(_T("权限不够，请用管理员身份运行本程序"));
				return FALSE;
			}
		}
		CloseHandle(hToken);
	}
	else
	{
		HANDLE hprocess=::GetCurrentProcess();

		TCHAR privilege[]=SE_DEBUG_NAME;

		TOKEN_PRIVILEGES Privileges;

		Privileges.PrivilegeCount=1;

		HANDLE hToken;   

		OpenProcessToken(hprocess,TOKEN_ADJUST_PRIVILEGES,&hToken);   

		LUID luid;

		LookupPrivilegeValue(NULL,privilege,&luid);

		Privileges.Privileges[0].Luid=luid;

		Privileges.Privileges[0].Attributes=SE_PRIVILEGE_ENABLED;

		if(AdjustTokenPrivileges(hToken,FALSE,&Privileges,NULL,NULL,NULL)!=0)
			return TRUE;

	}
	return TRUE;
}
//点击同步时间
void CMyDlg::Nsynctime()
{
	::CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)SyncTime,(LPVOID)this,NULL,NULL);//启动一个时间同步线程
}
//获取系统时间
CString CMyDlg::Getsystime()
{
	COleDateTime timeNow, dateNow,yearNow;
	yearNow = COleDateTime::GetCurrentTime();     // 获取当前日期时间
	timeNow = COleDateTime::GetCurrentTime();     // 获取当前日期时间
	dateNow = COleDateTime::GetCurrentTime(); // 同样获取当前日期时间，这么写只是为了清晰
	CString sYear = yearNow.Format(VAR_FOURDIGITYEARS);     // 获取当前年份
	CString sTime = timeNow.Format(VAR_TIMEVALUEONLY);     // 获取当前时间
	CString sDate = dateNow.Format(VAR_DATEVALUEONLY);     // 获取当前日期
	return _T("-")+sYear+_T("\r\n");
}

void CMyDlg::OnBnClickedButton2()
{

	Showprocessdlg.DeleteTempMap();
	Showprocessdlg.dispatchMap;
	Showprocessdlg.DestroyWindow();

	Showprocessdlg.MainWinThis = (DWORD)this;//在帐号窗体还没有初始化时就传递当前this指针给添加帐号窗体,这样可以在在帐号窗体init时才能获取到
	Showprocessdlg.Create(IDD_ProcessDIALOG);
	Showprocessdlg.ShowWindow(SW_SHOW);
	Showprocessdlg.SetWindowText(_T("进程管理"));
	// TODO: 在此添加控件通知处理程序代码
}


void CMyDlg::CallProcessManger()
{
	// TODO: 在此添加命令处理程序代码
	OnBnClickedButton2();
}


void CMyDlg::OnBnClickedButton1()
{
	Clearfiledlg.DeleteTempMap();
	Clearfiledlg.dispatchMap;
	Clearfiledlg.DestroyWindow();

	//Clearfiledlg.MainWinThis = (DWORD)this;//在帐号窗体还没有初始化时就传递当前this指针给添加帐号窗体,这样可以在在帐号窗体init时才能获取到
	Clearfiledlg.Create(IDD_CLEARTMPFILE_DIALOG);
	Clearfiledlg.ShowWindow(SW_SHOW);
	Clearfiledlg.SetWindowText(_T("垃圾清理"));
}


void CMyDlg::CallTmpFileDlg()
{
	OnBnClickedButton1();
}


void CMyDlg::ShowPingTool()
{
	Pingydlg.DeleteTempMap();
	Pingydlg.dispatchMap;
	Pingydlg.DestroyWindow();

	//Clearfiledlg.MainWinThis = (DWORD)this;//在帐号窗体还没有初始化时就传递当前this指针给添加帐号窗体,这样可以在在帐号窗体init时才能获取到
	Pingydlg.Create(IDD_DIALOG2);
	Pingydlg.ShowWindow(SW_SHOW);
	Pingydlg.SetWindowText(_T("PingTool"));

}

//往ini里添加关键字
void CMyDlg:: Addkeyword(CString Keyword )
{
	CStringArray arrKey,arrValue;
	CString InserStr=Keyword;
	int n=0;
	n=ini.GetKeyValues(arrKey,arrValue,"info");
	for(int i=0;i<n;i++)
	{
		if(InserStr==arrValue[i])
		{
			AfxMessageBox("已有相同关键字,无需再重复添加");
			return;
		}
	}


	CString Getkeys=ini.GetValue("info", "0");
	if(Getkeys=="")
	{
		ini.SetValue("info","0",InserStr);


	}
	else
	{
		
		
		CString Keys;
		Keys.Format("%d",n);
		//AfxMessageBox(Keys);
		ini.SetValue("info",Keys,InserStr);
	}
	ReBuildIni();
	LoadiniToListCtrl();

}
//删除ini里的关键字
void CMyDlg::DelAccount(CString Keyword)
{
	CStringArray arrKey,arrValue;
	CString AccountName;
	int n=0;
	n=ini.GetKeyValues(arrKey,arrValue,"info");

	for(int i=0;i<n;i++)
	{
		if (arrValue[i]==Keyword)
		{
			ini.DelKey("info",arrKey[i]);
		}

	}
	ReBuildIni();
	LoadiniToListCtrl();

}
//重组关键字ini
void CMyDlg::ReBuildIni()
{
	
	CStringArray arrKey,arrValue;
	int Count=ini.GetKeyValues(arrKey,arrValue,"info");
	CString Adminps=ini.GetValue("PS","Pass");
	ini.DelAllSections();

	for(int i=0;i<Count;i++)
	{
		CString keys;
		keys.Format("%d",i);
		ini.SetValue("info",keys,arrValue[i]);
	}
	
	ini.SetValue("PS","Pass",Adminps);
	
}
//读取ini文件内容到listcontrol列表
void CMyDlg::LoadiniToListCtrl()
{
	Keywordlist.ResetContent();
	int n=0;
	CStringArray arrKey,arrValue;
	CString Keyword;
	n=ini.GetKeyValues(arrKey,arrValue,"info");
	int i=0;
	CString AccountID="";
	for(int i=0;i<n;i++)
	{

		Keyword=arrValue[i];
		Keywordlist.AddString(Keyword);


	}
	ReadAccountini();
}

//读取关键字文件到结构体
void ReadAccountini()
{

	InitStruct(true);
	CStringArray arrKey,arrValue;

	int n=ini.GetKeyValues(arrKey,arrValue,"info");
	for(int i=0;i<n;i++)
	{

		//取出帐号

		CString2Char(arrValue[i],Ewindowsname[i].WindowsName);
		//	MyDebug("ini里的值：%s结构体里的值：%s",arrValue[i],Ewindowsname[i].WindowsName);

	}
}
//初始化结构体
void InitStruct(bool InitPskinAccount)
{
	for(int i=0;i<50;i++)
	{
		if (InitPskinAccount)
		{
			memset((PVOID)Ewindowsname[i].WindowsName,0,256);//初始化帐号结构体列表

		}
	}
}
void CMyDlg::OnBnClickedButton3()
{
	CString Keyword="";
	GetDlgItemText(IDC_EDIT1,Keyword);
	Addkeyword(Keyword);
	SetDlgItemText(IDC_EDIT1,"");
}

//双击Clistbox事件
void CMyDlg::OnLbnDblclkList1()
{
	// TODO: 在此添加控件通知处理程序代码
	int iItemSel = Keywordlist.GetCurSel();
	if(iItemSel!=-1)
	{
		CString Selword;
		Keywordlist.GetText(iItemSel,Selword);
		//AfxMessageBox(Selword);
		DelAccount(Selword);
	}

}
BOOL CALLBACK EnumWindowsProcess(HWND hwnd, LPARAM lParam)
{
	//	char classname[256];
	char szTitle[256];

	::GetWindowText(hwnd, szTitle, sizeof(szTitle)); // 获取窗口名称
	CString Winname=szTitle;
	//MyDebug("获取到的窗口名：%s\r\n",szTitle);
	//Winname.Format(_T("%s"), szTitle);
	//AfxMessageBox(szTitle);
	//::GetClassName(hwnd,classname,sizeof(classname));
	CString needfindname="";
	HWND Phwnd;
	for(int i=0;i<50;i++)
	{

		needfindname=Ewindowsname[i].WindowsName;
		//MyDebug("窗口名：%s要找到的字符串：%s\r\n",Winname,needfindname);
		if(Winname.Find(needfindname)!=-1 && Winname!="" && needfindname!="" )
		{
			//AfxMessageBox("找到窗口名了,窗口名是" + Winname);
			Phwnd=::FindWindow(NULL,Winname);
			if (Phwnd != NULL)
			{
				DWORD dwProcId = 0;
				if (GetWindowThreadProcessId(Phwnd, &dwProcId))
				{
					HANDLE hProc = OpenProcess(PROCESS_TERMINATE, FALSE, dwProcId);
					if (hProc != NULL)
					{
						TerminateProcess(hProc, 0);
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


void CMyDlg::OnBnClickedButton4()
{
	Setadminps.DeleteTempMap();
	Setadminps.dispatchMap;
	Setadminps.DestroyWindow();

	//Setadminps.MainWinThis = (DWORD)this;//在帐号窗体还没有初始化时就传递当前this指针给添加帐号窗体,这样可以在在帐号窗体init时才能获取到
	Setadminps.Create(IDD_DIALOG3);
	Setadminps.ShowWindow(SW_SHOW);
	Setadminps.SetWindowText(_T("设置密码"));
}

//检测窗口名
void CheckWindows(CMyDlg* MydlgThis)
{
	while(1)
	{
		if(CanMonwindows)
		{
			EnumWindows(EnumWindowsProcess,0);
		}
		Sleep(1000);
	}


}

void CMyDlg::OnBnClickedCheck1()
{
	if ( BST_CHECKED == IsDlgButtonChecked( IDC_CHECK1 ) )
	{
		CanMonwindows=true;
	}
	else
	{
		CanMonwindows=false;
	}
}
bool CMyDlg::Checkadminps(CString Pass)
{
	if(Pass==ini.GetValue("PS","Pass"))
	{
		return true; 
	}
	return false;
}
void CMyDlg::SetAdminmode(bool admin)
{
	if(admin)
	{
		Adminmode=true;

	}
	else
	{
		Adminmode=false;
	}
}
void CMyDlg::Showmainwindow()
{
	
	if(Adminmode)
	{
		this->ShowWindow(SW_SHOWNORMAL); // 显示主窗口
		SetForegroundWindow();
	}
	else
	{
		Inputadminps.DeleteTempMap();
		Inputadminps.dispatchMap;
		Inputadminps.DestroyWindow();

		Inputadminps.MainThis = (DWORD)this;//在帐号窗体还没有初始化时就传递当前this指针给添加帐号窗体,这样可以在在帐号窗体init时才能获取到
		Inputadminps.Create(IDD_CheckAdmin);
		Inputadminps.ShowWindow(SW_SHOW);
		Inputadminps.SetForegroundWindow();
		Inputadminps.SetWindowText(_T("输入密码"));
	}
}