
// AnmPicRgzDlg.cpp : ʵ���ļ�
//����ͷ�ļ��Ѹı� 2014.12.9

#include "stdafx.h"
#include "AnmPicRgz.h"
#include "AnmPicRgzDlg.h"
#include "afxdialogex.h"

#include <algorithm>
#include <stdlib.h>
#include <opencv2/opencv.hpp>
#include <ctime>
#include <string.h>
#include <fstream>
#include "AboutDlg.h"
#include "Histogram.h"
#include "InitialBlock.h"
#include "InitialFile.h"		
#include "mostmaybe.h"
#include "CloseDlg.h"

using namespace std;
using namespace cv;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

extern vector<Data> que_vct;			//��ʶ������
extern vector<Data>::iterator que;		//��ʶ�������еĵ�����
extern bool thread_flag;				//�߳����б�־
extern int groupNum,sumNum;
extern bool flashmark;
extern string apt;
bool restoremark=false;					//�жϴ��ڿɲ����Ի�ԭ
IplImage *pImg=NULL;



// CAnmPicRgzDlg �Ի���

CAnmPicRgzDlg::CAnmPicRgzDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CAnmPicRgzDlg::IDD, pParent)
	, AppointFile(_T(""))
	, text_mark(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	p_InitDlg=NULL;
}

void CAnmPicRgzDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_MFCEDITBROWSE1, AppointFile);
	DDX_Text(pDX, IDC_STATIC_MARK, text_mark);
	DDX_Text(pDX, IDC_STATIC_COUNT, text_count);
}

BEGIN_MESSAGE_MAP(CAnmPicRgzDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_TIMER()
	ON_WM_SIZE()
	ON_WM_SIZING()
	ON_WM_CLOSE()
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_BUTTON_START, &CAnmPicRgzDlg::OnBnClickedButtonStart)
	ON_BN_CLICKED(IDC_BUTTON_YES, &CAnmPicRgzDlg::OnBnClickedButtonYes)
	ON_BN_CLICKED(IDC_BUTTON_NO, &CAnmPicRgzDlg::OnBnClickedButtonNo)
	ON_BN_CLICKED(IDC_BUTTON_LAST, &CAnmPicRgzDlg::OnBnClickedButtonLast)
	ON_BN_CLICKED(IDC_BUTTON_NEXT, &CAnmPicRgzDlg::OnBnClickedButtonNext)
	ON_BN_CLICKED(IDC_BUTTON_COPY, &CAnmPicRgzDlg::OnBnClickedButtonCopy)
END_MESSAGE_MAP()


// CAnmPicRgzDlg ��Ϣ�������

BOOL CAnmPicRgzDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������
	INIT_EASYSIZE;
	ShowWindow(SW_MAXIMIZE);
	GetDlgItem(IDC_BUTTON_YES)->EnableWindow(false);
	GetDlgItem(IDC_BUTTON_NO)->EnableWindow(false);
	GetDlgItem(IDC_BUTTON_LAST)->EnableWindow(false);
	GetDlgItem(IDC_BUTTON_NEXT)->EnableWindow(false);
	GetDlgItem(IDC_BUTTON_COPY)->EnableWindow(false);
	GetDlgItem(IDC_STATIC_PIC)->ShowWindow(false);
	GetDlgItem(IDC_STATIC_WARN)->ShowWindow(false);

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CAnmPicRgzDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else if(nID == SC_RESTORE)
	{
		if(restoremark)
		{
			CDialogEx::OnSysCommand(nID, lParam);
			restoremark=false;
		}
	}
	else if(nID == SC_MINIMIZE)
	{
		restoremark=true;
		CDialogEx::OnSysCommand(nID, lParam);
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CAnmPicRgzDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		if(!que_vct.empty())
			if(que!=que_vct.end())
				if(pImg!=NULL)
				{
					ShowImageAuto(pImg,GetDlgItem(IDC_STATIC_PIC));
					DrawArrow();
				}

		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CAnmPicRgzDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CAnmPicRgzDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	UPDATE_EASYSIZE;

	CWnd *pWnd;
	pWnd = GetDlgItem(IDC_STATIC_PIC);     //��ȡ�ؼ����
	if(pWnd)//�ж��Ƿ�Ϊ�գ���Ϊ�Ի��򴴽�ʱ����ô˺���������ʱ�ؼ���δ����
	{
		CRect rect_grp;   //��ȡgroup�ؼ���С
		GetDlgItem(IDC_STATIC_GROUP)->GetWindowRect(&rect_grp);
		ScreenToClient(&rect_grp);//���ؼ���Сת��Ϊ�ڶԻ����е���������
		int width,height;
		width=cx-20;
		height=rect_grp.top-10;
		CRect rect_pic;   //����pic�ؼ���С
		if(width*3<height*4)
		{
			rect_pic.left=10;
			height=width*3/4;
			int center=(rect_grp.top-10-height)/2;
			rect_pic.top=10+center;
		}
		else
		{
			rect_pic.top=10;
			width=height*4/3;
			int center=(cx-20-width)/2;
			rect_pic.left=10+center;
		}
		rect_pic.right=rect_pic.left+width;
		rect_pic.bottom=rect_pic.top+height;
		pWnd->MoveWindow(rect_pic);//���ÿؼ���С
	}
	if(nType==SIZE_MINIMIZED )
		return;
}



