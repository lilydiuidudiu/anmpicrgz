#pragma once


// CInitDlg �Ի���

class CInitDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CInitDlg)

public:
	CInitDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CInitDlg();

// �Ի�������
	enum { IDD = IDD_INIT_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	CString text_init;
};
