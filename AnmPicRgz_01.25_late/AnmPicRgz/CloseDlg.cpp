// CloseDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "AnmPicRgz.h"
#include "CloseDlg.h"
#include "afxdialogex.h"


// CCloseDlg �Ի���

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


// CCloseDlg ��Ϣ�������


void CCloseDlg::OnBnClickedCancel()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	ExitNCopy=0;
	CDialogEx::OnCancel();
}


void CCloseDlg::OnBnClickedCopyYes()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	ExitNCopy=2;
	CDialogEx::OnCancel();
}


void CCloseDlg::OnBnClickedCopyNo()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	ExitNCopy=1;
	CDialogEx::OnCancel();
}


void CCloseDlg::OnCancel()
{
	// TODO: �ڴ����ר�ô����/����û���
	//CDialogEx::OnCancel();
}
