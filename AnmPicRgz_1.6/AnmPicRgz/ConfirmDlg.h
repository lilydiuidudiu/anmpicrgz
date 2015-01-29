#pragma once


// CConfirmDlg 对话框

class CConfirmDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CConfirmDlg)

public:
	CConfirmDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CConfirmDlg();

// 对话框数据
	enum { IDD = IDD_CONFIRM_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CString text_orig;
	CString text_new;

};
