// ClearTmpFileDlg.h : ͷ�ļ�
//

#pragma once

#include "afxwin.h"
#include "afxcmn.h"
#include "HSListBox.h"


// CClearTmpFileDlg �Ի���
class CClearTmpFileDlg : public CDialog
{
// ����
public:
	CClearTmpFileDlg(CWnd* pParent = NULL);	// ��׼���캯��
	//caizhiming
public:
	CList<CString, CString> m_fileExtList;//��¼��Ҫ���ҵ���ʱ�ļ���չ��
	bool m_bThreadExit;//�߳��Ƿ��˳�
	bool m_bFinding;//�Ƿ���ҽ�����
	HANDLE m_hThread;//�����ļ����߳̾��
	HANDLE m_hThread2;//bmp��ת�߳̾��
	CString m_szCurDisk;//���ҵĴ���
	HANDLE m_hEvent;//�¼������ڶԻ���ر�ʱ����ǰ��������
	bool m_bContinue;//�ж���ͣ�������ť����
	bool m_oneclickforclean;//�Ƿ�һ������


	DWORD GetDiskSize(char* strPath);//��ȡ������������ʹ�õģ�
	DWORD m_dwDiskVol;//������������С����λΪKB
	DWORD m_dwScanedVol;//��ɨ����ļ�������
	DWORD m_dwScanedTmpFileVol;//ɨ�赽����ʱ�ļ���������С
	DWORD m_dwScanedTmpFileNum;//ɨ�赽����ʱ�ļ���������С

	//��������
	CBrush m_bkBrush;  //������ˢ
	COLORREF m_textcolor,m_bkColor;//�ı���ɫ�ͱ�����ɫ  
	CFont * m_font;//����
	//�ж��Ƿ���ת
	bool m_bIsRotate;
	void ResearchFile(char* strPath);//��������Ŀ¼��������ʱ�ļ�
	bool GetTmpExtName();//��ȡ��չ���б����ѡ�е���չ��
	bool IsTmpFile(char * pszName);//�ж��Ƿ�Ϊ��ʱ�ļ�
	void Restore();//ֹͣɨ���ɨ����Ϻ�ָ����ݺ�״̬
	void ShowResultText();//��ʾɨ�����ı�
	void ShowImg(int i);//��ʾbmpͼ��
	void Rotating();//��תbmpͼ��
// �Ի�������
	enum { IDD = IDD_CLEARTMPFILE_DIALOG };
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��
// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedStart();
	
	
	
	CCheckListBox m_checkList;
	afx_msg void OnBnClickedSelall();
	CComboBox m_combox;
	afx_msg void OnBnClickedSelnull();
	
	afx_msg void OnBnClickedStop();
	CHSListBox m_listBoxResults;
	
	afx_msg void OnBnClickedSuspend();
	afx_msg void OnBnClickedDel();
	afx_msg void OnBnClickedDelall();
	CProgressCtrl m_progressCtl;
	afx_msg void OnClose();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);//���ÿؼ���ɫ
	CListBox OneClickDir;
	afx_msg void OnBnClickedButton1();
	CString GetTempDir();//��ȡ��ʱĿ¼
	CString GetIETempDir();
	CString GetWindowTempDir();
	void GetFolderSize(char* Targ , DWORD &foldersize);
	
};

