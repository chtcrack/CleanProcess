// ClearTmpFileDlg.h : 头文件
//

#pragma once

#include "afxwin.h"
#include "afxcmn.h"
#include "HSListBox.h"


// CClearTmpFileDlg 对话框
class CClearTmpFileDlg : public CDialog
{
// 构造
public:
	CClearTmpFileDlg(CWnd* pParent = NULL);	// 标准构造函数
	//caizhiming
public:
	CList<CString, CString> m_fileExtList;//记录需要查找的临时文件扩展名
	bool m_bThreadExit;//线程是否退出
	bool m_bFinding;//是否查找进行中
	HANDLE m_hThread;//查找文件的线程句柄
	HANDLE m_hThread2;//bmp旋转线程句柄
	CString m_szCurDisk;//查找的磁盘
	HANDLE m_hEvent;//事件对象，在对话框关闭时将提前结束查找
	bool m_bContinue;//判断暂停或继续按钮操作
	bool m_oneclickforclean;//是否一键清理


	DWORD GetDiskSize(char* strPath);//获取磁盘容量（已使用的）
	DWORD m_dwDiskVol;//磁盘总容量大小，单位为KB
	DWORD m_dwScanedVol;//已扫描的文件的容量
	DWORD m_dwScanedTmpFileVol;//扫描到的临时文件的容量大小
	DWORD m_dwScanedTmpFileNum;//扫描到的临时文件的容量大小

	//界面美观
	CBrush m_bkBrush;  //背景画刷
	COLORREF m_textcolor,m_bkColor;//文本颜色和背景颜色  
	CFont * m_font;//字体
	//判断是否旋转
	bool m_bIsRotate;
	void ResearchFile(char* strPath);//遍历磁盘目录，查找临时文件
	bool GetTmpExtName();//获取扩展名列表框中选中的扩展名
	bool IsTmpFile(char * pszName);//判断是否为临时文件
	void Restore();//停止扫描或扫描完毕后恢复数据和状态
	void ShowResultText();//显示扫描结果文本
	void ShowImg(int i);//显示bmp图像
	void Rotating();//旋转bmp图像
// 对话框数据
	enum { IDD = IDD_CLEARTMPFILE_DIALOG };
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持
// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedStart();
	
	
	
	CCheckListBox m_checkList;
	afx_msg void OnBnClickedSelall();
	CComboBox m_combox;
	afx_msg void OnBnClickedSelnull();
	
	afx_msg void OnBnClickedStop();
	CHSListBox m_listBoxResults;
	
	afx_msg void OnBnClickedSuspend();
	afx_msg void OnBnClickedDel();
	afx_msg void OnBnClickedDelall();
	CProgressCtrl m_progressCtl;
	afx_msg void OnClose();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);//设置控件颜色
	CListBox OneClickDir;
	afx_msg void OnBnClickedButton1();
	CString GetTempDir();//获取临时目录
	CString GetIETempDir();
	CString GetWindowTempDir();
	void GetFolderSize(char* Targ , DWORD &foldersize);
	
};

