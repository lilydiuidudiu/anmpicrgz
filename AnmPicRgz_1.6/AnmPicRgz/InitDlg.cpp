// InitDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "AnmPicRgz.h"
#include "InitDlg.h"
#include "afxdialogex.h"


// CInitDlg �Ի���

IMPLEMENT_DYNAMIC(CInitDlg, CDialogEx)

CInitDlg::CInitDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CInitDlg::IDD, pParent)
	, text_init(_T(""))
{

}

CInitDlg::~CInitDlg()
{
}

void CInitDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_STATIC_INIT, text_init);
}


BEGIN_MESSAGE_MAP(CInitDlg, CDialogEx)
END_MESSAGE_MAP()


// CInitDlg ��Ϣ�������
