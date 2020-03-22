#pragma once
#include "afxcmn.h"
#include "resource.h"		// 主符号

// CBackupProgress 对话框


class CBackupProgress : public CDialogEx
{
	DECLARE_DYNAMIC(CBackupProgress)

public:
	CBackupProgress(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CBackupProgress();

// 对话框数据
	enum { IDD = IDD_DIALOG1 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CString m_backupdest;
	CProgressCtrl m_progress;
	CString m_percentage;
	CString m_backupsource;
	CString m_lefttime;
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedCancel();
//	afx_msg void OnClose();
	afx_msg void OnClose();
protected:
	afx_msg LRESULT OnMyupdate(WPARAM wParam, LPARAM lParam);
};
