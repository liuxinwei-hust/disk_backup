// Backupsheet.cpp : 实现文件
//

#include "stdafx.h"
#include "backup_windows.h"
#include "Backupsheet.h"

extern int initdiskinfo();
// CBackupsheet

IMPLEMENT_DYNAMIC(CBackupsheet, CPropertySheet)

CBackupsheet::CBackupsheet(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
{
	m_page1.m_psp.dwFlags &= ~(PSP_HASHELP);
	m_page2.m_psp.dwFlags &= ~(PSP_HASHELP);
	m_page3.m_psp.dwFlags &= ~(PSP_HASHELP);
	initdiskinfo();
	AddPage(&m_page1);
	AddPage(&m_page2);
	AddPage(&m_page3);
	
}

CBackupsheet::CBackupsheet(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(pszCaption, pParentWnd, iSelectPage)
{
	m_page1.m_psp.dwFlags &= ~(PSP_HASHELP);
	m_page2.m_psp.dwFlags &= ~(PSP_HASHELP);
	m_page3.m_psp.dwFlags &= ~(PSP_HASHELP);
	initdiskinfo();
	AddPage(&m_page1);
	AddPage(&m_page2);
	AddPage(&m_page3);
	
}

CBackupsheet::~CBackupsheet()
{
}


BEGIN_MESSAGE_MAP(CBackupsheet, CPropertySheet)
END_MESSAGE_MAP()


// CBackupsheet 消息处理程序


//BOOL CBackupsheet::OnInitDialog()
//{
//	BOOL bResult = CPropertySheet::OnInitDialog();
//
//#5224
//
//	return bResult;
//
//#5224
//
//	return bResult;
//	// TODO:  在此添加您的专用代码
//
//	return bResult;
//	return bResult;
//}


BOOL CBackupsheet::OnInitDialog()
{
	BOOL bResult = CPropertySheet::OnInitDialog();

	ModifyStyleEx(WS_EX_TOOLWINDOW, WS_EX_APPWINDOW);

	return bResult;

}
