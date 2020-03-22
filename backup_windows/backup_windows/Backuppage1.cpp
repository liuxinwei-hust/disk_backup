// Backuppage1.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "backup_windows.h"
#include "Backuppage1.h"
#include "afxdialogex.h"
#include "Backupsheet.h"
#include "initpartitioninfo.h"
#include "backupinfo.h"

extern int diskmaxnum;
extern Int13ParaEx diskinfo[PhysicalDriverMAX];
extern Backup_Mission BackupPartinfo;
//extern int initdiskinfo();


// CBackuppage1 �Ի���

IMPLEMENT_DYNAMIC(CBackuppage1, CPropertyPage)

CBackuppage1::CBackuppage1()
	: CPropertyPage(CBackuppage1::IDD)
	, m_diskcyl(_T(""))
	, m_diskheads(_T(""))
	, m_disksecpercyl(_T(""))
	, m_disksector(_T(""))
	, m_disksecsize(_T(""))
	, m_midatype(_T(""))
	, m_disktotalsize(_T(""))
	, m_partotal(_T(""))
	, m_parused(_T(""))
	, m_parleft(_T(""))
	, m_parfilesys(_T(""))
	, m_partype(_T(""))
{

}

CBackuppage1::~CBackuppage1()
{
}

void CBackuppage1::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO1, m_diskselect);
	DDX_Text(pDX, IDC_DISKCYL, m_diskcyl);
	DDX_Text(pDX, IDC_DISKHEADS, m_diskheads);
	DDX_Text(pDX, IDC_DISKSECPERCYL, m_disksecpercyl);
	DDX_Text(pDX, IDC_DISKSECTOR, m_disksector);
	DDX_Text(pDX, IDC_DISKSECTORSIZE, m_disksecsize);
	//DDX_Text(pDX, IDC_LEFTSIZE, m_diskleftsize);
	DDX_Text(pDX, IDC_MEDIATYPE, m_midatype);
	DDX_Text(pDX, IDC_TOTALSIZE, m_disktotalsize);
	//DDX_Text(pDX, IDC_USEDSIZE, m_diskusedsize);
	DDX_Control(pDX, IDC_COMBO2, m_partitionsel);
	DDX_Text(pDX, IDC_PARTOTAL, m_partotal);
	DDX_Text(pDX, IDC_PARUSED, m_parused);
	DDX_Text(pDX, IDC_PARLEFT, m_parleft);
	DDX_Text(pDX, IDC_PARFILESYS, m_parfilesys);
	DDX_Text(pDX, IDC_PARTYPE, m_partype);
}


BEGIN_MESSAGE_MAP(CBackuppage1, CPropertyPage)
//	ON_CBN_DROPDOWN(IDC_COMBO1, &CBackuppage1::OnCbnDropdownCombo1)
	ON_CBN_SELCHANGE(IDC_COMBO1, &CBackuppage1::OnCbnSelchangeCombo1)
	ON_CBN_SELCHANGE(IDC_COMBO2, &CBackuppage1::OnCbnSelchangeCombo2)
END_MESSAGE_MAP()


// CBackuppage1 ��Ϣ�������


BOOL CBackuppage1::OnSetActive()
{
	// TODO: �ڴ����ר�ô����/����û���
	((CBackupsheet*)GetParent())->SetWizardButtons(PSWIZB_NEXT);
	return CPropertyPage::OnSetActive();
}


