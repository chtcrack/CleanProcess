// ��������Dlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "��������.h"
#include "��������Dlg.h"
#include "Tlhelp32.h"
#include ".\��������dlg.h"
#include "gdiplus.h" 
using namespace Gdiplus;
#pragma comment(lib,"gdiplus.lib")
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define WM_NC (WM_USER+1002)
#define Estructcount 50
#define Estructsize 256//��ؽṹ���ֽ���
// CMyDlg �Ի���
DWORD dwPID;
char DllPath[500];
CMyDlg* MainThis = 0;
CIni ini("Setting.ini");
WantMprocess EProcessStruct[Estructcount];
WantWindowsNameStruct Ewindowsname[50];
NOTIFYICONDATA NotifyIcon;
bool CanMon = true;
bool CanMonwindows = true;
bool Adminmode = false;
bool AutoCapture = false;
TCHAR outputbuffer[1024] = { 0 };
CString Dosinfo, Adminps;
HANDLE Monitorprocess;
HANDLE AdminH;
DWORD64 ConAddr;//���������������ڴ�ӳ���ļ���ַ


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
	ON_MESSAGE(WM_NC, OnNotifyIcon)
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
	ON_BN_CLICKED(IDC_CHECK2, &CMyDlg::OnBnClickedCheck2)
END_MESSAGE_MAP()

//���ͽṹ�����ݸ�dll
int SendDataToDll()
{
	AdminH = OpenFileMapping(FILE_MAP_WRITE, FALSE, "AdminWindows");
	if (AdminH == 0)
	{
		MyDebug("�����ڴ�ӳ��");
		AdminH = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, 0x5000, "AdminWindows");
	}
	if (AdminH != 0)
	{

		ConAddr = (DWORD64)(::MapViewOfFile(AdminH, FILE_MAP_WRITE, 0, 0, 0));
		MyDebug("��ȡ�ڴ�ӳ���ַ-%08X", ConAddr);
	}
	if (ConAddr == 0)
	{
		MyDebug("�ڴ�ӳ�䴴��ʧ��", ConAddr);
	}
	else
	{
		memset((PVOID)ConAddr, 0, 0x5000);
	}
	//���ͽṹ���dll
	DWORD64 tmpaddr = 0;
	if (ConAddr)
	{
		MyDebug("��ʼ���ͽṹ��");
		tmpaddr = (DWORD64)ConAddr;
		MyDebug("��ȡ�ڴ�ӳ���ַ2-%08X", tmpaddr);
		
		for (int i = 0; i < Estructcount; i++)
		{
			memcpy((LPVOID)tmpaddr, &Ewindowsname[i], Estructsize);
			tmpaddr += Estructsize;
		}
		MyDebug("���ͽṹ�����");
		if (BST_CHECKED == IsDlgButtonChecked(MainThis->GetSafeHwnd(),IDC_CHECK1))
		{
			CanMonwindows = true;
			if (!IsBadWritePtr((LPVOID)ConAddr, 4)) *(DWORD64*)(ConAddr + 0x4010) = 0x1;
		}
		else
		{
			CanMonwindows = false;
			if (!IsBadWritePtr((LPVOID)ConAddr, 4)) *(DWORD64*)(ConAddr + 0x4010) = 0x0;
		}
	}
	else {
		return 0;
	}
	return 1;
}

// CMyDlg ��Ϣ�������

BOOL CMyDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��
	LoadiniToListCtrl();
	//EnablePrivilege();
	int selfbit = sizeof(int*);
	if (selfbit==8)//������64λ�Ž���ע�����
	{
		int i;
		char DllName[] = "Monitor.dll";
		GetModuleFileName(NULL, DllPath, 500);

		for (i = strlen(DllPath) - 1; i >= 0; i--)
		{
			if (DllPath[i] == '\\')
			{
				break;
			}
		}
		CopyMemory(DllPath + (i + 1), DllName, strlen(DllName));
		DllPath[i + strlen(DllName) + 1] = 0x00;
		//dwPID = FindProcessID("515sa.exe");
		dwPID = FindProcessID("explorer.exe");
		if (dwPID == (DWORD)INVALID_HANDLE_VALUE)
		{
			MyDebug("Can't find Process explorer.exe\n");
		}
		else
		{
			//printf("dll·��:%s", DllPath);
			MyDebug("��ʼע��dll %s", DllPath);
			if (!SendDataToDll())
			{
				::CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)CheckWindows, (LPVOID)this, NULL, NULL);//����һ���߳�
			}
			else {
				if (!IsBadWritePtr((LPVOID)ConAddr, 4)) *(DWORD64*)(ConAddr + 0x4010) = 0x1;
				Inject(dwPID, DllPath);
			}
		}
	}

	
	((CButton*)GetDlgItem(IDC_CHECK1))->SetCheck(BST_CHECKED);
	Adminps = ini.GetValue("PS", "Pass");
	CString AutoStartValue = ini.GetValue("System", "AutoStart");
	if (AutoStartValue == "1")
	{
		((CButton*)GetDlgItem(IDC_CHECK2))->SetCheck(BST_CHECKED);
	}
	else {
		((CButton*)GetDlgItem(IDC_CHECK2))->SetCheck(BST_UNCHECKED);
	}
	AutoStartValue= ini.GetValue("System", "AutoCapture");
	if (AutoStartValue == "1")
	{
		((CButton*)GetDlgItem(IDC_CHECK3))->SetCheck(BST_CHECKED);
	}
	else {
		((CButton*)GetDlgItem(IDC_CHECK3))->SetCheck(BST_UNCHECKED);
	}
	synctimeing = false;
	MainThis = this;
	this->SetWindowText(_T("��������V2.0-����ʱ��:2020-06-05"));
	INITPorcessStruct();//��ʼ�����̽ṹ��
	InitNotify();//��ʼ��ͼ��
	Shell_NotifyIcon(NIM_ADD, &NotifyIcon);
	

	Monitorprocess = ::CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)CheckProcess, (LPVOID)this, NULL, NULL);//����һ���߳�
	
	Nsynctime();
	return TRUE;  // ���������˿ؼ��Ľ��㣬���򷵻� TRUE
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�
//ͬ��ʱ�亯�����ڶ��߳�
void SyncTime(CMyDlg* MydlgThis)
{
	MydlgThis->synctimeing = true;
	synctimereg();
	int SyncError = 0;
	CString InfoText = _T("");
	CString TimeServerAddress = _T("210.72.145.44");
	MydlgThis->GetDlgItemText(IDC_INFOP, InfoText);
	while (SyncError <= 3)
	{
		if (MydlgThis->StartSynctime(TimeServerAddress))
		{
			break;
		}
		else
		{
			TimeServerAddress = _T("time-a.nist.gov");
			SyncError++;
			if (SyncError == 3)
			{
				MydlgThis->SetDlgItemText(IDC_INFOP, _T("����ʱ��ͬ��ʧ��,ԭ��:") + Dosinfo + MydlgThis->Getsystime() + InfoText);
				MydlgThis->ShowTrayMessage(_T("����ʱ��ͬ��ʧ��,����鿴����"));
			}
		}
		Sleep(1000);
	}
	MydlgThis->synctimeing = false;
}
//ͬ��ʱ������ע���
bool synctimereg()
{


	HKEY hKey;
	//�ҵ�ϵͳ��������  

	LPCTSTR lpRun = TEXT("SYSTEM\\CurrentControlSet\\services\\W32Time\\Config");
	//��������Key
	long lRet = RegOpenKeyEx(HKEY_LOCAL_MACHINE, lpRun, 0, KEY_WRITE, &hKey);
	if (lRet == ERROR_SUCCESS)
	{
		DWORD dwValue = 0x0000D2F0;
		lRet = RegSetValueEx(hKey, "MaxNegPhaseCorrection", 0, REG_DWORD, (CONST BYTE*)&dwValue, sizeof(DWORD));
		lRet = RegSetValueEx(hKey, "MaxPosPhaseCorrection", 0, REG_DWORD, (CONST BYTE*)&dwValue, sizeof(DWORD));
		//�ر�ע���
		RegCloseKey(hKey);
		if (lRet == ERROR_SUCCESS)
		{
			//ע���д��ɹ�
			return true;
		}
		else
		{
			//ע���д��ʧ�� 
			return false;
		}
	}
	else
	{
		//ע����ʧ��
		return false;
	}
}

