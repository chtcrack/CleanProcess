// Monitor.cpp : ���� DLL �ĳ�ʼ�����̡�
//

#include "stdafx.h"
#include "Monitor.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
HMODULE g_hDll = NULL;
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
		//todo ��Ҫ��������

		//����յ�������������Ϣ���˳�ѭ��
		//FreeLibraryAndExitThread(g_hDll, 0);
	}
	FreeLibrary(g_hDll);//ж������

	return 1;
}