BOOL CBackuppage1::OnInitDialog()
{
	CPropertyPage::OnInitDialog();
	for(int i=0;i<diskmaxnum;i++)
	{
		CString temp;
		temp.Format("PhysicalDrive%d",diskinfo[i].Flags);
		m_diskselect.AddString(temp);
	}
	double total;
	total= diskinfo[0].SectorSize*diskinfo[0].Sectors/1024.0/1024.0/1024.0;
	m_diskselect.SetCurSel(0);
	m_diskcyl.Format("%lu",diskinfo[0].Cylinders);
	m_diskheads.Format("%lu",diskinfo[0].Heads);
	m_disksecpercyl.Format("%lu",diskinfo[0].SecPerTrack);
	m_disksecsize.Format("%lu",diskinfo[0].SectorSize);
	m_disksector.Format("%llu",diskinfo[0].Sectors);
	int typetem=diskinfo[0].mediatype;
	switch (typetem)
	{
	case 0:
		m_midatype.Format("δʶ��");
		break;
	case 11:
		m_midatype.Format("���ƶ��豸");
		break;
	case 12:
		m_midatype.Format("Ӳ��");
		break;
	default:
		m_midatype.Format("%d",typetem);
		break;
	}
	m_disktotalsize.Format("%.2fGB",total);

		for(int i=0;i<diskinfo[0].partitionnum;i++)
	   {	
		   if (diskinfo[0].partition[i].LogicDriveSymbol!='0')
           {
		      CString temp;
		      temp.Format("%c:",diskinfo[0].partition[i].LogicDriveSymbol);
		      m_partitionsel.AddString(temp);
		   }
		   else
		   {
			   m_partitionsel.AddString("���ط���");
		   }
	   }
	   m_partitionsel.SetCurSel(0);
	   CString path;
	   CString partname;
	   ULARGE_INTEGER   uiFreeBytesAvailableToCaller; 
       ULARGE_INTEGER   uiTotalNumberOfBytes; 
       ULARGE_INTEGER   uiTotalNumberOfFreeBytes;
	   if(diskinfo[0].partition[0].LogicDriveSymbol=='0')
	   {
		   partname.Format("\\Device\\HarddiskVolume1");
		   DefineDosDevice(DDD_RAW_TARGET_PATH, "]:", partname);
		   path.Format("]:");
		   GetDiskFreeSpaceEx(path,&uiFreeBytesAvailableToCaller,&uiTotalNumberOfBytes,&uiTotalNumberOfFreeBytes);
	       DefineDosDevice ( 
            DDD_RAW_TARGET_PATH|DDD_REMOVE_DEFINITION| 
            DDD_EXACT_MATCH_ON_REMOVE, "]:", 
            partname);
	   }
	   else
	   {
		   path.Format("%c:",diskinfo[0].partition[0].LogicDriveSymbol);
		   GetDiskFreeSpaceEx(path,&uiFreeBytesAvailableToCaller,&uiTotalNumberOfBytes,&uiTotalNumberOfFreeBytes);
	   } 


	   double all=(double)uiTotalNumberOfBytes.QuadPart/1024/1024/1024;
	   double left=(double)uiFreeBytesAvailableToCaller.QuadPart/1024/1024/1024;
	   double used=all-left;
	   m_partotal.Format("%.2fGB",all);
	   m_parused.Format("%.2fGB",used);
	   m_parleft.Format("%.2fGB",left);
	   BYTE parfilesys=diskinfo[0].partition[0].SystemFlag;
	   switch (parfilesys)
	   {
	   case 0x01:
		   m_parfilesys.Format("FAT12");
		   break;
	   case 0x04:
		   m_parfilesys.Format("FAT16");
		   break;
	   case 0x05:
		   m_parfilesys.Format("��չ����");
		   break;
	   case 0x06:
		   m_parfilesys.Format("FAT16");
		   break;
	   case 0x07:
		   m_parfilesys.Format("NTFS");
		   break;
	   case 0x0b:
		   m_parfilesys.Format("FAT32");
		   break;
	   case 0x0c:
		   m_parfilesys.Format("FAT32");
		   break;
	   case 0x0e:
		   m_parfilesys.Format("FAT16");
		   break;
	   case 0x0f:
		   m_parfilesys.Format("��չ����");
		   break;
	   default:
		   m_parfilesys.Format("δʶ��");
		   break;
	   }
	   if(diskinfo[0].partition[0].InExtend)
	   {
		   m_partype.Format("�߼���������չ������");
	   }
	   else
	   {
		   m_partype.Format("������");
		   if(diskinfo[0].partition[0].ActiveFlag==0x80)
		   {
			   m_partype+="�����";
		   }
	   }
	char winpath[MAX_PATH];
	GetWindowsDirectory(winpath,MAX_PATH);
	if(winpath[0]==diskinfo[0].partition[0].LogicDriveSymbol)
	{
		 m_partype+="��ϵͳ��";
	}
	

	UpdateData(false);
	return TRUE;  // return TRUE unless you set the focus to a control

}