//��ʼͬ��ʱ�亯��
bool CMyDlg::StartSynctime(CString TimeserverAddr)
{
	CString szDir;
	CString InfoText = _T("");
	GetSystemDirectory(szDir.GetBuffer(MAX_PATH), MAX_PATH);
	szDir.ReleaseBuffer(-1);
	PROCESS_INFORMATION ppi;
	ppi = RunApp(szDir, _T("net.exe start w32time"));
	WaitForSingleObject(ppi.hProcess, INFINITE);
	CloseHandle(ppi.hProcess);
	CloseHandle(ppi.hThread);
	ppi = RunApp(szDir, _T("w32tm.exe /config /update /manualpeerlist:") + TimeserverAddr);
	WaitForSingleObject(ppi.hProcess, INFINITE);
	CloseHandle(ppi.hProcess);
	CloseHandle(ppi.hThread);
	ppi = RunApp(szDir, _T("w32tm.exe /resync"));
	WaitForSingleObject(ppi.hProcess, INFINITE);
	CloseHandle(ppi.hProcess);
	CloseHandle(ppi.hThread);
	int n = Dosinfo.Find(_T("ִ����"));
	GetDlgItemText(IDC_INFOP, InfoText);

	if (n != -1)
	{
		SetDlgItemText(IDC_INFOP, _T("����ʱ��ͬ���ɹ�!") + Getsystime() + InfoText);
		ShowTrayMessage(_T("����ʱ��ͬ���ɹ�!"));
		return true;
	}
	else
	{

		return false;

	}
}
//����ָ������,����1·��,����
PROCESS_INFORMATION RunApp(CString runpath, CString exename, bool Showwindows)
{
	CString InfoText = _T("");
	CString str;
	SECURITY_ATTRIBUTES sa;
	HANDLE hRead, hWrite;

	sa.nLength = sizeof(SECURITY_ATTRIBUTES);
	sa.lpSecurityDescriptor = NULL;
	sa.bInheritHandle = TRUE;
	CreatePipe(&hRead, &hWrite, &sa, 0);

	STARTUPINFO si; //һЩ�ر���������

	memset(&si, 0, sizeof(STARTUPINFO));
	GetStartupInfo(&si);
	si.hStdError = hWrite;            //�Ѵ������̵ı�׼��������ض��򵽹ܵ�����
	si.hStdOutput = hWrite;  //�Ѵ������̵ı�׼����ض��򵽹ܵ�����
	si.cb = sizeof(STARTUPINFO);

	si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
	if (Showwindows)
	{
		si.wShowWindow = SW_SHOW;
	}
	else
	{
		si.wShowWindow = SW_HIDE;
	}
	PROCESS_INFORMATION pi; //�ر��������ý���

	str.Format(_T("%s\\%s"), runpath, exename);

	//CreateProcess(NULL,str.AllocSysString(),NULL,NULL,TRUE,0,NULL,runpath,&si,&pi);
	CreateProcess(NULL, (LPSTR)(LPCTSTR)str, NULL, NULL, TRUE, 0, NULL, runpath, &si, &pi);
	CloseHandle(hWrite);


	DWORD bytesRead = 0;
	memset(outputbuffer, 0, 1024);
	Dosinfo = "";
	while (true)
	{
		if (ReadFile(hRead, outputbuffer, 1024, &bytesRead, NULL) == NULL)
			break;
		//buffer�о���ִ�еĽ�������Ա��浽�ı���Ҳ����ֱ�����
		Dosinfo = outputbuffer;
		//MyDebug(_T("��Ϣ:%s"),Dosinfo);

	}
	CloseHandle(hRead);



	return pi;

}


