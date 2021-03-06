// ClearTmpFileDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "进程清理.h"
#include "ClearTmpFileDlg.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif
#define DLG_HS_LIST IDC_LIST1





// CClearTmpFileDlg 对话框
CClearTmpFileDlg::CClearTmpFileDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CClearTmpFileDlg::IDD, pParent)

{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	//SetIcon(m_hIcon,TRUE);

}

void CClearTmpFileDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);	

	DDX_Control(pDX, IDC_LIST2, m_checkList);
	DDX_Control(pDX, IDC_COMBO1, m_combox);

	DDX_Control(pDX, DLG_HS_LIST, m_listBoxResults);
	DDX_Control(pDX, IDC_PROGRESS1, m_progressCtl);
	DDX_Control(pDX, IDC_LIST3, OneClickDir);
}

BEGIN_MESSAGE_MAP(CClearTmpFileDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_START, &CClearTmpFileDlg::OnBnClickedStart)
	ON_BN_CLICKED(IDC_SELALL, &CClearTmpFileDlg::OnBnClickedSelall)
	ON_BN_CLICKED(IDC_SELNULL, &CClearTmpFileDlg::OnBnClickedSelnull)
	ON_BN_CLICKED(IDC_STOP, &CClearTmpFileDlg::OnBnClickedStop)

	ON_BN_CLICKED(IDC_SUSPEND, &CClearTmpFileDlg::OnBnClickedSuspend)
	ON_BN_CLICKED(IDC_DEL, &CClearTmpFileDlg::OnBnClickedDel)
	ON_BN_CLICKED(IDC_DELALL, &CClearTmpFileDlg::OnBnClickedDelall)
	ON_WM_CLOSE()
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_BUTTON1, &CClearTmpFileDlg::OnBnClickedButton1)
END_MESSAGE_MAP()


// CClearTmpFileDlg 消息处理程序
BOOL CClearTmpFileDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。




	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码


	//获取本地磁盘盘符
	TCHAR buf[100];
	DWORD len  =GetLogicalDriveStrings(200,buf);
	CString strDisks;
	for (TCHAR* s=buf; *s; s+=_tcslen(s)+1) 
	{ 
		LPCTSTR sDrivePath = s;     //单个盘符 
		strDisks += sDrivePath;      
		strDisks += " "; 
		m_combox.AddString(s);
	} 	
	m_combox.SetCurSel(0);

	m_bFinding =FALSE;
	m_bContinue = FALSE;
	m_hEvent =NULL;
	m_hThread  =NULL;

	m_bThreadExit = FALSE;
	m_dwScanedVol = 0;
	m_dwScanedTmpFileVol = 0;
	m_dwScanedTmpFileNum =0;
	m_bIsRotate = FALSE;
	GetDlgItem(IDC_STOP)->EnableWindow(FALSE);
	GetDlgItem(IDC_SUSPEND)->EnableWindow(FALSE);
	//界面颜色初始化
	m_bkColor=RGB(14,94,157);                      // 背景颜色   
	m_textcolor=RGB(255,255,255);                 // 文本颜色设置为白色  
	m_bkBrush.CreateSolidBrush(m_bkColor);      // 背景色画刷  
	//字体设置
	m_font =new CFont();
	m_font->CreateFont(14,0,0,0,FW_BOLD,FALSE,FALSE,0,ANSI_CHARSET
		,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,FF_SWISS,_T("宋体"));
	GetDlgItem(IDC_TEST)->SetFont(m_font,FALSE);


	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}
