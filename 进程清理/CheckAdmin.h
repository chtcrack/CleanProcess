#pragma once


// CheckAdmin �Ի���

class CheckAdmin : public CDialog
{
	DECLARE_DYNAMIC(CheckAdmin)

public:
	CheckAdmin(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CheckAdmin();
	DWORD MainThis;
// �Ի�������
	enum { IDD = IDD_CheckAdmin };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
	afx_msg void OnClose();
};
