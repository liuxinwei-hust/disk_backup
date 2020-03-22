// BackupProgress.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "backup_windows.h"
#include "BackupProgress.h"
#include "afxdialogex.h"
#include "backupinfo.h"

extern Backup_Mission BackupPartinfo;
extern BOOL g_bCancel;
extern CMutex backmutex;
// CBackupProgress �Ի���
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


// CBackupProgress ��Ϣ�������


BOOL CBackupProgress::OnInitDialog()
{	
	CDialogEx::OnInitDialog();
	ModifyStyleEx(WS_EX_TOOLWINDOW, WS_EX_APPWINDOW);
	m_backupdest=BackupPartinfo.strDestDrive;
	m_backupsource.Format("%c:\\��",BackupPartinfo.strSourceDrive);
	m_percentage.Format("0%%");
	m_lefttime.Format("����ͳ��...");
	UpdateData(false);
	backmutex.Unlock();
	return TRUE;  // return TRUE unless you set the focus to a control

}


void CBackupProgress::OnBnClickedCancel()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if(MessageBox("ȡ���󱸷��ļ���ɾ����ȷ��ȡ����","��ʾ",MB_YESNO)==IDYES)
	{
		g_bCancel=TRUE;
		//this->EndDialog(0);
	}
}


//void CBackupProgress::OnClose()
//{
//	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
//	/*if(MessageBox("ȡ���󱸷��ļ���ɾ����ȷ��ȡ����","��ʾ",MB_YESNO)==IDYES)
//	{
//		g_bCancel=TRUE;
//	    
//	}*/
//	this->EndDialog(0);
//}


void CBackupProgress::OnClose()
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	this->EndDialog(0);
	//CDialogEx::OnClose();
}


afx_msg LRESULT CBackupProgress::OnMyupdate(WPARAM wParam, LPARAM lParam)
{
	UpdateData(false);
	return 0;
}
