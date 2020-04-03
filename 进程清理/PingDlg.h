#pragma once
#include "afxwin.h"
#include "afxcmn.h"

// PingDlg 对话框

class PingDlg : public CDialog
{
	DECLARE_DYNAMIC(PingDlg)

public:
	PingDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~PingDlg();

// 对话框数据
	enum { IDD = IDD_DIALOG2 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();



	virtual BOOL OnInitDialog();
};
void PingAdress(PingDlg* PingDlgthis);