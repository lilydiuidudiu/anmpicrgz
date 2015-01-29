// ConfirmDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "AnmPicRgz.h"
#include "ConfirmDlg.h"
#include "afxdialogex.h"


// CConfirmDlg 对话框

IMPLEMENT_DYNAMIC(CConfirmDlg, CDialogEx)

CConfirmDlg::CConfirmDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CConfirmDlg::IDD, pParent)
	, text_orig(_T(""))
	, text_new(_T(""))
{

}

CConfirmDlg::~CConfirmDlg()
{
}

void CConfirmDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_STATIC_ORIG, text_orig);
	DDX_Text(pDX, IDC_STATIC_NEW, text_new);
}


BEGIN_MESSAGE_MAP(CConfirmDlg, CDialogEx)
END_MESSAGE_MAP()


// CConfirmDlg 消息处理程序