void CAnmPicRgzDlg::OnSizing(UINT fwSide, LPRECT pRect)
{
	CDialogEx::OnSizing(fwSide, pRect);

	// TODO: �ڴ˴������Ϣ����������
	EASYSIZE_MINSIZE(950,700,fwSide,pRect);
}



BEGIN_EASYSIZE_MAP(CAnmPicRgzDlg)

	EASYSIZE(IDC_STATIC_GROUP,ES_BORDER,ES_KEEPSIZE,ES_BORDER,ES_BORDER,ES_HCENTER)
	EASYSIZE(IDC_STATIC_COPYRIGHT,ES_BORDER,ES_BORDER,ES_BORDER,ES_BORDER,ES_HCENTER|ES_VCENTER)
	EASYSIZE(IDC_STATIC_TIP,IDC_STATIC_GROUP,IDC_STATIC_GROUP,ES_KEEPSIZE,ES_KEEPSIZE,0)
	EASYSIZE(IDC_STATIC_MARK,IDC_STATIC_GROUP,IDC_STATIC_GROUP,ES_KEEPSIZE,ES_KEEPSIZE,0)
	EASYSIZE(IDC_STATIC_WARN,IDC_STATIC_GROUP,IDC_STATIC_GROUP,ES_KEEPSIZE,ES_KEEPSIZE,0)
	EASYSIZE(IDC_STATIC_COUNT,IDC_STATIC_GROUP,IDC_STATIC_GROUP,ES_KEEPSIZE,ES_KEEPSIZE,0)
	EASYSIZE(IDC_MFCEDITBROWSE1,IDC_STATIC_GROUP,IDC_STATIC_GROUP,ES_KEEPSIZE,ES_KEEPSIZE,0)
	EASYSIZE(IDC_BUTTON_START,IDC_STATIC_GROUP,IDC_STATIC_GROUP,ES_KEEPSIZE,ES_KEEPSIZE,0)
	EASYSIZE(IDC_BUTTON_YES,IDC_STATIC_GROUP,IDC_STATIC_GROUP,ES_KEEPSIZE,ES_KEEPSIZE,0)
	EASYSIZE(IDC_BUTTON_NO,IDC_STATIC_GROUP,IDC_STATIC_GROUP,ES_KEEPSIZE,ES_KEEPSIZE,0)
	EASYSIZE(IDC_BUTTON_LAST,IDC_STATIC_GROUP,IDC_STATIC_GROUP,ES_KEEPSIZE,ES_KEEPSIZE,0)
	EASYSIZE(IDC_BUTTON_NEXT,IDC_STATIC_GROUP,IDC_STATIC_GROUP,ES_KEEPSIZE,ES_KEEPSIZE,0)
	EASYSIZE(IDC_BUTTON_COPY,IDC_STATIC_GROUP,IDC_STATIC_GROUP,ES_KEEPSIZE,ES_KEEPSIZE,0)

END_EASYSIZE_MAP



