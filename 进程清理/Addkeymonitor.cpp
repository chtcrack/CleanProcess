// Addkeymonitor.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "��������.h"
#include "Addkeymonitor.h"
#include "afxdialogex.h"
#include "��������Dlg.h"
CIni Setini("Setting.ini");
// Addkeymonitor �Ի���

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


// Addkeymonitor ��Ϣ�������


BOOL Addkeymonitor::OnInitDialog()
{
	CDialog::OnInitDialog();
	GetDlgItem(IDC_SETPS)->SetFocus();
	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	
	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}


void Addkeymonitor::OnBnClickedOk()
{
	// ������д��ini
	CString Keyword="";
	GetDlgItemText(IDC_SETPS,Keyword);
Setini.SetValue("PS","Pass",Keyword);
CDialog::OnOK();
}






void Addkeymonitor::OnClose()
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	
	CDialog::OnClose();
}