//��ʼ�����̽ṹ��
void INITPorcessStruct()
{
	for (int i = 0; i < Estructcount; i++)
	{
		EProcessStruct[i].NeedEndProcessName = _T("");
		EProcessStruct[i].PClassName = _T("");
		EProcessStruct[i].PWindowsName = _T("");

	}

	//����һֱǿ�Ƹɵ�ĳ��������ʱ,�ɽ�PClassName����PWindowsName����Ϊ��㲻���ڵ��ַ���
	EProcessStruct[0].NeedEndProcessName = _T("PPSKernel.exe");
	EProcessStruct[0].PClassName = _T("MainForm");
	EProcessStruct[0].PWindowsName = _T("������PPSӰ��");
	EProcessStruct[1].NeedEndProcessName = _T("QvodTerminal.exe");
	EProcessStruct[1].PClassName = _T("QVODPLAYERWND");
	EProcessStruct[1].PWindowsName = _T("");
	EProcessStruct[2].NeedEndProcessName = _T("PPAP.exe");
	EProcessStruct[2].PClassName = _T("PPLiveGUI");
	EProcessStruct[2].PWindowsName = _T("");
	EProcessStruct[3].NeedEndProcessName = _T("DuoMi.exe");
	EProcessStruct[3].PClassName = _T("duomi_maindlg");
	EProcessStruct[3].PWindowsName = _T("");
	EProcessStruct[4].NeedEndProcessName = _T("XLGameBoxToolTip.exe");
	EProcessStruct[4].PClassName = _T("Randsdsds");
	EProcessStruct[5].NeedEndProcessName = _T("BaiduP2PService.exe");
	EProcessStruct[5].PClassName = _T("WndBase");
	EProcessStruct[5].PWindowsName = _T("�ٶ�Ӱ��");
	EProcessStruct[6].NeedEndProcessName = _T("FunshionService.exe");
	EProcessStruct[6].PClassName = _T("funshion_player_tzdenjohn");
	EProcessStruct[6].PWindowsName = _T("���� 2.8");
	EProcessStruct[7].NeedEndProcessName = _T("PPSProtect.exe");
	EProcessStruct[7].PClassName = _T("MainForm");
	EProcessStruct[7].PWindowsName = _T("������PPSӰ��");
	EProcessStruct[8].NeedEndProcessName = _T("PPSDesktopTip.exe");
	EProcessStruct[8].PClassName = _T("MainForm");
	EProcessStruct[8].PWindowsName = _T("������PPSӰ��");
	EProcessStruct[9].NeedEndProcessName = _T("QiyiService.exe");
	EProcessStruct[9].PClassName = _T("#32770");
	EProcessStruct[9].PWindowsName = _T("��������Ƶ");
	EProcessStruct[10].NeedEndProcessName = _T("QiyiKernel.exe");
	EProcessStruct[10].PClassName = _T("#32770");
	EProcessStruct[10].PWindowsName = _T("��������Ƶ");
	EProcessStruct[11].NeedEndProcessName = _T("MobProtect.exe");
	EProcessStruct[11].PClassName = _T("E7C66FDE-F032-4741-A357-0FE17B1F8A37");
	EProcessStruct[11].PWindowsName = _T("������PPSӰ��");
	EProcessStruct[12].NeedEndProcessName = _T("AndroidService.exe");
	EProcessStruct[12].PClassName = _T("dfdfdf");
	EProcessStruct[12].PWindowsName = _T("������PPSӰ��");
	EProcessStruct[13].NeedEndProcessName = _T("QyKernel.exe");
	EProcessStruct[13].PClassName = _T("");
	EProcessStruct[13].PWindowsName = _T("������PPSӰ��");

	//EProcessStruct[0].eCount=11;
	//��ʱ������ɴ������ļ��ж����˽ṹ��
}


