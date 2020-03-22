#pragma once
#include "afxwin.h"


// CRestorepage1 对话框

class CRestorepage1 : public CPropertyPage
{
	DECLARE_DYNAMIC(CRestorepage1)

public:
	CRestorepage1();
	virtual ~CRestorepage1();

// 对话框数据
	enum { IDD = IDD_RESTOREPAGE1 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnSetActive();
	CString m_filepath;
	CString m_password;
	CEdit m_password2;
	CString m_bcrc;
	CString m_bcompress;
	CString m_packetnum;
	CString m_filenum;
	CString m_filesystype;
	CString m_sectnum;
	CString m_active;
	CString m_syspart;
	CString m_spaceneed;
	afx_msg void OnBnClickedButton1();
	virtual BOOL OnInitDialog();
	CString m_comment;
	virtual LRESULT OnWizardNext();
};
