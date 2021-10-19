#pragma once
#include "afxwin.h"



// CBackupset 对话框

class CBackupset : public CDialogEx
{
	DECLARE_DYNAMIC(CBackupset)

public:
	CBackupset(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CBackupset();

// 对话框数据
	enum { IDD = IDD_BACKUPSET };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	int m_compress;
	int m_crc;
	CComboBox m_split;
//	int m_check;
	int m_shutdown;
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedOk();
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	CButton m_endcheck;
	afx_msg void OnBnClickedRadio3();
	afx_msg void OnBnClickedRadio4();
};