//������
void CheckProcess(CMyDlg* MydlgThis)
{
	int estructcount = sizeof(EProcessStruct) / sizeof(WantMprocess);
	//MyDebug("�ṹ���С%i,�ṹ�������С%i,Ԫ�ظ���%i\r\n",sizeof(WantMprocess),sizeof(EProcessStruct),estructcount);

	HWND Processhwnd = 0;
	CString InfoText = _T("");
	while (CanMon)
	{
		for (int i = 0; i < estructcount; i++)
		{

			if (EProcessStruct[i].NeedEndProcessName != _T(""))
			{

				if (EProcessStruct[i].PWindowsName == _T("") && EProcessStruct[i].PClassName != _T(""))
				{
					Processhwnd = ::FindWindow(EProcessStruct[i].PClassName, NULL);
				}
				if (EProcessStruct[i].PClassName == _T("") && EProcessStruct[i].PWindowsName != _T(""))
				{
					Processhwnd = ::FindWindow(NULL, EProcessStruct[i].PWindowsName);
				}
				if (EProcessStruct[i].PClassName != _T("") && EProcessStruct[i].PWindowsName != _T(""))
				{
					Processhwnd = ::FindWindow(EProcessStruct[i].PClassName, EProcessStruct[i].PWindowsName);
				}
				if (Processhwnd == NULL)//û�ҵ���Ӧ�Ĳ��ų���,��ô���Ƿ��к�̨�������������
				{
					if (MydlgThis->CheckAndKillProcess(EProcessStruct[i].NeedEndProcessName))
					{
						MydlgThis->GetDlgItemText(IDC_INFOP, InfoText);
						MydlgThis->SetDlgItemText(IDC_INFOP, _T("����") + EProcessStruct[i].NeedEndProcessName + _T("�Ѹɵ�") + MydlgThis->Getsystime() + InfoText);
						MydlgThis->ShowTrayMessage(_T("����") + EProcessStruct[i].NeedEndProcessName + _T("�Ѹɵ�"));
					}
				}
			}
		}
		if(AutoCapture) CaptureScreen();
		Sleep(3000);
	}

}
//�жϽ����Ƿ����,���ھ͸ɵ�������,�����ڷ��ؼ�
bool CMyDlg::CheckAndKillProcess(LPCTSTR Processname)
{
	PROCESSENTRY32 pe;
	DWORD id = 0;
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	pe.dwSize = sizeof(PROCESSENTRY32);
	if (!Process32First(hSnapshot, &pe))
		return false;
	while (1)
	{
		pe.dwSize = sizeof(PROCESSENTRY32);
		if (Process32Next(hSnapshot, &pe) == FALSE)
			break;

		if (_tcscmp(pe.szExeFile, Processname) == 0)
		{
			id = pe.th32ProcessID;
			HANDLE hprocess = ::OpenProcess(PROCESS_ALL_ACCESS, FALSE, id);
			/*	DWORD errorCode=GetLastError();
			CString Infotext,errcodestr;
			GetDlgItemText(IDC_INFOP,Infotext);
			errcodestr.Format(_T("%d"),errorCode);
			SetDlgItemText(IDC_INFOP,_T("�������") +errcodestr+ Infotext);*/
			if (hprocess != NULL)
			{
				if (::TerminateProcess(hprocess, 0))
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
	if (id > 0)
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
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ��������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù����ʾ��
HCURSOR CMyDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}
//��ʼ������ͼ��
void CMyDlg::InitNotify()
{
	NotifyIcon.cbSize = (DWORD)sizeof(NOTIFYICONDATA);
	NotifyIcon.hWnd = this->m_hWnd;

	NotifyIcon.uID = IDR_MAINFRAME;//ͼ��ID ������Ϣ��Ӧ����OnNotifyIcon��� wParam����
	NotifyIcon.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP | NIF_INFO;
	NotifyIcon.dwInfoFlags = NIIF_INFO;
	NotifyIcon.uTimeout = 2000;//������Ϣ�Զ���ʧʱ��5��
	NotifyIcon.uCallbackMessage = WM_NC;// �Զ������Ϣ����
	NotifyIcon.hIcon = LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_MAINFRAME));
	_tcscpy_s(NotifyIcon.szInfoTitle, _T("��ʾ")); // ��Ϣ��ʾ��Ϊ"��������"
	_tcscpy_s(NotifyIcon.szTip, _T("��������")); // ��Ϣ��ʾ��Ϊ"��������"
	_tcscpy_s(NotifyIcon.szInfo, _T("������������������,�������Ҽ��������ͼ��������Ŷ!")); // ��Ϣ��ʾ��Ϊ"��������"



}
//��ʾ������Ϣ
void CMyDlg::ShowTrayMessage(CString Msg)
{

	_tcscpy_s(NotifyIcon.szInfoTitle, _T("��Ϣ:"));
	_tcscpy_s(NotifyIcon.szInfo, Msg);
	NotifyIcon.uTimeout = 2000;//������Ϣ�Զ���ʧʱ��3��
	Shell_NotifyIcon(NIM_MODIFY, &NotifyIcon);
}

//������Ӧ����¼�
LRESULT CMyDlg::OnNotifyIcon(WPARAM wParam, LPARAM IParam)
{
	if (wParam != IDR_MAINFRAME)
		return 0;
	switch (IParam)
	{
	case WM_RBUTTONUP: // �Ҽ�����ʱ�����˵�
	{
		LPPOINT lpoint = new tagPOINT;
		::GetCursorPos(lpoint); // �õ����λ��
		CMenu menu;
		//menu.CreatePopupMenu(); // ����һ������ʽ�˵�
		SetForegroundWindow();//�������������涥��������˵���������ʧ
		//menu.AppendMenu(MF_STRING, WM_DESTROY, _T("�ر�"));
		VERIFY(menu.LoadMenu(IDR_MENU1));
		CMenu* popup = menu.GetSubMenu(0);
		if (CanMon)
		{
			menu.EnableMenuItem(ID__32772, MF_DISABLED);

		}
		else
		{
			menu.EnableMenuItem(ID_32771, MF_DISABLED);
		}
		if (synctimeing)
		{
			menu.EnableMenuItem(ID_32777, MF_DISABLED);
		}
		popup->TrackPopupMenu(TPM_LEFTALIGN, lpoint->x, lpoint->y, this);
		HMENU hmenu = menu.Detach();
		menu.DestroyMenu();
		delete lpoint;
		popup = 0;
		//delete popup;
		break;
	}

	case WM_LBUTTONDOWN: // ��������Ĵ���
	{

		Showmainwindow();

		break;
	}

	case NIN_BALLOONUSERCLICK: // ���û���������ʾ�ϻ�����ͼ���ϵ�����꣨��ʱ��������ʾ״̬��ʱ���ʹ���Ϣ
	{
		Showmainwindow();
		break;
	}

	case NIN_BALLOONSHOW: // ��ʾ������ʾʱ����
	{
		break;
	}
	case NIN_BALLOONTIMEOUT: // �����ڳ�ʱ�����û����������ϵĹرհ�ť(X)��ʹ������ʧʱ���ʹ���Ϣ
	{
		break;
	}
	case NIN_BALLOONHIDE: // ������ʾ��ʧʱ���ͣ����磬��ͼ�걻ɾ���������Ϊ��ʱ�����û��������������ʧ������Ϣ���ᱻ����
	{
		break;
	}
	}
	return 1;
}

void CMyDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);
	if (nType == SIZE_MINIMIZED)
	{
		Adminmode = false;
		ShowWindow(SW_HIDE); // ����С��ʱ������������ 
	}
	// TODO: �ڴ˴������Ϣ����������
}



void CMyDlg::OnDestroy()
{
	
		

	CDialog::OnDestroy();
	UnmapViewOfFile((LPCVOID)ConAddr);
	CloseHandle(AdminH);
	// ��������ɾ��ͼ��
	Shell_NotifyIcon(NIM_DELETE, &NotifyIcon);

	// TODO: �ڴ˴������Ϣ����������
}

void CMyDlg::OnNcPaint()
{
	static int i = 2;
	if (i > 0)
	{
		i--;
		ShowWindow(SW_HIDE);
	}
	else
		CDialog::OnNcPaint();
}
//д��ϵͳע���������
bool AutoStart(bool DeleteKey)
{

	TCHAR StartupName[] = TEXT("CleanProcess");
	HKEY hKey;
	//�ҵ�ϵͳ��������  

	LPCTSTR lpRun = TEXT("Software\\Microsoft\\Windows\\CurrentVersion\\Run");
	//��������Key
	long lRet = RegOpenKeyEx(HKEY_LOCAL_MACHINE, lpRun, 0, KEY_WRITE, &hKey);
	if (lRet == ERROR_SUCCESS)
	{
		TCHAR pFileName[MAX_PATH] = { 0 };
		//�õ����������ȫ·��
		GetModuleFileName(NULL, pFileName, MAX_PATH);
		//MyDebug(_T("%s"),pFileName);
		//���һ����Key,������ֵ StartupName=pFileName
		if (DeleteKey)
		{
			lRet = RegDeleteValue(hKey, StartupName);
		}
		else {
			lRet = RegSetValueEx(hKey, StartupName, 0, REG_SZ, (BYTE*)pFileName, (lstrlen(pFileName) + 1) * sizeof(TCHAR));
		}

		//�ر�ע���
		RegCloseKey(hKey);
		if (lRet == ERROR_SUCCESS)
		{
			//ע���д��ɹ�
			return true;
		}
		else
		{
			//ע���д��ʧ�� 
			return false;
		}
	}
	else
	{
		//ע����ʧ��
		return false;
	}
}
void CMyDlg::OnClose()
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	Adminmode = false;
	Shell_NotifyIcon(NIM_ADD, &NotifyIcon);
	ShowWindow(SW_MINIMIZE);
	//CDialog::OnClose();
}

