#pragma once
#include "afxwin.h"
#include "afxcmn.h"

// PingDlg �Ի���

class PingDlg : public CDialog
{
	DECLARE_DYNAMIC(PingDlg)

public:
	PingDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~PingDlg();

// �Ի�������
	enum { IDD = IDD_DIALOG2 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();



	virtual BOOL OnInitDialog();
};
void PingAdress(PingDlg* PingDlgthis);