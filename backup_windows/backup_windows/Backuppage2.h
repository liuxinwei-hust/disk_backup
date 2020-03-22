#pragma once
#include "afxwin.h"


// CBackuppage2 对话框

class CBackuppage2 : public CPropertyPage
{
	DECLARE_DYNAMIC(CBackuppage2)

public:
	CBackuppage2();
	virtual ~CBackuppage2();

// 对话框数据
	enum { IDD = IDD_BACKUPPAGE2 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnSetActive();
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedButton1();
	int m_passwd;
	CString m_path;
	CString m_filename;
//	CEdit m_password1;
//	CEdit m_password2;
	afx_msg void OnBnClickedCheck1();
	virtual BOOL OnInitDialog();
	virtual LRESULT OnWizardNext();
//	CString m_password1;
//	CString m_password2;
	CEdit m_password1;
	CEdit m_password2;
};
