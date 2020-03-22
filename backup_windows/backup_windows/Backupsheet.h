#pragma once
#include "Backuppage1.h"
#include "Backuppage2.h"
#include "Backuppage3.h"


// CBackupsheet

class CBackupsheet : public CPropertySheet
{
	DECLARE_DYNAMIC(CBackupsheet)

public:
	CBackupsheet(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	CBackupsheet(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	virtual ~CBackupsheet();
	CBackuppage1 m_page1;
	CBackuppage2 m_page2;
	CBackuppage3 m_page3;

protected:
	DECLARE_MESSAGE_MAP()
public:
//	virtual BOOL OnInitDialog();
	virtual BOOL OnInitDialog();
};


