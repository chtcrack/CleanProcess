// ��������Dlg.h : ͷ�ļ�
//

#pragma once
#ifndef ShowAllProcess_H_H
#define  ShowAllProcess_H_H
#include "ShowAllProcess.h"
#endif

#include "ClearTmpFileDlg.h"
#include "PingDlg.h"
#include "Ini.h"
#include "afxwin.h"
#include "Addkeymonitor.h"
#ifndef CheckAdmin_H_H
#define  CheckAdmin_H_H
#include "CheckAdmin.h"
#endif

typedef struct NeedMProcess
{
CString PClassName;//��������
CString PWindowsName;//���ڱ���
CString NeedEndProcessName;//��Ҫ�ɵ��Ľ�����
//CStringArray NeedEndprocessarry;
//byte eCount;//���м���Ҫ�ɵ��Ľ���
}WantMprocess,*PWantMprocess;

typedef struct WindowsNameStruct
{
	char WindowsName[256];//��������
	
}WantWindowsNameStruct,*PWindowsNameStruct;


// CMyDlg �Ի���
class CMyDlg : public CDialog
{
// ����
public:
	CMyDlg(CWnd* pParent = NULL);	// ��׼���캯��
	bool synctimeing;
// �Ի�������
	enum { IDD = IDD_MY_DIALOG };
	
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg LRESULT OnNotifyIcon(WPARAM wParam,LPARAM IParam);

	
	DECLARE_MESSAGE_MAP()
	
	
public:
	bool CheckAndKillProcess(LPCTSTR Processname);//�жϽ����Ƿ����,���ھ͸ɵ�������,�����ڷ��ؼ�
	void InitNotify();//��ʼ������ͼ��
	WantMprocess EProcessStruct[50];
	WantWindowsNameStruct Ewindowsname[50];
    NOTIFYICONDATA NotifyIcon;
	void ShowTrayMessage(CString Msg);//��ʾ������Ϣ
	bool StartSynctime(CString TimeserverAddr);//��ʼͬ��ʱ�亯��
	afx_msg void OnSize(UINT nType, int cx, int cy);

	afx_msg void OnDestroy();
	afx_msg void OnNcPaint();
	afx_msg void OnClose();
	
	afx_msg void NPause();//�����ͣ����
	afx_msg void ResumeMon();//����ָ�����
	afx_msg void DestorySelf();
	afx_msg void Nsynctime();//���ͬ��ʱ��
	CString Getsystime();//��ȡϵͳʱ��
	ShowAllProcess Showprocessdlg;//����һ�����̹�����
	CClearTmpFileDlg Clearfiledlg;//����һ������������
	Addkeymonitor Setadminps;
	CheckAdmin Inputadminps;
	PingDlg Pingydlg;//����һ��ping������
	BOOL IsLaterNT();//�ж��Ƿ�ΪNT6�Ժ�İ汾
	BOOL EnablePrivilege();//������ǰ����Ȩ��
	afx_msg void OnBnClickedButton2();
	afx_msg void CallProcessManger();
	afx_msg void OnBnClickedButton1();
	afx_msg void CallTmpFileDlg();
	afx_msg void ShowPingTool();
	afx_msg void OnBnClickedButton3();
	CListBox Keywordlist;
	void Addkeyword(CString Keyword );
	void DelAccount(CString Keyword);
	void ReBuildIni();
	void LoadiniToListCtrl();
	bool Checkadminps(CString Pass);
	void SetAdminmode(bool admin);
	void Showmainwindow();
	afx_msg void OnLbnDblclkList1();
	afx_msg void OnBnClickedButton4();
	afx_msg void OnBnClickedCheck1();
	afx_msg void OnBnClickedCheck2();
};
	void CheckProcess(CMyDlg* MydlgThis);//������
	void INITPorcessStruct();//��ʼ�����̽ṹ��
	bool AutoStart(bool DeleteKey=false);//д��ϵͳע���������
	void CheckWindows(CMyDlg* MydlgThis);//��ⴰ����
	PROCESS_INFORMATION RunApp(CString runpath,CString exename,bool Showwindows=false);//����ָ������,����1·��,�ļ���������,�Ƿ���ʾ����
	void SyncTime(CMyDlg* MydlgThis);//ͬ��ʱ�亯�����ڶ��߳�
	bool synctimereg();//ͬ��ʱ������ע���
	BOOL CALLBACK EnumWindowsProcess(HWND hwnd, LPARAM lParam,CMyDlg* MydlgThis);
	void ReadAccountini();
	void InitStruct(bool InitPskinAccount);