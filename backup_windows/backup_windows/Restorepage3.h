#pragma once
#include "afxwin.h"


// CRestorepage3 �Ի���

class CRestorepage3 : public CPropertyPage
{
	DECLARE_DYNAMIC(CRestorepage3)

public:
	CRestorepage3();
	virtual ~CRestorepage3();

// �Ի�������
	enum { IDD = IDD_RESTOREPAGE3 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnSetActive();
	CString m_filename;
	CString m_partition;
	CButton m_shutdown;
//	CButton m_bpb;
	CButton m_active;
	afx_msg void OnPaint();
//	CString m_comment1;
	CString m_comment2;
	virtual BOOL OnWizardFinish();
	CButton m_restcheck;
};