void CBackuppage1::OnCbnSelchangeCombo1()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	UpdateData();
	int i=m_diskselect.GetCurSel();
	double total;
	total= diskinfo[i].SectorSize*diskinfo[i].Sectors/1024.0/1024.0/1024.0;
	m_diskcyl.Format("%lu",diskinfo[i].Cylinders);
	m_diskheads.Format("%lu",diskinfo[i].Heads);
	m_disksecpercyl.Format("%lu",diskinfo[i].SecPerTrack);
	m_disksecsize.Format("%lu",diskinfo[i].SectorSize);
	m_disksector.Format("%llu",diskinfo[i].Sectors);
	m_disktotalsize.Format("%.2fGB",total);
	int typetem=diskinfo[i].mediatype;
	switch (typetem)
	{
	case 0:
		m_midatype.Format("δʶ��");
		break;
	case 11:
		m_midatype.Format("���ƶ��豸");
		break;
	case 12:
		m_midatype.Format("Ӳ��");
		break;
	default:
		m_midatype.Format("%d",typetem);
		break;
	}

	m_partitionsel.ResetContent();


		for(int num=0;num<diskinfo[i].partitionnum;num++)
	    {
		   if (diskinfo[i].partition[num].LogicDriveSymbol!='0')
	       {
		      CString temp;
		      temp.Format("%c:",diskinfo[i].partition[num].LogicDriveSymbol);
		      m_partitionsel.AddString(temp);
		   }
		   else
		   {
			   m_partitionsel.AddString("���ط���");
		   }
	    }
	   m_partitionsel.SetCurSel(0);
	   CString path;
	   CString partname;
	   ULARGE_INTEGER   uiFreeBytesAvailableToCaller; 
       ULARGE_INTEGER   uiTotalNumberOfBytes; 
       ULARGE_INTEGER   uiTotalNumberOfFreeBytes;
	   if(diskinfo[i].partition[0].LogicDriveSymbol=='0')
	   {
		    int volumid=1;
	        for(int count1=0;count1<i;count1++)
	        {
		       volumid+=diskinfo[count1].partitionnum;
	        }
		 	partname.Format("\\Device\\HarddiskVolume%d",volumid);
		    DefineDosDevice(DDD_RAW_TARGET_PATH, "]:", partname);
		    path.Format("]:");
		    GetDiskFreeSpaceEx(path,&uiFreeBytesAvailableToCaller,&uiTotalNumberOfBytes,&uiTotalNumberOfFreeBytes);
	        DefineDosDevice ( 
            DDD_RAW_TARGET_PATH|DDD_REMOVE_DEFINITION| 
            DDD_EXACT_MATCH_ON_REMOVE, "]:", 
            partname);
	   }
	   else
	   {
		   path.Format("%c:",diskinfo[i].partition[0].LogicDriveSymbol);
		   GetDiskFreeSpaceEx(path,&uiFreeBytesAvailableToCaller,&uiTotalNumberOfBytes,&uiTotalNumberOfFreeBytes);
	   } 
	   double all=(double)uiTotalNumberOfBytes.QuadPart/1024/1024/1024;
	   double left=(double)uiFreeBytesAvailableToCaller.QuadPart/1024/1024/1024;
	   double used=all-left;
	   m_partotal.Format("%.2fGB",all);
	   m_parused.Format("%.2fGB",used);
	   m_parleft.Format("%.2fGB",left);
	   BYTE parfilesys=diskinfo[i].partition[0].SystemFlag;
	   switch (parfilesys)
	   {
	   case 0x01:
		   m_parfilesys.Format("FAT12");
		   break;
	   case 0x04:
		   m_parfilesys.Format("FAT16");
		   break;
	   case 0x05:
		   m_parfilesys.Format("��չ����");
		   break;
	   case 0x06:
		   m_parfilesys.Format("FAT16");
		   break;
	   case 0x07:
		   m_parfilesys.Format("NTFS");
		   break;
	   case 0x0b:
		   m_parfilesys.Format("FAT32");
		   break;
	   case 0x0c:
		   m_parfilesys.Format("FAT32");
		   break;
	   case 0x0e:
		   m_parfilesys.Format("FAT16");
		   break;
	   case 0x0f:
		   m_parfilesys.Format("��չ����");
		   break;
	   default:
		   m_parfilesys.Format("δʶ��");
		   break;
	   }
	   if(diskinfo[i].partition[0].InExtend)
	   {
		   m_partype.Format("�߼���������չ������");
	   }
	   else
	   {
		   m_partype.Format("������");
		   if(diskinfo[i].partition[0].ActiveFlag==0x80)
		   {
			   m_partype+="�����";
		   }
	   }
	  char winpath[MAX_PATH];
	  GetWindowsDirectory(winpath,MAX_PATH);
	  if(winpath[0]==diskinfo[i].partition[0].LogicDriveSymbol)
	  {
		 m_partype+="��ϵͳ��";
	  }

	UpdateData(false);
}


