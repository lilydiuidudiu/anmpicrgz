// CloseDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "AnmPicRgz.h"
#include "CloseDlg.h"
#include "afxdialogex.h"


// CCloseDlg 对话框

IMPLEMENT_DYNAMIC(CCloseDlg, CDialogEx)

CCloseDlg::CCloseDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CCloseDlg::IDD, pParent)
{

}

CCloseDlg::~CCloseDlg()
{
}

void CCloseDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CCloseDlg, CDialogEx)
	ON_BN_CLICKED(IDCANCEL, &CCloseDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_COPY_YES, &CCloseDlg::OnBnClickedCopyYes)
	ON_BN_CLICKED(IDC_COPY_NO, &CCloseDlg::OnBnClickedCopyNo)
END_MESSAGE_MAP()


// CCloseDlg 消息处理程序


void CCloseDlg::OnBnClickedCancel()
{
	// TODO: 在此添加控件通知处理程序代码
	ExitNCopy=0;
	CDialogEx::OnCancel();
}


void CCloseDlg::OnBnClickedCopyYes()
{
	// TODO: 在此添加控件通知处理程序代码
	ExitNCopy=2;
	CDialogEx::OnCancel();
}


void CCloseDlg::OnBnClickedCopyNo()
{
	// TODO: 在此添加控件通知处理程序代码
	ExitNCopy=1;
	CDialogEx::OnCancel();
}


void CCloseDlg::OnCancel()
{
	// TODO: 在此添加专用代码和/或调用基类
	//CDialogEx::OnCancel();
}
