#pragma once


// Addkeymonitor �Ի���

class Addkeymonitor : public CDialog
{
	DECLARE_DYNAMIC(Addkeymonitor)

public:
	Addkeymonitor(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~Addkeymonitor();
	
// �Ի�������
	enum { IDD = IDD_DIALOG3 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	
	virtual BOOL OnInitDialog();
	afx_msg void OnClose();
};
