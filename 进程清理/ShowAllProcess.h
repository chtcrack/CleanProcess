#pragma once
#include "afxcmn.h"
int WINAPI GetProcessCommandLine(DWORD dwPID, LPTSTR lpszCommandLine, DWORD dwByteOfSize);

// ShowAllProcess �Ի���

class ShowAllProcess : public CDialog
{
	DECLARE_DYNAMIC(ShowAllProcess)

public:
	ShowAllProcess(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~ShowAllProcess();

// �Ի�������
	enum { IDD = IDD_ProcessDIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
	void GetAllProcess();
public:
	typedef DWORD (WINAPI *NtSuspendProcess)(HANDLE ProcessHandle); 
	typedef DWORD (WINAPI *NtResumeProcess)(HANDLE hProcess);
	NtSuspendProcess m_NtSuspendProcess;
	NtResumeProcess   m_NtResumeProcess;

	DWORD MainWinThis;
	CString PidToPath(DWORD id);//֮�ܵõ�x86�µ�·��
	BOOL EnablePrivileges(HANDLE hToken,LPCSTR szPrivName);
	afx_msg void OnBnClickedOk();

	virtual BOOL OnInitDialog();
	CListCtrl m_showlist;
	afx_msg void On32778();
	afx_msg void OnNMRClickListShow(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedButton1();
	afx_msg void OpenExeDir();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnClose();
	int GetProcessMem(DWORD th32ProcessID);//���ݽ���pid��ý���ռ���ڴ�
	afx_msg void HideSelectedWindow();
	afx_msg void ShowSelectedWindow();
	afx_msg void SuspendProcess();
	afx_msg void ResumeProcess();
};
HWND GetAppWindow(DWORD Pid);//����Pid��ȡ���ھ��
BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam);//��ȡ���ھ���Ļص�����