void CAnmPicRgzDlg::AutoNextPic()
{
	while(1)
	{
		que++;
		if(que==que_vct.end())
		{
			//GetDlgItem(IDC_BUTTON_COPY)->EnableWindow(false);
			//que--;
			return;
		}
		else if(que->mark==0)			//��ȷ��
		{
			/*cvReleaseImage(&pImg);
			pImg = cvLoadImage((*que).everyfilename.c_str(),1);
			ShowImageAuto(pImg,GetDlgItem(IDC_STATIC_PIC));
			DrawArrow();
			text_mark="";
			UpdateData(false);*/
			ShowPicAndInfo();
			return;
		}
	}
}
void CAnmPicRgzDlg:: NextPicControl(vector<Data> ::iterator it)
{
	++it;
	while(1)
	{
		while(it->counter!=0)
		{
			SetTimer(10,100,NULL);
		}
		if(it->mark==0)
		{
			AutoNextPic();
			break;
		}
		else
			++it;
		if(it==que_vct.end())
		{
			que=que_vct.end();
			break;
		}
	}
}
void CAnmPicRgzDlg::OnTimer(UINT_PTR nIDEvent)
{
	//bool flashmark=false;
	switch(nIDEvent)
	{
		case 10: break;
		case  5: 
			flashmark=!flashmark;
			if(flashmark)
				GetDlgItem(IDC_STATIC_WARN)->ShowWindow(true);
			else
				GetDlgItem(IDC_STATIC_WARN)->ShowWindow(false);
			break;
	}
	CDialog::OnTimer(nIDEvent);
}



BOOL CAnmPicRgzDlg::PreTranslateMessage(MSG* pMsg)
{
	if(pMsg->message==WM_LBUTTONDBLCLK)
	{
		int nBtnID =  GetWindowLong(pMsg->hwnd,GWL_ID); 
		if(nBtnID == IDC_BUTTON_YES)
			//OnBnDoubleclickedButtonYes();
			AfxMessageBox(_T("����ϸ�鿴������������"));
		if(nBtnID == IDC_BUTTON_NO)
			//OnBnDoubleclickedButtonNo();
			AfxMessageBox(_T("����ϸ�鿴������������"));
	}

	return CDialogEx::PreTranslateMessage(pMsg);
}


void CAnmPicRgzDlg::OnClose()
{
	if(que_vct.empty())
		CDialogEx::OnCancel();
	else if(que_vct.begin()->mark==0)
		CDialogEx::OnCancel();
	else
	{
		CCloseDlg CloseDlg;
		CloseDlg.DoModal();		//����ģ̬�Ի���
		if (CloseDlg.ExitNCopy!=0)
		{
			if(CloseDlg.ExitNCopy==2)
				Copy();
			
			CDialogEx::OnCancel();
		}
	}
}

void CAnmPicRgzDlg::OnCancel()
{
	//CDialogEx::OnCancel();
}

void CAnmPicRgzDlg::ShowPicAndInfo()
{
	cvReleaseImage(&pImg);
	string cmplete_filename;
	cmplete_filename=apt+"\\"+(*que).everyfilename;
	pImg = cvLoadImage(cmplete_filename.c_str(),1);
	ShowImageAuto(pImg,GetDlgItem(IDC_STATIC_PIC));
	DrawArrow();
	int countNum=(groupNum-1)*PicNumInGroup+que->number;
	CString str_count,str_sum;
	str_count.Format(_T("%d"),countNum);
	str_sum.Format(_T("%d"),sumNum);
	text_count=_T("�� ")+str_count+_T(" �ţ��� ")+str_sum+_T(" ��");
	if(que->mark>0)			text_mark="���ж�Ϊ �ж���";
	else if(que->mark<0)	text_mark="���ж�Ϊ �޶���";
	else					text_mark="";
	UpdateData(false);
}


HBRUSH CAnmPicRgzDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  �ڴ˸��� DC ���κ�����
	/*CFont * cFont=new CFont;  
	cFont->CreateFont(16,0,0,0,FW_NORMAL,FALSE,FALSE,0,   
		ANSI_CHARSET,OUT_DEFAULT_PRECIS,  
		CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,  
		DEFAULT_PITCH&FF_SWISS,_T("MS Shell Dlg"));  
	switch (pWnd->GetDlgCtrlID())   
	{   
		//���IDΪIDC_CTL1��IDC_CTL2��IDC_CTL3�Ŀؼ�����ͬ��������  
	//case IDC_CTL1:  
	//case IDC_CTL2:  
	case IDC_STATIC_COPYRIGHT:
		{  
			//pDC->SetBkMode(TRANSPARENT);  
			//pDC->SetTextColor(RGB(255,255, 0));  
			pWnd->SetFont(cFont);  
			//HBRUSH B = CreateSolidBrush(RGB(125,125,255));  
			return hbr;  
		}  
	default:  
		return CDialog::OnCtlColor(pDC, pWnd, nCtlColor);  
	}  */

	if (pWnd->GetDlgCtrlID() == IDC_STATIC_WARN)
	{
		pDC->SetTextColor(RGB(255, 0, 0));
	} 

	// TODO:  ���Ĭ�ϵĲ������軭�ʣ��򷵻���һ������
	return hbr;
}