void CBackuppage1::OnCbnSelchangeCombo2()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
    UpdateData();
	int i=m_diskselect.GetCurSel();
	int j=m_partitionsel.GetCurSel();

	   CString path;
	   CString partname;
	   ULARGE_INTEGER   uiFreeBytesAvailableToCaller; 
       ULARGE_INTEGER   uiTotalNumberOfBytes; 
       ULARGE_INTEGER   uiTotalNumberOfFreeBytes;
	   if(diskinfo[i].partition[j].LogicDriveSymbol=='0')
	   {
		    int volumid=j+1;
	        for(int count1=0;count1<i;count1++)
	        {
		       volumid+=diskinfo[count1].partitionnum;
	        }
			partname.Format("\\Device\\HarddiskVolume%d",volumid);
		    DefineDosDevice(DDD_RAW_TARGET_PATH, "]:", partname);
		    path.Format("]:");
			GetDiskFreeSpaceEx(path,&uiFreeBytesAvailableToCaller,&uiTotalNumberOfBytes,&uiTotalNumberOfFreeBytes);
	        DefineDosDevice ( 
            DDD_RAW_TARGET_PATH|DDD_REMOVE_DEFINITION| 
            DDD_EXACT_MATCH_ON_REMOVE, "]:", 
            partname);
	   }
	   else
	   {
		   path.Format("%c:",diskinfo[i].partition[j].LogicDriveSymbol);
		   GetDiskFreeSpaceEx(path,&uiFreeBytesAvailableToCaller,&uiTotalNumberOfBytes,&uiTotalNumberOfFreeBytes);
	   } 	   
	   double all=(double)uiTotalNumberOfBytes.QuadPart/1024/1024/1024;
	   double left=(double)uiFreeBytesAvailableToCaller.QuadPart/1024/1024/1024;
	   double used=all-left;
	   m_partotal.Format("%.2fGB",all);
	   m_parused.Format("%.2fGB",used);
	   m_parleft.Format("%.2fGB",left);
	   BYTE parfilesys=diskinfo[i].partition[j].SystemFlag;
	   switch (parfilesys)
	   {
	   case 0x01:
		   m_parfilesys.Format("FAT12");
		   break;
	   case 0x04:
		   m_parfilesys.Format("FAT16");
		   break;
	   case 0x05:
		   m_parfilesys.Format("��չ����");
		   break;
	   case 0x06:
		   m_parfilesys.Format("FAT16");
		   break;
	   case 0x07:
		   m_parfilesys.Format("NTFS");
		   break;
	   case 0x0b:
		   m_parfilesys.Format("FAT32");
		   break;
	   case 0x0c:
		   m_parfilesys.Format("FAT32");
		   break;
	   case 0x0e:
		   m_parfilesys.Format("FAT16");
		   break;
	   case 0x0f:
		   m_parfilesys.Format("��չ����");
		   break;
	   default:
		   m_parfilesys.Format("δʶ��");
		   break;
	   }
	   if(diskinfo[i].partition[j].InExtend)
	   {
		   m_partype.Format("�߼���������չ������");
	   }
	   else
	   {
		   m_partype.Format("������");
		   if(diskinfo[i].partition[j].ActiveFlag==0x80)
		   {
			   m_partype+="�����";
		   }

	   }
	   char winpath[MAX_PATH];
	   GetWindowsDirectory(winpath,MAX_PATH);
	  if(winpath[0]==diskinfo[i].partition[j].LogicDriveSymbol)
	  {
		 m_partype+="��ϵͳ��";
	  }
	  UpdateData(false);
}


