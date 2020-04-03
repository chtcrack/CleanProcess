// CheckAdmin.cpp : 实现文件
//

#include "stdafx.h"
#include "进程清理.h"
#include "CheckAdmin.h"
#include "afxdialogex.h"
#include "进程清理Dlg.h"

CMyDlg* Mydlgptr2;

// CheckAdmin 对话框

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


// CheckAdmin 消息处理程序


BOOL CheckAdmin::OnInitDialog()
{
	CDialog::OnInitDialog();
	Mydlgptr2=(CMyDlg*)MainThis;
	// TODO:  在此添加额外的初始化
	GetDlgItem(IDC_INPUTPS)->SetFocus();
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}


void CheckAdmin::OnBnClickedOk()
{
	CString Password;
	GetDlgItemText(IDC_INPUTPS,Password);
	if(Mydlgptr2->Checkadminps(Password))
	{
		Mydlgptr2->SetAdminmode(true);
		Mydlgptr2->ShowWindow(SW_SHOWNORMAL); // 显示主窗口
		Mydlgptr2->SetForegroundWindow();
	}
	else
	{
		Mydlgptr2->SetAdminmode(false);
	}
	// TODO: 在此添加控件通知处理程序代码
	CDialog::OnOK();
}


void CheckAdmin::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	Mydlgptr2=0;
	delete Mydlgptr2;
	CDialog::OnClose();
}
