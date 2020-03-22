#pragma once
#include "afxwin.h"


// CRestorepage2 对话框

class CRestorepage2 : public CPropertyPage
{
	DECLARE_DYNAMIC(CRestorepage2)

public:
	CRestorepage2();
	virtual ~CRestorepage2();

// 对话框数据
	enum { IDD = IDD_RESTOREPAGE2 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnSetActive();
	CComboBox m_diskselect;
	CComboBox m_partitionsel;
	CString m_diskheads;
	CString m_diskcyl;
	CString m_disksector;
	CString m_disksecpercyl;
	CString m_disksecsize;
	CString m_midatype;
	CString m_disktotalsize;
	CString m_partotal;
	CString m_parfilesys;
	CString m_parused;
	CString m_parleft;
	virtual BOOL OnInitDialog();
	CString m_partype;
	afx_msg void OnCbnSelchangeRestcombo1();
	afx_msg void OnCbnSelchangeRestcombo2();
	virtual LRESULT OnWizardNext();
	afx_msg void OnBnClickedCheck1();
	BOOL m_setwindowsys;
};
