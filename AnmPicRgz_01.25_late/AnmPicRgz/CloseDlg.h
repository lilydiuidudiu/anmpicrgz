#pragma once


// CCloseDlg �Ի���

class CCloseDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CCloseDlg)

public:
	CCloseDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CCloseDlg();

// �Ի�������
	enum { IDD = IDD_CLOSE_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedCopyYes();
	afx_msg void OnBnClickedCopyNo();
	int ExitNCopy;
	virtual void OnCancel();
};
