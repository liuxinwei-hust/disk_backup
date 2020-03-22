// RestoreProgress.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "backup_windows.h"
#include "RestoreProgress.h"
#include "afxdialogex.h"
#include "backupinfo.h"

extern Restore_Mission RestorePart;
extern BOOL g_bCancel;
extern CMutex restmutex;
// CRestoreProgress �Ի���
#define WM_RESTUPDATE (WM_USER + 102)

IMPLEMENT_DYNAMIC(CRestoreProgress, CDialogEx)

CRestoreProgress::CRestoreProgress(CWnd* pParent /*=NULL*/)
	: CDialogEx(CRestoreProgress::IDD, pParent)
	, m_percentage(_T(""))
	, m_filename(_T(""))
	, m_partiton(_T(""))
	, m_lefttime(_T(""))
{

}

CRestoreProgress::~CRestoreProgress()
{
}

void CRestoreProgress::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_RESTPROPER, m_percentage);
	DDX_Text(pDX, IDC_RESTPROFILE, m_filename);
	DDX_Text(pDX, IDC_RESTPROPAR, m_partiton);
	DDX_Text(pDX, IDC_RESTPROTIME, m_lefttime);
	//DDX_Control(pDX, IDD_RESTPRO, m_progress);
	DDX_Control(pDX, IDD_RestPro, m_prog);
}


BEGIN_MESSAGE_MAP(CRestoreProgress, CDialogEx)
	ON_BN_CLICKED(IDCANCEL, &CRestoreProgress::OnBnClickedCancel)
	ON_WM_CLOSE()
	ON_MESSAGE(WM_RESTUPDATE, &CRestoreProgress::OnRestupdate)
END_MESSAGE_MAP()


// CRestoreProgress ��Ϣ�������


void CRestoreProgress::OnBnClickedCancel()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	//CDialogEx::OnCancel();
	if(MessageBox("ȡ������������޷�����ʹ�ã�ȷ��ȡ����","��ʾ",MB_YESNO)==IDYES)
	{
		g_bCancel=TRUE;
	}
}


BOOL CRestoreProgress::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	ModifyStyleEx(WS_EX_TOOLWINDOW, WS_EX_APPWINDOW);
	m_filename=RestorePart.FilePath;
	m_partiton=RestorePart.strSourceDrive;
	m_prog.SetRange(1,100);
	m_partiton+=":\\��";
	m_percentage.Format("0%%");
	m_lefttime.Format("����ͳ��...");
	UpdateData(false);
	restmutex.Unlock();
	return TRUE;  // return TRUE unless you set the focus to a control

}


void CRestoreProgress::OnClose()
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	this->EndDialog(0);
	//CDialogEx::OnClose();
}


afx_msg LRESULT CRestoreProgress::OnRestupdate(WPARAM wParam, LPARAM lParam)
{
	UpdateData(false);
	return 0;
}