void CClearTmpFileDlg::OnSysCommand(UINT nID, LPARAM lParam)
{

	CDialog::OnSysCommand(nID, lParam);

}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CClearTmpFileDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);	
	}
	else
	{
		CRect   rect;
		CPaintDC   dc(this);
		GetClientRect(&rect);
		//设置对话框背景颜色
		dc.FillSolidRect(rect,m_bkColor);   //设置为窗口背景

		CDialog::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CClearTmpFileDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


/*====================自定义函数=============================================================*/

//=============================判断是否为临时文件=========================================
bool CClearTmpFileDlg::IsTmpFile(char * pszName)
{


	CString FileName;
	FileName.Format(_T("%s"),pszName);
	int Where;
	Where=FileName.ReverseFind('\\');
	if(Where==-1)
	{
		Where=FileName.ReverseFind('/');
	}
	CString FileTitle=FileName.Right(FileName.GetLength()-1-Where);
	int Which=FileTitle.ReverseFind('.');
	CString ExtName =FileTitle.Right(FileTitle.GetLength()-Which-1);
	if (Which!=-1)
	{
		FileTitle=FileTitle.Left(Which);
	}
	POSITION position = m_fileExtList.GetHeadPosition();
	while(position !=NULL)
	{
		CString	strExt = m_fileExtList.GetNext(position);
		strExt = strExt.Right(strExt.GetLength()-2);
		if(!m_bThreadExit)
		{
			GetDlgItem(IDC_TEST)->SetWindowText(FileName);//设置扫描路径
		}
		//MyDebug("获取到的扩展名:%s-比较的扩展名%s",ExtName,strExt);
		if(ExtName.CompareNoCase(strExt)==0 || m_oneclickforclean)
		{
			return TRUE;
		}		
	}

	return FALSE;
}
//================遍历磁盘目录，查找临时文件===========================================
void CClearTmpFileDlg::ResearchFile(char * pszPath)
{

	SHFILEOPSTRUCT FileOp;
	ZeroMemory((void*)&FileOp,sizeof(SHFILEOPSTRUCT));
	FileOp.fFlags = FOF_SILENT|FOF_NOCONFIRMATION;
	//FileOp.fFlags=FOF_NOCONFIRMATION;
	FileOp.hNameMappings = NULL;
	FileOp.hwnd = NULL;
	FileOp.lpszProgressTitle = NULL;
	FileOp.pTo = NULL;
	FileOp.wFunc = FO_DELETE;
	
	char szTmp[MAX_PATH*2]={0};//定义一个临时字符数组
	strcpy_s(szTmp,pszPath);
	if(szTmp[strlen(szTmp)-1]!='\\')//将目录以“\\*.*”形式结尾
	{
		strcat_s(szTmp,"\\*.*");//连接字符串
	}
	else
	{
		strcat_s(szTmp,"*.*");//连接字符串
	}
	WIN32_FIND_DATA findData;//定义一个文件查找数据结构
	memset(&findData,0,sizeof(WIN32_FIND_DATA));

	HANDLE hFind = FindFirstFile(szTmp,&findData);//开始查找文件
	//由于查找是在线程中进行的，这里判读用户是否退出线程，如果是则提前结束线程函数
	if(m_bThreadExit)
	{
		FindClose(hFind);//关闭查找句柄
		SetEvent(m_hEvent);//设置事件为有信号
		return;
	}
	if(hFind != INVALID_HANDLE_VALUE)//文件查找成功
	{

		while(FindNextFile(hFind,&findData)==TRUE)//查找下一个文件
		{
			//由于查找是在线程中进行的，这里判读用户是否退出线程，如果是则提前结束线程函数
			if (m_bThreadExit)
			{
				FindClose(hFind);//关闭查找句柄
				SetEvent(m_hEvent);//设置事件为有信号
				return;
			}
			//如果文件不是一个目录
			if(!(findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
			{
				DWORDLONG dwHighBase = MAXDWORD;
				dwHighBase += 1;
				DWORD dwFileSize = (DWORD)((findData.nFileSizeHigh* dwHighBase + findData.nFileSizeLow)/(1024));//获取文件大小,单位为KB
				m_dwScanedVol += dwFileSize;//累计已扫描文件的容量大小,单位为KB
				//m_dwScanedVol = m_dwScanedVol/1024;//单位转换为：MB
				//设置进度条进度
				m_progressCtl.SetPos((m_dwScanedVol/1024));


				TCHAR szFileName[MAX_PATH*2] = {0};//定义字符数组，存储完整的文件名,为防止溢出,所以MAX_PATH*2
				strcpy_s(szFileName,pszPath);//获取完整文件名
				if(szFileName[strlen(szFileName)-1] != '\\')
				{
					strcat_s(szFileName,"\\");
				}
				strcat_s(szFileName,(char *)findData.cFileName);

				if(IsTmpFile(szFileName))//判断szFileName是否是临时文件
				{
					m_dwScanedTmpFileVol += dwFileSize;//累计扫描到的临时文件容量大小，单位为KB
					m_dwScanedTmpFileNum ++;//累计扫描到的临时文件的数目
					m_listBoxResults.AddString((LPCTSTR)szFileName);
					//如果是一键清理,扫描到可立即删除
					if(m_oneclickforclean)
					{
						
						FileOp.pFrom = (LPCTSTR)szFileName;
						//if(DeleteFile((LPCTSTR)szFileName)==false)
						if(!SHFileOperation(&FileOp))
						{
						m_dwScanedTmpFileVol-=dwFileSize;//删除失败则减掉原来加上的临时文件容量大小
						m_dwScanedTmpFileNum--;
						}
					}

				}
			}
			else//如果文件是一个目录，则递归遍历该目录
			{
				if((strcmp((const char *)&findData.cFileName,"...")!=0) &&
					(strcmp((const char *)&findData.cFileName,"..")!=0)&&
					(strcmp((const char *)&findData.cFileName,".")!=0))
				{

					TCHAR szFileName[MAX_PATH*2] = {0};//定义字符数组，存储完整的文件名
					strcpy_s(szFileName,pszPath);//获取完整文件名
					if(szFileName[strlen(szFileName)-1]!='\\')
					{
					
						strcat_s(szFileName,"\\");
					}
					strcat_s(szFileName,(TCHAR *)findData.cFileName);
					FileOp.pFrom = (LPCTSTR)szFileName;
					SHFileOperation(&FileOp);
					//由于查找是在线程中进行的，这里判读用户是否退出线程，如果是则提前结束线程函数
					if(m_bThreadExit)
					{
						FindClose(hFind);//关闭查找句柄
						SetEvent(m_hEvent);//设置事件为有信号
						return;
					}
					ResearchFile(szFileName);//递归调用
				}
			}
		}
	}
	FindClose(hFind);//关闭文件查找句柄
}
//=============线程函数--查找文件线程===========================================================
DWORD _stdcall FindTmpFile(LPVOID lpParameter)
{
	CClearTmpFileDlg* pDlg = (CClearTmpFileDlg*) lpParameter;//获取线程参数
	WaitForSingleObject(pDlg->m_hEvent,INFINITE);//等待事件有信号
	if(pDlg->m_oneclickforclean)
	{
		for(int i=0;i<pDlg->OneClickDir.GetCount();i++)
		{
			CString dir;
			pDlg->OneClickDir.GetText(i,dir);//根据当前盘符目录磁盘目录
			//AfxMessageBox(dir);
			TCHAR *s = (LPSTR)(LPCTSTR)dir;	
			pDlg->ResearchFile(s);

		}
	}
	else
	{
		CString dir = pDlg->m_szCurDisk.GetBuffer();//根据当前盘符目录磁盘目录
		TCHAR *s = (LPSTR)(LPCTSTR)dir;	
		pDlg->ResearchFile(s);

	}
	pDlg->ShowResultText();//显示扫描临时文件的数目和大小
	pDlg->Restore();


	//恢复数据为初始状态
	pDlg->m_dwScanedTmpFileVol = 0;
	pDlg->m_dwScanedTmpFileNum = 0;
	pDlg->m_fileExtList.RemoveAll();
	return 0;
}
//=============线程函数--bmp图像旋转线程===========================================================
DWORD _stdcall RotatingImg(LPVOID lpParameter)
{
	CClearTmpFileDlg* pDlg = (CClearTmpFileDlg*)lpParameter;
	pDlg->Rotating();
	return 0;
}
//=========================获取扩展名列表框中选中的扩展名================================
bool CClearTmpFileDlg::GetTmpExtName()
{//获取扩展名列表框中选中的扩展名
	bool bHasSel = false;
	int j;
	for(int i = 0; i< m_checkList.GetCount();i++)
	{
		j = m_checkList.GetCheck(i);//读取复选框状态
		if(j ==1)//1 ，为选中，0，为未选中
		{
			bHasSel = true;
			CString str ;
			m_checkList.GetText(i,str);
			m_fileExtList.AddTail(str);//将扩展名加入CCheckListBox链表中
		}
	}
	return bHasSel;
}

//===========================获取磁盘容量==================================================
DWORD CClearTmpFileDlg::GetDiskSize(char* strPath)
{
	//CString strRootPath="c:\\";//带根目录标记的磁盘符号
	CString strRootPath;//带根目录标记的磁盘符号
	strRootPath.Format(_T("%s"),strPath);
	DWORD dwSectorsPerCluster=0;//每簇中扇区数
	DWORD dwBytesPerSector=0;//每扇区中字节数
	DWORD dwFreeClusters=0;//剩余簇数
	DWORD dwTotalClusters=0;//总簇数
	if (GetDiskFreeSpace(strRootPath,&dwSectorsPerCluster,&dwBytesPerSector,
		&dwFreeClusters,&dwTotalClusters))
	{
		//m_dwVolSize=dwTotalClusters*dwSectorsPerCluster*dwBytesPerSector;//不能这样，否则越界
		//double dd=dwSectorsPerCluster*dwBytesPerSector/(1024.*1024.);
		//dd=dd/1024.;
		double dd = dwSectorsPerCluster*dwBytesPerSector/(1024.);//单位为:KB
		DWORD dm_dVolSize=(DWORD)((dwTotalClusters-dwFreeClusters)*dd);//该磁盘总大小，单位为：KB

		return dm_dVolSize;//返回磁盘容量大小(已使用的)，单位为KB
	}
	return 0;
}
//=========================停止扫描或扫描完毕后恢复数据和状态=============================
void CClearTmpFileDlg::Restore()
{
	m_bThreadExit = TRUE;
	m_hThread = NULL;
	m_bFinding = FALSE;
	m_oneclickforclean=false;
	SetEvent(m_hEvent);
	GetDlgItem(IDC_START)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON1)->EnableWindow(TRUE);
	GetDlgItem(IDC_SUSPEND)->EnableWindow(FALSE);
	GetDlgItem(IDC_STOP)->EnableWindow(FALSE);
	m_bIsRotate = FALSE;
	m_dwScanedVol = 0;
	m_progressCtl.SetPos(m_dwDiskVol);
}
//==================显示扫描结果文本 即：显示扫描临时文件的数目和大小===========================
void CClearTmpFileDlg::ShowResultText()
{
	//显示扫描临时文件的数目和大小
	CString strSize;
	CString strNum;
	CString strType;
	DWORD m_dwSzie;
	DWORD m_dwYushu;//1.32GB，0.32部分MB数目
	double m_dYushu;
	m_dwSzie = m_dwScanedTmpFileVol;	
	if(m_dwSzie < 1024)
	{
		strType=_T("KB");
	}else if((m_dwSzie > 1024) && (m_dwSzie < (1024*1024)))
	{
		strType=_T("MB");
		m_dwSzie = m_dwSzie/1024;
	}else
	{
		strType=_T("GB");

		m_dwYushu = m_dwSzie%(1024*1024);//KB转GB后的余数

		m_dYushu = (double)m_dwYushu/(1024*1024);
		m_dwSzie = m_dwSzie/(1024*1024);		
	}
	if(strType.Compare(_T("GB"))==0)
	{
		CString strZhengshu;
		CString strXiaoshu;
		strZhengshu.Format(_T("%d"),m_dwSzie);
		strXiaoshu.Format(_T("%f"),m_dYushu);
		//取小数部分的字符串
		strXiaoshu=strXiaoshu.Left(4);
		strXiaoshu=strXiaoshu.Right(2);
		strSize = strZhengshu+"."+strXiaoshu;
	}else{
		strSize.Format(_T("%d"),m_dwSzie);
	}
	strNum.Format(_T("%d"),m_dwScanedTmpFileNum);
	if(m_oneclickforclean)
	{
		strSize = _T("一键清理已完成！本次共清理 ")+strNum+_T(" 个垃圾文件，已帮您节省 ")+strSize +strType+_T(" 磁盘空间。");
	}
	else
	{
		strSize = _T("扫描已完成！本次共扫描出 ")+strNum+_T(" 个垃圾文件，立即清除可帮您节省 ")+strSize +strType+_T(" 磁盘空间。");
	}
	GetDlgItem(IDC_TEST)->SetWindowText(strSize);
}

//========================bmp图像旋转====================================================
void CClearTmpFileDlg::Rotating()
{
	int i =1;
	while(i < 7)
	{
		ShowImg(i);

		i ++;
		if(i >=7 && m_bIsRotate)
			i=1;
	}
}
//==========================循环显示bmp图像====================================================
void CClearTmpFileDlg::ShowImg(int i)
{
	CWindowDC windowDC(this);
	CBitmap bmp;
	CDC memDC;
	BITMAPINFO bmpInfo;
	memDC.CreateCompatibleDC(&windowDC);
	switch(i)
	{
	case 1:bmp.LoadBitmap(IDB_BITMAP1);
		break;
	case 2:bmp.LoadBitmap(IDB_BITMAP2);
		break;
	case 3:bmp.LoadBitmap(IDB_BITMAP3);
		break;
	case 4:bmp.LoadBitmap(IDB_BITMAP4);
		break;
	case 5:bmp.LoadBitmap(IDB_BITMAP5);
		break;
	case 6:bmp.LoadBitmap(IDB_BITMAP6);
		break;

	}
	memDC.SelectObject(&bmp);
	bmp.GetObject(sizeof(BITMAPINFO),&bmpInfo);
	int nBmpCx = bmpInfo.bmiHeader.biWidth;
	int nBmpCy = bmpInfo.bmiHeader.biHeight;
	int nFrameCY = GetSystemMetrics(SM_CYFIXEDFRAME);//获取对话框边框的高度
	int nFrameCX = GetSystemMetrics(SM_CXFIXEDFRAME);//获取对话框边框的高度
	int nBorderCy = GetSystemMetrics(SM_CYBORDER) + nFrameCY;
	int nBorderCX = GetSystemMetrics(SM_CXBORDER)+nFrameCX;
	int nTitleHeight = GetSystemMetrics(SM_CYCAPTION) + nBorderCy;
	windowDC.StretchBlt(nBorderCX+10,nTitleHeight + 12,24,24,&memDC,0,0,nBmpCx,nBmpCy,SRCCOPY);
	Sleep(40);
}
/*========================按钮操作*=======================================================*/
/*========================按钮操作*=======================================================*/

//========================开始查找文件==================================================
void CClearTmpFileDlg::OnBnClickedStart()
{
	m_oneclickforclean=false;
	//动态添加复选框
	m_checkList.ResetContent();
	m_checkList.AddString(_T("*.log"));
	m_checkList.AddString(_T("*.chk"));
	m_checkList.AddString(_T("*.ipch"));
	m_checkList.AddString(_T("*.sdf"));
	//m_checkList.AddString(_T("*.obj"));
	//m_checkList.AddString(_T("*.pdb"));
	m_checkList.AddString(_T("*.tmp"));
	m_checkList.AddString(_T("*.??~"));
	m_checkList.AddString(_T("*.old"));
	m_checkList.AddString(_T("*.gid"));
	m_checkList.AddString(_T("*._mp"));
	m_checkList.AddString(_T("*.syd"));

	for(int i= 0; i<m_checkList.GetCount();i++)
	{
		m_checkList.SetCheck(i,1);
	}
	// TODO: 在此添加控件通知处理程序代码
	//如果查找没有结束，则不允许开始新的文件查找
	if(!m_bFinding && GetTmpExtName())//获取文件扩展名
	{
		GetDlgItem(IDC_PROGRESS1)->ShowWindow(TRUE);//显示进度条


		m_bThreadExit  = FALSE;
		m_bFinding = TRUE;
		m_combox.GetWindowText(m_szCurDisk);//获取当前盘符

		// 初始化进度条相关数据
		m_dwDiskVol =GetDiskSize((LPSTR)(LPCTSTR)m_szCurDisk);//获取当前磁盘的容量大小(已使用的)

		m_progressCtl.SetRange32(0,m_dwDiskVol/1024);//初始化进度条，设置进度条的范围,范围为MB的数量
		if(m_hEvent!=NULL)
		{
			CloseHandle(m_hEvent);//关闭事件对象
			m_hEvent = NULL;
		}
		m_listBoxResults.ResetContent();//清空查找结果列表
		m_hEvent = CreateEvent(NULL,FALSE,TRUE,_T("Event"));//创建事件对象

		//创建一个线程，开始执行线程函数
		m_hThread = CreateThread(NULL,0,FindTmpFile,this,0,NULL);	
		m_hThread2 = CreateThread(NULL,0,RotatingImg,this,0,NULL);

		GetDlgItem(IDC_START)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON1)->EnableWindow(FALSE);
		GetDlgItem(IDC_STOP)->EnableWindow(TRUE);
		GetDlgItem(IDC_SUSPEND)->EnableWindow(TRUE);
		GetDlgItem(IDC_SUSPEND)->SetWindowText(_T("暂停"));

		UpdateData(FALSE);
	}
	else
	{
		GetDlgItem(IDC_TEST)->SetWindowText(_T("请选择文件类型！"));
	}
	m_bIsRotate = TRUE;
}
//========================停止查找文件==================================================
void CClearTmpFileDlg::OnBnClickedStop()
{
	// TODO: 在此添加控件通知处理程序代码
	m_bIsRotate = FALSE;
	if(m_bContinue)
	{
		ResumeThread(m_hThread);
	}
	GetDlgItem(IDC_SUSPEND)->SetWindowText(_T("暂停"));
	ShowResultText();
	Restore();	
}
//========================暂停查找文件==================================================
void CClearTmpFileDlg::OnBnClickedSuspend()
{
	// TODO: 在此添加控件通知处理程序代码
	if(!m_bContinue)//暂停操作
	{
		SuspendThread(m_hThread);
		SuspendThread(m_hThread2);
		m_bContinue = TRUE;
		GetDlgItem(IDC_START)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON1)->EnableWindow(FALSE);
		GetDlgItem(IDC_SUSPEND)->SetWindowText(_T("继续"));


	}
	else//继续扫描操作
	{
		ResumeThread(m_hThread);
		ResumeThread(m_hThread2);
		m_bContinue = FALSE;		
		GetDlgItem(IDC_SUSPEND)->SetWindowText(_T("暂停"));
	}

}
//========================全选临时文件类型==================================================
void CClearTmpFileDlg::OnBnClickedSelall()
{
	// TODO: 在此添加控件通知处理程序代码
	for(int i = 0; i< 6;i++)
	{
		m_checkList.SetCheck(i,1);
	}
	GetTmpExtName();
}
//========================取消文件类型==================================================
void CClearTmpFileDlg::OnBnClickedSelnull()
{
	// TODO: 在此添加控件通知处理程序代码
	for(int i = 0; i< 6;i++)
	{
		m_checkList.SetCheck(i,0);
	}	
}

