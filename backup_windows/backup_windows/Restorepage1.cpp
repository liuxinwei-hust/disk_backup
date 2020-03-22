// Restorepage1.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "backup_windows.h"
#include "Restorepage1.h"
#include "afxdialogex.h"
#include "RestoreSheet.h"
#include "backupinfo.h"
#include "initpartitioninfo.h"

Restore_Mission RestorePart;
Header head;
extern int diskmaxnum;
extern Int13ParaEx diskinfo[PhysicalDriverMAX];
// CRestorepage1 �Ի���

IMPLEMENT_DYNAMIC(CRestorepage1, CPropertyPage)

CRestorepage1::CRestorepage1()
	: CPropertyPage(CRestorepage1::IDD)
	, m_filepath(_T(""))
	, m_password(_T(""))
	, m_bcrc(_T(""))
	, m_bcompress(_T(""))
	, m_packetnum(_T(""))
	, m_filenum(_T(""))
	, m_filesystype(_T(""))
	, m_sectnum(_T(""))
	, m_active(_T(""))
	, m_syspart(_T(""))
	, m_spaceneed(_T(""))
	, m_comment(_T(""))
{
	
}

CRestorepage1::~CRestorepage1()
{
}

void CRestorepage1::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_filepath);
	DDX_Text(pDX, IDC_RESTPASS, m_password);
	DDX_Control(pDX, IDC_EDIT2, m_password2);
	DDX_Text(pDX, IDC_RESTCRC, m_bcrc);
	DDX_Text(pDX, IDC_RESTCOMPRESS, m_bcompress);
	DDX_Text(pDX, IDC_RESTPACKETNUM, m_packetnum);
	DDX_Text(pDX, IDC_RESTFILENUM, m_filenum);
	DDX_Text(pDX, IDC_RESTFILESYS, m_filesystype);
	DDX_Text(pDX, IDC_RESSECTNUM, m_sectnum);
	DDX_Text(pDX, IDC_RESTACTIVE, m_active);
	DDX_Text(pDX, IDC_RESTSYSPAR, m_syspart);
	DDX_Text(pDX, IDC_RESTNEED, m_spaceneed);
	DDX_Text(pDX, IDC_RESTCOMMENT, m_comment);
}


BEGIN_MESSAGE_MAP(CRestorepage1, CPropertyPage)
	ON_BN_CLICKED(IDC_BUTTON1, &CRestorepage1::OnBnClickedButton1)
END_MESSAGE_MAP()


// CRestorepage1 ��Ϣ�������


BOOL CRestorepage1::OnSetActive()
{
	// TODO: �ڴ����ר�ô����/����û���
	((CRestoreSheet*)GetParent())->SetWizardButtons(PSWIZB_NEXT);
	return CPropertyPage::OnSetActive();
}


void CRestorepage1::OnBnClickedButton1()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
  CFileDialog dlg(TRUE);
  if(dlg.DoModal()==IDOK)
  m_filepath=dlg.GetPathName();
  else
  {
	  return;
  }
  FILE *pfile;
  if(fopen_s(&pfile,m_filepath,"rb+"))
  {
	  MessageBox("���ļ�ʧ��","��ʾ",MB_OK);
	  return;
  }
  fread(&head,sizeof(Header),1,pfile);
  fclose(pfile);
  if(head.dig_sign!=0x55AA55AA55AA55AA)
  {
	  UpdateData(false);
	  MessageBox("��ѡ��Ĳ��ǵ�һ�������ļ�,������ѡ��");
	  return;
  }

  m_filenum.Format("%d", head.wDataFileCount);
  m_packetnum.Format("%u", head.packetnum);
  m_sectnum.Format("%u",head.TotalUsedSectors);
  if(head.BootFromHDZ)
  {
	  m_syspart="��";
  }
  else
  {
	   m_syspart="��";
  }
  if(head.CRCInfo)
  {
	  m_bcrc="��";
  }
  else
  {
	  m_bcrc="��";
  }
 
  if(head.bpassword)
  {
	  m_password="��";
	  m_password2.EnableWindow(true);
  }
  else
  {
	  m_password="��";
	  m_password2.EnableWindow(false);
  }
  if(head.bActivePart)
  {
	  m_active="��";
  }
  else
  {
	  m_active="��";
  }
  if(head.bcompress)
  {
	  m_bcompress="��";
  }
  else
  {
	  m_bcompress="��";
  }
  double temp;
  unsigned long long allsector=head.ToatalSize;
  temp=(double)(allsector)*head.SectorSize/1024.0/1024.0/1024.0;
  m_spaceneed.Format("%lld����,\r\n%.2lfGB",allsector,temp);
  m_comment.Format("%s",head.szComment);
  BYTE parfilesys=head.FileSystem;
	   switch (parfilesys)
	   {
	   case 0x01:
		   m_filesystype.Format("FAT12");
		   break;
	   case 0x04:
		   m_filesystype.Format("FAT16");
		   break;
	   case 0x05:
		   m_filesystype.Format("��չ����");
		   break;
	   case 0x06:
		   m_filesystype.Format("FAT16");
		   break;
	   case 0x07:
		   m_filesystype.Format("NTFS");
		   break;
	   case 0x0b:
		   m_filesystype.Format("FAT32");
		   break;
	   case 0x0c:
		   m_filesystype.Format("FAT32");
		   break;
	   case 0x0e:
		   m_filesystype.Format("FAT16");
		   break;
	   case 0x0f:
		   m_filesystype.Format("��չ����");
		   break;
	   default:
		   m_filesystype.Format("δʶ��");
		   break;
	   }

  CString temppath;
  temppath=m_filepath;
  int k=temppath.GetLength()-4;
  temppath.GetBuffer()[k]='\0';
  for(int count=1;count<head.wDataFileCount;count++)
  {
	  CString temp2;
	  temp2.Format("%s_%d.hdz",temppath,count);
	  if (TRUE!=(BOOL)PathFileExists(temp2)) 
      {
		UpdateData(false);
	    MessageBox("ȱ���ļ����޷���ԭ��");
		return;
	  }
  }
  UpdateData(false);
}


BOOL CRestorepage1::OnInitDialog()
{

	CPropertyPage::OnInitDialog();
	m_password2.EnableWindow(false);
	m_password2.SetLimitText(8);
	return TRUE;  // return TRUE unless you set the focus to a control

}


LRESULT CRestorepage1::OnWizardNext()
{
	// TODO: �ڴ����ר�ô����/����û���
	UpdateData(true);

   if(head.dig_sign!=0x55AA55AA55AA55AA)
   {
	  MessageBox("��ѡ��Ĳ��ǵ�һ�������ļ�,������ѡ��");
	  return -1;
   }
   CString pass;
   CString pass2;
   pass2.Format("%s",head.passwd);
   m_password2.GetWindowTextA(pass);
   for(int i=0;i<pass.GetLength();i++)
   {
	   pass.GetBuffer()[i]+=(char)0x80;
   }
   if(pass!=pass2)
   {
	  MessageBox("���벻��ȷ��");
	  return -1;
   }

   CString temppath;
   temppath=m_filepath;
   int k=temppath.GetLength()-4;
   temppath.GetBuffer()[k]='\0';
   for(int count=1;count<head.wDataFileCount;count++)
   {
 	  CString temp2;
	  temp2.Format("%s_%d.hdz",temppath,count);
	  if (TRUE!=(BOOL)PathFileExists(temp2)) 
      {
	    MessageBox("ȱ���ļ����޷���ԭ��");
		return -1;
	  }
   }
   RestorePart.FilePath=m_filepath;
   return CPropertyPage::OnWizardNext();
}
