#pragma once


// CBackuppage3 �Ի���

class CBackuppage3 : public CPropertyPage
{
	DECLARE_DYNAMIC(CBackuppage3)

public:
	CBackuppage3();
	virtual ~CBackuppage3();

// �Ի�������
	enum { IDD = IDD_BACKUPPAGE3 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnSetActive();
	CString m_check;
	CString m_compress;
	CString m_crc;
	CString m_destdrive;
	CString m_sourcedisk;
	CString m_filename;
	CString m_filesystem;
	CString m_sourcepart;
	CString m_parttype;
	CString m_password;
	CString m_shutdown;
	CString m_split;
//	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
};
