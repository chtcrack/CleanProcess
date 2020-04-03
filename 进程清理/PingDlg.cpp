// PingDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "��������.h"
#include "PingDlg.h"
#include "afxdialogex.h"
#include "ping.h"

// PingDlg �Ի���
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


// PingDlg ��Ϣ�������
void PingDlg::OnSysCommand(UINT nID, LPARAM lParam)
{

	CDialog::OnSysCommand(nID, lParam);

}

void PingDlg::OnBnClickedOk()
{
	if(CanPing)
	{
		CanPing=false;
		SetDlgItemText(IDOK,"��ʼ");
	}
	else
	{
		CanPing=true;
		SetDlgItemText(IDOK,"ֹͣ");
		CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)PingAdress,(LPVOID)this,NULL,NULL);//����һ���߳�
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
	PingDlgthis->GetDlgItemText(IDC_EDIT3,PingCountStr);//��ȡping�Ĵ���
	pingcount=atoi(PingCountStr);
	PingDlgthis->GetDlgItemText(IDC_EDIT2,PingHostName);//��ȡip������
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
			
				PingReplyInfo.Format("IP��ַ:%d.%d.%d.%d,�ظ�ʱ��:%d\r\n",pr.Address.S_un.S_un_b.s_b1, pr.Address.S_un.S_un_b.s_b2, pr.Address.S_un.S_un_b.s_b3, 
					pr.Address.S_un.S_un_b.s_b4,pr.RTT);
		
			PingDlgthis->SetDlgItemText(IDC_EDIT1,PingReplyInfo+InfoText +"\r\n");
			pingcount--;	
		}
		else
		{
			
			PingDlgthis->SetDlgItemText(IDC_EDIT1,"ping " + PingHostName+"��ʱ!\r\n"+InfoText);
		}
		if(pingcount<=0)
		{
			CanPing=false;
			break;
		}
		Sleep(50);
	}
	CanPing=false;
	PingDlgthis->SetDlgItemText(IDOK,"��ʼ");
	phostent=nullptr;
	delete phostent;
	WSACleanup();
}



BOOL PingDlg::OnInitDialog()//�����ʼ��
{
	CDialog::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	SetDlgItemText(IDC_EDIT2,"www.ip138.com");
	SetDlgItemText(IDC_EDIT3,"4");
	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}