//========================删除临时文件==================================================
void CClearTmpFileDlg::OnBnClickedDel()
{
	CString strDel;
	CFile file;
	m_listBoxResults.GetText(m_listBoxResults.GetCurSel(),strDel);
	m_listBoxResults.DeleteString(m_listBoxResults.GetCurSel());
	//file.Remove(strDel);
	//DeleteFile(strDel);
	SHFILEOPSTRUCT FileOp;
	ZeroMemory((void*)&FileOp,sizeof(SHFILEOPSTRUCT));
	FileOp.fFlags = FOF_SILENT|FOF_NOCONFIRMATION;
	FileOp.hNameMappings = NULL;
	FileOp.hwnd = NULL;
	FileOp.lpszProgressTitle = NULL;
	FileOp.pFrom = strDel;
	FileOp.pTo = NULL;
	FileOp.wFunc = FO_DELETE;
	SHFileOperation(&FileOp);
}
//========================删除全部临时文件==================================================
void CClearTmpFileDlg::OnBnClickedDelall()
{
	CString strDel;
	CFile file;
	SHFILEOPSTRUCT FileOp;
	ZeroMemory((void*)&FileOp,sizeof(SHFILEOPSTRUCT));
	FileOp.fFlags = FOF_SILENT|FOF_NOCONFIRMATION;
	FileOp.hNameMappings = NULL;
	FileOp.hwnd = NULL;
	FileOp.lpszProgressTitle = NULL;
	 FileOp.wFunc = FO_DELETE;
	FileOp.pTo = NULL;
	for(int i=0;i<m_listBoxResults.GetCount();i ++)
	{

		m_listBoxResults.GetText(i,strDel);
		GetDlgItem(IDC_TEST)->SetWindowText(strDel);
		//file.Remove(strDel);
	//	DeleteFile(strDel);
		
		
	FileOp.pFrom = strDel;	
   
    SHFileOperation(&FileOp);
		
		

	}
	m_listBoxResults.ResetContent();
	GetDlgItem(IDC_TEST)->SetWindowText(_T("清理完毕！"));

}

