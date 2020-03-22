// Restorepage2.cpp : 实现文件
//

#include "stdafx.h"
#include "backup_windows.h"
#include "Restorepage2.h"
#include "afxdialogex.h"
#include "RestoreSheet.h"
#include "backupinfo.h"
#include "initpartitioninfo.h"

extern Restore_Mission RestorePart;
extern int diskmaxnum;
extern Int13ParaEx diskinfo[PhysicalDriverMAX];
extern Header head;
// CRestorepage2 对话框

IMPLEMENT_DYNAMIC(CRestorepage2, CPropertyPage)

CRestorepage2::CRestorepage2()
	: CPropertyPage(CRestorepage2::IDD)
	, m_diskheads(_T(""))
	, m_diskcyl(_T(""))
	, m_disksector(_T(""))
	, m_disksecpercyl(_T(""))
	, m_disksecsize(_T(""))
	, m_midatype(_T(""))
	, m_disktotalsize(_T(""))
	, m_partotal(_T(""))
	, m_parfilesys(_T(""))
	, m_parused(_T(""))
	, m_parleft(_T(""))
	, m_partype(_T(""))
	, m_setwindowsys(FALSE)
{

}

CRestorepage2::~CRestorepage2()
{
}

void CRestorepage2::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_RESTCOMBO1, m_diskselect);
	DDX_Control(pDX, IDC_RESTCOMBO2, m_partitionsel);
	DDX_Text(pDX, IDC_RESTDISKHEADS, m_diskheads);
	DDX_Text(pDX, IDC_RESTDISKCYL, m_diskcyl);
	DDX_Text(pDX, IDC_RESTDISKSECTOR, m_disksector);
	DDX_Text(pDX, IDC_RESTDISKSECPERCYL, m_disksecpercyl);
	DDX_Text(pDX, IDC_RESTDISKSECTORSIZE, m_disksecsize);
	DDX_Text(pDX, IDC_RESTMEDIATYPE, m_midatype);
	DDX_Text(pDX, IDC_RESTTOTALSIZE, m_disktotalsize);
	DDX_Text(pDX, IDC_RESTPARTOTAL, m_partotal);
	DDX_Text(pDX, IDC_RESTPARFILESYS, m_parfilesys);
	DDX_Text(pDX, IDC_RESTPARUSED, m_parused);
	DDX_Text(pDX, IDC_RESTLEFE, m_parleft);
	DDX_Text(pDX, IDC_RESTPARTYPE, m_partype);
	DDX_Check(pDX, IDC_CHECK1, m_setwindowsys);
}


BEGIN_MESSAGE_MAP(CRestorepage2, CPropertyPage)
	ON_CBN_SELCHANGE(IDC_RESTCOMBO1, &CRestorepage2::OnCbnSelchangeRestcombo1)
	ON_CBN_SELCHANGE(IDC_RESTCOMBO2, &CRestorepage2::OnCbnSelchangeRestcombo2)
	ON_BN_CLICKED(IDC_CHECK1, &CRestorepage2::OnBnClickedCheck1)
END_MESSAGE_MAP()


// CRestorepage2 消息处理程序


BOOL CRestorepage2::OnSetActive()
{
	// TODO: 在此添加专用代码和/或调用基类
	((CRestoreSheet*)GetParent())->SetWizardButtons(PSWIZB_NEXT|PSWIZB_BACK);
	return CPropertyPage::OnSetActive();
}


BOOL CRestorepage2::OnInitDialog()
{
	CPropertyPage::OnInitDialog();
	m_setwindowsys=FALSE;
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
		m_midatype.Format("未识别");
		break;
	case 11:
		m_midatype.Format("可移动设备");
		break;
	case 12:
		m_midatype.Format("硬盘");
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
			   m_partitionsel.AddString("隐藏分区");
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
		   m_parfilesys.Format("扩展分区");
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
		   m_parfilesys.Format("扩展分区");
		   break;
	   default:
		   m_parfilesys.Format("未识别");
		   break;
	   }
	   if(diskinfo[0].partition[0].InExtend)
	   {
		   m_partype.Format("逻辑分区（扩展分区）");
	   }
	   else
	   {
		   m_partype.Format("主分区");
		   if(diskinfo[0].partition[0].ActiveFlag==0x80)
		   {
			   m_partype+="（活动）";
		   }
	   }
	char winpath[MAX_PATH];
	GetWindowsDirectory(winpath,MAX_PATH);
	if(winpath[0]==diskinfo[0].partition[0].LogicDriveSymbol)
	{
		 m_partype+="（系统）";
	}
	

	UpdateData(false);

	return TRUE;  // return TRUE unless you set the focus to a control

	
}


