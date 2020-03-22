// Backuppage2.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "backup_windows.h"
#include "Backuppage2.h"
#include "afxdialogex.h"
#include "Backupsheet.h"
#include "Backupset.h"
#include "backupinfo.h"

extern Backup_Mission BackupPartinfo;
// CBackuppage2 �Ի���

IMPLEMENT_DYNAMIC(CBackuppage2, CPropertyPage)

CBackuppage2::CBackuppage2()
	: CPropertyPage(CBackuppage2::IDD)
	, m_passwd(0)
	, m_path(_T(""))
	, m_filename(_T(""))

{
	
}

CBackuppage2::~CBackuppage2()
{
}

void CBackuppage2::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_CHECK1, m_passwd);
	DDX_Text(pDX, IDC_EDIT2, m_path);
	DDX_Text(pDX, IDC_EDIT1, m_filename);

	DDX_Control(pDX, IDC_EDIT3, m_password1);
	DDX_Control(pDX, IDC_EDIT4, m_password2);
}


BEGIN_MESSAGE_MAP(CBackuppage2, CPropertyPage)
	ON_BN_CLICKED(IDC_BUTTON2, &CBackuppage2::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON1, &CBackuppage2::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_CHECK1, &CBackuppage2::OnBnClickedCheck1)
END_MESSAGE_MAP()


// CBackuppage2 ��Ϣ�������


BOOL CBackuppage2::OnSetActive()
{
	// TODO: �ڴ����ר�ô����/����û���
	((CBackupsheet*)GetParent())->SetWizardButtons(PSWIZB_NEXT|PSWIZB_BACK);
	return CPropertyPage::OnSetActive();
}


void CBackuppage2::OnBnClickedButton2()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
		char szPath[MAX_PATH];     //���ѡ���Ŀ¼·�� 

    ZeroMemory(szPath, sizeof(szPath));   

    BROWSEINFO bi;   
    bi.hwndOwner = m_hWnd;   
    bi.pidlRoot = NULL;   
    bi.pszDisplayName = szPath;   
    bi.lpszTitle = "��ѡ�񱸷��ļ����λ�ã�";   
    bi.ulFlags = BIF_NEWDIALOGSTYLE;   
    bi.lpfn = NULL;   
    bi.lParam = 0;   
    bi.iImage = 0;   
    //����ѡ��Ŀ¼�Ի���
    LPITEMIDLIST lp = SHBrowseForFolder(&bi);   

    if(lp && SHGetPathFromIDList(lp, szPath))   
    {
		UpdateData();
		m_path.Format("%s\\",  szPath);
		UpdateData(false);
    }
}


void CBackuppage2::OnBnClickedButton1()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CBackupset dlg;
	dlg.DoModal();
}


void CBackuppage2::OnBnClickedCheck1()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	UpdateData();
	if(m_passwd==0)
	{
		m_password1.EnableWindow(false);
		m_password2.EnableWindow(false);
	}
	else
	{
		m_password1.EnableWindow(true);
		m_password2.EnableWindow(true);
	}
	UpdateData(false);
}


BOOL CBackuppage2::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

    m_password1.EnableWindow(false);
	m_password2.EnableWindow(false);
	m_password1.SetLimitText(8);
	m_password2.SetLimitText(8);
	BackupPartinfo.bCompress=FALSE;
	BackupPartinfo.bCrc=FALSE;
	BackupPartinfo.SplitType=0;
	BackupPartinfo.Split=0; //������
	BackupPartinfo.bpasswd=FALSE;
	BackupPartinfo.strPassword="";
	BackupPartinfo.bEndCheck=FALSE;
	BackupPartinfo.bEndShutdown=FALSE;

	return TRUE;  // return TRUE unless you set the focus to a control

}


LRESULT CBackuppage2::OnWizardNext()
{
	// TODO: �ڴ����ר�ô����/����û���
	UpdateData();
	if(m_filename=="" )
	{
		MessageBox("�������ļ�����");
		return -1;
	}
	if (m_path=="")
	{
	    MessageBox("��ѡ���ļ��洢·����");
		return -1;
	}
	if (TRUE!=(BOOL)PathFileExists(m_path)) 
	{
	    MessageBox("�ļ��洢·�����Ϸ���");
		return -1;
	}
	char m_path_sym=m_path[0];
	if(m_path_sym==BackupPartinfo.strSourceDrive ||(m_path_sym-32==BackupPartinfo.strSourceDrive)||(m_path_sym+32==BackupPartinfo.strSourceDrive) )
	{
	     MessageBox("�ļ��洢·���뱸��Դ��ͬ��");
		return -1;
	}
	 CString path;
	 path.Format("%c:",m_path_sym);
     ULARGE_INTEGER   uiTotalNumberOfFreeBytes;
	 GetDiskFreeSpaceEx(path,&BackupPartinfo.DestLeft,&BackupPartinfo.DestTotal,&uiTotalNumberOfFreeBytes);
	 if(BackupPartinfo.DestLeft.QuadPart<BackupPartinfo.SourceTotal.QuadPart-BackupPartinfo.SourceLeft.QuadPart)
	 {
	    MessageBox("���̿ռ䲻�㣬������ѡ��洢·����");
		return -1;
	 }
	BackupPartinfo.strDestDrive=m_path;
	BackupPartinfo.strHdzFileName=m_filename;
	if(m_passwd)
	{
		BackupPartinfo.bpasswd=TRUE;
		CString pass1;
		CString pass2;
		m_password1.GetWindowTextA(pass1);
		m_password2.GetWindowTextA(pass2);
		if(pass1=="")
		{
			MessageBox("���벻��Ϊ�գ�");
			return -1;
		}
		if(pass1!=pass2)
		{
			MessageBox("�������벻һ�£����������룡");
			return -1;
		}
		else
		{
			BackupPartinfo.strPassword=pass1;
		}
	}
	else
	{
		BackupPartinfo.bpasswd=FALSE;
		BackupPartinfo.strPassword="";
	}
	UpdateData(false);
	return CPropertyPage::OnWizardNext();
}
