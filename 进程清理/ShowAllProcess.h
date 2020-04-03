#pragma once
#include "afxcmn.h"
int WINAPI GetProcessCommandLine(DWORD dwPID, LPTSTR lpszCommandLine, DWORD dwByteOfSize);

// ShowAllProcess 对话框

class ShowAllProcess : public CDialog
{
	DECLARE_DYNAMIC(ShowAllProcess)

public:
	ShowAllProcess(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~ShowAllProcess();

// 对话框数据
	enum { IDD = IDD_ProcessDIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
	void GetAllProcess();
public:
	typedef DWORD (WINAPI *NtSuspendProcess)(HANDLE ProcessHandle); 
	typedef DWORD (WINAPI *NtResumeProcess)(HANDLE hProcess);
	NtSuspendProcess m_NtSuspendProcess;
	NtResumeProcess   m_NtResumeProcess;

	DWORD MainWinThis;
	CString PidToPath(DWORD id);//之能得到x86下的路径
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
	int GetProcessMem(DWORD th32ProcessID);//根据进程pid获得进程占用内存
	afx_msg void HideSelectedWindow();
	afx_msg void ShowSelectedWindow();
	afx_msg void SuspendProcess();
	afx_msg void ResumeProcess();
};
HWND GetAppWindow(DWORD Pid);//根据Pid获取窗口句柄
BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam);//获取窗口句柄的回调函数