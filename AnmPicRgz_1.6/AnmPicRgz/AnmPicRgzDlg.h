
// AnmPicRgzDlg.h : ͷ�ļ�
//

#pragma once
#include "resource.h"
#include <opencv2/opencv.hpp>
#include "InitDlg.h"
#include <vector>
#include <string>
#include "EasySize.h"
#include "Data.h"
// CAnmPicRgzDlg �Ի���
using namespace std;
class CAnmPicRgzDlg : public CDialogEx
{
DECLARE_EASYSIZE

// ����
public:
	CAnmPicRgzDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_ANMPICRGZ_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
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
	//����2015.1.20�޸���AutoNextPic/NextPicControl/OnTimer/InitialAll/Copy/OnBnClickedButton
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