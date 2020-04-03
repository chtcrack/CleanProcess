// PingDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "进程清理.h"
#include "PingDlg.h"
#include "afxdialogex.h"
#include "ping.h"

// PingDlg 对话框
bool CanPing=false;
IMPLEMENT_DYNAMIC(PingDlg, CDialog)

	PingDlg::PingDlg(CWnd* pParent /*=NULL*/)
	: CDialog(PingDlg::IDD, pParent)
{

}

PingDlg::~PingDlg()
{
}

void PingDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(PingDlg, CDialog)
	ON_BN_CLICKED(IDOK, &PingDlg::OnBnClickedOk)

END_MESSAGE_MAP()


// PingDlg 消息处理程序
void PingDlg::OnSysCommand(UINT nID, LPARAM lParam)
{

	CDialog::OnSysCommand(nID, lParam);

}

void PingDlg::OnBnClickedOk()
{
	if(CanPing)
	{
		CanPing=false;
		SetDlgItemText(IDOK,"开始");
	}
	else
	{
		CanPing=true;
		SetDlgItemText(IDOK,"停止");
		CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)PingAdress,(LPVOID)this,NULL,NULL);//启动一个线程
	}


}
void PingAdress(PingDlg* PingDlgthis)
{
	CPing p;
	CPingReply pr;
	WSADATA wsa;
	WSAStartup(MAKEWORD(1, 1), &wsa);
	BOOL bSuccess = FALSE;
	int pingcount=0;
	CString PingHostName,PingCountStr;
	PingDlgthis->GetDlgItemText(IDC_EDIT3,PingCountStr);//获取ping的次数
	pingcount=atoi(PingCountStr);
	PingDlgthis->GetDlgItemText(IDC_EDIT2,PingHostName);//获取ip或域名
	CString InfoText=_T("");
	
	CString PingReplyInfo=_T("");
	hostent* phostent = NULL;
	UCHAR nTTL = 10; DWORD dwTimeout = 5000; WORD wDataSize = 32; UCHAR nTOS = 0; BOOL bDontFragment = FALSE;
	while(CanPing)
	{
		PingDlgthis->GetDlgItemText(IDC_EDIT1,InfoText);
		bSuccess = p.PingUsingICMP(PingHostName, pr,nTTL,dwTimeout,wDataSize,nTOS,bDontFragment);
		
		if(bSuccess)
		{
			
				PingReplyInfo.Format("IP地址:%d.%d.%d.%d,回复时间:%d\r\n",pr.Address.S_un.S_un_b.s_b1, pr.Address.S_un.S_un_b.s_b2, pr.Address.S_un.S_un_b.s_b3, 
					pr.Address.S_un.S_un_b.s_b4,pr.RTT);
		
			PingDlgthis->SetDlgItemText(IDC_EDIT1,PingReplyInfo+InfoText +"\r\n");
			pingcount--;	
		}
		else
		{
			
			PingDlgthis->SetDlgItemText(IDC_EDIT1,"ping " + PingHostName+"超时!\r\n"+InfoText);
		}
		if(pingcount<=0)
		{
			CanPing=false;
			break;
		}
		Sleep(50);
	}
	CanPing=false;
	PingDlgthis->SetDlgItemText(IDOK,"开始");
	phostent=nullptr;
	delete phostent;
	WSACleanup();
}



BOOL PingDlg::OnInitDialog()//窗体初始化
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
	SetDlgItemText(IDC_EDIT2,"www.ip138.com");
	SetDlgItemText(IDC_EDIT3,"4");
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}