//�����ͣ����
void CMyDlg::NPause()
{
	if (!Adminmode) return;
	CanMon = FALSE;

	DWORD Checkthreadcode = 0;
	if (!IsBadWritePtr((LPVOID)ConAddr, 4)) *(DWORD64*)(ConAddr + 0x4010) = 0x0;//д���ڴ���dllֹͣ���
// 	while (1)
// 	{
// 		Checkthreadcode = WaitForSingleObject(Monitorprocess, 2000);
// 		if (Checkthreadcode == WAIT_TIMEOUT)
// 		{
// 		}
// 		else
// 		{
// 			break;
// 		}
// 		Sleep(200);
// 	}
	NotifyIcon.hIcon = LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_ICON1));
	_tcscpy_s(NotifyIcon.szInfoTitle, _T("������!"));
	_tcscpy_s(NotifyIcon.szInfo, _T("����������ֹͣ������!"));
	NotifyIcon.dwInfoFlags = NIIF_ERROR;
	NotifyIcon.uTimeout = 100;//������Ϣ�Զ���ʧʱ��500����
	Shell_NotifyIcon(NIM_MODIFY, &NotifyIcon);
}
//����ָ�����
void CMyDlg::ResumeMon()
{
	CanMon = TRUE;
	if (!IsBadWritePtr((LPVOID)ConAddr, 4)) *(DWORD64*)(ConAddr + 0x4010) = 0x1;//д���ڴ���dll��ʼ���
	Monitorprocess = ::CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)CheckProcess, (LPVOID)this, NULL, NULL);//����һ���߳�
	NotifyIcon.hIcon = LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_MAINFRAME));
	_tcscpy_s(NotifyIcon.szInfoTitle, _T("�ٺ�!"));
	_tcscpy_s(NotifyIcon.szInfo, _T("���������ֿ�ʼ������!"));
	NotifyIcon.dwInfoFlags = NIIF_INFO;
	NotifyIcon.uTimeout = 100;//������Ϣ�Զ���ʧʱ��500����
	Shell_NotifyIcon(NIM_MODIFY, &NotifyIcon);
}

void CMyDlg::DestorySelf()//����˳�����
{
	if (Adminmode)
	{
		MyDebug("��������");
		//ToDo ֪ͨdll����ж��

		if (!IsBadWritePtr((LPVOID)ConAddr, 4))
		{
			MyDebug("����֪ͨdll�����˳�");
			*(DWORD64*)(ConAddr + 0x4000) = 0x1;
		}
		UnmapViewOfFile((LPCVOID)ConAddr);
		CloseHandle(AdminH);
		// ��������ɾ��ͼ��
		Shell_NotifyIcon(NIM_DELETE, &NotifyIcon);
		HANDLE hself = GetCurrentProcess();

		TerminateProcess(hself, 0);
		this->DestroyWindow();
	}

}
BOOL CMyDlg::IsLaterNT()//�ж��Ƿ�ΪNT6�Ժ�İ汾
{
	OSVERSIONINFO osvi;//�汾��Ϣ�ṹ��
	ZeroMemory(&osvi, sizeof(OSVERSIONINFO));
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);

	GetVersionEx(&osvi);
	if (osvi.dwMajorVersion >= 6)
	{
		return TRUE;
	}
	return FALSE;
}
BOOL CMyDlg::EnablePrivilege()//������ǰ����Ȩ��
{
	if (IsLaterNT())
	{
		HANDLE hToken = NULL;
		BOOL bFlag = FALSE;

		if (OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES, &hToken))//��ȡ���̵�������Ϣ
		{
			TOKEN_PRIVILEGES tp;
			tp.PrivilegeCount = 1;
			if (!LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &tp.Privileges[0].Luid))//
			{
				CloseHandle(hToken);
				return FALSE;
			}
			tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
			if (!AdjustTokenPrivileges(hToken, FALSE, &tp, sizeof(tp), NULL, NULL))
			{
				AfxMessageBox(_T("Ȩ�޲��������ù���Ա������б�����"));
				return FALSE;
			}
		}
		CloseHandle(hToken);
	}
	else
	{
		HANDLE hprocess = ::GetCurrentProcess();

		TCHAR privilege[] = SE_DEBUG_NAME;

		TOKEN_PRIVILEGES Privileges;

		Privileges.PrivilegeCount = 1;

		HANDLE hToken;

		OpenProcessToken(hprocess, TOKEN_ADJUST_PRIVILEGES, &hToken);

		LUID luid;

		LookupPrivilegeValue(NULL, privilege, &luid);

		Privileges.Privileges[0].Luid = luid;

		Privileges.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

		if (AdjustTokenPrivileges(hToken, FALSE, &Privileges, NULL, NULL, NULL) != 0)
			return TRUE;

	}
	return TRUE;
}
//���ͬ��ʱ��
void CMyDlg::Nsynctime()
{
	::CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)SyncTime, (LPVOID)this, NULL, NULL);//����һ��ʱ��ͬ���߳�
}
//��ȡϵͳʱ��
CString CMyDlg::Getsystime()
{
	COleDateTime timeNow, dateNow, yearNow;
	yearNow = COleDateTime::GetCurrentTime();     // ��ȡ��ǰ����ʱ��
	timeNow = COleDateTime::GetCurrentTime();     // ��ȡ��ǰ����ʱ��
	dateNow = COleDateTime::GetCurrentTime(); // ͬ����ȡ��ǰ����ʱ�䣬��ôдֻ��Ϊ������
	CString sYear = yearNow.Format(VAR_FOURDIGITYEARS);     // ��ȡ��ǰ���
	CString sTime = timeNow.Format(VAR_TIMEVALUEONLY);     // ��ȡ��ǰʱ��
	CString sDate = dateNow.Format(VAR_DATEVALUEONLY);     // ��ȡ��ǰ����
	return _T("-") + sYear + _T("\r\n");
}

void CMyDlg::OnBnClickedButton2()
{
	Showprocessdlg.DeleteTempMap();
	Showprocessdlg.dispatchMap;
	Showprocessdlg.DestroyWindow();
	Showprocessdlg.MainWinThis = (DWORD64)this;
	Showprocessdlg.Create(IDD_ProcessDIALOG);
	Showprocessdlg.ShowWindow(SW_SHOW);
	Showprocessdlg.SetWindowText(_T("���̹���"));
	// TODO: �ڴ���ӿؼ�֪ͨ����������
}


void CMyDlg::CallProcessManger()
{
	// TODO: �ڴ���������������
	OnBnClickedButton2();
}