void CRestorepage2::OnCbnSelchangeRestcombo1()
{
	// TODO: 在此添加控件通知处理程序代码
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
		m_midatype.Format("未识别");
		break;
	case 11:
		m_midatype.Format("可移动设备");
		break;
	case 12:
		m_midatype.Format("硬盘");
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
			   m_partitionsel.AddString("隐藏分区");
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
		   m_parfilesys.Format("扩展分区");
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
		   m_parfilesys.Format("扩展分区");
		   break;
	   default:
		   m_parfilesys.Format("未识别");
		   break;
	   }
	   if(diskinfo[i].partition[0].InExtend)
	   {
		   m_partype.Format("逻辑分区（扩展分区）");
	   }
	   else
	   {
		   m_partype.Format("主分区");
		   if(diskinfo[i].partition[0].ActiveFlag==0x80)
		   {
			   m_partype+="（活动）";
		   }
	   }
	  char winpath[MAX_PATH];
	  GetWindowsDirectory(winpath,MAX_PATH);
	  if(winpath[0]==diskinfo[i].partition[0].LogicDriveSymbol)
	  {
		 m_partype+="（系统）";
	  }

	UpdateData(false);
}


void CRestorepage2::OnCbnSelchangeRestcombo2()
{
	// TODO: 在此添加控件通知处理程序代码
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
		   m_parfilesys.Format("扩展分区");
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
		   m_parfilesys.Format("扩展分区");
		   break;
	   default:
		   m_parfilesys.Format("未识别");
		   break;
	   }
	   if(diskinfo[i].partition[j].InExtend)
	   {
		   m_partype.Format("逻辑分区（扩展分区）");
	   }
	   else
	   {
		   m_partype.Format("主分区");
		   if(diskinfo[i].partition[j].ActiveFlag==0x80)
		   {
			   m_partype+="（活动）";
		   }

	   }
	   char winpath[MAX_PATH];
	   GetWindowsDirectory(winpath,MAX_PATH);
	  if(winpath[0]==diskinfo[i].partition[j].LogicDriveSymbol)
	  {
		 m_partype+="（系统）";
	  }
	  UpdateData(false);
}



LRESULT CRestorepage2::OnWizardNext()
{
	// TODO: 在此添加专用代码和/或调用基类
	int i=m_diskselect.GetCurSel();
	int j=m_partitionsel.GetCurSel();
	if(diskinfo[i].partition[j].SectorNum<(head.LasPackStar+head.LastSector))
	{
		MessageBox("磁盘空间不足!");
		return -1;
	}
	if(diskinfo[i].partition[j].SystemFlag!=head.FileSystem)
	{
		if(IDNO== MessageBox("文件系统类型不匹配，但仍可还原,\r\n\t\t确认继续？","警告",MB_YESNO))
		{
			return -1;
		}	
	}
	RestorePart.SourceDisk=diskinfo[i].Flags;
	RestorePart.strSourceDrive=diskinfo[i].partition[j].LogicDriveSymbol;
	RestorePart.partID=j+1;
	for(int count=0;count<i;count++)
	{
		RestorePart.partID+=diskinfo[count].partitionnum;
	}
	RestorePart.AcvtiveFlag=diskinfo[i].partition[j].ActiveFlag;
	RestorePart.StartLogicSecNo=diskinfo[i].partition[j].StartLogicSecNo;
	RestorePart.SectorNum=diskinfo[i].partition[j].SectorNum;
	RestorePart.PartitionSec=diskinfo[i].partition[j].PartitionSec;
	RestorePart.InExtend=diskinfo[i].partition[j].InExtend;
	RestorePart.SectorSize=diskinfo[i].SectorSize;
	RestorePart.SystemFlag=diskinfo[i].partition[j].SystemFlag;
	RestorePart.SetWindSys=m_setwindowsys;
	memcpy(RestorePart.SectorBuffer,diskinfo[i].SectorBuffer,514);
	char winpath[MAX_PATH];
	GetWindowsDirectory(winpath,MAX_PATH);
	if(winpath[0]==RestorePart.strSourceDrive)
	{
		RestorePart.SysFlag=TRUE;
		if(RestorePart.AcvtiveFlag!=0x80)
		{
			for(int kk=0;kk<diskinfo[i].partitionnum;kk++)
			{
				if (diskinfo[i].partition[kk].ActiveFlag==0x80)
				{
					RestorePart.ActPartSymbol=diskinfo[i].partition[kk].LogicDriveSymbol;
					RestorePart.ActStartLogicSecNo=diskinfo[i].partition[kk].StartLogicSecNo;
					RestorePart.ActSectorNum=diskinfo[i].partition[kk].SectorNum;
					break;
				}
			}
		}
		else
		{
				RestorePart.ActPartSymbol='0';
				RestorePart.ActStartLogicSecNo=0;
				RestorePart.ActSectorNum=0;
		}
	}
	else
	{
		RestorePart.SysFlag=FALSE;
		RestorePart.ActPartSymbol='0';
		RestorePart.ActStartLogicSecNo=0;
		RestorePart.ActSectorNum=0;
	}

	 CString path;
	 path.Format("%c:",RestorePart.strSourceDrive);
     ULARGE_INTEGER   uiTotalNumberOfFreeBytes;
	 GetDiskFreeSpaceEx(path,&RestorePart.SourceLeft,&RestorePart.SourceTotal,&uiTotalNumberOfFreeBytes);
	 if(RestorePart.SysFlag==TRUE)
	 {
		MessageBox("当前分区为系统分区，正在被使用，不能对其还原!");
		return -1;
	 }
	return CPropertyPage::OnWizardNext();
}


