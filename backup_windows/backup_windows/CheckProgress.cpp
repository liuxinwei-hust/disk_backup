// CheckProgress.cpp : 实现文件
//

#include "stdafx.h"
#include "backup_windows.h"
#include "CheckProgress.h"
#include "afxdialogex.h"


// CCheckProgress 对话框

IMPLEMENT_DYNAMIC(CCheckProgress, CDialogEx)

#define WM_CHECKUPDATE (WM_USER + 103)
extern BOOL g_bchCancel;
extern CMutex backmutex;
extern CString checkfilepath;


CCheckProgress::CCheckProgress(CWnd* pParent /*=NULL*/)
	: CDialogEx(CCheckProgress::IDD, pParent)
	, m_filepath(_T(""))
	, m_percentage(_T(""))
	, m_lefttime(_T(""))
{

}

CCheckProgress::~CCheckProgress()
{
}

void CCheckProgress::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_CHECKFIPATH, m_filepath);
	DDX_Text(pDX, IDC_CHECKPER, m_percentage);
	DDX_Text(pDX, IDC_CHECKTIME, m_lefttime);
	DDX_Control(pDX, IDC_CHECKPRO, m_progress);
}


BEGIN_MESSAGE_MAP(CCheckProgress, CDialogEx)
	ON_WM_CLOSE()
	ON_MESSAGE(WM_CHECKUPDATE, &CCheckProgress::OnCheckupdate)
	ON_BN_CLICKED(IDCANCEL, &CCheckProgress::OnBnClickedCancel)
END_MESSAGE_MAP()


// CCheckProgress 消息处理程序


BOOL CCheckProgress::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	ModifyStyleEx(WS_EX_TOOLWINDOW, WS_EX_APPWINDOW);
	m_progress.SetRange(1,100);
	m_filepath=checkfilepath+".hdz";
	m_percentage.Format("0%%");
	m_lefttime.Format("正在统计...");
	UpdateData(false);
	backmutex.Unlock();

	return TRUE;  // return TRUE unless you set the focus to a control

}


void CCheckProgress::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	this->EndDialog(0);
	//CDialogEx::OnClose();
}


afx_msg LRESULT CCheckProgress::OnCheckupdate(WPARAM wParam, LPARAM lParam)
{
	UpdateData(false);
	return 0;
}


void CCheckProgress::OnBnClickedCancel()
{
	// TODO: 在此添加控件通知处理程序代码
	g_bchCancel=TRUE;
}
