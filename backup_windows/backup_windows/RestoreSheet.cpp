// RestoreSheet.cpp : 实现文件
//

#include "stdafx.h"
#include "backup_windows.h"
#include "RestoreSheet.h"

extern int initdiskinfo();
// CRestoreSheet

IMPLEMENT_DYNAMIC(CRestoreSheet, CPropertySheet)

CRestoreSheet::CRestoreSheet(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
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

CRestoreSheet::CRestoreSheet(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
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

CRestoreSheet::~CRestoreSheet()
{
}


BEGIN_MESSAGE_MAP(CRestoreSheet, CPropertySheet)
END_MESSAGE_MAP()


// CRestoreSheet 消息处理程序


BOOL CRestoreSheet::OnInitDialog()
{
	BOOL bResult = CPropertySheet::OnInitDialog();

	ModifyStyleEx(WS_EX_TOOLWINDOW, WS_EX_APPWINDOW);

	return bResult;
}
