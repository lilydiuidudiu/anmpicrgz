#pragma once


// CCloseDlg 对话框

class CCloseDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CCloseDlg)

public:
	CCloseDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CCloseDlg();

// 对话框数据
	enum { IDD = IDD_CLOSE_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedCopyYes();
	afx_msg void OnBnClickedCopyNo();
	int ExitNCopy;
	virtual void OnCancel();
};
