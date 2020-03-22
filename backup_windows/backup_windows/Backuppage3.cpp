// Backuppage3.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "backup_windows.h"
#include "Backuppage3.h"
#include "afxdialogex.h"
#include "Backupsheet.h"
#include "backupinfo.h"

extern Backup_Mission BackupPartinfo;
// CBackuppage3 �Ի���

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


// CBackuppage3 ��Ϣ�������


BOOL CBackuppage3::OnSetActive()
{
	// TODO: �ڴ����ר�ô����/����û���
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
	m_sourcepart.Format("%c��",BackupPartinfo.strSourceDrive);
	if(BackupPartinfo.InExtend)
	{
		m_parttype.Format("�߼���������չ������");
	}
	else
	{
		m_parttype.Format("������");
	}
	if(BackupPartinfo.SysFlag)
	{
		m_parttype+="(ϵͳ����)";
	}
	if(BackupPartinfo.AcvtiveFlag==0x80)
	{
		m_parttype+="(�����)";
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
		   m_filesystem.Format("��չ����");
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
		   m_filesystem.Format("��չ����");
		   break;
	   default:
		   m_filesystem.Format("δʶ��");
		   break;
	}
	m_filename=BackupPartinfo.strHdzFileName;
	m_destdrive=BackupPartinfo.strDestDrive;
	if (BackupPartinfo.bCompress)
	{
		m_compress.Format("��");
	}
	else
	{
		m_compress.Format("��");
	}
	if (BackupPartinfo.bCrc)
	{
		m_crc.Format("��");
	}
	else
	{
		m_crc.Format("��");
	}
	if(BackupPartinfo.bEndCheck)
	{
		m_check.Format("��");
	}
	else
	{
		m_check.Format("��");
	}
	if(BackupPartinfo.bEndShutdown)
	{
		m_shutdown.Format("��");
	}
	else
	{
		m_shutdown.Format("��");
	}
	if (BackupPartinfo.bpasswd)
	{
		m_password.Format("��");
	}
	else
	{
		m_password.Format("��");
	}
	switch (BackupPartinfo.SplitType)
	{
	case 0:
		m_split.Format("������");
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
		m_split.Format("������");
		break;

	}
	UpdateData(false);
		// TODO: �ڴ˴������Ϣ����������
		// ��Ϊ��ͼ��Ϣ���� CPropertyPage::OnPaint()
}