void CMyDlg::OnBnClickedButton1()
{
	Clearfiledlg.DeleteTempMap();
	Clearfiledlg.dispatchMap;
	Clearfiledlg.DestroyWindow();
	Clearfiledlg.Create(IDD_CLEARTMPFILE_DIALOG);
	Clearfiledlg.ShowWindow(SW_SHOW);
	Clearfiledlg.SetWindowText(_T("��������"));
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
	Pingydlg.Create(IDD_DIALOG2);
	Pingydlg.ShowWindow(SW_SHOW);
	Pingydlg.SetWindowText(_T("PingTool"));

}

//��ini����ӹؼ���
void CMyDlg::Addkeyword(CString Keyword)
{
	CStringArray arrKey, arrValue;
	CString InserStr = Keyword;
	int n = 0;
	n = ini.GetKeyValues(arrKey, arrValue, "info");
	for (int i = 0; i < n; i++)
	{
		if (InserStr == arrValue[i])
		{
			AfxMessageBox("������ͬ�ؼ���,�������ظ����");
			return;
		}
	}


	CString Getkeys = ini.GetValue("info", "0");
	if (Getkeys == "")
	{
		ini.SetValue("info", "0", InserStr);


	}
	else
	{


		CString Keys;
		Keys.Format("%d", n);
		//AfxMessageBox(Keys);
		ini.SetValue("info", Keys, InserStr);
	}
	ReBuildIni();
	LoadiniToListCtrl();

}
//ɾ��ini��Ĺؼ���
void CMyDlg::DelAccount(CString Keyword)
{
	CStringArray arrKey, arrValue;
	CString AccountName;
	int n = 0;
	n = ini.GetKeyValues(arrKey, arrValue, "info");

	for (int i = 0; i < n; i++)
	{
		if (arrValue[i] == Keyword)
		{
			ini.DelKey("info", arrKey[i]);
		}

	}
	ReBuildIni();
	LoadiniToListCtrl();

}
//����ؼ���ini
void CMyDlg::ReBuildIni()
{

	CStringArray arrKey, arrValue;
	int Count = ini.GetKeyValues(arrKey, arrValue, "info");
	CString Adminps = ini.GetValue("PS", "Pass");
	ini.DelAllSections();

	for (int i = 0; i < Count; i++)
	{
		CString keys;
		keys.Format("%d", i);
		ini.SetValue("info", keys, arrValue[i]);
	}

	ini.SetValue("PS", "Pass", Adminps);

}
//��ȡini�ļ����ݵ�listcontrol�б�
void CMyDlg::LoadiniToListCtrl()
{
	Keywordlist.ResetContent();
	int n = 0;
	CStringArray arrKey, arrValue;
	CString Keyword;
	n = ini.GetKeyValues(arrKey, arrValue, "info");
	int i = 0;
	CString AccountID = "";
	for (int i = 0; i < n; i++)
	{

		Keyword = arrValue[i];
		Keywordlist.AddString(Keyword);


	}
	ReadAccountini();
}

//��ȡ�ؼ����ļ����ṹ��
void ReadAccountini()
{

	InitStruct(true);
	CStringArray arrKey, arrValue;

	int n = ini.GetKeyValues(arrKey, arrValue, "info");
	for (int i = 0; i < n; i++)
	{

		CString2Char(arrValue[i], Ewindowsname[i].WindowsName);
		//MyDebug("ini���ֵ��%s�ṹ�����ֵ��%s",arrValue[i],Ewindowsname[i].WindowsName);

	}
}
//��ʼ���ṹ��
void InitStruct(bool InitPskinAccount)
{
	for (int i = 0; i < 50; i++)
	{
		if (InitPskinAccount)
		{
			memset((PVOID)Ewindowsname[i].WindowsName, 0, 256);//��ʼ���ṹ���б�

		}
	}
}
void CMyDlg::OnBnClickedButton3()
{
	CString Keyword = "";
	GetDlgItemText(IDC_EDIT1, Keyword);
	Addkeyword(Keyword);
	SendDataToDll();
	SetDlgItemText(IDC_EDIT1, "");
}

//˫��Clistbox�¼�
void CMyDlg::OnLbnDblclkList1()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	int iItemSel = Keywordlist.GetCurSel();
	if (iItemSel != -1)
	{
		CString Selword;
		Keywordlist.GetText(iItemSel, Selword);
		//AfxMessageBox(Selword);
		DelAccount(Selword);
		SendDataToDll();
	}

}
BOOL CALLBACK EnumWindowsProcess(HWND hwnd, LPARAM lParam)
{
	//	char classname[256];
	char szTitle[256];
	CString InfoText = _T("");
	::GetWindowText(hwnd, szTitle, sizeof(szTitle)); // ��ȡ��������
	CString Winname = szTitle;
	Winname = Winname.MakeLower();//ǿ��ת����Сд
	//MyDebug("��ȡ���Ĵ�������%s\r\n",szTitle);
	//Winname.Format(_T("%s"), szTitle);
	//AfxMessageBox(szTitle);
	//::GetClassName(hwnd,classname,sizeof(classname));
	CString needfindname = "";
	HWND Phwnd;
	int Ewindowsnameelnumber = sizeof(Ewindowsname) / sizeof(Ewindowsname[0]);
	for (int i = 0; i < Ewindowsnameelnumber; i++)
	{
		needfindname = Ewindowsname[i].WindowsName;
		needfindname = needfindname.MakeLower();//ǿ��ת����Сд
		//MyDebug("��������%sҪ�ҵ����ַ�����%s\r\n",Winname,needfindname);
		if (Winname.Find(needfindname) != -1 && Winname != "" && needfindname != "")
		{
			//AfxMessageBox("�ҵ���������,��������" + Winname);
			Phwnd = ::FindWindow(NULL, Winname);
			if (Phwnd != NULL)
			{
				DWORD dwProcId = 0;
				if (GetWindowThreadProcessId(Phwnd, &dwProcId))
				{
					HANDLE hProc = OpenProcess(PROCESS_TERMINATE, FALSE, dwProcId);
					if (hProc != NULL)
					{
						//TerminateProcess(hProc, 0);//ǿ�ƽ���
						::PostMessage(Phwnd, WM_CLOSE, 0, 0);//ͨ��ϵͳ���͹ر���Ϣ��������
						MainThis->GetDlgItemText(IDC_INFOP, InfoText);
						MainThis->SetDlgItemText(IDC_INFOP, _T("����Ӧ�ó������:") + Winname + _T("�����ؼ���->") + needfindname + _T("<-,�Ѿ����͹ر���Ϣ.") + MainThis->Getsystime() + InfoText);
						CloseHandle(hProc);
					}
				}
			}
			return TRUE;
		}

	}


	//Ҫһֱreturn 1.��Ϊ��Ҫ�������еĴ��ڣ�һ������0 ��ֹͣ������ 
	return TRUE;
}


