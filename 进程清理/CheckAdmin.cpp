// CheckAdmin.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "��������.h"
#include "CheckAdmin.h"
#include "afxdialogex.h"
#include "��������Dlg.h"

CMyDlg* Mydlgptr2;

// CheckAdmin �Ի���

IMPLEMENT_DYNAMIC(CheckAdmin, CDialog)

CheckAdmin::CheckAdmin(CWnd* pParent /*=NULL*/)
	: CDialog(CheckAdmin::IDD, pParent)
{

}

CheckAdmin::~CheckAdmin()
{
}

void CheckAdmin::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CheckAdmin, CDialog)
	ON_BN_CLICKED(IDOK, &CheckAdmin::OnBnClickedOk)
	ON_WM_CLOSE()
END_MESSAGE_MAP()


// CheckAdmin ��Ϣ�������


BOOL CheckAdmin::OnInitDialog()
{
	CDialog::OnInitDialog();
	Mydlgptr2=(CMyDlg*)MainThis;
	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	GetDlgItem(IDC_INPUTPS)->SetFocus();
	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}


void CheckAdmin::OnBnClickedOk()
{
	CString Password;
	GetDlgItemText(IDC_INPUTPS,Password);
	if(Mydlgptr2->Checkadminps(Password))
	{
		Mydlgptr2->SetAdminmode(true);
		Mydlgptr2->ShowWindow(SW_SHOWNORMAL); // ��ʾ������
		Mydlgptr2->SetForegroundWindow();
	}
	else
	{
		Mydlgptr2->SetAdminmode(false);
	}
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CDialog::OnOK();
}


void CheckAdmin::OnClose()
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	Mydlgptr2=0;
	delete Mydlgptr2;
	CDialog::OnClose();
}
