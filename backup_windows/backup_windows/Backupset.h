#pragma once
#include "afxwin.h"



// CBackupset �Ի���

class CBackupset : public CDialogEx
{
	DECLARE_DYNAMIC(CBackupset)

public:
	CBackupset(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CBackupset();

// �Ի�������
	enum { IDD = IDD_BACKUPSET };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

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