void CClearTmpFileDlg::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	//ExitThread(0);
	if(m_bFinding)
	{
		if(MessageBox(_T("扫描还未完成，您确定要关闭程序码？"),_T("垃圾清理"),MB_YESNO|MB_ICONQUESTION)==IDYES)
		{
			//exit(0);
			CDialog::OnClose();
		}
	}
	else
	{
		//exit(0);
		CDialog::OnClose();
	}
}
//=================设置控件颜色=======================================
HBRUSH CClearTmpFileDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  在此更改 DC 的任何属性
	if(pWnd->GetDlgCtrlID()==IDC_SEL_DISK||pWnd->GetDlgCtrlID()==IDC_TEST)
	{

		pDC->SetTextColor(m_textcolor);//设置控件字体颜色
		pDC->SetBkColor(m_bkColor);//设置控件字体背景颜色
		hbr = (HBRUSH) m_bkBrush;			
	}
	// TODO:  如果默认的不是所需画笔，则返回另一个画笔
	return hbr;
}

//一键清理
void CClearTmpFileDlg::OnBnClickedButton1()
{

	m_oneclickforclean=true;
	OneClickDir.ResetContent();
	OneClickDir.AddString(GetTempDir());
	OneClickDir.AddString(GetIETempDir());
	OneClickDir.AddString(GetWindowTempDir());
	m_checkList.ResetContent();
	m_checkList.AddString(_T("*.*"));
	m_checkList.SetCheck(0,1);


	if(!m_bFinding && GetTmpExtName())//获取文件扩展名
	{
		GetDlgItem(IDC_PROGRESS1)->ShowWindow(TRUE);//显示进度条


		m_bThreadExit  = FALSE;
		m_bFinding = TRUE;
		CString DirspaceSzie;
		DWORD tmpszie=0;
		m_dwDiskVol=0;
		for(int i=0;i<OneClickDir.GetCount();i++)
		{
			OneClickDir.GetText(i,DirspaceSzie);
			// 初始化进度条相关数据
			GetFolderSize((LPSTR)(LPCTSTR)DirspaceSzie,tmpszie);
			m_dwDiskVol =m_dwDiskVol+tmpszie;//获取需要清理的文件夹大小
		}
		MyDebug("目录:%s,大小:%d",DirspaceSzie,m_dwDiskVol/(1048576));

		m_progressCtl.SetRange32(0,m_dwDiskVol/1048576);//初始化进度条，设置进度条的范围,范围为MB的数量
		if(m_hEvent!=NULL)
		{
			CloseHandle(m_hEvent);//关闭事件对象
			m_hEvent = NULL;
		}
		m_listBoxResults.ResetContent();//清空查找结果列表
		m_hEvent = CreateEvent(NULL,FALSE,TRUE,_T("Event"));//创建事件对象

		//创建一个线程，开始执行线程函数
		m_hThread = CreateThread(NULL,0,FindTmpFile,this,0,NULL);	
		m_hThread2 = CreateThread(NULL,0,RotatingImg,this,0,NULL);

		GetDlgItem(IDC_START)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON1)->EnableWindow(FALSE);
		GetDlgItem(IDC_STOP)->EnableWindow(TRUE);
		GetDlgItem(IDC_SUSPEND)->EnableWindow(TRUE);
		GetDlgItem(IDC_SUSPEND)->SetWindowText(_T("暂停"));

		UpdateData(FALSE);
	}
	else
	{
		GetDlgItem(IDC_TEST)->SetWindowText(_T("请选择文件类型！"));
	}
	m_bIsRotate = TRUE;

}
//获取IE缓存目录
CString CClearTmpFileDlg::GetIETempDir()
{
	TCHAR UserName[MAX_PATH];
	TCHAR DeleteTemp[MAX_PATH];
	DWORD Size = MAX_PATH;
	GetUserName(UserName, &Size);

	GetWindowsDirectory(DeleteTemp, MAX_PATH);
	DeleteTemp[3] = '\0';
	OSVERSIONINFO OS;
	OS.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	GetVersionEx(&OS);

	switch(OS.dwMajorVersion)
	{
		//XP
	case 5:
		strcat_s(DeleteTemp, "Documents and Settings\\");

		strcat_s(DeleteTemp, UserName);


		strcat_s(DeleteTemp, "\\Local Settings\\Temporary Internet Files\\");

		break;

		//Win7
	case 6:
		strcat_s(DeleteTemp, "Users\\");

		strcat_s(DeleteTemp, UserName);

		strcat_s(DeleteTemp, "\\AppData\\Local\\Microsoft\\Windows\\Temporary Internet Files\\");


		break;
	}
	return DeleteTemp;
}

