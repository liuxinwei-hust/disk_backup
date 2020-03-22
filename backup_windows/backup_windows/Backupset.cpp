// Backupset.cpp : 实现文件
//

#include "stdafx.h"
#include "backup_windows.h"
#include "Backupset.h"
#include "afxdialogex.h"
#include "backupinfo.h"

extern Backup_Mission BackupPartinfo;

// CBackupset 对话框

IMPLEMENT_DYNAMIC(CBackupset, CDialogEx)

CBackupset::CBackupset(CWnd* pParent /*=NULL*/)
	: CDialogEx(CBackupset::IDD, pParent)
	, m_compress(0)
	, m_crc(0)
	, m_shutdown(0)
{

}

CBackupset::~CBackupset()
{
}

void CBackupset::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Radio(pDX, IDC_RADIO1, m_compress);
	DDX_Radio(pDX, IDC_RADIO3, m_crc);
	DDX_Control(pDX, IDC_COMBO1, m_split);
	//  DDX_Check(pDX, IDC_CHECK1, m_check);
	DDX_Check(pDX, IDC_CHECK2, m_shutdown);
	DDX_Control(pDX, IDC_CHECK1, m_endcheck);
}


BEGIN_MESSAGE_MAP(CBackupset, CDialogEx)
	ON_BN_CLICKED(IDCANCEL, &CBackupset::OnBnClickedCancel)
	ON_BN_CLICKED(IDOK, &CBackupset::OnBnClickedOk)
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_RADIO3, &CBackupset::OnBnClickedRadio3)
	ON_BN_CLICKED(IDC_RADIO4, &CBackupset::OnBnClickedRadio4)
END_MESSAGE_MAP()


// CBackupset 消息处理程序


void CBackupset::OnBnClickedCancel()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData();
	BackupPartinfo.bCompress=(BOOL)m_compress;
	BackupPartinfo.bCrc=(BOOL)m_crc;
	BackupPartinfo.bEndCheck=(BOOL)m_endcheck.GetCheck();
	BackupPartinfo.bEndShutdown=(BOOL)m_shutdown;
	int i=m_split.GetCurSel();
	BackupPartinfo.SplitType=i;
	switch (i)
	{
	case 0:
		BackupPartinfo.Split=0;
		break;
	case 1:
		BackupPartinfo.Split=4267704320;
		break;
	case 2:
		BackupPartinfo.Split=0x80000000 - 0xD00000;
		break;
	case 3:
		BackupPartinfo.Split=1150*1024*1024;
		break;
	case 4:
		BackupPartinfo.Split=680*1024*1024;
		break;
	case 5:
		BackupPartinfo.Split=630*1024*1024;
		break;
	case 6:
		BackupPartinfo.Split=595*1024*1024;
		break;
	case 7:
		BackupPartinfo.Split=200*1024*1024;
		break;
	default:
		BackupPartinfo.Split=0;
		break;
	}

	this->EndDialog(0);
	//CDialogEx::OnCancel();
}


void CBackupset::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	//CDialogEx::OnOK();
	UpdateData();
	m_compress=0;
	m_crc=0;
	m_endcheck.SetCheck(0);
	m_endcheck.EnableWindow(false);
	m_shutdown=0;
	m_split.SetCurSel(0);
	UpdateData(false);
}


BOOL CBackupset::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	m_split.AddString("无限制");
	m_split.AddString("4GB");
	m_split.AddString("2GB");
	m_split.AddString("1.3GB");
	m_split.AddString("700MB");
	m_split.AddString("650MB");
	m_split.AddString("640MB");
	m_split.AddString("230MB");
	m_split.SetCurSel(BackupPartinfo.SplitType);

	return TRUE;


}


void CBackupset::OnPaint()
{	
	CPaintDC dc(this); // device context for painting
	m_split.SetCurSel(BackupPartinfo.SplitType);

	m_compress=(int)BackupPartinfo.bCompress;
	m_crc=(int)BackupPartinfo.bCrc;
	if(m_crc)
	{
		m_endcheck.EnableWindow(true);
		m_endcheck.SetCheck(BackupPartinfo.bEndCheck);
	}
	else
	{
		m_endcheck.EnableWindow(false);
		m_endcheck.SetCheck(0);
	}
	m_shutdown=(int)BackupPartinfo.bEndShutdown;
	UpdateData(false);
	// TODO: 在此处添加消息处理程序代码
	// 不为绘图消息调用 CDialogEx::OnPaint()
}


void CBackupset::OnBnClickedRadio3()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData();
	m_endcheck.EnableWindow(false);
    m_endcheck.SetCheck(0);
	UpdateData(false);
}


void CBackupset::OnBnClickedRadio4()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData();
	m_endcheck.EnableWindow(true);
//	UpdateData(false);
}
