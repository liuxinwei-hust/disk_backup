// BackupProgress.cpp : 实现文件
//

#include "stdafx.h"
#include "backup_windows.h"
#include "BackupProgress.h"
#include "afxdialogex.h"
#include "backupinfo.h"

extern Backup_Mission BackupPartinfo;
extern BOOL g_bCancel;
extern CMutex backmutex;
// CBackupProgress 对话框
#define WM_MYUPDATE (WM_USER + 101)

IMPLEMENT_DYNAMIC(CBackupProgress, CDialogEx)

CBackupProgress::CBackupProgress(CWnd* pParent /*=NULL*/)
	: CDialogEx(CBackupProgress::IDD, pParent)
	, m_backupdest(_T(""))
	, m_percentage(_T(""))
	, m_backupsource(_T(""))
	, m_lefttime(_T(""))
{

}

CBackupProgress::~CBackupProgress()
{
}

void CBackupProgress::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_PRODEST, m_backupdest);
	DDX_Control(pDX, IDC_PROGRESS1, m_progress);
	DDX_Text(pDX, IDC_PROPERCE, m_percentage);
	DDX_Text(pDX, IDC_PROSOURCE, m_backupsource);
	DDX_Text(pDX, IDC_PROTIME, m_lefttime);
}



BEGIN_MESSAGE_MAP(CBackupProgress, CDialogEx)
	ON_BN_CLICKED(IDCANCEL, &CBackupProgress::OnBnClickedCancel)
ON_WM_CLOSE()
ON_MESSAGE(WM_MYUPDATE, &CBackupProgress::OnMyupdate)
END_MESSAGE_MAP()


// CBackupProgress 消息处理程序


BOOL CBackupProgress::OnInitDialog()
{	
	CDialogEx::OnInitDialog();
	ModifyStyleEx(WS_EX_TOOLWINDOW, WS_EX_APPWINDOW);
	m_backupdest=BackupPartinfo.strDestDrive;
	m_backupsource.Format("%c:\\盘",BackupPartinfo.strSourceDrive);
	m_percentage.Format("0%%");
	m_lefttime.Format("正在统计...");
	UpdateData(false);
	backmutex.Unlock();
	return TRUE;  // return TRUE unless you set the focus to a control

}


void CBackupProgress::OnBnClickedCancel()
{
	// TODO: 在此添加控件通知处理程序代码
	if(MessageBox("取消后备份文件将删除，确认取消？","提示",MB_YESNO)==IDYES)
	{
		g_bCancel=TRUE;
		//this->EndDialog(0);
	}
}


//void CBackupProgress::OnClose()
//{
//	// TODO: 在此添加消息处理程序代码和/或调用默认值
//	/*if(MessageBox("取消后备份文件将删除，确认取消？","提示",MB_YESNO)==IDYES)
//	{
//		g_bCancel=TRUE;
//	    
//	}*/
//	this->EndDialog(0);
//}


void CBackupProgress::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	this->EndDialog(0);
	//CDialogEx::OnClose();
}


afx_msg LRESULT CBackupProgress::OnMyupdate(WPARAM wParam, LPARAM lParam)
{
	UpdateData(false);
	return 0;
}
