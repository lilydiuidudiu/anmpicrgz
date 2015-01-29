
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

using namespace std;
using namespace cv;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

extern vector<Data> que_vct;			//待识别序列
extern vector<Data>::iterator que;		//待识别序列中的迭代器
extern bool thread_flag;			//线程运行标志

IplImage *pImg=NULL;

//vector<Data> bg_vct;			//背景缓冲队列
//vector<Data>::iterator bg;	//背景序列中的迭代器

//filename;						//用来存放当前检测照片的文件名
//string apt;					//用户路径
//vector<string> foldlist;		//储存子文件夹路径
//vector<string>::iterator it_fold;
//bool multi_flag=true;			//多文件夹处理标志



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
	//  DDX_Text(pDX, IDC_MFCEDITBROWSE1, AppointFile);
	DDX_Text(pDX, IDC_MFCEDITBROWSE1, AppointFile);
	DDX_Text(pDX, IDC_STATIC_MARK, text_mark);
}

BEGIN_MESSAGE_MAP(CAnmPicRgzDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_START, &CAnmPicRgzDlg::OnBnClickedButtonStart)
	ON_BN_CLICKED(IDC_BUTTON_YES, &CAnmPicRgzDlg::OnBnClickedButtonYes)
	ON_BN_CLICKED(IDC_BUTTON_NO, &CAnmPicRgzDlg::OnBnClickedButtonNo)
	ON_BN_CLICKED(IDC_BUTTON_LAST, &CAnmPicRgzDlg::OnBnClickedButtonLast)
	ON_BN_CLICKED(IDC_BUTTON_NEXT, &CAnmPicRgzDlg::OnBnClickedButtonNext)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON_COPY, &CAnmPicRgzDlg::OnBnClickedButtonCopy)
	ON_WM_SIZE()
//	ON_WM_SIZECLIPBOARD()
	ON_WM_SIZING()
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

	//ShowWindow(SW_MAXIMIZE);

	// TODO: 在此添加额外的初始化代码
	INIT_EASYSIZE;
	GetDlgItem(IDC_BUTTON_YES)->EnableWindow(false);
	GetDlgItem(IDC_BUTTON_NO)->EnableWindow(false);
	GetDlgItem(IDC_BUTTON_LAST)->EnableWindow(false);
	GetDlgItem(IDC_BUTTON_NEXT)->EnableWindow(false);
	//GetDlgItem(IDC_BUTTON_COPY)->EnableWindow(false);

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CAnmPicRgzDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
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
		CDialogEx::OnPaint();

		/*if(!que_vct.empty())
		{
			if(que!=que_vct.end())
			{*/
		if(pImg!=NULL)
				//pImg = cvLoadImage((*que).everyfilename.c_str(),1);
				ShowImageAuto(pImg,GetDlgItem(IDC_STATIC_PIC));
				//cvReleaseImage(&pImg);
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
	//EASYSIZE_MINSIZE(470,20);
	EASYSIZE_MINSIZE(850,200,fwSide,pRect);
}



BEGIN_EASYSIZE_MAP(CAnmPicRgzDlg)

	EASYSIZE(IDC_STATIC_GROUP,ES_BORDER,ES_KEEPSIZE,ES_BORDER,ES_BORDER,ES_HCENTER)
	EASYSIZE(IDC_STATIC_MARK,IDC_STATIC_GROUP,IDC_STATIC_GROUP,ES_KEEPSIZE,ES_KEEPSIZE,0)
	EASYSIZE(IDC_MFCEDITBROWSE1,IDC_STATIC_GROUP,IDC_STATIC_GROUP,ES_KEEPSIZE,ES_KEEPSIZE,0)
	EASYSIZE(IDC_BUTTON_START,IDC_STATIC_GROUP,IDC_STATIC_GROUP,ES_KEEPSIZE,ES_KEEPSIZE,0)
	EASYSIZE(IDC_BUTTON_YES,IDC_STATIC_GROUP,IDC_STATIC_GROUP,ES_KEEPSIZE,ES_KEEPSIZE,0)
	EASYSIZE(IDC_BUTTON_NO,IDC_STATIC_GROUP,IDC_STATIC_GROUP,ES_KEEPSIZE,ES_KEEPSIZE,0)
	EASYSIZE(IDC_BUTTON_LAST,IDC_STATIC_GROUP,IDC_STATIC_GROUP,ES_KEEPSIZE,ES_KEEPSIZE,0)
	EASYSIZE(IDC_BUTTON_NEXT,IDC_STATIC_GROUP,IDC_STATIC_GROUP,ES_KEEPSIZE,ES_KEEPSIZE,0)
	EASYSIZE(IDC_BUTTON_COPY,IDC_STATIC_GROUP,IDC_STATIC_GROUP,ES_KEEPSIZE,ES_KEEPSIZE,0)
	//EASYSIZE(IDC_STATIC_PIC,left,top,right,bottom,options)

END_EASYSIZE_MAP



void CAnmPicRgzDlg::AutoNextPic()
{
	while(1)
	{
		que++;
		if(que->mark==0)			//不确定
		{
			cvReleaseImage(&pImg);
			pImg = cvLoadImage((*que).everyfilename.c_str(),1);
			ShowImageAuto(pImg,GetDlgItem(IDC_STATIC_PIC));
			text_mark="";
			UpdateData(false);
			break;
		}
		else if(que==que_vct.end())
		{
			break;
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
	}
}
void CAnmPicRgzDlg::OnTimer(UINT_PTR nIDEvent)
{
	switch(nIDEvent)
		case 10:
			break;
	CDialog::OnTimer(nIDEvent);
}

