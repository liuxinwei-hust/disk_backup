// Restorepage3.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "backup_windows.h"
#include "Restorepage3.h"
#include "afxdialogex.h"
#include "RestoreSheet.h"
#include "backupinfo.h"

extern Restore_Mission RestorePart;
extern Header head;
// CRestorepage3 �Ի���


IMPLEMENT_DYNAMIC(CRestorepage3, CPropertyPage)

CRestorepage3::CRestorepage3()
	: CPropertyPage(CRestorepage3::IDD)
	, m_filename(_T(""))
	, m_partition(_T(""))
	, m_comment2(_T(""))
{

}

CRestorepage3::~CRestorepage3()
{
}

void CRestorepage3::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_PAGE3FILE, m_filename);
	DDX_Text(pDX, IDC_PAGE3PART, m_partition);
	DDX_Control(pDX, IDC_CHECK1, m_shutdown);
	//  DDX_Control(pDX, IDC_CHECK2, m_bpb);
	DDX_Control(pDX, IDC_CHECK3, m_active);
	//  DDX_Text(pDX, IDC_PAGE3COMM1, m_comment1);
	DDX_Text(pDX, IDC_PAGE3COMM2, m_comment2);
	DDX_Control(pDX, IDC_CHECK2, m_restcheck);
}


BEGIN_MESSAGE_MAP(CRestorepage3, CPropertyPage)
	ON_WM_PAINT()
END_MESSAGE_MAP()


// CRestorepage3 ��Ϣ�������


BOOL CRestorepage3::OnSetActive()
{
	// TODO: �ڴ����ר�ô����/����û���
	((CRestoreSheet*)GetParent())->SetWizardButtons(PSWIZB_BACK | PSWIZB_FINISH);
	return CPropertyPage::OnSetActive();
}


void CRestorepage3::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	m_comment2="";
	m_filename=RestorePart.FilePath;
	m_partition=RestorePart.strSourceDrive;
	m_partition+=":\\��";
	if(RestorePart.AcvtiveFlag)
	{
		m_active.SetCheck(0);
		m_active.EnableWindow(false);
		m_comment2+="(��ԭ�������ǻ����)";
	}
	else if(RestorePart.InExtend)
	{
		m_active.SetCheck(0);
		m_active.EnableWindow(false);
		m_comment2+="(��ԭ����Ϊ��չ����)";
	}

	else
	{
		m_active.EnableWindow(true);
		m_comment2="";
	}
	if(head.CRCInfo)
	{
		m_restcheck.EnableWindow(true);
	}
	else
	{
		m_restcheck.SetCheck(0);
		m_restcheck.EnableWindow(false);
	}
	UpdateData(false);
		// TODO: �ڴ˴������Ϣ����������
		// ��Ϊ��ͼ��Ϣ���� CPropertyPage::OnPaint()
}


BOOL CRestorepage3::OnWizardFinish()
{
	// TODO: �ڴ����ר�ô����/����û���
	RestorePart.bEndShutdown=m_shutdown.GetCheck();
	RestorePart.SetActive=m_active.GetCheck();
	RestorePart.RestCheck=m_restcheck.GetCheck();
	return CPropertyPage::OnWizardFinish();
}
