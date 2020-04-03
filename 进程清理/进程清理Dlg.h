// 进程清理Dlg.h : 头文件
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
CString PClassName;//窗口类名
CString PWindowsName;//窗口标题
CString NeedEndProcessName;//想要干掉的进程名
//CStringArray NeedEndprocessarry;
//byte eCount;//共有几个要干掉的进程
}WantMprocess,*PWantMprocess;

typedef struct WindowsNameStruct
{
	char WindowsName[256];//窗口类名
	
}WantWindowsNameStruct,*PWindowsNameStruct;


// CMyDlg 对话框
class CMyDlg : public CDialog
{
// 构造
public:
	CMyDlg(CWnd* pParent = NULL);	// 标准构造函数
	bool synctimeing;
// 对话框数据
	enum { IDD = IDD_MY_DIALOG };
	
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg LRESULT OnNotifyIcon(WPARAM wParam,LPARAM IParam);

	
	DECLARE_MESSAGE_MAP()
	
	
public:
	bool CheckAndKillProcess(LPCTSTR Processname);//判断进程是否存在,存在就干掉返回真,不存在返回假
	void InitNotify();//初始化托盘图标
	WantMprocess EProcessStruct[50];
	WantWindowsNameStruct Ewindowsname[50];
    NOTIFYICONDATA NotifyIcon;
	void ShowTrayMessage(CString Msg);//显示托盘信息
	bool StartSynctime(CString TimeserverAddr);//开始同步时间函数
	afx_msg void OnSize(UINT nType, int cx, int cy);

	afx_msg void OnDestroy();
	afx_msg void OnNcPaint();
	afx_msg void OnClose();
	
	afx_msg void NPause();//点击暂停清理
	afx_msg void ResumeMon();//点击恢复清理
	afx_msg void DestorySelf();
	afx_msg void Nsynctime();//点击同步时间
	CString Getsystime();//获取系统时间
	ShowAllProcess Showprocessdlg;//声明一个进程管理类
	CClearTmpFileDlg Clearfiledlg;//声明一个清理垃圾类
	Addkeymonitor Setadminps;
	CheckAdmin Inputadminps;
	PingDlg Pingydlg;//声明一个ping工具类
	BOOL IsLaterNT();//判断是否为NT6以后的版本
	BOOL EnablePrivilege();//提升当前进程权限
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
	void CheckProcess(CMyDlg* MydlgThis);//检测进程
	void INITPorcessStruct();//初始化进程结构体
	bool AutoStart(bool DeleteKey=false);//写入系统注册表启动项
	void CheckWindows(CMyDlg* MydlgThis);//检测窗口名
	PROCESS_INFORMATION RunApp(CString runpath,CString exename,bool Showwindows=false);//运行指定程序,参数1路径,文件名及参数,是否显示窗口
	void SyncTime(CMyDlg* MydlgThis);//同步时间函数用于多线程
	bool synctimereg();//同步时间所需注册表
	BOOL CALLBACK EnumWindowsProcess(HWND hwnd, LPARAM lParam,CMyDlg* MydlgThis);
	void ReadAccountini();
	void InitStruct(bool InitPskinAccount);