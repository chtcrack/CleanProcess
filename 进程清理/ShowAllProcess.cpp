// ShowAllProcess.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "��������.h"
#include "ShowAllProcess.h"
#include "afxdialogex.h"
#include "��������Dlg.h"
#include "Tlhelp32.h"
#include <windows.h>
#include <Psapi.h>
#pragma comment (lib,"Psapi.lib")

CMyDlg* Mydlgptr;
// ShowAllProcess �Ի���
//����pid��ȡ������
/*
TCHAR szBuffer[256] = {0};
GetProcessCommandLine(628, szBuffer, sizeof(szBuffer)); //628�ĳ���Ҫ��ȡ�Ľ���PID,*/
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


// ShowAllProcess ��Ϣ�������


void ShowAllProcess::OnBnClickedOk()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	GetAllProcess();
}




BOOL ShowAllProcess::OnInitDialog()
{
	CDialog::OnInitDialog();
	MyDebug("���̹����ʼ��");
	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	Mydlgptr=(CMyDlg*)MainWinThis;
	DWORD exstyle = m_showlist.GetExtendedStyle();
	m_showlist.SetExtendedStyle(exstyle | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES |
		WS_EX_STATICEDGE );
	CRect rect;
	m_showlist.GetClientRect(&rect);
	int nColInterval = rect.Width()/6;//����ÿ��Ŀ��Ϊlist�б��ȵ�4��֮һ
	m_showlist.InsertColumn(0,_T("������"), LVCFMT_LEFT, nColInterval);
	m_showlist.InsertColumn(1,_T("�߳���"), LVCFMT_LEFT, nColInterval/3);
	m_showlist.InsertColumn(2,_T("PID"), LVCFMT_LEFT, nColInterval/4);
	m_showlist.InsertColumn(3,_T("���ȼ�"), LVCFMT_LEFT, nColInterval/3);
	m_showlist.InsertColumn(4,_T("·��"), LVCFMT_LEFT, nColInterval*2);
	m_showlist.InsertColumn(5,_T("����"), LVCFMT_LEFT, nColInterval);
	m_showlist.InsertColumn(6,_T("�ڴ�"), LVCFMT_LEFT, nColInterval/2);
	m_showlist.InsertColumn(7,_T("���ڱ���"), LVCFMT_LEFT, nColInterval/2);
	GetAllProcess();
	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}
//��ȡ���н���
void ShowAllProcess::GetAllProcess()
{
	m_showlist.DeleteAllItems();
	HANDLE Snapshot=::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);
	//���ĳһʱ��ϵͳ�Ľ��̡��ѣ�heap����ģ�飨module�����̵߳Ŀ�����Ϣ
	PROCESSENTRY32 processListStr;
	processListStr.dwSize=sizeof(PROCESSENTRY32);
	BOOL return_value;
	return_value=::Process32First(Snapshot,&processListStr);
	//���ϵͳ���������е�һ�����̵���Ϣ
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
		//���ϵͳ������������һ�����̵���Ϣ
		i++;
	}
	
	CloseHandle(Snapshot);
}

//����������̰�ť
void ShowAllProcess::On32778()
{
	int iItemSel = m_showlist.GetNextItem(-1, LVIS_SELECTED);
	CString exepid,exename,InfoText=_T("");
	while(iItemSel != -1)//��������ѡ��Ľ���
	{
		//m_cListCtrl.DeleteItem(iItemSel);//delete selected item in the list control
		exepid=m_showlist.GetItemText(iItemSel,2);//���ø÷������Ի�ȡ���ݼ�¼�����,�˴�����2�ǽ���pid
		exename=m_showlist.GetItemText(iItemSel,0);//���ø÷������Ի�ȡ���ݼ�¼�����,�˴�����0�ǽ�����
		//�ر�ָ����PID����
		HANDLE hprocess=::OpenProcess(PROCESS_ALL_ACCESS,FALSE,atoi(exepid));
		if(hprocess!=NULL && atoi(exepid)!=0)
		{
			::TerminateProcess(hprocess,0);
			::CloseHandle(hprocess);
			Mydlgptr->GetDlgItemText(IDC_INFOP,InfoText);
			Mydlgptr->SetDlgItemText(IDC_INFOP,_T("����")+exename+_T("�Ѹɵ�")+ Mydlgptr->Getsystime() + InfoText );
		}
		else
		{
			Mydlgptr->GetDlgItemText(IDC_INFOP,InfoText);
			Mydlgptr->SetDlgItemText(IDC_INFOP,_T("��������")+exename+_T("ʧ��")+ Mydlgptr->Getsystime() + InfoText );
			
		}
		iItemSel = m_showlist.GetNextItem(iItemSel, LVIS_SELECTED);
	}
	GetAllProcess();
}