LRESULT CBackuppage1::OnWizardNext()
{
	// TODO: �ڴ����ר�ô����/����û���
    int i=m_diskselect.GetCurSel();
	int j=m_partitionsel.GetCurSel();
	BackupPartinfo.SourceDisk=diskinfo[i].Flags;
	BackupPartinfo.strSourceDrive=diskinfo[i].partition[j].LogicDriveSymbol;
	BackupPartinfo.partID=j+1;
	for(int count=0;count<i;count++)
	{
		BackupPartinfo.partID+=diskinfo[count].partitionnum;
	}
	BackupPartinfo.AcvtiveFlag=diskinfo[i].partition[j].ActiveFlag;
	BackupPartinfo.StartLogicSecNo=diskinfo[i].partition[j].StartLogicSecNo;
	BackupPartinfo.SectorNum=diskinfo[i].partition[j].SectorNum;
	BackupPartinfo.PartitionSec=diskinfo[i].partition[j].PartitionSec;
	BackupPartinfo.InExtend=diskinfo[i].partition[j].InExtend;
	BackupPartinfo.SectorSize=diskinfo[i].SectorSize;
	BackupPartinfo.SystemFlag=diskinfo[i].partition[j].SystemFlag;
	memcpy(BackupPartinfo.SectorBuffer,diskinfo[i].SectorBuffer,514);
	char winpath[MAX_PATH];
	GetWindowsDirectory(winpath,MAX_PATH);
	if(winpath[0]==BackupPartinfo.strSourceDrive)
	{
		BackupPartinfo.SysFlag=TRUE;
		if(BackupPartinfo.AcvtiveFlag!=0x80)
		{
			for(int kk=0;kk<diskinfo[i].partitionnum;kk++)
			{
				if (diskinfo[i].partition[kk].ActiveFlag==0x80)
				{
					BackupPartinfo.ActPartSymbol=diskinfo[i].partition[kk].LogicDriveSymbol;
					BackupPartinfo.ActStartLogicSecNo=diskinfo[i].partition[kk].StartLogicSecNo;
					BackupPartinfo.ActSectorNum=diskinfo[i].partition[kk].SectorNum;
					break;
				}
			}
		}
		else
		{
				BackupPartinfo.ActPartSymbol='0';
				BackupPartinfo.ActStartLogicSecNo=0;
				BackupPartinfo.ActSectorNum=0;
		}
	}
	else
	{
		BackupPartinfo.SysFlag=FALSE;
		BackupPartinfo.ActPartSymbol='0';
		BackupPartinfo.ActStartLogicSecNo=0;
		BackupPartinfo.ActSectorNum=0;
	}

	 CString path;
	 path.Format("%c:",BackupPartinfo.strSourceDrive);
     ULARGE_INTEGER   uiTotalNumberOfFreeBytes;
	 GetDiskFreeSpaceEx(path,&BackupPartinfo.SourceLeft,&BackupPartinfo.SourceTotal,&uiTotalNumberOfFreeBytes);

	return CPropertyPage::OnWizardNext();
}
