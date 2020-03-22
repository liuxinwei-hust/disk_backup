#pragma once
#include "Restorepage1.h"
#include "Restorepage2.h"
#include "Restorepage3.h"


// CRestoreSheet

class CRestoreSheet : public CPropertySheet
{
	DECLARE_DYNAMIC(CRestoreSheet)

public:
	CRestoreSheet(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	CRestoreSheet(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	virtual ~CRestoreSheet();
	CRestorepage1 m_page1;
	CRestorepage2 m_page2;
	CRestorepage3 m_page3;
protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
};


