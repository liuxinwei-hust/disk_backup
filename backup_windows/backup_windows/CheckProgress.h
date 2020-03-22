#pragma once
#include "afxcmn.h"
#include "resource.h"		// ������

// CCheckProgress �Ի���

class CCheckProgress : public CDialogEx
{
	DECLARE_DYNAMIC(CCheckProgress)

public:
	CCheckProgress(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CCheckProgress();

// �Ի�������
	enum { IDD = IDD_CHECKDLAG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	CString m_filepath;
	CString m_percentage;
	CString m_lefttime;
	CProgressCtrl m_progress;
	virtual BOOL OnInitDialog();
	afx_msg void OnClose();
protected:
	afx_msg LRESULT OnCheckupdate(WPARAM wParam, LPARAM lParam);
public:
	afx_msg void OnBnClickedCancel();
};