void CRestorepage2::OnBnClickedCheck1()
{
	UpdateData();
	if(m_setwindowsys)
	{
		int i=m_diskselect.GetCurSel();
	    int j=m_partitionsel.GetCurSel();
		if((j==0)&&((diskinfo[i].partition[j].SectorNum+diskinfo[i].partition[j].StartLogicSecNo)>=(head.PartStartPos+head.PartToSec)))
		{
			m_diskselect.EnableWindow(false);
			m_partitionsel.EnableWindow(false);
		}
		else if((diskinfo[i].partition[j].StartLogicSecNo<=head.PartStartPos)&&((diskinfo[i].partition[j].SectorNum+diskinfo[i].partition[j].StartLogicSecNo)>=(head.PartStartPos+head.PartToSec)))
		{
			m_diskselect.EnableWindow(false);
			m_partitionsel.EnableWindow(false);
		}
		else
		{
			CString temp="此分区不合适，";
			CString temp2="";
			m_setwindowsys=FALSE;
			j=0;
			for(;j<diskinfo[i].partitionnum;j++)
			{
				if((j==0)&&((diskinfo[i].partition[j].SectorNum+diskinfo[i].partition[j].StartLogicSecNo)>=(head.PartStartPos+head.PartToSec)))
		        {
					break;
		        }
		        else if((diskinfo[i].partition[j].StartLogicSecNo<=head.PartStartPos)&&((diskinfo[i].partition[j].SectorNum+diskinfo[i].partition[j].StartLogicSecNo)>=(head.PartStartPos+head.PartToSec)))
		        {
					break;
		        }
			}
			if(j!=diskinfo[i].partitionnum)
			{
				temp2.Format("此硬盘只有%c区适合\r\n是否将其设为系统分区？",diskinfo[i].partition[j].LogicDriveSymbol);
				temp+=temp2;
				if(IDYES==MessageBox(temp,"提示",MB_YESNO))
				{
					  m_setwindowsys=TRUE;
					  m_partitionsel.SetCurSel(j);
					  m_diskselect.EnableWindow(false);
			          m_partitionsel.EnableWindow(false);
				}
			}
			else
			{
				double temp3;
				double temp4;
				temp4=(double)(head.PartStartPos*head.SectorSize)/1024.0/1024.0/1024.0;
				temp3=(double)(head.PartToSec)*head.SectorSize/1024.0/1024.0/1024.0;
				temp2.Format("此硬盘上没有合适的分区！\r\n硬盘%.2lfGB处需%.2lfGB空间",temp4,temp3);
				temp+=temp2;
				MessageBox(temp);
			}
		}
	}
	else
	{
		m_diskselect.EnableWindow(true);
		m_partitionsel.EnableWindow(true);
	}
	UpdateData(false);
}
