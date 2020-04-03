#pragma once


// Addkeymonitor 对话框

class Addkeymonitor : public CDialog
{
	DECLARE_DYNAMIC(Addkeymonitor)

public:
	Addkeymonitor(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~Addkeymonitor();
	
// 对话框数据
	enum { IDD = IDD_DIALOG3 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	
	virtual BOOL OnInitDialog();
	afx_msg void OnClose();
};
