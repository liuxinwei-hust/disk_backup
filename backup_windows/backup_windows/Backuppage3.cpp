// Backuppage3.cpp : 实现文件
//

#include "stdafx.h"
#include "backup_windows.h"
#include "Backuppage3.h"
#include "afxdialogex.h"
#include "Backupsheet.h"
#include "backupinfo.h"

extern Backup_Mission BackupPartinfo;
// CBackuppage3 对话框

IMPLEMENT_DYNAMIC(CBackuppage3, CPropertyPage)

CBackuppage3::CBackuppage3()
	: CPropertyPage(CBackuppage3::IDD)
	, m_check(_T(""))
	, m_compress(_T(""))
	, m_crc(_T(""))
	, m_destdrive(_T(""))
	, m_sourcedisk(_T(""))
	, m_filename(_T(""))
	, m_filesystem(_T(""))
	, m_sourcepart(_T(""))
	, m_parttype(_T(""))
	, m_password(_T(""))
	, m_shutdown(_T(""))
	, m_split(_T(""))
{

}

CBackuppage3::~CBackuppage3()
{
}

void CBackuppage3::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_PAGE3CHECK, m_check);
	DDX_Text(pDX, IDC_PAGE3COMPR, m_compress);
	DDX_Text(pDX, IDC_PAGE3CRC, m_crc);
	DDX_Text(pDX, IDC_PAGE3DEST, m_destdrive);
	DDX_Text(pDX, IDC_PAGE3DISK, m_sourcedisk);
	DDX_Text(pDX, IDC_PAGE3FILENAME, m_filename);
	DDX_Text(pDX, IDC_PAGE3FILESYS, m_filesystem);
	DDX_Text(pDX, IDC_PAGE3PARTIT, m_sourcepart);
	DDX_Text(pDX, IDC_PAGE3PARTTYPE, m_parttype);
	DDX_Text(pDX, IDC_PAGE3PASS, m_password);
	DDX_Text(pDX, IDC_PAGE3SHUT, m_shutdown);
	DDX_Text(pDX, IDC_PAGE3SPLIT, m_split);
}


BEGIN_MESSAGE_MAP(CBackuppage3, CPropertyPage)
	ON_WM_PAINT()
END_MESSAGE_MAP()


// CBackuppage3 消息处理程序


BOOL CBackuppage3::OnSetActive()
{
	// TODO: 在此添加专用代码和/或调用基类
	((CBackupsheet*)GetParent())->SetWizardButtons(PSWIZB_BACK | PSWIZB_FINISH);
	return CPropertyPage::OnSetActive();
}


//BOOL CBackuppage3::OnInitDialog()
//{
//	CPropertyPage::OnInitDialog();
//
//	return TRUE;  // return TRUE unless you set the focus to a control
//
//}


void CBackuppage3::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	m_sourcedisk.Format("PhysicsDrive%d",BackupPartinfo.SourceDisk);
	m_sourcepart.Format("%c盘",BackupPartinfo.strSourceDrive);
	if(BackupPartinfo.InExtend)
	{
		m_parttype.Format("逻辑分区（扩展分区）");
	}
	else
	{
		m_parttype.Format("主分区");
	}
	if(BackupPartinfo.SysFlag)
	{
		m_parttype+="(系统分区)";
	}
	if(BackupPartinfo.AcvtiveFlag==0x80)
	{
		m_parttype+="(活动分区)";
	}
	switch (BackupPartinfo.SystemFlag)
	{
	   case 0x01:
		   m_filesystem.Format("FAT12");
		   break;
	   case 0x04:
		   m_filesystem.Format("FAT16");
		   break;
	   case 0x05:
		   m_filesystem.Format("扩展分区");
		   break;
	   case 0x06:
		   m_filesystem.Format("FAT16");
		   break;
	   case 0x07:
		   m_filesystem.Format("NTFS");
		   break;
	   case 0x0b:
		   m_filesystem.Format("FAT32");
		   break;
	   case 0x0c:
		   m_filesystem.Format("FAT32");
		   break;
	   case 0x0e:
		   m_filesystem.Format("FAT16");
		   break;
	   case 0x0f:
		   m_filesystem.Format("扩展分区");
		   break;
	   default:
		   m_filesystem.Format("未识别");
		   break;
	}
	m_filename=BackupPartinfo.strHdzFileName;
	m_destdrive=BackupPartinfo.strDestDrive;
	if (BackupPartinfo.bCompress)
	{
		m_compress.Format("是");
	}
	else
	{
		m_compress.Format("否");
	}
	if (BackupPartinfo.bCrc)
	{
		m_crc.Format("是");
	}
	else
	{
		m_crc.Format("否");
	}
	if(BackupPartinfo.bEndCheck)
	{
		m_check.Format("是");
	}
	else
	{
		m_check.Format("否");
	}
	if(BackupPartinfo.bEndShutdown)
	{
		m_shutdown.Format("是");
	}
	else
	{
		m_shutdown.Format("否");
	}
	if (BackupPartinfo.bpasswd)
	{
		m_password.Format("是");
	}
	else
	{
		m_password.Format("否");
	}
	switch (BackupPartinfo.SplitType)
	{
	case 0:
		m_split.Format("无限制");
		break;
	case 1:
		m_split.Format("4GB");
		break;
	case 2:
		m_split.Format("2GB");
		break;
	case 3:
		m_split.Format("1.3GB");
		break;
	case 4:
		m_split.Format("700MB");
		break;
	case 5:
		m_split.Format("650MB");
		break;
	case 6:
		m_split.Format("640MB");
		break;
    case 7:
		m_split.Format("230MB");
		break;
	default:
		m_split.Format("无限制");
		break;

	}
	UpdateData(false);
		// TODO: 在此处添加消息处理程序代码
		// 不为绘图消息调用 CPropertyPage::OnPaint()
}
