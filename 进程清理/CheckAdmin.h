#pragma once


// CheckAdmin 对话框

class CheckAdmin : public CDialog
{
	DECLARE_DYNAMIC(CheckAdmin)

public:
	CheckAdmin(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CheckAdmin();
	DWORD MainThis;
// 对话框数据
	enum { IDD = IDD_CheckAdmin };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
	afx_msg void OnClose();
};
