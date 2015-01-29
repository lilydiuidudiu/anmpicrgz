
// AnmPicRgzDlg.h : 头文件
//

#pragma once
#include "resource.h"
#include <opencv2/opencv.hpp>
#include "InitDlg.h"
#include <vector>
#include <string>
#include "EasySize.h"
#include "Data.h"
// CAnmPicRgzDlg 对话框
using namespace std;
class CAnmPicRgzDlg : public CDialogEx
{
DECLARE_EASYSIZE

// 构造
public:
	CAnmPicRgzDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_ANMPICRGZ_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();

	CInitDlg *p_InitDlg;

	DECLARE_MESSAGE_MAP()

public:
	CString AppointFile;
	afx_msg void OnBnClickedButtonStart();
	afx_msg void OnBnClickedButtonYes();
	afx_msg void OnBnClickedButtonNo();
	afx_msg void OnBnClickedButtonLast();
	afx_msg void OnBnClickedButtonNext();

	void FillBitmapInfo( BITMAPINFO *bmi, int width, int height, int bpp );
	void ShowImage(IplImage *pImg, CWnd *wnd, BITMAPINFO *bmi);
	void ShowImageAuto( IplImage *pImg, CWnd *wnd );
	void DrawArrow(CPoint p1,CPoint p2,double theta,int length);

	//void InitializeProcess();
	//↓↓2015.1.20修改了AutoNextPic/NextPicControl/OnTimer/InitialAll/Copy/OnBnClickedButton
	void AutoNextPic();
	void NextPicControl(vector<Data>::iterator it);
	void OnTimer (UINT_PTR nIDEvent) ;
	void InitialAll(vector<string>* allList,int* num);
	void Copy();
	CString text_mark;
	afx_msg void OnBnClickedButtonCopy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
//	afx_msg void OnSizeClipboard(CWnd* pClipAppWnd, HGLOBAL hRect);
	afx_msg void OnSizing(UINT fwSide, LPRECT pRect);
};


UINT CompareThread(LPVOID pParam);
UINT InitializeThread(LPVOID pParam);