//Ϊ�������Ҽ���Ӳ˵�
void ShowAllProcess::OnNMRClickListShow(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	DWORD dwPos = GetMessagePos();
	CPoint point( LOWORD(dwPos), HIWORD(dwPos) );   
	CMenu menu;
	VERIFY( menu.LoadMenu( IDR_ProcessMENU ) );       //IDR_ProcessMENU�ǽ��̲˵�ID
	CMenu* popup = menu.GetSubMenu(0);
	popup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y,this );//TPM_RIGHTBUTTONʹ�Ҽ�����˵�Ҳ������
	popup=0;
	*pResult = 0;
}
CString ShowAllProcess::PidToPath(DWORD id)//���ݽ���pid��ȡ����·��
{  
	CString s;
	HANDLE  hToken;
	TCHAR path[MAX_PATH+1]={0};
	OpenProcessToken(GetCurrentProcess(),TOKEN_ADJUST_PRIVILEGES,&hToken);  //��ȡ����
	EnablePrivileges(hToken,SE_DEBUG_NAME);  //����Ȩ��
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

	LookupPrivilegeValue( NULL,szPrivName,&tkp.Privileges[0].Luid );//�޸Ľ���Ȩ��
	tkp.PrivilegeCount=1;
	tkp.Privileges[0].Attributes=SE_PRIVILEGE_ENABLED;
	AdjustTokenPrivileges( hToken,FALSE,&tkp,sizeof tkp,NULL,NULL );//֪ͨϵͳ�޸Ľ���Ȩ��

	return( (GetLastError()==ERROR_SUCCESS) );

}
//�������ѡ����̰�ť
void ShowAllProcess::OnBnClickedButton1()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	On32778();
}


void ShowAllProcess::OpenExeDir()
{
	int iItemSel = m_showlist.GetNextItem(-1, LVIS_SELECTED);
	CString exepath="";
	while(iItemSel != -1)
	{
		//m_cListCtrl.DeleteItem(iItemSel);//delete selected item in the list control
		exepath=m_showlist.GetItemText(iItemSel,4);//���ø÷������Ի�ȡ���ݼ�¼�����,�˴�����4�ǽ���·��

		//�ر�ָ����PID����
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
	// TODO: �ڴ���ӿؼ�֪ͨ����������

	CDialog::OnCancel();
}


void ShowAllProcess::OnClose()
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	Mydlgptr=0;
	delete Mydlgptr;
	CDialog::OnClose();
}
//���ݽ���pid��ý���ռ���ڴ�,���ص�λ��СKB
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
			ret = pmc.PagefileUsage/1024; //�����ڴ�ֵ����λKByte
			
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
	GetWindowText(hwnd, szTitle, sizeof(szTitle) / sizeof(TCHAR)); // ��ȡ��������
	GetClassName(hwnd,classname,sizeof(classname));
	CString TempClassName=classname;
	if(dwProcessId == dwCurProcessId && GetParent(hwnd) == NULL)
	{
		*((HWND *)lParam) = hwnd;
		return FALSE;
	}
	return TRUE;
}
//����pid��ȡ���ھ��
HWND GetAppWindow(DWORD Pid)
{
	if(!EnumWindows(EnumWindowsProc, (LPARAM)&Pid))
	{    
		return (HWND)Pid;
	}
	return NULL;
}

//����ָ������
void ShowAllProcess::HideSelectedWindow()
{
	int iItemSel = m_showlist.GetNextItem(-1, LVIS_SELECTED);
	CString exepid="";
	while(iItemSel != -1)
	{
		//m_cListCtrl.DeleteItem(iItemSel);//delete selected item in the list control
		exepid=m_showlist.GetItemText(iItemSel,2);//���ø÷������Ի�ȡ���ݼ�¼�����,�˴�����2�ǽ���pid
		HWND AppHwnd=GetAppWindow(atoi(exepid));
		
		::ShowWindow(AppHwnd, SW_HIDE);
		iItemSel = m_showlist.GetNextItem(iItemSel, LVIS_SELECTED);
	}
}

//��ʾָ������
void ShowAllProcess::ShowSelectedWindow()
{
	int iItemSel = m_showlist.GetNextItem(-1, LVIS_SELECTED);
	CString exepid="";
	while(iItemSel != -1)
	{
		//m_cListCtrl.DeleteItem(iItemSel);//delete selected item in the list control
		exepid=m_showlist.GetItemText(iItemSel,2);//���ø÷������Ի�ȡ���ݼ�¼�����,�˴�����2�ǽ���pid
		HWND AppHwnd=GetAppWindow(atoi(exepid));
		::ShowWindow(AppHwnd, SW_SHOW);
		::SetForegroundWindow(AppHwnd);
		iItemSel = m_showlist.GetNextItem(iItemSel, LVIS_SELECTED);
	}
}

//�������
void ShowAllProcess::SuspendProcess()
{
	int iItemSel = m_showlist.GetNextItem(-1, LVIS_SELECTED);
	
	while(iItemSel != -1)
	{
		//m_cListCtrl.DeleteItem(iItemSel);//delete selected item in the list control
		DWORD processID= _ttol(m_showlist.GetItemText(iItemSel,2).GetBuffer(0));//���ø÷������Ի�ȡ���ݼ�¼�����,�˴�����2�ǽ���pid
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

//�ָ�����
void ShowAllProcess::ResumeProcess()
{
	int iItemSel = m_showlist.GetNextItem(-1, LVIS_SELECTED);

	while(iItemSel != -1)
	{
		//m_cListCtrl.DeleteItem(iItemSel);//delete selected item in the list control
		DWORD processID= _ttol(m_showlist.GetItemText(iItemSel,2).GetBuffer(0));//���ø÷������Ի�ȡ���ݼ�¼�����,�˴�����2�ǽ���pid
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