CString CClearTmpFileDlg::GetTempDir()
{
	TCHAR UserName[MAX_PATH];
	TCHAR DeleteTemp[MAX_PATH];
	DWORD Size = MAX_PATH;
	GetUserName(UserName, &Size);

	GetWindowsDirectory(DeleteTemp, MAX_PATH);
	DeleteTemp[3] = '\0';
	OSVERSIONINFO OS;
	OS.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	GetVersionEx(&OS);

	switch(OS.dwMajorVersion)
	{
		//XP
	case 5:
		strcat_s(DeleteTemp, "Documents and Settings\\");

		strcat_s(DeleteTemp, UserName);


		strcat_s(DeleteTemp, "\\Local Settings\\Temp\\");

		break;

		//Win7
	case 6:
		strcat_s(DeleteTemp, "Users\\");

		strcat_s(DeleteTemp, UserName);

		strcat_s(DeleteTemp, "\\AppData\\Local\\Temp\\");


		break;
	}
	return DeleteTemp;
}
//获取windows下的temp目录
CString CClearTmpFileDlg::GetWindowTempDir()
{
	TCHAR DeleteTemp[MAX_PATH];
	GetWindowsDirectory(DeleteTemp, MAX_PATH);
	strcat_s(DeleteTemp, "\\Temp\\");


	return DeleteTemp;
}



