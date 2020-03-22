
// backup_windowsDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "backup_windows.h"
#include "backup_windowsDlg.h"
#include "afxdialogex.h"
#include "Backupsheet.h"
#include "backupinfo.h"
#include "BackupProgress.h"
#include "RestoreSheet.h"
#include "RestoreProgress.h"
#include "CheckProgress.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

Backup_Mission BackupPartinfo;
BOOL g_bCancel=FALSE;
BOOL g_bchCancel=FALSE;
CString checkfilepath="";

CMutex restmutex;
CMutex backmutex;

extern int backup(CBackupProgress *progressdlg);
extern int restore(CRestoreProgress *progressdlg);
extern int do_check(CCheckProgress *progressdlg);
extern Header head;
extern Restore_Mission RestorePart;

// 用于应用程序“关于”菜单项的 CAboutDlg 对话框
DWORD WINAPI  threadfun3(LPVOID paraneter)
{
	CCheckProgress *progressdlg3;
	progressdlg3=(CCheckProgress *)paraneter;
	backmutex.Lock();
	do_check(progressdlg3);
	progressdlg3->SendMessage(WM_CLOSE,0,0);
	backmutex.Unlock();
	return 0;
}

DWORD WINAPI  threadfun(LPVOID paraneter)
{
	CBackupProgress *progressdlg3;
	progressdlg3=(CBackupProgress *)paraneter;
	backmutex.Lock();
	backup(progressdlg3);
	progressdlg3->SendMessage(WM_CLOSE,0,0);
	backmutex.Unlock();


	return 0;
}
DWORD WINAPI  threadfun2(LPVOID paraneter)
{
	CRestoreProgress *progressdlg3;
	progressdlg3=(CRestoreProgress *)paraneter;
	restmutex.Lock();
	restore(progressdlg3);
	progressdlg3->SendMessage(WM_CLOSE,0,0);
	restmutex.Unlock();
	return 0;
}



class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// Cbackup_windowsDlg 对话框



Cbackup_windowsDlg::Cbackup_windowsDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(Cbackup_windowsDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void Cbackup_windowsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(Cbackup_windowsDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, &Cbackup_windowsDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &Cbackup_windowsDlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDCANCEL, &Cbackup_windowsDlg::OnBnClickedCancel)
END_MESSAGE_MAP()


// Cbackup_windowsDlg 消息处理程序

BOOL Cbackup_windowsDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void Cbackup_windowsDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void Cbackup_windowsDlg::OnPaint()
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
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR Cbackup_windowsDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void Cbackup_windowsDlg::OnBnClickedButton1()
{
	// TODO: 在此添加控件通知处理程序代码
	this->EndDialog(0);
	CBackupsheet m_sheet("备份");
	m_sheet.m_psh.dwFlags &= ~(PSH_HASHELP);
	m_sheet.SetWizardMode();
	if(ID_WIZFINISH == m_sheet.DoModal())
	{
		backmutex.Lock();
		CBackupProgress progressdlg;
		HANDLE mythread;
		mythread=CreateThread(NULL,0,threadfun,&progressdlg,0,NULL);
		CloseHandle(mythread);
		progressdlg.DoModal();
		

		if(BackupPartinfo.bCrc&&BackupPartinfo.bEndCheck)
		{
			checkfilepath=BackupPartinfo.strDestDrive;
			checkfilepath+=BackupPartinfo.strHdzFileName;
			backmutex.Lock();
			CCheckProgress check_dlg;
			HANDLE mythread2;
	     	mythread2=CreateThread(NULL,0,threadfun3,&check_dlg,0,NULL);
		    CloseHandle(mythread2);
		    check_dlg.DoModal();
		}
		if(BackupPartinfo.bEndShutdown)
		{
			system("shutdown -s -t 0");
		}
	
	}
	
}


void Cbackup_windowsDlg::OnBnClickedButton2()
{
	this->EndDialog(0);
	CRestoreSheet m_sheet("还原");
	m_sheet.m_psh.dwFlags &= ~(PSH_HASHELP);
	m_sheet.SetWizardMode();
    if(ID_WIZFINISH == m_sheet.DoModal())
	{
		if(head.CRCInfo&&RestorePart.RestCheck)
		{
			checkfilepath=RestorePart.FilePath;
            int k=checkfilepath.GetLength()-4;
            checkfilepath.GetBuffer()[k]='\0';
			backmutex.Lock();
			CCheckProgress check_dlg;
			HANDLE mythread2;
	     	mythread2=CreateThread(NULL,0,threadfun3,&check_dlg,0,NULL);
		    CloseHandle(mythread2);
		    check_dlg.DoModal();
		}
		restmutex.Lock();
		HANDLE mythread;
		CRestoreProgress progressdlg2;
		mythread=CreateThread(NULL,0,threadfun2,&progressdlg2,0,NULL);
		CloseHandle(mythread);
		progressdlg2.DoModal();
		if(RestorePart.bEndShutdown)
		{
			 system("shutdown -s -t 0");
		}
	}
	
}


void Cbackup_windowsDlg::OnBnClickedCancel()
{
	// TODO: 在此添加控件通知处理程序代码
	this->EndDialog(0);
}
