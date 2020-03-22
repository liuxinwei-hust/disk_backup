#pragma once
#include "afxwin.h"


// CBackuppage1 对话框

class CBackuppage1 : public CPropertyPage
{
	DECLARE_DYNAMIC(CBackuppage1)

public:
	CBackuppage1();
	virtual ~CBackuppage1();

// 对话框数据
	enum { IDD = IDD_BACKUPPAGE1 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnSetActive();
	virtual BOOL OnInitDialog();
	CComboBox m_diskselect;
	CString m_diskcyl;
	CString m_diskheads;
	CString m_disksecpercyl;
	CString m_disksector;
	CString m_disksecsize;
	//CString m_diskleftsize;
	CString m_midatype;
	CString m_disktotalsize;
	//CString m_diskusedsize;
//	afx_msg void OnCbnDropdownCombo1();
	afx_msg void OnCbnSelchangeCombo1();
	CComboBox m_partitionsel;
	CString m_partotal;
	CString m_parused;
	CString m_parleft;
	CString m_parfilesys;
	CString m_partype;
	afx_msg void OnCbnSelchangeCombo2();
	virtual LRESULT OnWizardNext();
};
