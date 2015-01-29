
// AnmPicRgzDlg.cpp : 实现文件
//以下头文件已改变 2014.12.9

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

extern vector<Data> que_vct;			//待识别序列
extern vector<Data>::iterator que;		//待识别序列中的迭代器
extern bool thread_flag;				//线程运行标志
extern int groupNum,sumNum;
extern bool flashmark;
extern string apt;
bool restoremark=false;					//判断窗口可不可以还原
IplImage *pImg=NULL;



// CAnmPicRgzDlg 对话框

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


// CAnmPicRgzDlg 消息处理程序

BOOL CAnmPicRgzDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
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

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	INIT_EASYSIZE;
	ShowWindow(SW_MAXIMIZE);
	GetDlgItem(IDC_BUTTON_YES)->EnableWindow(false);
	GetDlgItem(IDC_BUTTON_NO)->EnableWindow(false);
	GetDlgItem(IDC_BUTTON_LAST)->EnableWindow(false);
	GetDlgItem(IDC_BUTTON_NEXT)->EnableWindow(false);
	GetDlgItem(IDC_BUTTON_COPY)->EnableWindow(false);
	GetDlgItem(IDC_STATIC_PIC)->ShowWindow(false);
	GetDlgItem(IDC_STATIC_WARN)->ShowWindow(false);

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
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

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CAnmPicRgzDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
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

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CAnmPicRgzDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CAnmPicRgzDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	UPDATE_EASYSIZE;

	CWnd *pWnd;
	pWnd = GetDlgItem(IDC_STATIC_PIC);     //获取控件句柄
	if(pWnd)//判断是否为空，因为对话框创建时会调用此函数，而当时控件还未创建
	{
		CRect rect_grp;   //获取group控件大小
		GetDlgItem(IDC_STATIC_GROUP)->GetWindowRect(&rect_grp);
		ScreenToClient(&rect_grp);//将控件大小转换为在对话框中的区域坐标
		int width,height;
		width=cx-20;
		height=rect_grp.top-10;
		CRect rect_pic;   //计算pic控件大小
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
		pWnd->MoveWindow(rect_pic);//设置控件大小
	}
	if(nType==SIZE_MINIMIZED )
		return;
}



void CAnmPicRgzDlg::OnSizing(UINT fwSide, LPRECT pRect)
{
	CDialogEx::OnSizing(fwSide, pRect);

	// TODO: 在此处添加消息处理程序代码
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
		else if(que->mark==0)			//不确定
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
			AfxMessageBox(_T("请仔细查看，请勿连击！"));
		if(nBtnID == IDC_BUTTON_NO)
			//OnBnDoubleclickedButtonNo();
			AfxMessageBox(_T("请仔细查看，请勿连击！"));
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
		CloseDlg.DoModal();		//弹出模态对话框
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
	text_count=_T("第 ")+str_count+_T(" 张，共 ")+str_sum+_T(" 张");
	if(que->mark>0)			text_mark="已判断为 有动物";
	else if(que->mark<0)	text_mark="已判断为 无动物";
	else					text_mark="";
	UpdateData(false);
}


HBRUSH CAnmPicRgzDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  在此更改 DC 的任何特性
	/*CFont * cFont=new CFont;  
	cFont->CreateFont(16,0,0,0,FW_NORMAL,FALSE,FALSE,0,   
		ANSI_CHARSET,OUT_DEFAULT_PRECIS,  
		CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,  
		DEFAULT_PITCH&FF_SWISS,_T("MS Shell Dlg"));  
	switch (pWnd->GetDlgCtrlID())   
	{   
		//针对ID为IDC_CTL1、IDC_CTL2和IDC_CTL3的控件进行同样的设置  
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

	// TODO:  如果默认的不是所需画笔，则返回另一个画笔
	return hbr;
}
