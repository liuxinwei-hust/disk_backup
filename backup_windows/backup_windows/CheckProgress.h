#pragma once
#include "afxcmn.h"
#include "resource.h"		// 主符号

// CCheckProgress 对话框

class CCheckProgress : public CDialogEx
{
	DECLARE_DYNAMIC(CCheckProgress)

public:
	CCheckProgress(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CCheckProgress();

// 对话框数据
	enum { IDD = IDD_CHECKDLAG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

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
