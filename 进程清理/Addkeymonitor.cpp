// Addkeymonitor.cpp : 实现文件
//

#include "stdafx.h"
#include "进程清理.h"
#include "Addkeymonitor.h"
#include "afxdialogex.h"
#include "进程清理Dlg.h"
CIni Setini("Setting.ini");
// Addkeymonitor 对话框

IMPLEMENT_DYNAMIC(Addkeymonitor, CDialog)

Addkeymonitor::Addkeymonitor(CWnd* pParent /*=NULL*/)
	: CDialog(Addkeymonitor::IDD, pParent)
{

}

Addkeymonitor::~Addkeymonitor()
{
}

void Addkeymonitor::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(Addkeymonitor, CDialog)
	ON_BN_CLICKED(IDOK, &Addkeymonitor::OnBnClickedOk)

	ON_WM_CLOSE()
END_MESSAGE_MAP()


// Addkeymonitor 消息处理程序


BOOL Addkeymonitor::OnInitDialog()
{
	CDialog::OnInitDialog();
	GetDlgItem(IDC_SETPS)->SetFocus();
	// TODO:  在此添加额外的初始化
	
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}


void Addkeymonitor::OnBnClickedOk()
{
	// 将密码写入ini
	CString Keyword="";
	GetDlgItemText(IDC_SETPS,Keyword);
Setini.SetValue("PS","Pass",Keyword);
CDialog::OnOK();
}






void Addkeymonitor::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	
	CDialog::OnClose();
}
