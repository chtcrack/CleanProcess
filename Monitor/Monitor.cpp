// Monitor.cpp : ���� DLL �ĳ�ʼ�����̡�
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
DWORD64 ConAddr ;//���������������ڴ�ӳ���ļ���ַ
//
//TODO:  ����� DLL ����� MFC DLL �Ƕ�̬���ӵģ�
//		��Ӵ� DLL �������κε���
//		MFC �ĺ������뽫 AFX_MANAGE_STATE ����ӵ�
//		�ú�������ǰ�档
//
//		����: 
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// �˴�Ϊ��ͨ������
//		}
//
//		�˺������κ� MFC ����
//		������ÿ��������ʮ����Ҫ��  ����ζ��
//		��������Ϊ�����еĵ�һ�����
//		���֣������������ж������������
//		������Ϊ���ǵĹ��캯���������� MFC
//		DLL ���á�
//
//		�й�������ϸ��Ϣ��
//		����� MFC ����˵�� 33 �� 58��
//

// CMonitorApp

BEGIN_MESSAGE_MAP(CMonitorApp, CWinApp)
END_MESSAGE_MAP()


// CMonitorApp ����

CMonitorApp::CMonitorApp()
{
	// TODO:  �ڴ˴���ӹ�����룬
	// ��������Ҫ�ĳ�ʼ�������� InitInstance ��
}


// Ψһ��һ�� CMonitorApp ����

CMonitorApp theApp;


// CMonitorApp ��ʼ��

BOOL CMonitorApp::InitInstance()
{
	MyDebug("Dllע��ɹ�");
	for (int i=0;i<Estructcount;i++)
	{
		memset((PVOID)Ewindowsname[i].WindowsName, 0, 256);//��ʼ���ṹ���б�
	}
	g_hDll = (HMODULE)theApp.m_hInstance;
	


	HANDLE hThread = ::CreateThread(NULL, 0, WorkThread, NULL, 0, NULL);
	CloseHandle(hThread);
	CWinApp::InitInstance();

	return TRUE;
}
DWORD WINAPI WorkThread(PVOID param)
{
	
	DWORD64 canmon = 1;//�Ƿ���Ҫ��ر��
	DWORD64 stopFlag = 0;//dll�����˳����
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
			//MyDebug("dll�������س����нṹ�����,��ȡ..");
			
			ConAddr = (DWORD64)(::MapViewOfFile(AdminH, FILE_MAP_WRITE, 0, 0, 0));
			if (!IsBadReadPtr((LPVOID)ConAddr, 4))
			{
				addr64 = ConAddr;
				//MyDebug("��ȡ�ڴ�ӳ���ַ4-%08X", ConAddr);
				for (int i = 0; i < Estructcount; i++)
				{
					memcpy(&Ewindowsname[i], (PVOID)addr64, 256);//�����ڴ浽�ṹ��
					addr64 += 256;
				}
				
			}
			
		}
	
		
		Sleep(1000);
		//todo ��Ҫ��������
		CString tmpstr;
		//MyDebug("Ҫ��صı���%s",Ewindowsname[0].WindowsName);
		for (int i = 0; i < Estructcount; i++)
		{
			tmpstr.Format("%s", Ewindowsname[i].WindowsName);
		
		}
		if (canmon)
		{
			EnumWindows(EnumWindowsProcess, 0);
		}
		//����յ�������������Ϣ���˳�ѭ��
		//FreeLibraryAndExitThread(g_hDll, 0);
	}
	CloseHandle(AdminH);
	MyDebug("dll����ж��");
	FreeLibrary(g_hDll);//ж������

	return 1;
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
						//Todo д����־�ļ�
						

						//MainThis->GetDlgItemText(IDC_INFOP, InfoText);
						//MainThis->SetDlgItemText(IDC_INFOP, _T("����Ӧ�ó������:") + Winname + _T("�����ؼ���->") + needfindname + _T("<-,�Ѿ����͹ر���Ϣ.") + MainThis->Getsystime() + InfoText);
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