void CMyDlg::OnBnClickedButton4()
{
	Setadminps.DeleteTempMap();
	Setadminps.dispatchMap;
	Setadminps.DestroyWindow();
	Setadminps.Create(IDD_DIALOG3);
	Setadminps.ShowWindow(SW_SHOW);
	Setadminps.SetWindowText(_T("��������"));
}

//��ⴰ����
void CheckWindows(CMyDlg* MydlgThis)
{
	while (1)
	{
		if (CanMonwindows)
		{
			EnumWindows(EnumWindowsProcess, 0);
		}
		Sleep(1000);
	}


}
//������ѡ��
void CMyDlg::OnBnClickedCheck1()
{
	if (BST_CHECKED == IsDlgButtonChecked(IDC_CHECK1))
	{
		CanMonwindows = true;
		if (!IsBadWritePtr((LPVOID)ConAddr, 4)) *(DWORD64*)(ConAddr + 0x4010) = 0x1;
	}
	else
	{
		CanMonwindows = false;
		if (!IsBadWritePtr((LPVOID)ConAddr, 4)) *(DWORD64*)(ConAddr + 0x4010) = 0x0;
	}
}
bool CMyDlg::Checkadminps(CString Pass)
{
	if (Pass == ini.GetValue("PS", "Pass"))
	{
		return true;
	}
	return false;
}
void CMyDlg::SetAdminmode(bool admin)
{
	if (admin)
	{
		Adminmode = true;

	}
	else
	{
		Adminmode = false;
	}
}
void CMyDlg::Showmainwindow()
{

	if (Adminmode)
	{
		this->ShowWindow(SW_SHOWNORMAL); // ��ʾ������
		SetForegroundWindow();
	}
	else
	{
		Inputadminps.DeleteTempMap();
		Inputadminps.dispatchMap;
		Inputadminps.DestroyWindow();
		Inputadminps.MainThis = (DWORD64)this;
		Inputadminps.Create(IDD_CheckAdmin);
		Inputadminps.ShowWindow(SW_SHOW);
		Inputadminps.SetForegroundWindow();
		Inputadminps.SetWindowText(_T("��������"));
	}
}
//�������������ѡ��
void CMyDlg::OnBnClickedCheck2()
{
	if (BST_CHECKED == IsDlgButtonChecked(IDC_CHECK2))//ѡ��
	{
		AutoStart();
		ini.SetValue("System", "Autostart", "1");
	}
	else//δѡ��
	{
		AutoStart(true);
		ini.SetValue("System", "Autostart", "0");
	}
	if (BST_CHECKED == IsDlgButtonChecked(IDC_CHECK3))//ѡ��
	{
		AutoCapture = true;
		ini.SetValue("System", "AutoCapture", "1");
	}
	else//δѡ��
	{
		AutoCapture = false;
		ini.SetValue("System", "AutoCapture", "0");
	}
}
//����
void CaptureScreen()
{
	CString FileName;
	COleDateTime timeNow, dateNow, yearNow;
	yearNow = COleDateTime::GetCurrentTime();     // ��ȡ��ǰ����ʱ��
	timeNow = COleDateTime::GetCurrentTime();     // ��ȡ��ǰ����ʱ��
	dateNow = COleDateTime::GetCurrentTime(); // ͬ����ȡ��ǰ����ʱ�䣬��ôдֻ��Ϊ������
	CString sYear = yearNow.Format(VAR_FOURDIGITYEARS);     // ��ȡ��ǰ���
	CString sTime = timeNow.Format(VAR_TIMEVALUEONLY);     // ��ȡ��ǰʱ��
	CString sDate = dateNow.Format(VAR_DATEVALUEONLY);     // ��ȡ��ǰ����
	TCHAR pFileName[MAX_PATH] = { 0 };
	GetModuleFileName(NULL, pFileName, MAX_PATH);
	CString csFullPath(pFileName);
int iPos = csFullPath.ReverseFind('\\');
		if (iPos < 0)
		{
			return;
		}
		else
		{
			FileName=csFullPath.Left(iPos + 1);
		}
	CreateDir(".\\CapScreen");
	CString YearFile= sYear + TEXT(".jpg");
	YearFile.Replace(TEXT("/"), TEXT(""));
	YearFile.Replace(TEXT(":"), TEXT(""));
	YearFile.Replace(TEXT("-"), TEXT(""));
	YearFile.Replace(TEXT(" "), TEXT(""));
	 FileName = FileName+"\\CapScreen\\"+ YearFile;
	CStringW strWide = CT2CW(FileName);
	WCHAR *buf = strWide.GetBuffer();
	strWide.ReleaseBuffer();
	HDC hDCSrc = ::GetDC(NULL);
	int nBitPerPixel = GetDeviceCaps(hDCSrc, BITSPIXEL);//����λ
	int nWidth = GetDeviceCaps(hDCSrc, HORZRES);//// �õ���ǰ��ʾ�豸��ˮƽ������
	int nHeight = GetDeviceCaps(hDCSrc, VERTRES);
	CImage cImage;
	cImage.Create(nWidth, nHeight, nBitPerPixel);
	BitBlt(cImage.GetDC(), 0, 0, nWidth, nHeight, hDCSrc, 0, 0, SRCCOPY);
	::ReleaseDC(NULL, hDCSrc);
	cImage.ReleaseDC();
	COleStreamFile cImgStream;
	cImgStream.CreateMemoryStream(NULL);
	cImage.Save(cImgStream.GetStream(), Gdiplus::ImageFormatBMP);
	Image image(cImgStream.GetStream());
	cImgStream.Close();
	CLSID encoderClsid;
	GetEncoderClsid(L"image/jpeg", &encoderClsid);//��ȡ����CLSID
	UINT quality = 90;//ѹ����
	EncoderParameters encoderParameters;
	encoderParameters.Count = 1;
	encoderParameters.Parameter[0].Guid = EncoderQuality;
	encoderParameters.Parameter[0].Type = EncoderParameterValueTypeLong;
	encoderParameters.Parameter[0].NumberOfValues = 1;
	encoderParameters.Parameter[0].Value = &quality;
	image.Save(buf, &encoderClsid, &encoderParameters); 
}
int GetEncoderClsid(const WCHAR* format, CLSID* pClsid)
{
	UINT num = 0;                     //   number   of   image   encoders 
	UINT size = 0;                   //   size   of   the   image   encoder   array   in   bytes 
	ImageCodecInfo* pImageCodecInfo = NULL;
	GetImageEncodersSize(&num, &size);
	if (size == 0)
		return   -1;     //   Failure 
	pImageCodecInfo = (ImageCodecInfo*)(malloc(size));
	if (pImageCodecInfo == NULL)
		return -1;     //   Failure 
	GetImageEncoders(num, size, pImageCodecInfo);

	for (UINT j = 0; j < num; ++j)
	{
		if (wcscmp(pImageCodecInfo[j].MimeType, format) == 0)
		{
			*pClsid = pImageCodecInfo[j].Clsid;
			free(pImageCodecInfo);
			return j;     //   Success 
		}
	}
	free(pImageCodecInfo);
	return   -1;     //   Failure 
}
//�����ļ���
void CreateDir(CString dirName) {

	if (!PathIsDirectory(dirName))
	{
		::CreateDirectory(dirName, NULL);//����Ŀ¼,���еĻ���Ӱ��
	}
}

