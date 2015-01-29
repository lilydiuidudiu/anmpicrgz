#pragma once


// CInitDlg 对话框

class CInitDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CInitDlg)

public:
	CInitDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CInitDlg();

// 对话框数据
	enum { IDD = IDD_INIT_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CString text_init;
};