//获取文件夹字节大小,
void CClearTmpFileDlg::GetFolderSize(char* Targ , DWORD &foldersize)
{
	HANDLE hFirstFile = NULL; 
	WIN32_FIND_DATA FindData; 

	TCHAR currdir[MAX_PATH] = {0};
	sprintf_s(currdir, "%s\\*.*", Targ);
	hFirstFile = ::FindFirstFile(currdir, &FindData); 
	if( hFirstFile == INVALID_HANDLE_VALUE ) 
		return ;
	BOOL bRes = TRUE; 
	while(1) 
	{ 
		bRes = ::FindNextFile(hFirstFile, &FindData); 
		if( bRes == FALSE ) 
			break ;
		if( (FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) ) //发现目录
		{
			if( !strcmp(FindData.cFileName, ".") || !strcmp(FindData.cFileName, "..") ) //.或..
				continue;
			else
			{
				TCHAR tmppath[MAX_PATH] = {0};
				sprintf_s(tmppath, "%s\\%s", Targ, FindData.cFileName);

				GetFolderSize(tmppath,foldersize);
			}
		}
		else               //发现文件
		{
			DWORD64 FileSize = ((__int64)FindData.nFileSizeHigh << 32) + FindData.nFileSizeLow;
			//foldersize += FileSize;
			foldersize=(DWORD)(foldersize+FileSize);
		}
	}

	::FindClose(hFirstFile);
}