void DebugPrivilege()
{
	HANDLE hToken = NULL;
	BOOL bRet = OpenProcessToken(GetCurrentProcess(), TOKEN_ALL_ACCESS, &hToken);

	if (bRet == TRUE)
	{
		TOKEN_PRIVILEGES tp;
		tp.PrivilegeCount = 1;

		LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &tp.Privileges[0].Luid);

		tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

		AdjustTokenPrivileges(hToken, FALSE, &tp, sizeof(tp), NULL, NULL);

		CloseHandle(hToken);
	}
}

BOOL Inject(DWORD dwPid, const char *szDllname)
{
	DebugPrivilege();

	BOOL bRet = FALSE;
	char *pFunName = "LoadLibraryA";
	//��Ŀ�����
	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwPid);
	if (hProcess == NULL)
		return FALSE;

	//������ע��DLL�ļ�����·������
#ifndef _WIN64
	int nDllLen = strlen(szDllname) + sizeof(char);
#else
	SIZE_T nDllLen = strlen(szDllname) + sizeof(char);
#endif
	//��Ŀ���������һ�鳤��ΪnDllLen��С���ڴ�ռ�
	PVOID pDllAddr = VirtualAllocEx(hProcess, NULL, nDllLen, MEM_COMMIT, PAGE_READWRITE);
	if (pDllAddr == NULL)
		return FALSE;

	//����ע��DLL�ļ�������·��д��Ŀ�����������Ŀռ���
#ifndef _WIN64
	DWORD dwWriteNum = 0;
#else
	SIZE_T dwWriteNum = 0;
#endif
	WriteProcessMemory(hProcess, pDllAddr, szDllname, nDllLen, &dwWriteNum);

	//��ȡLoadLibraryA�����ĵ�ַ
	FARPROC pFunAddr = GetProcAddress(GetModuleHandleA("kernel32.dll"), pFunName);

	HANDLE hThread = CreateRemoteThread(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)pFunAddr, pDllAddr, 0, NULL);
	if (hThread)
	{
		WaitForSingleObject(hThread, INFINITE);//�ȴ��߳̽���
		CloseHandle(hThread);
		bRet = TRUE;
	}

	VirtualFreeEx(hProcess, pDllAddr, sizeof(pDllAddr), MEM_RELEASE);
	CloseHandle(hProcess);
	return bRet;
}
BOOL UnInject(DWORD dwPid, TCHAR *szDllname)
{
	DebugPrivilege();

	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, dwPid);

	BOOL isFind = FALSE;
	MODULEENTRY32 me32;
	me32.dwSize = sizeof(me32);
	BOOL bRet = Module32First(hSnap, &me32);
	while (bRet)
	{
		if (lstrcmp(me32.szExePath, szDllname) == 0)
		{
			isFind = TRUE;
			break;
		}
		bRet = Module32Next(hSnap, &me32);
	}
	CloseHandle(hSnap);
	if (isFind == FALSE)
		return FALSE;

	char *pFunName = "FreeLibrary";
	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwPid);
	if (hProcess == NULL)
		return FALSE;

	FARPROC pFunAddr = GetProcAddress(GetModuleHandleA("kernel32.dll"), pFunName);

	HANDLE hThread = CreateRemoteThread(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)pFunAddr, me32.hModule, 0, NULL);
	WaitForSingleObject(hThread, INFINITE);

	CloseHandle(hThread);
	CloseHandle(hProcess);
	return TRUE;
}
DWORD FindProcessID(LPCTSTR szProcessName)
{
	DWORD dwPID = 0xFFFFFFFF;
	HANDLE hSnapShot = INVALID_HANDLE_VALUE;
	PROCESSENTRY32 pe;

	//��ȡϵͳ����
	pe.dwSize = sizeof(PROCESSENTRY32);
	hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPALL, NULL);

	//���ҽ���
	Process32First(hSnapShot, &pe);

	do
	{
		if (!strcmp(szProcessName, (LPCSTR)pe.szExeFile))
		{
			dwPID = pe.th32ProcessID;
			break;
		}
	} while (Process32Next(hSnapShot, &pe));

	CloseHandle(hSnapShot);
	return dwPID;
}
// ��ȫ��ȡ����ʵϵͳ��Ϣ
VOID SafeGetNativeSystemInfo(__out LPSYSTEM_INFO lpSystemInfo)
{
	if (NULL == lpSystemInfo)    return;
	typedef VOID(WINAPI *LPFN_GetNativeSystemInfo)(LPSYSTEM_INFO lpSystemInfo);
	LPFN_GetNativeSystemInfo fnGetNativeSystemInfo = (LPFN_GetNativeSystemInfo)GetProcAddress(GetModuleHandle(_T("kernel32")), "GetNativeSystemInfo");;
	if (NULL != fnGetNativeSystemInfo)
	{
		fnGetNativeSystemInfo(lpSystemInfo);
	}
	else
	{
		GetSystemInfo(lpSystemInfo);
	}
}
// ��ȡ����ϵͳλ��
int GetSystemBits()
{
	SYSTEM_INFO si;
	SafeGetNativeSystemInfo(&si);
	if (si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64 ||
		si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_IA64)
	{
		return 64;
	}
	return 32;
}
