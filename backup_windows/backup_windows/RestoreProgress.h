#pragma once
#include "afxcmn.h"
#include "resource.h"		// 主符号

// CRestoreProgress 对话框

class CRestoreProgress : public CDialogEx
{
	DECLARE_DYNAMIC(CRestoreProgress)

public:
	CRestoreProgress(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CRestoreProgress();

// 对话框数据
	enum { IDD = IDD_RESTPROGRESS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedCancel();
	CString m_percentage;
	CString m_filename;
	CString m_partiton;
	CString m_lefttime;
	virtual BOOL OnInitDialog();
	afx_msg void OnClose();
	//CProgressCtrl m_progress;
	CProgressCtrl m_prog;
protected:
	afx_msg LRESULT OnRestupdate(WPARAM wParam, LPARAM lParam);
};
